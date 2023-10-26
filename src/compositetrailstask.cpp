#include "compositetrailstask.h"

#include <QDebug>
#include <QElapsedTimer>

#include "libraw/libraw.h"

CompositeTrailsTask::~CompositeTrailsTask()
{
    delete m_out_image;
    m_out_image=0;
}


void CompositeTrailsTask::run()
{
    qDebug() << "CompositeTrailsTask::run() starts";
    if (*m_stopped)
        return;

    m_out_image->read(m_sourceFiles.first().toStdString());
    StarTrailer::Image image;

    if (*m_stopped)
        return;

    QElapsedTimer timer;
    timer.start();


    int counter = 0;
    qint64 redraw_each_ms = m_preview_each_n_ms + m_task_index * m_preview_each_n_ms;
    for (int i = 1; i < m_sourceFiles.size(); ++i)
    {
        if (*m_stopped) return;

        std::string s = m_sourceFiles[i].toStdString();
        image.read(s, m_raw_processing_mode);

        if (*m_stopped) return;

        m_out_image->composite(image, m_compose_op);

        ++counter;

        if (*m_stopped) return;

        // set progress
        QMetaObject::invokeMethod(m_receiver, "announceProgress", Qt::QueuedConnection);
        if (m_preview_each_n_ms>0 && timer.elapsed() > redraw_each_ms)
        {
            timer.restart();
            if (m_mutex->tryLock())
            {
                StarTrailer::Image *resized_img = new StarTrailer::Image(*m_out_image);
                resized_img->resample(m_preview_image->width(), m_preview_image->height());
                m_preview_image->composite(*resized_img, m_compose_op);
                delete resized_img;
                QMetaObject::invokeMethod(m_receiver, "redrawPreview", Qt::QueuedConnection);
                m_mutex->unlock();
            }
        }

    }

    if (counter > 0)
    {
        m_mutex->lock();

        qDebug() << "preview WxH = " << m_preview_image->width() << "x" << m_preview_image->height();
        qDebug() << "Out WxH = " << m_out_image->width() << "x" << m_out_image->height();

        StarTrailer::Image *resized_img = new StarTrailer::Image(*m_out_image);
        resized_img->resample(m_preview_image->width(), m_preview_image->height());
        m_preview_image->composite(*resized_img, m_compose_op);
        delete resized_img;

        qDebug() << "Out WxH = " << m_out_image->width() << "x" << m_out_image->height();

        QMetaObject::invokeMethod(m_receiver, "redrawPreview", Qt::QueuedConnection, Q_ARG(bool, true));
        m_mutex->unlock();
    }

    QMetaObject::invokeMethod(m_receiver, "composingFinished", Qt::QueuedConnection);
    qDebug() << "CompositeTrailsTask::run() ended";
}
