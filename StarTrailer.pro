TEMPLATE = subdirs
CONFIG += debug_and_release
SUBDIRS = src tests
QMAKE_CXXFLAGS += -Wno-deprecated
