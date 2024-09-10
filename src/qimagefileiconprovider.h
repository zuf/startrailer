#ifndef QIMAGEFILEICONPROVIDER_H
#define QIMAGEFILEICONPROVIDER_H

#include <QFileIconProvider>
#include <libexif/exif-loader.h>

class QImageFileIconProvider : public QFileIconProvider
{
public:
    QImageFileIconProvider();
    QIcon icon(const QFileInfo &info) const;
    virtual ~QImageFileIconProvider();

protected:
    ExifLoader *exif_loader=0;
};

#endif // QIMAGEFILEICONPROVIDER_H
