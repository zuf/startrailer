#include "qiconthumbextractor.h"
#include <QDebug>

QIconThumbExtractor::QIconThumbExtractor() {
    exif_loader = exif_loader_new();
}


QIconThumbExtractor::~QIconThumbExtractor() {
    if (exif_loader) {
        exif_loader_unref(exif_loader);
        exif_loader = 0;
    }
}


QIcon QIconThumbExtractor::icon(const QFileInfo &info) const {
    const QString &file_path = info.absoluteFilePath();

    //return icon_provider.icon(info);

    if (!file_path.isEmpty() && info.isFile()) {
        if (QFile(file_path).exists()) {
            ExifData *ed = exif_loader_get_data(exif_loader);
            exif_loader_write_file(exif_loader, file_path.toStdString().c_str());
            ed = exif_loader_get_data(exif_loader);

            if (ed) {
                QPixmap p;
                if (ed->data && ed->size) {
                    p.loadFromData(ed->data, ed->size);
                } else {
                    qDebug() << "No EXIF thumbnail in file " << file_path;
                    // return QIcon(file_path); // try to make icon from whole image
                }
                exif_data_unref(ed);
                return QIcon(p);
            }
        }
    }

    return QIcon(); //icon_provider.icon(info);
}

QIcon QIconThumbExtractor::icon(const QString &path) const{
    return icon(QFileInfo(path));
}
