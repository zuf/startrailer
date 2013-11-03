#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QModelIndexList>
#include <QMimeData>
#include <QMimeDatabase>
#include <QMimeType>
#include <QElapsedTimer>
#include <QMessageBox>
#include <QListIterator>
#include <QtConcurrent/QtConcurrent>
#include <QThread>
#include <QMutex>
#include <Magick++.h>
#include <QFileDialog>
#include "playbackreader.h"

#include "compositetrailstask.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{    
    preview_image = new QuteImage();
    Q_ASSERT(preview_image);
    compose_op = MagickCore::LightenIntensityCompositeOp;

    ui->setupUi(this);

    model = new QFileSystemModel;

    QString start_path = QDir::currentPath();
    QStringList cmdline_args = QCoreApplication::arguments();

    if (cmdline_args.size()>1)
    {
        cmdline_args.pop_front();
        start_path = cmdline_args.first();
    }

    model->setRootPath(start_path);

    ui->filesList->setModel(model);
    ui->filesList->setRootIndex(model->index(start_path));
    model->sort(0);

    item = new QGraphicsPixmapItem();

    scene = new QGraphicsScene();
    scene->addItem(item);

    qDebug() << "QThread::idealThreadCount(): " << QThread::idealThreadCount();

    ui->graphicsView->setBackgroundBrush(QBrush(QColor(32,32,32), Qt::SolidPattern));
    ui->graphicsView->setResizeAnchor(QGraphicsView::AnchorViewCenter);
    ui->graphicsView->setScene(scene);
    item->setTransformationMode(Qt::SmoothTransformation);

    ui->splitter->setStretchFactor(1, 10);

    QObject::connect(ui->filesList->selectionModel(), &QItemSelectionModel::selectionChanged,
                     this, &MainWindow::selectionChanged);

    progress_bar = new QProgressBar(this);
    progress_bar->hide();
    ui->statusBar->addPermanentWidget(progress_bar);
    started_threads=0;

    ui->actionPreviewEach_2s->trigger();

    preview_each_n_group = new QActionGroup( this );
    ui->actionPreviewEach_1s->setActionGroup(preview_each_n_group);
    ui->actionPreviewEach_2s->setActionGroup(preview_each_n_group);
    ui->actionPreviewEach_5s->setActionGroup(preview_each_n_group);
    ui->actionPreviewEach_10s->setActionGroup(preview_each_n_group);
    ui->actionPreviewEach_15s->setActionGroup(preview_each_n_group);
    ui->actionPreviewEach_30s->setActionGroup(preview_each_n_group);
    ui->actionPreviewEach_60s->setActionGroup(preview_each_n_group);
    ui->actionWithout_preview->setActionGroup(preview_each_n_group);
}

MainWindow::~MainWindow()
{       
    delete progress_bar;
    delete scene;
    delete ui;
    delete model;
    delete preview_each_n_group;
}

//void MainWindow::handleFinished()
//{
//    static int finished_runners = 0;
//    ++finished_runners;
//    qDebug() << "handleFinished(): " << finished_runners;

//    if (finished_runners >= QThread::idealThreadCount())
//    {
//        finished_runners=0;
//    }
//    redrawPreview();
//}

void MainWindow::redrawPreview(bool force)
{
    //qDebug() << "Redraw at progress: " << progress_bar->value() << " / " << progress_bar->maximum();
    if (force)
    {
        mutex_preview_image.lock();
        drawImage(*preview_image);
        mutex_preview_image.unlock();
    }
    else
    {
        if (mutex_preview_image.tryLock())
        {
            drawImage(*preview_image);
            mutex_preview_image.unlock();
        }
    }
}

void MainWindow::on_filesList_doubleClicked(const QModelIndex &index)
{
    QString path = model->filePath(index);

    if (model->fileInfo(index).isDir())
    {
        ui->filesList->setRootIndex(index);
        model->setRootPath(model->filePath(index));
        model->sort(0);
    }
    else
    {
        QMimeDatabase mimeDatabase;
        QMimeType mimeType;
        mimeType = mimeDatabase.mimeTypeForFile(path);

        //        QImage image(path);
        //        item->setPixmap(QPixmap::fromImage(image));
        //        item->setTransformationMode(Qt::SmoothTransformation);
        //        ui->graphicsView->fitInView(item, Qt::KeepAspectRatio);
    }

}


