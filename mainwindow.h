#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>
#include <QImage>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_filesList_doubleClicked(const QModelIndex &index);    

    void on_actionBack_triggered();

    void on_actionComposite_triggered();

    void on_filesList_clicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;
    QFileSystemModel *model;

    QGraphicsScene* scene;
    QGraphicsPixmapItem* item;
};

#endif // MAINWINDOW_H
