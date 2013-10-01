#ifndef STARTRAILER_H
#define STARTRAILER_H
#include <Magick++.h>
#include <QByteArray>

class StarTrailer
{
public:
    StarTrailer();
//    void compose(const Image::Image &one, const Image::Image &another, CompositeOperator method);
    const QByteArray *compose(const std::string &image_one_path, const std::string &image_another_path);
};

#endif // STARTRAILER_H
