#-------------------------------------------------
#
# Project created by QtCreator 2013-11-06T22:41:57
#
#-------------------------------------------------

include(../common.pri)

QT       += testlib

QT       -= gui

TARGET = tst_imagebenchmark
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_imagebenchmark.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"


INCLUDEPATH += ../../src/
SOURCES += ../../src/image.cpp
HEADERS += ../../src/image.h
