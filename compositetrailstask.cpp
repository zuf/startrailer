#include "compositetrailstask.h"
#include "startrailer.h"

CompositeTrailsTask::~CompositeTrailsTask()
{
    if (m_out_image)
        delete m_out_image;
}

void CompositeTrailsTask::run()
{
    int counter = 0;
    StarTrailer st;
    foreach (const QString &source, m_sourceFiles) {
        if (*m_stopped)
            return;
        // open source image
        Magick::Image *image = new Magick::Image(source.toStdString());

        if (*m_stopped)
            return;
        // composite image
        m_out_image->composite(*image, 0,0, Magick::LightenCompositeOp);

        ++counter;

        delete image;

        // set progress
        QMetaObject::invokeMethod(m_receiver, "announceProgress", Qt::QueuedConnection, Q_ARG(int, counter));
    }
    QMetaObject::invokeMethod(m_receiver, "sendMagickImage", Qt::QueuedConnection, Q_ARG(Magick::Image, Magick::Image(*m_out_image)));
}