void MainWindow::on_actionBack_triggered()
{
    QModelIndex parent_index = model->parent(ui->filesList->rootIndex());
    ui->filesList->setRootIndex(parent_index);
    model->setRootPath(model->filePath(parent_index));
    model->sort(0);
}


void MainWindow::on_actionComposite_triggered()
{
    compositeSelected();
}

void MainWindow::on_filesList_clicked(const QModelIndex &index)
{
    //    QString path = model->filePath(index);
    //    //        qDebug() << path;

    //    QMimeDatabase mimeDatabase;
    //    QMimeType mimeType;
    //    mimeType = mimeDatabase.mimeTypeForFile(path);
    //        qDebug() << mimeType.name();
    //        qDebug() << "Valid: " << mimeType.isValid();

    //    QImage image(path);
    //    if (item)
    //        delete item;
    //    item = new QGraphicsPixmapItem(QPixmap::fromImage(image));
    //    scene->addItem(item);
    //    ui->graphicsView->setScene(scene);
    //    item->setTransformationMode(Qt::SmoothTransformation);
    //    ui->graphicsView->fitInView(item, Qt::KeepAspectRatio);


}

void MainWindow::compositeSelected()
{
    static QMutex mutex;
    mutex.lock();
    stopped = true;
    QThreadPool::globalInstance()->waitForDone();

    benchmark_timer.start();
    qDebug() << "Start composing...";

    QStringList files;
    QModelIndexList selected_rows = ui->filesList->selectionModel()->selectedRows(0);
    QListIterator<QModelIndex> i(selected_rows);
    if (selected_rows.size()>1 && i.hasNext())
    {
        while (i.hasNext())
        {
            QModelIndex index = i.next();
            if (!model->isDir(index))
                files << model->filePath(index);
        }

        files.sort();

        //        qDebug() << "files.count(): " << files.count();

        QVector<int> sizes = chunkSizes(files.count(), QThread::idealThreadCount());

        //        qDebug() << "schunk sizes: " << sizes;

        progress_bar->setMaximum(files.size() + sizes.size() - sizes.count(0));
        progress_bar->setValue(0);
        progress_bar->show();
        int offset=0;
        //preview_image.read(files[0].toStdString());
        //        preview_image = st.read_image(files[0].toStdString());
        Q_ASSERT(preview_image!=0);
        preview_image->read(files[0].toStdString());

        //        qDebug() << "Files list made from selection and first image prepared in " << timer.elapsed() << "milliseconds";

        QThreadPool::globalInstance()->waitForDone();

        started_threads=0;

        int tasks_count=0;
        for (int n=0; n<sizes.size(); n++)
        {
            if (sizes[n]>0)
                ++tasks_count;
        }

        for (int n=0; n<sizes.size(); n++)
        {
            if (sizes[n]>0)
            {
                CompositeTrailsTask *task = new CompositeTrailsTask(this,
                                                                    &stopped,
                                                                    files.mid(offset, sizes[n]),
                                                                    preview_each_n_ms,
                                                                    n,
                                                                    tasks_count,
                                                                    &mutex_preview_image,
                                                                    preview_image,
                                                                    compose_op);
                QThreadPool::globalInstance()->start(task);
                ++started_threads;
                offset += sizes[n];
            }
        }
    }
    else
    {
        ui->statusBar->showMessage(tr("Please select files to composite."), 3000);
    }
    qDebug() << "MainWindow::compositeSelected() ---------------------------\n";
    mutex.unlock();
}

void MainWindow::selectEachNRow(int n)
{
    QItemSelection selection;
    int row=0;
    while(ui->filesList->rootIndex().child(row, 0).isValid())
    {
        selection.append(QItemSelectionRange(ui->filesList->rootIndex().child(row, 0)));
        row+=n;
    }
    ui->filesList->selectionModel()->select(selection, QItemSelectionModel::Select | QItemSelectionModel::Rows);
}

