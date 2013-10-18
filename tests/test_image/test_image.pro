#-------------------------------------------------
#
# Project created by QtCreator 2013-10-18T12:57:32
#
#-------------------------------------------------

include(../common.pri)

QT       += testlib

QT       -= gui

TARGET = tst_imagetest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += tst_imagetest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"


INCLUDEPATH += ../../src/
SOURCES += ../../src/image.cpp
HEADERS += ../../src/image.h



