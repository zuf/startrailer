#-------------------------------------------------
#
# Project created by QtCreator 2013-10-01T12:43:41
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = StarTrailer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    startrailer.cpp

HEADERS  += mainwindow.h \
    startrailer.h

FORMS    += mainwindow.ui

#LIBS += `Magick++-config --cppflags --cxxflags --ldflags --libs`
CONFIG += link_pkgconfig
PKGCONFIG += Magick++


#QMAKE_CXXFLAGS+=-fopenmp
#LIBS+=-lgomp
