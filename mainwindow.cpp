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

    qDebug() << "cmdline_args: " << cmdline_args;
    cmdline_args.pop_front();
    qDebug() << "cmdline_args: " << cmdline_args.first();
    if (cmdline_args.size()>0)
        start_path = cmdline_args.first();

    model->setRootPath(start_path);
    qDebug() << "QDir::currentPath(): " << start_path;
    //model->setFilter( QDir::AllDirs | QDir::NoDotAndDotDot );
    model->sort(0);

    ui->filesList->setModel(model);
    ui->filesList->setSortingEnabled(true);    
    ui->filesList->setRootIndex(model->index(start_path));

    scene = new QGraphicsScene();

}

MainWindow::~MainWindow()
{
    if (item)
      delete item;

    delete scene;
    delete ui;
    delete model;
}

void MainWindow::on_filesList_doubleClicked(const QModelIndex &index)
{
    QString path = model->filePath(index);
//    qDebug() << index;
//    qDebug() << path;
//    model->setRootPath(model->filePath(index));

    if (model->fileInfo(index).isDir())
    {
        ui->filesList->setRootIndex(index);
        model->setRootPath(model->filePath(index));
        model->sort(0);
    }
    else
    {
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
    }

}


void MainWindow::on_actionBack_triggered()
{
//    qDebug() << "Got to upper dir";    
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

    const QVector<int> sizes = chunkSizes(files.count(), QThread::idealThreadCount());

    qDebug() << "schunk sizes: " << sizes;

    //const QByteArray *image_bytes = st.q_compose_list_and_return_qbyte_array(files);
    int offset = 0;
    QFuture<Magick::Image *> futuries[sizes.size()]; // TODO: QList it
    QFutureWatcher<Magick::Image *> watchers[sizes.size()]; // TODO: QList it
    int n=0;
    foreach (const int chunkSize, sizes) {
        futuries[n]= QtConcurrent::run(&st, &StarTrailer::compose_list, files.mid(offset, chunkSize));
        //connect(&watchers[n], SIGNAL(finished()), &myObject, SLOT(handleFinished()));
        watchers[n].setFuture(futuries[n]);
        ++n;
        offset += chunkSize;
    }

    Magick::Image *out = 0;
    for (int i=0; i<sizes.size();++i)
    {
        Magick::Image *img = futuries[i].result();
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
    /////////////////

//    QModelIndexList list = ui->filesList->selectionModel()->selectedIndexes();

//    QModelIndexList::const_iterator constIterator;

//    Magick::Image *out_image = new Magick::Image(model->filePath(*list.constBegin()).toStdString());
//    Magick::Image *tmp_image = new Magick::Image(*out_image);
//    for (constIterator = list.constBegin(); constIterator != list.constEnd(); ++constIterator)
//    {
//        // TODO: Skip first iteration
//        //std::out << "Trail file: " << (*constIterator).toStdString();
//        qDebug() << "Trail file: " << (model->filePath((*constIterator)));;
//        tmp_image->read(model->filePath((*constIterator)).toStdString());
//        st.compose_first_with_second(out_image, tmp_image);
//    }

//    delete tmp_image;
//    const QByteArray *image_bytes = st.image_to_qbyte_array(out_image);
//    delete out_image;

    ///////////////

    QPixmap qpm;
    qpm.loadFromData(*image_bytes);
    delete image_bytes;

    if (item)
      delete item;
    item = new QGraphicsPixmapItem(qpm);
    scene->addItem(item);
    ui->graphicsView->setScene(scene);
    item->setTransformationMode(Qt::SmoothTransformation);
    ui->graphicsView->fitInView(item, Qt::KeepAspectRatio);

    qDebug() << "The slow operation took " << timer.elapsed() << " milliseconds";
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
}

void MainWindow::checkIfDone()
{

}

