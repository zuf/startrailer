#include "startrailer.h"


StarTrailer::StarTrailer()
{    
}

//void StarTrailer::compose(Image::Image &one, Image::Image &another, CompositeOperator method)
//{

//}

const QByteArray *StarTrailer::compose(const std::string &image_one_path, const std::string &image_another_path)
{
    Magick::Image *one = new Magick::Image(image_one_path);
    Magick::Image *another = new Magick::Image(image_another_path);
    one->composite(*another, 0,0, Magick::LightenCompositeOp);
    delete another;
    one->magick("BMP");
    Magick::Blob *blob = new Magick::Blob();
    one->write(blob);
    delete one;
    const QByteArray *imageData = new QByteArray((char*)(blob->data()), blob->length());
    delete blob;

    return imageData;
}
