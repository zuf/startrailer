#include "image.h"

Image::Image()
{
    init();
}

Image::Image(const std::string &file, Image::RawProcessingMode raw_processing_mode)
{
    init();
    read(file, raw_processing_mode);
}

Image::~Image()
{
    if (image)
    {
        delete image;
    }
    if (raw_processor)
    {
        delete raw_processor;
    }
}

void Image::read(const std::string &file, RawProcessingMode raw_processing_mode)
{
    if (raw_processor==0)
        raw_processor = new LibRaw();

    if (LIBRAW_SUCCESS==raw_processor->open_file(file.c_str()))
    {
        // STUB: Now only loads full sized previews from RAW
        if (FullPreview!=raw_processing_mode)
        {
          throw std::runtime_error("Unsupported RawProcessingMode");
        }

        int result = raw_processor->unpack_thumb();

        if (LIBRAW_SUCCESS!=result)
        {
            throw std::runtime_error(std::string("Can't unpack thumbnail for file ") + file  + ". Error: " + raw_processor->strerror(result));
        }

        switch(raw_processor->imgdata.thumbnail.tformat){
        case LIBRAW_THUMBNAIL_JPEG:
        {
            // memory copied here
            Magick::Blob blob(raw_processor->imgdata.thumbnail.thumb, raw_processor->imgdata.thumbnail.tlength);
            image->read(blob);
        }
            break;

        case LIBRAW_THUMBNAIL_BITMAP:
            // TODO: read this bitmaps (RGB-8 format)
            throw std::runtime_error("Unsupported thumbnail format. Please add issue to my github. LibRaw reports: LIBRAW_THUMBNAIL_BITMAP");
            break;

        case LIBRAW_THUMBNAIL_LAYER:
            throw std::runtime_error("Unsupported thumbnail format. LibRaw reports: LIBRAW_THUMBNAIL_LAYER");
            break;

        case LIBRAW_THUMBNAIL_UNKNOWN:
            throw std::runtime_error("Unsupported or unknown thumbnail format. LibRaw reports: LIBRAW_THUMBNAIL_UNKNOWN");
            break;

        default:
            throw std::runtime_error("Thumbnail error: Unholy error!"); // my favorite :)
        }

        raw_processor->recycle();
    }
    else
    {
        image->read(file);
    }
}

void Image::write(const std::string &new_file)
{
    image->write(new_file);
}

void Image::init()
{
    image=new Magick::Image();
    raw_processor=0;
}

