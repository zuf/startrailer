#ifndef VIEW_H
#define VIEW_H

#include <QGraphicsView>

class View : public QGraphicsView
{
    Q_OBJECT
public:
    explicit View(QWidget *parent = 0);

protected:
    void resizeEvent(QResizeEvent *event);

signals:

public slots:

};

#endif // VIEW_H
