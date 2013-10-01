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

#include <Magick++.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    model = new QFileSystemModel;
    model->setRootPath(QDir::currentPath());

    ui->filesList->setModel(model);
    ui->filesList->setSortingEnabled(true);    

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

void MainWindow::on_filesList_customContextMenuRequested(const QPoint &pos)
{

}

void MainWindow::on_actionBack_triggered()
{
//    qDebug() << "Got to upper dir";
    ui->filesList->rootIndex();
    ui->filesList->setRootIndex(model->parent(ui->filesList->rootIndex()));

}

void MainWindow::on_actionComposite_triggered()
{
    qDebug() << "Start composing...";

    QElapsedTimer timer;
    timer.start();

    QModelIndexList list = ui->filesList->selectionModel()->selectedIndexes();

    if (list.size() != 2)
    {
        QMessageBox::information(NULL, "Please chose only two image files!", "Select two images");
        return;
    }

    StarTrailer st;
    const QByteArray *image_bytes = st.compose(model->filePath(list[0]).toStdString(), model->filePath(list[1]).toStdString());
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