void MainWindow::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{

    static QTimer *timer=0;
    if (timer)
    {
        timer->stop();
        delete timer;
        timer=0;
    }

    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SLOT(slot_compositeSelected()));
    timer->start(20);
}

void MainWindow::drawImage(StarTrailer::Image &image)
{
    QPixmap qpm;

    Q_ASSERT(image.width()>0);
    Q_ASSERT(image.height()>0);

#define CONVER_IMAGE_WITH_RGB888
#ifdef CONVER_IMAGE_WITH_RGB888

    size_t image_length = image.width()*image.height()*3;

    uchar* data __attribute__((align(64))) = new uchar[image_length];
    void* vd=data;

    size_t copied_bytes = image.to_buffer(vd);
    Q_ASSERT(copied_bytes==image_length);
    Q_ASSERT(copied_bytes>0);

    QImage *qimg = new QImage(data, image.width(), image.height(), QImage::Format_RGB888);
    Q_ASSERT(qimg->byteCount()>0);


    Q_ASSERT(qpm.isNull()==true);

    bool res = qpm.convertFromImage(*qimg);
    Q_ASSERT(res);

    delete qimg;
    delete[] data;

    Q_ASSERT(qpm.isNull()==false);
#else
    Magick::Image img(*image.get_magick_image());

    Magick::Blob blob;
    img.magick("BMP");
    img.write( &blob );
    qpm.loadFromData((uchar*)blob.data(), blob.length());
#endif
    item->setPixmap(qpm);
    ui->graphicsView->fitInView(item, Qt::KeepAspectRatio);
}

void MainWindow::on_actionUpdateFileList_triggered()
{
    ui->filesList->update(ui->filesList->rootIndex());
}

void MainWindow::on_actionClearSelection_triggered()
{
    ui->filesList->selectionModel()->clearSelection();
    ui->statusBar->showMessage(tr("Selection cleared"), 5000);
}

void MainWindow::announceProgress(int counter)
{
    progress_bar->setValue( progress_bar->value() + 1 );
}

void MainWindow::composingFinished()
{
    //qDebug() << "QThreadPool::globalInstance()->activeThreadCount(): " << QThreadPool::globalInstance()->activeThreadCount();
    --started_threads;
    progress_bar->setValue( progress_bar->value() + 1 );
    if (started_threads<=0)
    {
        progress_bar->hide();
        progress_bar->setValue(0);
        redrawPreview(true);
        qDebug() << "benchmark_timer.elapsed(): " << benchmark_timer.elapsed();
    }
}



void MainWindow::on_action_Save_as_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                    model->filePath(ui->filesList->rootIndex()),
                                                    tr("Images (*.png *.jpg *.tif *.bmp)"));
    preview_image->write(fileName.toStdString());
}

void MainWindow::on_actionE_xit_triggered()
{
    stopped = true;
    QThreadPool::globalInstance()->waitForDone();

    QApplication::quit();
}

void MainWindow::on_action_About_triggered()
{
    QMessageBox::about(this, tr("Startrailer"), tr("<h1>Startrailer</h1>"
                                                   "Helps to make star trails from your photos.<br>"
                                                   "<a href=\"https://github.com/zuf/startrailer\">https://github.com/zuf/startrailer</a><br><br>"
                                                   "Git: %1<br>"
                                                   "From: %2").arg(APP_REVISION).arg(QString::fromLocal8Bit(BUILDDATE)));
}

