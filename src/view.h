#ifndef VIEW_H
#define VIEW_H

#include <QGraphicsView>

class View : public QGraphicsView
{
    Q_OBJECT
public:
    explicit View(QWidget *parent = 0);

    qreal zoomFactor() const;

protected:
    void resizeEvent(QResizeEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

signals:
    void zoomChanged();

public slots:
    void zoomIn();
    void zoomOut();
    void resetZoom();
    void zoomFit();

private:
    void zoomBy(qreal factor);

};

#endif // VIEW_H
