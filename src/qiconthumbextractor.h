#ifndef QICONTHUMBEXTRACTOR_H
#define QICONTHUMBEXTRACTOR_H
#include <libexif/exif-loader.h>
#include <QFileInfo>
#include <QIcon>
#include <QFileIconProvider>

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
};

#endif // QICONTHUMBEXTRACTOR_H
