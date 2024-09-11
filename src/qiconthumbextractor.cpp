#include "qiconthumbextractor.h"
#include <QDebug>

QIconThumbExtractor::QIconThumbExtractor() {
    exif_loader = exif_loader_new();
    raw_processor = new LibRaw();
}


QIconThumbExtractor::~QIconThumbExtractor() {
    if (exif_loader) {
        exif_loader_unref(exif_loader);
        exif_loader = 0;
    }

    if(raw_processor) {
        raw_processor->recycle();
        delete raw_processor;
    }
}


QIcon QIconThumbExtractor::icon(const QFileInfo &info) const {
    const QString &file_path = info.absoluteFilePath();

    //return icon_provider.icon(info);

    if (!file_path.isEmpty() && info.isFile()) {
        if (info.exists()) {            
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
                if (p.isNull()) {
                    return QIcon();
                } else {
                    return QIcon(p);
                }
            } else {
                // try to fetch thumbnail with libraw
                if (LIBRAW_SUCCESS == raw_processor->open_file(file_path.toStdString().c_str())) {
                    qDebug() << "Try to read EXIF thumbnail with libraw from file " << file_path;
                    if (raw_processor->imgdata.thumbs_list.thumbcount <= 0) {
                        raw_processor->recycle();
                        return QIcon();
                    }

                    const int maxSize = 6000*4000;
                    int min_area = maxSize;
                    int min_index = -1;
                    for(int i=0; i<raw_processor->imgdata.thumbs_list.thumbcount;i++) {
                        int area = raw_processor->imgdata.thumbs_list.thumblist[i].twidth * raw_processor->imgdata.thumbs_list.thumblist[i].theight;
                        if (area < min_area) {
                            min_area = area;
                            min_index = i;
                        }
                    }
                    if (min_index < 0) {
                        raw_processor->recycle();
                        return QIcon();
                    }

                    qDebug() << "Loading thumbnail number " << min_index << " with size " << raw_processor->imgdata.thumbs_list.thumblist[min_index].twidth << "x" << raw_processor->imgdata.thumbs_list.thumblist[min_index].theight;

                    int result = raw_processor->unpack_thumb_ex(min_index);
                    if (LIBRAW_SUCCESS == result) {
                        QPixmap p;
                        p.loadFromData((uchar*)raw_processor->imgdata.thumbnail.thumb, raw_processor->imgdata.thumbnail.tlength);
                        raw_processor->recycle();
                        if (p.isNull()) {
                            return QIcon();
                        } else {
                            return QIcon(p);
                        }
                    }
                }
            }
        }
    }

    return QIcon(); //icon_provider.icon(info);
}

QIcon QIconThumbExtractor::icon(const QString &path) const{
    return icon(QFileInfo(path));
}

