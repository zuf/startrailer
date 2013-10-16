#ifndef IMAGE_H
#define IMAGE_H

#include <Magick++.h>
#include "libraw/libraw.h"

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
///  img6 = new Image(buffer_ptr, buffer_size); // load blob from memory (blob is content of file), guess format with ImageMagick
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

class Image
{
public:
    Image();
    virtual ~Image();

    void compose(const Image &with_image);
    void compose(const Image *with_image);

private:
    Magick::Image image;
    LibRaw *raw_processor;
};

#endif // IMAGE_H
