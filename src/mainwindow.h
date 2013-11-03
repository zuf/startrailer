#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>
#include <QImage>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QItemSelection>
#include <Magick++.h>
#include <QProgressBar>
#include <QActionGroup>
//#include "startrailer.h"
#include <QMutex>
#include "image.h"

#include <QElapsedTimer>

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
    //void handleFinished();
    void redrawPreview(bool force=false);

private slots:
    void on_filesList_doubleClicked(const QModelIndex &index);    

    void on_actionBack_triggered();

    void on_actionComposite_triggered();

    void on_filesList_clicked(const QModelIndex &index);


    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

    void composing_finished(){

    }

    void on_actionUpdateFileList_triggered();

    void on_actionClearSelection_triggered();

    void announceProgress(int counter);    

    void composingFinished();


    void on_action_Save_as_triggered();

    void on_actionE_xit_triggered();

    void on_action_About_triggered();

    void slot_compositeSelected();

    void on_actionClear_2_triggered();

    void on_actionAll_triggered();

    void on_actionSelectEach_2_triggered();

    void on_actionSelectEach_5th_triggered();

    void on_actionSelectEach_10th_triggered();

    void on_actionSelectEach_25th_triggered();

    void on_actionSelectEach_50th_triggered();

    void on_actionSelectEach_100_triggered();

    void on_actionAbout_Qt_triggered();    

    void on_actionWithout_preview_triggered();

    void on_actionDifference_triggered();

    void on_actionPlay_triggered();    

    void on_actionPreviewEach_1s_triggered();

    void on_actionPreviewEach_5s_triggered();

    void on_actionPreviewEach_2s_triggered();

    void on_actionPreviewEach_10s_triggered();

    void on_actionPreviewEach_15s_triggered();

    void on_actionPreviewEach_30s_triggered();

    void on_actionPreviewEach_60s_triggered();

protected:
    void closeEvent(QCloseEvent *event);

private:

    void compositeSelected();

    void selectEachNRow(int n);

    void drawImage(Image &image);

    Ui::MainWindow *ui;
    QFileSystemModel *model;

    QGraphicsScene* scene;
    QGraphicsPixmapItem* item;

    volatile bool stopped;
    //Magick::Image *preview_image;
    Image *preview_image;
    QMutex mutex_preview_image;

    QProgressBar *progress_bar;

//    StarTrailer st;
    Magick::CompositeOperator compose_op;
    int started_threads;
    int preview_each_n_ms;
    QActionGroup* preview_each_n_group;

    QElapsedTimer benchmark_timer;

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
