include (../../shared.pri)
HEADERS += filter_csg.h \
    intercept.h \
    gmpfrac.h

SOURCES += filter_csg.cpp
TARGET = filter_csg

macx:INCLUDEPATH += ../../external/inc/macx/mpir-2.1.3
win32-g++:INCLUDEPATH += ../../external/inc/win32-gcc/mpir-2.2.1
macx:LIBS         += ../../external/lib/macx/libmpirxx.a ../../external/lib/macx/libgmp.a
win32-g++:LIBS		 += ../../external/lib/win32-gcc/libmpirxx.a ../../external/lib/win32-gcc/libmpir.a


