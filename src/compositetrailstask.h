#ifndef COMPOSITETRAILSTASK_H
#define COMPOSITETRAILSTASK_H

#include <Magick++.h>
#include <QRunnable>
#include <QMutex>
#include <QStringList>
#include "image.h"

class CompositeTrailsTask : public QRunnable
{
public:
    explicit CompositeTrailsTask(QObject *receiver,
                                 volatile bool *stopped,
                                 const QStringList &files,
                                 const int preview_each_n_ms,
                                 const int task_index,
                                 const int tasks_count,
                                 QMutex *mutex,
                                 Image *preview,
                                 const Magick::CompositeOperator compose_op=Magick::LightenCompositeOp)

        : m_receiver(receiver),
          m_stopped(stopped),
          m_sourceFiles(files),
          m_preview_each_n_ms(preview_each_n_ms),
          m_task_index(task_index),
          m_tasks_count(tasks_count),
          m_mutex(mutex),
          m_preview_image(preview),
          m_compose_op(compose_op)
    {
        //m_out_image(new Magick::Image(files.first().toStdString()))
        *m_stopped=false;
        m_out_image = new Image();
    }

    Image const *getImage()const{ return m_out_image;}

    virtual ~CompositeTrailsTask();

private:
    void run();

    const int m_preview_each_n_ms;
    QObject *m_receiver;
    volatile bool *m_stopped;
    const QStringList m_sourceFiles;
    Image * m_out_image;
    QMutex *m_mutex;
    Image * m_preview_image;
    const Magick::CompositeOperator m_compose_op;
    const int m_task_index;
    const int m_tasks_count;
};

#endif // COMPOSITETRAILSTASK_H
