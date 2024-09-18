#include "image.h"
#include <cassert>
#include <cstddef>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <filesystem> // C++17
#include <algorithm>
#include <string>

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
    empty = false;
}

Image::Image(const std::string &file, Image::RawProcessingMode raw_processing_mode, Image::JPEGProcessingMode jpeg_processing_mode)
{
    init();
    read(file, raw_processing_mode, jpeg_processing_mode);
}

Image::Image(const Magick::Image &from_image)
{
    raw_processor = new LibRaw();
    image = new Magick::Image(from_image);
    empty = false;
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

void Image::read(const std::string &file, RawProcessingMode raw_processing_mode, JPEGProcessingMode jpeg_processing_mode) {
    const std::lock_guard<std::mutex> lock(image_mutex);
    std::string extension = file.substr(file.find_last_of(".") + 1);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::toupper);

    if (jpeg_processing_mode == PreviewJPEG && (extension == "JPG" || extension == "JPEG" || extension == "JPE")) {
        try {
            read_preview_from_jpeg(file);
            return;
        } catch (const std::runtime_error &e) {
            // do nothing
        }
    }

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
    const std::lock_guard<std::mutex> lock(image_mutex);
    assert(image != 0);
    image->quality(DEFAULT_JPEG_QUALITY);
    image->write(new_file);    
}

size_t Image::to_buffer(void * &write_to_ptr)
{
    const std::lock_guard<std::mutex> lock(image_mutex);
    const Magick::StorageType storage_type=Magick::CharPixel;    
    image->write(0, 0, image->columns(), image->rows(), "RGB", storage_type, write_to_ptr);
    return 3*image->columns()*image->rows();
}

void Image::composite(const Image &with_image, Magick::CompositeOperator mode)
{
    const std::lock_guard<std::mutex> lock(image_mutex);
    image->composite(*(with_image.get_magick_image()), 0, 0, mode);
}

void Image::reset()
{
    const std::lock_guard<std::mutex> lock(reset_mutex);
    delete_data();
    init();
}

void Image::init()
{
    const std::lock_guard<std::mutex> lock(image_mutex);

    image=new Magick::Image();
    image->quiet( false );
    empty = true;
    raw_processor = new LibRaw();
}

void Image::read_with_image_magick(const std::string &file)
{
    assert(image != 0);
    try
    {
        image->read(file);
        empty = false;
    }
    catch(Magick::Error &error)
    {
       throw std::runtime_error(std::string("Can't open file: ") + file + std::string("GraphicsMagick reports: ") + error.what());
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

        // How no avoid double free (via blob destructor and libraw's resycle method?)
        // Magick::Blob blob;//(buf, preview_length);
        // blob.updateNoCopy(raw_processor->imgdata.thumbnail.thumb, raw_processor->imgdata.thumbnail.tlength, Magick::Blob::MallocAllocator);

        image->read(blob);
        empty = false;
    }
        break;

    case LIBRAW_THUMBNAIL_BITMAP:
    {
        image->read( raw_processor->imgdata.thumbnail.twidth, raw_processor->imgdata.thumbnail.theight, "RGB", Magick::CharPixel, raw_processor->imgdata.thumbnail.thumb);
        empty = false;
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

void Image::read_preview_from_jpeg(const std::string &file)
{
    assert(raw_processor != 0);
    struct stat statbuf;

    if (stat(file.c_str(), &statbuf) == -1)
    {
        throw std::runtime_error("failed to stat!");
    }

    FILE *f = fopen(file.c_str(), "rb");
    if (f == NULL) {
        throw std::runtime_error("failed open file!");
    }

    int c=-1;
    int prev=-1;
    bool in_image = 0;
    size_t from=0;
    size_t to=0;

    const int max_preview_length = 1024*1024;
    fseek(f, statbuf.st_size - max_preview_length, SEEK_SET);

    while ((c = fgetc(f)) != EOF){
        if (prev == 0xFF) {
            if (in_image == 0 && c == 0xD8) {
                in_image = 1;
                from = ftell(f)-2;
            } else {
                if (in_image && c == 0xD9) {
                    to = ftell(f);
                    in_image = 0;
                }
            }
        }

        prev = c;
    };

    size_t preview_length = to - from;
    if (preview_length == 0) {
        fclose(f);
        throw std::runtime_error("jpeg preview not found");
    }

    fseek(f, from, SEEK_SET);
    void *buf = malloc(preview_length);
    if (buf == nullptr) {
        throw std::runtime_error("Can't allocate buffer!");

    }
    size_t result = fread(buf, 1, preview_length, f);
    if (result != preview_length) {
        free(buf);
        throw std::runtime_error("File reading error!");
    }
    fclose(f);

    Magick::Blob blob;//(buf, preview_length);
    blob.updateNoCopy(buf, preview_length, Magick::Blob::MallocAllocator);
    image->read(blob);
    empty = false;

    //free(buf); // should no free when updateNoCopy() from Magick::Blob used
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

    Magick::StorageType bpp;
    if (processed_image->bits == 8)
        bpp = Magick::CharPixel;
    else if (processed_image->bits == 16)
        bpp = Magick::ShortPixel;
    else
        throw std::runtime_error(std::string("Unsupported bits per fixel for ") + file);

    image->read( processed_image->width, processed_image->height, "RGB", bpp, processed_image->data);
    empty = false;

    raw_processor->dcraw_clear_mem(processed_image);
    raw_processor->recycle();
}

void Image::delete_data()
{
    const std::lock_guard<std::mutex> lock(image_mutex);

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

