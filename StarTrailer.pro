TEMPLATE = subdirs
CONFIG += debug_and_release
CONFIG += c++17
SUBDIRS = src tests
QMAKE_CXXFLAGS += -Wall -Wno-error

