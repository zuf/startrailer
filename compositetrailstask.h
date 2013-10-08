#ifndef COMPOSITETRAILSTASK_H
#define COMPOSITETRAILSTASK_H

#include <Magick++.h>
#include <QRunnable>
#include <QStringList>
#include "startrailer.h"

class CompositeTrailsTask : public QRunnable
{
public:
    explicit CompositeTrailsTask(QObject *receiver,
                                 volatile bool *stopped, const QStringList &files, const int preview_each_n_image)
        : m_receiver(receiver), m_stopped(stopped),
          m_sourceFiles(files), m_preview_each_n_image(preview_each_n_image)
    {
        //m_out_image(new Magick::Image(files.first().toStdString()))
        *m_stopped=false;        
    }

    Magick::Image const *getImage(){ return m_out_image;}

    virtual ~CompositeTrailsTask();

private:
    void run();

    const int m_preview_each_n_image;
    QObject *m_receiver;
    volatile bool *m_stopped;
    const QStringList m_sourceFiles;
    Magick::Image * m_out_image;
    StarTrailer *st;
};

#endif // COMPOSITETRAILSTASK_H
