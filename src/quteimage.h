#ifndef QUTEIMAGE_H
#define QUTEIMAGE_H

#include "image.h"

///
/// Inherited from Image class with some Qt additions (export to QByteArray, etc)
///
///

class QuteImage : public StarTrailer::Image
{
public:
    QuteImage();
};

#endif // QUTEIMAGE_H
