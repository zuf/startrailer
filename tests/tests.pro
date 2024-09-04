TEMPLATE = subdirs
CONFIG += ordered
#SUBDIRS = ../src test_image
SUBDIRS = test_image \
    ImageBenchmark

include(common.pri)
QMAKE_CXXFLAGS += -Wall -Wno-error
