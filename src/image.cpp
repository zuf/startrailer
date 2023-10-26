#include "image.h"
#include <cassert>

const int DEFAULT_JPEG_QUALITY=95;

namespace StarTrailer
{

Image::Image(){
    init();
}

Image::Image(const Image &from_image)
{
    image=new Magick::Image(*from_image.get_magick_image());
    raw_processor = new LibRaw();
}

Image::Image(const std::string &file, Image::RawProcessingMode raw_processing_mode)
{
    init();
    read(file, raw_processing_mode);
}

Image::Image(const Magick::Image &from_image)
{
    raw_processor = new LibRaw();
    image = new Magick::Image(from_image);    
}

Image::~Image()
{
    delete_data();

//    if (image)
//    {
//        delete image;
//    }
//    if (raw_processor)
//    {
//        raw_processor->recycle();
//        delete raw_processor;
//    }
}

void Image::read(const std::string &file, RawProcessingMode raw_processing_mode) {
    assert(raw_processor != 0);    
    if (LIBRAW_SUCCESS == raw_processor->open_file(file.c_str())) {
        try {
          switch (raw_processing_mode) {
          case FullPreview:
            read_preview_with_libraw(file);
            break;
          case HalfRaw:
            read_raw_with_libraw(file, true);
            break;
          case FullRaw:
            read_raw_with_libraw(file);
            break;
          case TinyPreview:
            throw std::runtime_error(
                "Read RAW with mode TinyPreview not yet implemented");
            break;
          case SmallPreview:
            throw std::runtime_error(
                "Read RAW with mode SmallPreview not yet implemented");
            break;
          default:
            throw std::runtime_error("Unknown raw_processing_mode");
          }
        } catch (const std::runtime_error &e) {
          raw_processor->recycle();
          read_with_image_magick(file);
        }
    } else {
        read_with_image_magick(file);
    }
}

void Image::write(const std::string &new_file)
{
    assert(image != 0);
    image->quality(DEFAULT_JPEG_QUALITY);
    image->write(new_file);    
}

size_t Image::to_buffer(void * &write_to_ptr)
{
    const Magick::StorageType storage_type=Magick::CharPixel;    
    image->write(0, 0, image->columns(), image->rows(), "RGB", storage_type, write_to_ptr);
    return 3*image->columns()*image->rows();
}

void Image::composite(const Image &with_image, Magick::CompositeOperator mode)
{
    image->composite(*(with_image.get_magick_image()), 0, 0, mode);
}

void Image::reset()
{
    delete_data();
    init();
}

void Image::init()
{
    image=new Magick::Image();
    raw_processor = new LibRaw();
}

void Image::read_with_image_magick(const std::string &file)
{
    assert(image != 0);
    try
    {
        image->read(file);
    }
    catch(Magick::Error &error)
    {
       throw std::runtime_error(std::string("Can't open file: ") + file + std::string("ImageMagick reports: ") + error.what());
    }
    catch( std::exception &error )
    {
       throw std::runtime_error(std::string("Can't open file: ") + file + std::string("Exception: ") + error.what());
    }
}

void Image::read_preview_with_libraw(const std::string &file)
{
    assert(raw_processor != 0);

    int result = raw_processor->unpack_thumb();

    if (LIBRAW_SUCCESS!=result)
    {
        throw std::runtime_error(std::string("Can't unpack thumbnail for file ") + file  + ". Error: " + raw_processor->strerror(result));
    }

    switch(raw_processor->imgdata.thumbnail.tformat)
    {
    case LIBRAW_THUMBNAIL_JPEG:
    {
        // memory copied here
        Magick::Blob blob(raw_processor->imgdata.thumbnail.thumb, raw_processor->imgdata.thumbnail.tlength);
        image->read(blob);
    }
        break;

    case LIBRAW_THUMBNAIL_BITMAP:
    {
        image->read( raw_processor->imgdata.thumbnail.twidth, raw_processor->imgdata.thumbnail.theight, "RGB", Magick::CharPixel, raw_processor->imgdata.thumbnail.thumb);
    }
        break;

    case LIBRAW_THUMBNAIL_LAYER:
        throw std::runtime_error("Unsupported thumbnail format. LibRaw reports: LIBRAW_THUMBNAIL_LAYER");
        break;

    case LIBRAW_THUMBNAIL_UNKNOWN:
        throw std::runtime_error("Unsupported or unknown thumbnail format. LibRaw reports: LIBRAW_THUMBNAIL_UNKNOWN");
        break;

    default:
        throw std::runtime_error("Thumbnail read error: Unholy error!"); // my favorite :)
    }

    raw_processor->recycle();
}

void Image::read_raw_with_libraw(const std::string &file, const bool half_size)
{
    assert(raw_processor != 0);

    raw_processor->imgdata.params.half_size = half_size ? 1 : 0;


    raw_processor->imgdata.params.use_camera_wb = 1;
    raw_processor->imgdata.params.output_bps = 8;
    raw_processor->imgdata.params.no_auto_bright = 1;

    //HACK: Expo corection
//    raw_processor->imgdata.params.exp_correc = 0;
//    raw_processor->imgdata.params.exp_shift = 1.0; // 0.25 = -2ev; 8.0 = +3ev; 1.0 = 0ev
//    raw_processor->imgdata.params.exp_preser = 0.0;
//    raw_processor->imgdata.params.user_qual = 3;
//    raw_processor->imgdata.params.output_tiff = 0;
//    raw_processor->imgdata.params.med_passes = 3;

    int result = raw_processor->unpack();
    if (LIBRAW_SUCCESS!=result){
        throw std::runtime_error(std::string("Can't unpack raw file ") + file  + ". Error: " + raw_processor->strerror(result));
    }

    result = raw_processor->dcraw_process();
    if (LIBRAW_SUCCESS!=result){
        throw std::runtime_error(std::string("Can't process raw file ") + file  + ". Error: " + raw_processor->strerror(result));
    }

    libraw_processed_image_t *processed_image = raw_processor->dcraw_make_mem_image(&result);

    if (LIBRAW_SUCCESS!=result){
        throw std::runtime_error(std::string("Can't process raw file ") + file  + ". Error: " + raw_processor->strerror(result));
    }

    MagickCore::StorageType bpp;
    if (processed_image->bits == 8)
        bpp = Magick::CharPixel;
    else if (processed_image->bits == 16)
        bpp = Magick::ShortPixel;
    else
        throw std::runtime_error(std::string("Unsupported bits per fixel for ") + file);

    image->read( processed_image->width, processed_image->height, "RGB", bpp, processed_image->data);

    raw_processor->dcraw_clear_mem(processed_image);
    raw_processor->recycle();
}

void Image::delete_data()
{
    if (image)
    {
        delete image;
    }
    if (raw_processor)
    {
        raw_processor->recycle();
        delete raw_processor;
    }
}

}

