#include "mainwindow.h"
#include <QApplication>
#include <Magick++.h>

Q_DECLARE_METATYPE(Magick::Image*)

int main(int argc, char *argv[])
{
    Magick::InitializeMagick(*argv); // Windows ImageMagick DLLs initialization shit
    QApplication a(argc, argv);
    MainWindow w;
    w.show();


    qRegisterMetaType<Magick::Image*>("Magick::Image*");

    return a.exec();
}
