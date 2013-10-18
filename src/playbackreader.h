#ifndef PLAYBACKREADER_H
#define PLAYBACKREADER_H

#include <QRunnable>
#include <QQueue>
#include <QMutex>
#include <Magick++.h>
#include <QStringList>
#include "startrailer.h"

class PlaybackReader : public QRunnable
{
public:
    explicit PlaybackReader(QQueue<const QByteArray *> *the_queue, QMutex *the_mutex, const QStringList &the_files):
        queue(the_queue),
        mutex(the_mutex),
        files(the_files)
    {
        st = new StarTrailer();
    }

    virtual ~PlaybackReader();

private:
    void run();

    QMutex *mutex;
    QQueue<const QByteArray *> *queue;
    const QStringList files;
    StarTrailer *st;
};

#endif // PLAYBACKREADER_H
