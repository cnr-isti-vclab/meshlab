include (../../shared.pri)
HEADERS += filter_csg.h \
    intercept.h \
    gmpfrac.h

SOURCES += filter_csg.cpp
TARGET = filter_csg

#macx:INCLUDEPATH += ../../external/inc/macx/mpir-2.1.3
macx:INCLUDEPATH += ../../external/inc/macx64/mpir-2.4.0
win32-g++:INCLUDEPATH += ../../external/inc/win32-gcc/mpir-2.2.1
win32-msvc:INCLUDEPATH += ../../external/inc/win32-msvc/mpir-2.2.1_x64

#macx:LIBS         += ../../external/lib/macx/libmpirxx.a ../../external/lib/macx/libgmp.a
macx:LIBS         += $$MACLIBDIR/libmpirxx.a $$MACLIBDIR/libmpir.a
win32-g++:LIBS		 += ../../external/lib/win32-gcc/libmpirxx.a ../../external/lib/win32-gcc/libmpir.a
win32-msvc:LIBS		 += ../../external/lib/win32-msvc/mpirxx.lib ../../external/lib/win32-msvc/mpir.lib
#using a different compiled version for for msvc 2008 - 2013
win32-msvc2008:LIBS		 += ../../external/lib/win32-msvc2008/mpirxx.lib ../../external/lib/win32-msvc2008/mpir.lib
win32-msvc2010:LIBS		 += ../../external/lib/win32-msvc2008/mpirxx.lib ../../external/lib/win32-msvc2008/mpir.lib
win32-msvc2012:LIBS		 += ../../external/lib/win32-msvc2008/mpirxx.lib ../../external/lib/win32-msvc2008/mpir.lib
win32-msvc2013:LIBS		 += ../../external/lib/win32-msvc2008/mpirxx.lib ../../external/lib/win32-msvc2008/mpir.lib
linux:INCLUDEPATH += ../../external/inc/linux-g++/mpir-2.4.0
