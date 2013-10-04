#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QModelIndexList>
#include <QMimeData>
#include <QMimeDatabase>
#include <QMimeType>
#include <QElapsedTimer>
#include "startrailer.h"
#include <QMessageBox>
#include <QListIterator>
#include <QtConcurrent/QtConcurrent>
#include <QThread>
#include <QMutex>
#include <Magick++.h>
#include <QFileDialog>

#include "compositetrailstask.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{    
    ui->setupUi(this);

    ui->progressBar->hide();

    model = new QFileSystemModel;

    QString start_path = QDir::currentPath();
    QStringList cmdline_args = QCoreApplication::arguments();

    if (cmdline_args.size()>1)
    {
        cmdline_args.pop_front();
        start_path = cmdline_args.first();
    }

    model->setRootPath(start_path);
    qDebug() << "QDir::currentPath(): " << start_path;
    //model->setFilter( QDir::AllDirs | QDir::NoDotAndDotDot );
    model->sort(0);

    ui->filesList->setModel(model);
    ui->filesList->setSortingEnabled(true);
    ui->filesList->setRootIndex(model->index(start_path));

    item = new QGraphicsPixmapItem();

    scene = new QGraphicsScene();
    scene->addItem(item);

    qDebug() << "QThread::idealThreadCount(): " << QThread::idealThreadCount();

    ui->graphicsView->setBackgroundBrush(QBrush(QColor(32,32,32), Qt::SolidPattern));
    ui->graphicsView->setResizeAnchor(QGraphicsView::AnchorViewCenter);
    //scene->addItem(item);
    ui->graphicsView->setScene(scene);
    item->setTransformationMode(Qt::SmoothTransformation);

    ui->splitter->setStretchFactor(1, 10);

    QObject::connect(ui->filesList->selectionModel(), &QItemSelectionModel::selectionChanged,
                     this, &MainWindow::selectionChanged);
}

MainWindow::~MainWindow()
{       
    delete scene;
    //delete item;
    delete ui;
    delete model;
}

void MainWindow::handleFinished()
{
    static int finished_runners = 0;
    ++finished_runners;
    qDebug() << "handleFinished(): " << finished_runners;

    if (finished_runners >= QThread::idealThreadCount())
    {
        finished_runners=0;
        StarTrailer st;


        //drawMagickImage(out);
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

        QImage image(path);
        item->setPixmap(QPixmap::fromImage(image));
        item->setTransformationMode(Qt::SmoothTransformation);
        ui->graphicsView->fitInView(item, Qt::KeepAspectRatio);
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
    QString path = model->filePath(index);
    //        qDebug() << path;

    QMimeDatabase mimeDatabase;
    QMimeType mimeType;
    mimeType = mimeDatabase.mimeTypeForFile(path);
    //        qDebug() << mimeType.name();
    //        qDebug() << "Valid: " << mimeType.isValid();

    QImage image(path);
    if (item)
        delete item;
    item = new QGraphicsPixmapItem(QPixmap::fromImage(image));
    scene->addItem(item);
    ui->graphicsView->setScene(scene);
    item->setTransformationMode(Qt::SmoothTransformation);
    ui->graphicsView->fitInView(item, Qt::KeepAspectRatio);
    //        qDebug() << model->filePath(model->parent(ui->filesList->rootIndex()));

    ui->statusBar->showMessage(tr("Selected: %1").arg(ui->filesList->selectionModel()->selectedRows().count()));
}

void MainWindow::compositeSelected()
{
    static QMutex mutex;
    mutex.lock();
    stopped = true;
    QThreadPool::globalInstance()->waitForDone();
    qDebug() << "Start composing...";

    QElapsedTimer timer;
    timer.start();

    StarTrailer st;
    //    const QByteArray *image_bytes = st.q_compose(model->filePath(list[0]).toStdString(), model->filePath(list[1]).toStdString());

    //const QByteArray *image_bytes = st.q_compose_model_list(model, ui->filesList->selectionModel()->selectedIndexes());

    QStringList files;

    QModelIndexList selected_rows = ui->filesList->selectionModel()->selectedRows(0);

    QListIterator<QModelIndex> i(selected_rows);
    if (selected_rows.size()>1 && i.hasNext())
    {
        while (i.hasNext())
        {
            files << model->filePath(i.next());
        }

        //qDebug() << "Files: " << files;
        qDebug() << "files.count(): " << files.count();

        QVector<int> sizes = chunkSizes(files.count(), QThread::idealThreadCount());
        qDebug() << "schunk sizes: " << sizes;

        ui->progressBar->setMaximum(files.size());
        ui->progressBar->setValue(0);
        ui->progressBar->show();
        int offset=0;
        preview_image.read(files[0].toStdString());
        for (int n=0; n<sizes.size(); n++)
        {
            CompositeTrailsTask *task = new CompositeTrailsTask(this, &stopped, files.mid(offset, sizes[n]));
            QThreadPool::globalInstance()->start(task);
            offset += sizes[n];
        }
    }
    else
    {
        //QMessageBox::information(this, tr("No files selected"), tr("Please select files to composite."));
        ui->statusBar->showMessage(tr("Please select files to composite."), 3000);
    }
    qDebug() << "---------------------------\n";
    mutex.unlock();
}

void MainWindow::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    compositeSelected(); // TODO: Doesn't redraw all. If selection only added, then add them to current preview.
                         // If deselected.size()>0 then redraw all
}

void MainWindow::drawMagickImage(Magick::Image image)
{
    StarTrailer st;
    const QByteArray *image_bytes = st.image_to_qbyte_array(image);

    qDebug() << "Start format image for Qt";

    QPixmap qpm;
    qpm.loadFromData(*image_bytes);

    qDebug() << "QPixmap loaded";

    delete image_bytes;

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
    ui->progressBar->setValue( ui->progressBar->value() + 1 );
}

void MainWindow::receiveMagickImage(Magick::Image *image)
{
    static QMutex mutex;
    mutex.lock();
    qDebug() << "receiveMagickImage()";
    StarTrailer st;

    st.compose_first_with_second(&preview_image, image);
    drawMagickImage(preview_image);
    delete image;
    ui->action_Save_as->setEnabled(true);
    mutex.unlock();
}

void MainWindow::composingFinished()
{
    ui->progressBar->hide();
    ui->progressBar->setValue(0);
}



void MainWindow::on_action_Save_as_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                               model->filePath(ui->filesList->rootIndex()),
                               tr("Images (*.png *.jpg *.tif *.bmp)"));
    preview_image.write(fileName.toStdString());
}

void MainWindow::on_actionE_xit_triggered()
{
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


