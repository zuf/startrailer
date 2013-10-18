#include "playbackreader.h"


PlaybackReader::~PlaybackReader()
{
    delete st;
}

void PlaybackReader::run()
{
    QString file;
    Magick::Image *image;
    foreach(file, files){
        image = st->read_image(file.toStdString());
        const QByteArray * bytes = st->image_to_qbyte_array(image);

        // TODO: control size of queue

        mutex->lock();
        queue->enqueue(bytes);
        mutex->unlock();
    }
}
