#-------------------------------------------------
#
# Project created by QtCreator 2013-10-01T12:43:41
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = startrailer
TEMPLATE = app

#CONFIG += debug_and_release

#INCLUDEPATH += src

SOURCES += main.cpp\
    iconproxy.cpp \
        mainwindow.cpp \
    compositetrailstask.cpp \
    qiconthumbextractor.cpp \
    qimagefileiconprovider.cpp \
    view.cpp \
    playbackreader.cpp \
    image.cpp \
    quteimage.cpp

HEADERS  += mainwindow.h \
    compositetrailstask.h \
    iconproxy.h \
    qiconthumbextractor.h \
    qimagefileiconprovider.h \
    view.h \
    playbackreader.h \
    image.h \
    quteimage.h

FORMS    += ui/mainwindow.ui

release: DESTDIR = ../build/release
debug:   DESTDIR = ../build/debug

OBJECTS_DIR = $$DESTDIR/obj
MOC_DIR = $$DESTDIR/moc
RCC_DIR = $$DESTDIR/qrc
UI_DIR = $$DESTDIR/ui

#LIBS += `Magick++-config --cppflags --cxxflags --ldflags --libs`
CONFIG += link_pkgconfig
#PKGCONFIG += Magick++
PKGCONFIG = GraphicsMagick++

#LIBS+=-lgomp

QMAKE_CXXFLAGS_RELEASE += -O3

OTHER_FILES += \
    README.md

REVISION = $$system(git rev-parse HEAD)
#DEFINES += APP_REVISION=$$REVISION
DEFINES += APP_REVISION=\\\"$$REVISION\\\"

BUILDDATE = $$system(date "+%Y%m%d%H%M")
DEFINES += BUILDDATE=\\\"\"$$BUILDDATE\"\\\"

PKGCONFIG += libraw_r
PKGCONFIG += libexif
#LIBS+=-lraw_r
unix: QMAKE_CXXFLAGS += -std=c++11
unix: QMAKE_CXXFLAGS_DEBUG += -fprofile-arcs -ftest-coverage
release: QMAKE_CXXFLAGS += -march=native -mtune=native -O3
LIBS += \
-lgcov
