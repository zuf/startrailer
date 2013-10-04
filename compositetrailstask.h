#ifndef COMPOSITETRAILSTASK_H
#define COMPOSITETRAILSTASK_H

#include <Magick++.h>
#include <QRunnable>
#include <QStringList>

class CompositeTrailsTask : public QRunnable
{
public:
    explicit CompositeTrailsTask(QObject *receiver,
                                 volatile bool *stopped, const QStringList &files)
        : m_receiver(receiver), m_stopped(stopped),
          m_sourceFiles(files), m_out_image(new Magick::Image(files.first().toStdString()))
    {*m_stopped=false;}

    Magick::Image const *getImage(){ return m_out_image;}

    virtual ~CompositeTrailsTask();

private:
    void run();

    QObject *m_receiver;
    volatile bool *m_stopped;
    const QStringList m_sourceFiles;
    Magick::Image * m_out_image;
};

#endif // COMPOSITETRAILSTASK_H
