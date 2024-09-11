#include "mainwindow.h"
#include <QApplication>
#include <Magick++.h>

Q_DECLARE_METATYPE(Magick::Image*)

int main(int argc, char *argv[])
{
    Magick::InitializeMagick(*argv);    
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
