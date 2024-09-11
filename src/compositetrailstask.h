#ifndef COMPOSITETRAILSTASK_H
#define COMPOSITETRAILSTASK_H

#include <Magick++.h>
#include <QObject>
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
//                                 StarTrailer::Image *out,
                                 StarTrailer::Image *preview,
                                 const Magick::CompositeOperator compose_op=Magick::LightenCompositeOp,
                                 const StarTrailer::Image::RawProcessingMode raw_mode=StarTrailer::Image::FullPreview,
                                 const StarTrailer::Image::JPEGProcessingMode jpeg_mode=StarTrailer::Image::FullJpeg)

        : m_receiver(receiver),
          m_stopped(stopped),
          m_sourceFiles(files),
          m_preview_each_n_ms(preview_each_n_ms),
          m_task_index(task_index),
          m_tasks_count(tasks_count),
          m_mutex(mutex),
          m_preview_image(preview),
//          m_out_image(out),
          m_compose_op(compose_op),
        m_raw_processing_mode(raw_mode),
        m_jpeg_processing_mode(jpeg_mode)
    {
        //m_out_image(new Magick::Image(files.first().toStdString()))
        *m_stopped=false;
//        Q_ASSERT(m_out_image);
        Q_ASSERT(m_preview_image);
        m_tmp_image = new StarTrailer::Image();
    }

//    StarTrailer::Image const *getImage()const{ return m_out_image;}

    virtual ~CompositeTrailsTask();

private:
    void run();

    QObject *m_receiver;
    volatile bool *m_stopped;
    const QStringList m_sourceFiles;
    const int m_preview_each_n_ms;
    const int m_task_index;
    const int m_tasks_count;
    QMutex *m_mutex;
    StarTrailer::Image * m_preview_image = 0;
//    StarTrailer::Image * m_out_image = 0;
    StarTrailer::Image * m_tmp_image = 0;
    const Magick::CompositeOperator m_compose_op;
    StarTrailer::Image::RawProcessingMode m_raw_processing_mode = StarTrailer::Image::FullPreview;
    StarTrailer::Image::JPEGProcessingMode m_jpeg_processing_mode = StarTrailer::Image::FullJpeg;
};

#endif // COMPOSITETRAILSTASK_H
