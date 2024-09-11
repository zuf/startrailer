release: DESTDIR = ../build/release
debug:   DESTDIR = ../build/debug

OBJECTS_DIR = $$DESTDIR/obj
MOC_DIR = $$DESTDIR/moc
RCC_DIR = $$DESTDIR/qrc
UI_DIR = $$DESTDIR/ui


CONFIG += link_pkgconfig
PKGCONFIG = GraphicsMagick++
PKGCONFIG += libraw_r

QMAKE_CXXFLAGS += -g -Wall -fprofile-arcs -ftest-coverage -O0
QMAKE_LFLAGS += -g -Wall -fprofile-arcs -ftest-coverage -O0

unix: QMAKE_CXXFLAGS += -std=c++17
unix: QMAKE_CXXFLAGS_DEBUG +=  -g -fprofile-arcs -ftest-coverage

LIBS += \
-lgcov
