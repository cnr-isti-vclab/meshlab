include (../../shared.pri)
HEADERS += filter_csg.h \
    intercept.h \
    gmpfrac.h

SOURCES += filter_csg.cpp
TARGET = filter_csg

macx:INCLUDEPATH += ../../external/inc/macx/gmp-5.0.1

macx:LIBS         += ../../external/lib/macx/libgmpxx.a ../../external/lib/macx/libgmp.a
win32-g++:LIBS		 += ../../external/lib/win32-gcc/libgmpxx.a ../../external/lib/win32-gcc/libgmp.a