void MainWindow::slot_compositeSelected()
{
    progress_bar->hide();
    progress_bar->setValue(0);

    ui->statusBar->showMessage(tr("Selected: %1").arg(ui->filesList->selectionModel()->selectedRows().count()));

    if (ui->filesList->selectionModel()->selectedRows().size()==1)
    {
        QModelIndex index = ui->filesList->selectionModel()->selectedIndexes().first();
        if (!model->isDir(index))
        {
            //            preview_image = st.read_image(model->filePath(index).toStdString());
            Q_ASSERT(preview_image);
            preview_image->read(model->filePath(index).toStdString());
            drawImage(*preview_image);
        }
    }
    else
    {
        if (ui->actionAuto_trail_on_select->isChecked())
            compositeSelected(); // TODO: Doesn't redraw all. If selection only added, then add them to current preview.
        // If deselected.size()>0 then redraw all
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{    
    stopped = true;
    QThreadPool::globalInstance()->waitForDone();
}

void MainWindow::on_actionClear_2_triggered()
{
    // TODO: Dry with toolbar button
    ui->filesList->selectionModel()->clearSelection();
    ui->statusBar->showMessage(tr("Selection cleared"), 5000);
}

void MainWindow::on_actionAll_triggered()
{
    ui->filesList->selectAll();
}


void MainWindow::on_actionSelectEach_2_triggered()
{
    selectEachNRow(2);
}

void MainWindow::on_actionSelectEach_5th_triggered()
{
    selectEachNRow(5);
}

void MainWindow::on_actionSelectEach_10th_triggered()
{
    selectEachNRow(10);
}

void MainWindow::on_actionSelectEach_25th_triggered()
{
    selectEachNRow(25);
}

void MainWindow::on_actionSelectEach_50th_triggered()
{
    selectEachNRow(50);
}

void MainWindow::on_actionSelectEach_100_triggered()
{
    selectEachNRow(100);
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::on_actionPlay_triggered()
{
    stopped = true;
    QThreadPool::globalInstance()->waitForDone();

    QStringList files;
    QModelIndexList selected_rows = ui->filesList->selectionModel()->selectedRows(0);
    QListIterator<QModelIndex> i(selected_rows);

    static QQueue<const QByteArray*> images_bytes_queue;
    static QMutex images_bytes_queue_mutex;

    if (selected_rows.size()>1 && i.hasNext())
    {
        stopped = false;
        while (i.hasNext())
        {
            QModelIndex index = i.next();
            if (!model->isDir(index))
                files << model->filePath(index);
        }

        files.sort();
        QVector<int> sizes = chunkSizes(files.count(), QThread::idealThreadCount());


        int offset=0;
        Q_ASSERT(preview_image);
        preview_image->read(files[0].toStdString());

        QThreadPool::globalInstance()->waitForDone();

        started_threads=0;
        for (int n=0; n<sizes.size(); n++)
        {
            if (sizes[n]>0)
            {
                PlaybackReader *task = new PlaybackReader(&images_bytes_queue,
                                                          &images_bytes_queue_mutex,
                                                          files.mid(offset, sizes[n]));

                QThreadPool::globalInstance()->start(task);
                ++started_threads;
                offset += sizes[n];
            }
        }

        static int processed_files=0;
        processed_files=0;
        while(processed_files<files.size())
        {
            images_bytes_queue_mutex.lock();
            if (images_bytes_queue.size()>1)
            {
                const QByteArray *image_bytes = images_bytes_queue.dequeue();
                QPixmap qpm;
                qpm.loadFromData(*image_bytes);
                delete image_bytes;
                item->setPixmap(qpm);
                ui->graphicsView->fitInView(item, Qt::KeepAspectRatio);
                delete image_bytes;
                ++processed_files;
            }
            images_bytes_queue_mutex.unlock();
            QApplication::processEvents();
        }

    }
    else
    {
        ui->statusBar->showMessage(tr("Please select files to play."), 3000);
    }
}

void MainWindow::on_actionWithout_preview_triggered()
{
    preview_each_n_ms = 0;
}

void MainWindow::on_actionPreviewEach_1s_triggered()
{
    preview_each_n_ms = 1000;
}

void MainWindow::on_actionPreviewEach_2s_triggered()
{
    preview_each_n_ms = 2000;
}

void MainWindow::on_actionPreviewEach_5s_triggered()
{
    preview_each_n_ms = 5000;
}

void MainWindow::on_actionPreviewEach_10s_triggered()
{
    preview_each_n_ms = 10000;
}

void MainWindow::on_actionPreviewEach_15s_triggered()
{
    preview_each_n_ms = 15000;
}

void MainWindow::on_actionPreviewEach_30s_triggered()
{
    preview_each_n_ms = 30000;
}

void MainWindow::on_actionPreviewEach_60s_triggered()
{
    preview_each_n_ms = 60000;
}

void MainWindow::on_actionDifference_triggered()
{
    compose_op = Magick::DifferenceCompositeOp;
}
