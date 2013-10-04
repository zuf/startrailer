#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>
#include <QImage>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>

#include <Magick++.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void handleFinished();

private slots:
    void on_filesList_doubleClicked(const QModelIndex &index);    

    void on_actionBack_triggered();

    void on_actionComposite_triggered();

    void on_filesList_clicked(const QModelIndex &index);


    void composing_finished(){

    }

    void on_actionUpdateFileList_triggered();

    void on_actionClearSelection_triggered();

    void announceProgress(int counter);

    void receiveMagickImage(Magick::Image *image);

    void composingFinished();


    void on_action_Save_as_triggered();

private:
    void checkIfDone();

    void drawMagickImage(Magick::Image image);

    Ui::MainWindow *ui;
    QFileSystemModel *model;

    QGraphicsScene* scene;
    QGraphicsPixmapItem* item;

    volatile bool stopped;
    Magick::Image preview_image;

    QVector<int> chunkSizes(const int size, const int chunkCount)
    {
        Q_ASSERT(size > 0 && chunkCount > 0);
        if (chunkCount == 1)
            return QVector<int>() << size;
        QVector<int> result(chunkCount, size / chunkCount);
        if (int remainder = size % chunkCount) {
            int index = 0;
            for (int i = 0; i < remainder; ++i) {
                ++result[index];
                ++index;
                index %= chunkCount;
            }
        }
        return result;
    }
};

#endif // MAINWINDOW_H
