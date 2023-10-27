#include <QWheelEvent>
#include <QGraphicsPixmapItem>
#include <qmath.h>
#include "view.h"

View::View(QWidget *parent) :
    QGraphicsView(parent)
{
    setTransformationAnchor(AnchorUnderMouse);
    setDragMode(ScrollHandDrag);
    setViewportUpdateMode(FullViewportUpdate);

}

qreal View::zoomFactor() const
{
    return transform().m11();
}

void View::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
//    fitInView(scene()->itemsBoundingRect(), Qt::KeepAspectRatio);
}


void View::wheelEvent(QWheelEvent *event)
{
    zoomBy(qPow(1.2, event->angleDelta().y() / 240.0));
}

void View::zoomIn()
{
    zoomBy(2);
}

void View::zoomOut()
{
    zoomBy(0.5);
}

void View::zoomFit()
{
    fitInView(scene()->itemsBoundingRect(), Qt::KeepAspectRatio);
}


void View::resetZoom()
{
    if (!qFuzzyCompare(zoomFactor(), qreal(1))) {
        resetTransform();
        emit zoomChanged();
    }
}

void View::zoomBy(qreal factor)
{
    const qreal currentZoom = zoomFactor();
    if ((factor < 1 && currentZoom < 0.1) || (factor > 1 && currentZoom > 25))
        return;

    if (!scene()->items().isEmpty()) {
        if (zoomFactor() > qreal(1)) {
          setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing, true);
          ((QGraphicsPixmapItem *)scene()->items().last())
              ->setTransformationMode(Qt::FastTransformation);
        } else {
          setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing, false);
          ((QGraphicsPixmapItem *)scene()->items().last())
              ->setTransformationMode(Qt::SmoothTransformation);
        }
    }

    scale(factor, factor);
    emit zoomChanged();
}
