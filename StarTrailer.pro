#-------------------------------------------------
#
# Project created by QtCreator 2013-10-01T12:43:41
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = startrailer
TEMPLATE = app

release: DESTDIR = build/release
debug:   DESTDIR = build/debug

OBJECTS_DIR = $$DESTDIR/obj
MOC_DIR = $$DESTDIR/moc
RCC_DIR = $$DESTDIR/qrc
UI_DIR = $$DESTDIR/ui


INCLUDEPATH += src

SOURCES += src/main.cpp\
        src/mainwindow.cpp \
    src/startrailer.cpp \
    src/compositetrailstask.cpp \
    src/view.cpp \
    src/playbackreader.cpp \
    src/image.cpp \
    src/quteimage.cpp

HEADERS  += src/mainwindow.h \
    src/startrailer.h \
    src/compositetrailstask.h \
    src/view.h \
    src/playbackreader.h \
    src/image.h \
    src/quteimage.h

FORMS    += ui/mainwindow.ui

#LIBS += `Magick++-config --cppflags --cxxflags --ldflags --libs`
CONFIG += link_pkgconfig
PKGCONFIG += Magick++


#QMAKE_CXXFLAGS+=-fopenmp
#LIBS+=-lgomp

#QMAKE_CXXFLAGS_RELEASE += -O2

OTHER_FILES += \
    README.md

REVISION = $$system(git rev-parse HEAD)
#DEFINES += APP_REVISION=$$REVISION
DEFINES += APP_REVISION=\\\"$$REVISION\\\"

BUILDDATE = $$system(date "+%Y%m%d%H%M")
DEFINES += BUILDDATE=\\\"\"$$BUILDDATE\"\\\"

PKGCONFIG += libraw_r
#LIBS+=-lraw_r


