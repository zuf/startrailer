#include "compositetrailstask.h"
#include "startrailer.h"

#include <QDebug>

CompositeTrailsTask::~CompositeTrailsTask()
{
//    if (m_out_image)
//        delete m_out_image;
}


void CompositeTrailsTask::run()
{
    qDebug() << "CompositeTrailsTask::run() starts";
    if (*m_stopped)
        return;

    int counter = 0;    
    StarTrailer st;
    foreach (const QString &source, m_sourceFiles) {
        if (*m_stopped)
            return;
        // open source image
        Magick::Image image(source.toStdString());

        if (*m_stopped)
            return;
        // composite image
        m_out_image->composite(image, 0, 0, Magick::LightenCompositeOp);

        ++counter;

        if (*m_stopped)
            return;

        // set progress
        QMetaObject::invokeMethod(m_receiver, "announceProgress", Qt::QueuedConnection, Q_ARG(int, counter));        
        if (counter%5==0)
            QMetaObject::invokeMethod(m_receiver, "receiveMagickImage", Qt::QueuedConnection, Q_ARG(Magick::Image*, (new Magick::Image(*m_out_image))));
    }

    if (counter > 0)
        QMetaObject::invokeMethod(m_receiver, "receiveMagickImage", Qt::QueuedConnection, Q_ARG(Magick::Image*, m_out_image));

    QMetaObject::invokeMethod(m_receiver, "composingFinished", Qt::QueuedConnection);
    qDebug() << "CompositeTrailsTask::run() ended";
}
