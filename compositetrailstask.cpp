#include "compositetrailstask.h"
#include "startrailer.h"

#include <QDebug>
#include <QMimeData>
#include <QMimeDatabase>
#include <QMimeType>
#include <QElapsedTimer>

#include "libraw/libraw.h"

CompositeTrailsTask::~CompositeTrailsTask()
{
//    if (m_out_image)
//        delete m_out_image;
    delete st;
}


void CompositeTrailsTask::run()
{
    qDebug() << "CompositeTrailsTask::run() starts";
    if (*m_stopped)
        return;

    st = new StarTrailer();
    m_out_image = st->read_image(m_sourceFiles.first().toStdString());

    if (*m_stopped)
        return;

    int counter = 0;        
//    QMimeDatabase mimeDatabase;
//    QMimeType mimeType;
//    LibRaw iProcessor;
    foreach (const QString &source, m_sourceFiles) {
        QElapsedTimer timer;
        timer.start();
        if (*m_stopped)
            return;

        Magick::Image *image = st->read_image(source.toStdString());

        if (*m_stopped)
            return;
        // composite image
        //m_out_image->composite(image, 0, 0, Magick::LightenCompositeOp);
        st->compose_first_with_second(m_out_image, image);

        ++counter;

        if (*m_stopped)
            return;

        delete image;

         qDebug() << "Two images composed in " << timer.elapsed() << "milliseconds";

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
