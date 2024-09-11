TEMPLATE = subdirs
CONFIG += ordered
CONFIG += c++17
#SUBDIRS = ../src test_image
SUBDIRS = test_image \
    ImageBenchmark

include(common.pri)
QMAKE_CXXFLAGS += -Wall -Wno-error
