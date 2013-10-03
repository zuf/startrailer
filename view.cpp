#include "view.h"

View::View(QWidget *parent) :
    QGraphicsView(parent)
{
}

void View::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    fitInView(scene()->itemsBoundingRect(), Qt::KeepAspectRatio);
}
