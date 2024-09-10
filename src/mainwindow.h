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
#include <QLabel>
#include <QActionGroup>
#include <QMutex>
#include <QOpenGLWidget>
#include "image.h"
#include "quteimage.h"
#include <QElapsedTimer>
#include "qimagefileiconprovider.h"
#include "qiconthumbextractor.h"
#include "iconproxy.h"

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


    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

    void composing_finished(){

    }

    void on_actionUpdateFileList_triggered();

    void on_actionClearSelection_triggered();

    void announceProgress();

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

    void on_actionOpen_directory_triggered();

    void on_actionOnly_jpeg_preview_triggered();

    void on_actionLighten_triggered();

    void on_actionDarken_triggered();

    void on_actionLibraw_half_size_triggered();

    void on_actionLibraw_full_size_triggered();

    void on_actionCopy_to_clipboard_triggered();

    void on_actionZoom_In_triggered();

    void on_actionZoom_Out_triggered();

    void on_actionZoom_Reset_triggered();

    void on_actionZoom_1_1_triggered();

    void on_actionLighten_2_triggered();

protected:
    void closeEvent(QCloseEvent *event);

private:

    void compositeSelected();

    void selectEachNRow(int n);

    void drawImage(StarTrailer::Image &image);

    Ui::MainWindow *ui;
    QOpenGLWidget *gl=0;
    bool use_opengl=false;
    QFileSystemModel *model=0;
    IconProxy *icon_proxy=0;

    QGraphicsScene* scene;
    QGraphicsPixmapItem* item;

    volatile bool stopped;    
    QuteImage *preview_image;
//    QuteImage *out_image;
    QMutex mutex_preview_image;

    QProgressBar *progress_bar;
    QLabel *image_info_label;

//    StarTrailer st;
    Magick::CompositeOperator compose_op;
    StarTrailer::Image::RawProcessingMode raw_processing_mode = StarTrailer::Image::FullPreview;
    int started_threads;
    int preview_each_n_ms;
    QActionGroup* preview_each_n_group;

    QElapsedTimer benchmark_timer;

    QItemSelection selected_items;

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

    QImageFileIconProvider *icon_provider=0;

    void openDir(QString dir);

    void stopCompositing();

    void selectFirstImageFile();
};

#endif // MAINWINDOW_H
