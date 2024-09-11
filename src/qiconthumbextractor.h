#ifndef QICONTHUMBEXTRACTOR_H
#define QICONTHUMBEXTRACTOR_H
#include <libexif/exif-loader.h>
#include <QFileInfo>
#include <QIcon>
#include <QFileIconProvider>
#include "libraw/libraw.h"

class QIconThumbExtractor
{
public:
    QIconThumbExtractor();
    virtual ~QIconThumbExtractor();

    QIcon icon(const QFileInfo &info) const;
    QIcon icon(const QString &path) const;

protected:
    ExifLoader *exif_loader=0;
    QFileIconProvider icon_provider;
    LibRaw *raw_processor=0;
};

#endif // QICONTHUMBEXTRACTOR_H
