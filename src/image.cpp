#include "image.h"
#include <cassert>

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

void Image::read(const std::string &file, RawProcessingMode raw_processing_mode)
{    
    assert(raw_processor != 0);    
    if (LIBRAW_SUCCESS==raw_processor->open_file(file.c_str()))
    {
        switch(raw_processing_mode)
        {
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
            throw std::runtime_error("Read RAW with mode TinyPreview not yet implemented");
            break;
        case SmallPreview:
            throw std::runtime_error("Read RAW with mode SmallPreview not yet implemented");
            break;
        default:
            throw std::runtime_error("Unknown raw_processing_mode");
        }
    }
    else
    {
        read_with_image_magick(file);
    }

//    if (raw_processor)
//    {
//        delete raw_processor;
//        raw_processor=0;
//    }
}

void Image::write(const std::string &new_file)
{
    assert(image != 0);
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

void Image::init()
{
    image=new Magick::Image();
    raw_processor = new LibRaw();
}

void Image::read_with_image_magick(const std::string &file)
{
    assert(image != 0);
    image->read(file);
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

    raw_processor->imgdata.params.half_size = half_size;

    int result = raw_processor->unpack();
    if (LIBRAW_SUCCESS!=result){
        throw std::runtime_error(std::string("Can't unpack raw file ") + file  + ". Error: " + raw_processor->strerror(result));
    }

    result = raw_processor->dcraw_process();
    if (LIBRAW_SUCCESS!=result){
        throw std::runtime_error(std::string("Can't process raw file ") + file  + ". Error: " + raw_processor->strerror(result));
    }

    raw_processor->recycle();
}

