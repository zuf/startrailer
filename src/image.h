#ifndef IMAGE_H
#define IMAGE_H

#include <Magick++.h>
#include "libraw/libraw.h"
//#include <iostream>
#include <string>
#include <stdexcept>
#include <cassert>
#include <mutex>

///
/// \brief Image class provides interface to composite different images
///
///  Internally stored as Magick::Image object?
///
///  Planned usage:
///
///  // Load from files
///  img1 = new Image('image.jpg'); // load from file with image magick
///  img2 = new Image('photo.cr2'); // load from file with libraw (preview) and image magick
///  img3 = new Image('photo.cr2', Image::ProcessingMode::HalfRaw); // read raw with libraw (very basic raw processing)
///  img4 = new Image('photo.cr2', Image::ProcessingMode::FullRaw);
///  img5 = new Image('photo.cr2', Image::ProcessingMode::Preview);  // same as img2
///
///  // From memory
///  img6 = new Image(buffer_ptr, buffer_size); // load blob from memory (blob is content of file), guess format with GraphicsMagick
///  img7 = new Image(buffer_ptr, buffer_size, Image::Format::RGB24);
///
///  // From other objects
///  img8 = new Image(Magick::Image('file.jpg'));
///
///  Image img;
///  img.read('file.png'); // and other variants like constructors
///
///  // Composing
///  img2.composite(img3); // compose img2 with img3, result stored in img2
///  img2.composite(img3, Image::ComposeMethods::Lighten);
///  img.composite(Magick::Image('pic.tga'));
///
///  // Composite images and produce new one
///  new_image = Image::composite(img1, img2); // img1 and img2 is untouched
///
///  // Exporting
///  img1.write('image.bmp');
///  buffer_size = img2.to_buffer(buffer_ptr);
///

namespace StarTrailer {

class ReadError : public std::runtime_error
{
public:
    explicit ReadError ( const std::string& what_ );
    ~ReadError() throw ();
};

class Image
{
public:
    enum RawProcessingMode {UndefinedRawProcessingMode, HalfRaw, FullRaw, TinyPreview, SmallPreview, FullPreview};
    enum JPEGProcessingMode {UndefinedJPEGProcessingMode, FullJpeg, PreviewJPEG};

    Image();
    Image(const std::string &file, RawProcessingMode raw_processing_mode=FullPreview, JPEGProcessingMode jpeg_processing_mode=FullJpeg);
    Image(const Magick::Image &image);
    Image(const Image &from_image);
    Image & operator = (const Image & other)
    {
        if (this != &other)
        {
            if (image != NULL)
                delete image;
            image = new Magick::Image(*other.image);
        }
        return *this;
    }


    virtual ~Image();

    void read(const std::string &file, RawProcessingMode raw_processing_mode=FullPreview, JPEGProcessingMode jpeg_processing_mode=FullJpeg);
    void write(const std::string &new_file);

    size_t to_buffer(void * &write_to_ptr);

    void composite(const Image &with_image, Magick::CompositeOperator mode = Magick::LightenCompositeOp);
    //void composite(const Image *with_image, Magick::CompositeOperator mode = Magick::LightenCompositeOp);


    size_t width() const {
        assert(image != 0);

        return image->columns();
    }

    size_t height() const {
        assert(image != 0);

        return image->rows();
    }

    void resample(int wigth, int height){
        Magick::Geometry g(wigth,height);
        image->sample(g);
    }

    friend bool operator==(const Image &left, const Image &right){
        try {
            if (left.image->rows() == 0 || right.image->rows() == 0)
            {
                return true;
            }
            else
            {
                left.image->compare(*(right.image));
                return 0.0 == left.image->meanErrorPerPixel();
            }
        }
        catch(Magick::ErrorImage const&){
            return false;
        }
        catch(Magick::ErrorOption const&){
            return false;
        }
    }

    friend bool operator!=(const Image &left, const Image &right){
        try{
            if (left.image->rows() == 0 || right.image->rows() == 0)
            {
                return true;
            }
            else
            {
                left.image->compare(*(right.image));
                return 0.0 != left.image->meanErrorPerPixel();
            }
        }
        catch(Magick::ErrorImage const&){
            return true;
        }
        catch(Magick::ErrorOption const&){
            return true;
        }
    }

    const Magick::Image *get_magick_image() const {
        return image;
    }
    void reset();
private:


    void init();

    void read_with_image_magick(const std::string &file);
    void read_preview_with_libraw(const std::string &file);
    void read_preview_from_jpeg(const std::string &file);
    void read_raw_with_libraw(const std::string &file, const bool half_size=false);

    void delete_data();

    Magick::Image *image;
    LibRaw *raw_processor;
    std::mutex image_mutex;
    std::mutex reset_mutex;
};

}
#endif // IMAGE_H
