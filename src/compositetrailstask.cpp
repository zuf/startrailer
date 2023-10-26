#include "compositetrailstask.h"

#include <QDebug>
#include <QElapsedTimer>

#include "libraw/libraw.h"

CompositeTrailsTask::~CompositeTrailsTask()
{
    delete m_tmp_image;
//    delete m_out_image;
//    m_out_image=0;
}


void CompositeTrailsTask::run()
{
    qDebug() << "CompositeTrailsTask::run() starts";
    if (*m_stopped)
        return;

    //m_tmp_image->read(m_sourceFiles.first().toStdString());
    StarTrailer::Image image;

    if (*m_stopped)
        return;

    QElapsedTimer timer;
    timer.start();


    int counter = 0;
    qint64 redraw_each_ms = m_preview_each_n_ms + m_task_index * m_preview_each_n_ms;
    for (int i = 0; i < m_sourceFiles.size(); ++i)
    {
        if (*m_stopped) return;

        std::string s = m_sourceFiles[i].toStdString();
        try {
         image.read(s, m_raw_processing_mode);
        } catch (std::runtime_error &e) {
         qDebug() << "Can't read file: " << m_sourceFiles[i] << "\n" << e.what();
         continue;
        }

        if (m_tmp_image->height() == 0 && m_tmp_image->width() == 0) {
         m_mutex->lock();
         delete m_tmp_image;
         m_tmp_image = new StarTrailer::Image(image);
         m_mutex->unlock();
        }

        if (m_preview_image->height() == 0 && m_preview_image->width() == 0) {
         m_mutex->lock();
         delete m_preview_image;
         m_preview_image = new StarTrailer::Image(image);
         m_mutex->unlock();
        }

        if (*m_stopped) return;

        m_tmp_image->composite(image, m_compose_op);

        ++counter;

        if (*m_stopped) return;

        // set progress
        QMetaObject::invokeMethod(m_receiver, "announceProgress", Qt::QueuedConnection);
        if (m_preview_each_n_ms>0 && timer.elapsed() > redraw_each_ms)
        {
            timer.restart();
            if (m_mutex->tryLock())
            {
                Q_ASSERT(m_tmp_image);
                StarTrailer::Image *resized_img = new StarTrailer::Image(*m_tmp_image);
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
        qDebug() << "Tmp WxH = " << m_tmp_image->width() << "x" << m_tmp_image->height();

//        if (m_out_image->height() == 0 && m_out_image->width() == 0) {
//            //delete m_out_image;
//            m_out_image = new StarTrailer::Image(*m_tmp_image);
//        } else {
//            m_out_image->composite(*m_tmp_image, m_compose_op);
//        }

        if (m_tmp_image->height() == m_preview_image->height() &&
            m_tmp_image->width() == m_preview_image->width()) {
            m_preview_image->composite(*m_tmp_image, m_compose_op);
        } else {
            StarTrailer::Image *resized_img = new StarTrailer::Image(*m_tmp_image);
            resized_img->resample(m_preview_image->width(),
                                  m_preview_image->height());
            m_preview_image->composite(*resized_img, m_compose_op);
            delete resized_img;
        }

        qDebug() << "Tmp WxH = " << m_tmp_image->width() << "x" << m_tmp_image->height();

        QMetaObject::invokeMethod(m_receiver, "redrawPreview", Qt::QueuedConnection, Q_ARG(bool, true));
        m_mutex->unlock();
    }

    QMetaObject::invokeMethod(m_receiver, "composingFinished", Qt::QueuedConnection);
    qDebug() << "CompositeTrailsTask::run() ended";
}
