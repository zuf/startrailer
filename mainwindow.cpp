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

#include <Magick++.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{    
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

}

MainWindow::~MainWindow()
{       
    delete scene;
    //delete item;
    delete ui;
    delete model;
}

QVector<int> sizes;
int finished_runners = 0;
QFuture<Magick::Image *> futuries[100]; // TODO: QList it
QFutureWatcher<Magick::Image *> watchers[100]; // TODO: QList it

void MainWindow::handleFinished()
{
    ++finished_runners;
    qDebug() << "handleFinished(): " << finished_runners;

    if (finished_runners >= QThread::idealThreadCount())
    {
        finished_runners=0;
        StarTrailer st;
        Magick::Image *out = 0;
        qDebug() << "sizes.size(): " << sizes.size();
        for (int i=0; i<sizes.size();++i)
        {
            qDebug() << "Wait for QtConcurrent::run #" << i ;
            Magick::Image *img = futuries[i].result();
            futuries[i].waitForFinished ();
            // TODO: Delete/reset future
            if (out==0)
            {
                out = new Magick::Image(*img);
            }
            else
            {
                st.compose_first_with_second(out, img);
            }
            delete img;
        }
        const QByteArray *image_bytes = st.image_to_qbyte_array(out);
        delete out;

        qDebug() << "Start format image for Qt";

        QPixmap qpm;
        qpm.loadFromData(*image_bytes);

        qDebug() << "QPixmap loaded";

        delete image_bytes;

        item->setPixmap(qpm);
        ui->graphicsView->fitInView(item, Qt::KeepAspectRatio);
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
    qDebug() << "Start composing...";

    QElapsedTimer timer;
    timer.start();    

    StarTrailer st;
//    const QByteArray *image_bytes = st.q_compose(model->filePath(list[0]).toStdString(), model->filePath(list[1]).toStdString());

    //const QByteArray *image_bytes = st.q_compose_model_list(model, ui->filesList->selectionModel()->selectedIndexes());

    QStringList files;
    QListIterator<QModelIndex> i(ui->filesList->selectionModel()->selectedIndexes());
    while (i.hasNext())
    {
        files << model->filePath(i.next());
    }

    sizes = chunkSizes(files.count(), QThread::idealThreadCount());

    qDebug() << "schunk sizes: " << sizes;

    //const QByteArray *image_bytes = st.q_compose_list_and_return_qbyte_array(files);
    int offset = 0;    
    int n=0;    
    finished_runners = 0;
    foreach (const int chunkSize, sizes) {
        bool connected = connect(&watchers[n], SIGNAL(finished()), this, SLOT(handleFinished()));
        qDebug() << "connected: " << connected;
        futuries[n]= QtConcurrent::run(&st, &StarTrailer::compose_list, files.mid(offset, chunkSize));        
        watchers[n].setFuture(futuries[n]);        
        qDebug() << "QtConcurrent::run #" << n  ;
        ++n;        
        offset += chunkSize;        
    }

    qDebug() << "---------------------------";


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

void MainWindow::checkIfDone()
{

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


