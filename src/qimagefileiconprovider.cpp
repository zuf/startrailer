#include "qimagefileiconprovider.h"
#include <QDebug>

QImageFileIconProvider::QImageFileIconProvider() : QFileIconProvider() {
    exif_loader = exif_loader_new();
}


QImageFileIconProvider::~QImageFileIconProvider() {
    if (exif_loader) {
        exif_loader_unref(exif_loader);
        exif_loader = 0;
    }

}


QIcon QImageFileIconProvider::icon(const QFileInfo &info) const
{
    const QString &file_path = info.absoluteFilePath();

    //return QFileIconProvider::icon(info);

    if (!file_path.isEmpty() && info.isFile()) {
        if (QFile(file_path).exists()) {            
            exif_loader_write_file(exif_loader, file_path.toStdString().c_str());
            ExifData *ed = exif_loader_get_data(exif_loader);

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

    return QFileIconProvider::icon(info);
}
