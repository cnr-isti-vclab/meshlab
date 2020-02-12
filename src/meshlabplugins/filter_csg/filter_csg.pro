include (../../shared.pri)

HEADERS += \
    filter_csg.h \
    intercept.h \
    gmpfrac.h

SOURCES += \
    filter_csg.cpp

TARGET = filter_csg

macx:INCLUDEPATH += $$MESHLAB_EXTERNAL_DIRECTORY/inc/macx64/mpir-2.4.0
win32-g++:INCLUDEPATH += $$MESHLAB_EXTERNAL_DIRECTORY/inc/win32-gcc/mpir-2.2.1
win32-msvc:INCLUDEPATH += $$MESHLAB_EXTERNAL_DIRECTORY/inc/win32-msvc/mpir-2.2.1_x64

macx:LIBS += $$MESHLAB_DISTRIB_DIRECTORY/lib/macx64/libmpirxx.a $$MESHLAB_DISTRIB_DIRECTORY/lib/macx64/libmpir.a
win32-g++:LIBS += $$MESHLAB_DISTRIB_DIRECTORY/lib/win32-gcc/libmpirxx.a $$MESHLAB_DISTRIB_DIRECTORY/lib/win32-gcc/libmpir.a
win32-msvc:LIBS		 += $$MESHLAB_DISTRIB_DIRECTORY/lib/win32-msvc/mpirxx.lib $$MESHLAB_DISTRIB_DIRECTORY/lib/win32-msvc/mpir.lib
#using a different compiled version for for msvc 2008 - 2013
win32-msvc2008:LIBS		 += $$MESHLAB_DISTRIB_DIRECTORY/lib/win32-msvc2008/mpirxx.lib $$MESHLAB_DISTRIB_DIRECTORY/win32-msvc2008/mpir.lib
win32-msvc2010:LIBS		 += $$MESHLAB_DISTRIB_DIRECTORY/lib/win32-msvc2008/mpirxx.lib $$MESHLAB_DISTRIB_DIRECTORY/win32-msvc2008/mpir.lib
win32-msvc2012:LIBS		 += $$MESHLAB_DISTRIB_DIRECTORY/win32-msvc2008/mpirxx.lib $$MESHLAB_DISTRIB_DIRECTORY/win32-msvc2008/mpir.lib
win32-msvc2013:LIBS		 += $$MESHLAB_DISTRIB_DIRECTORY/win32-msvc2008/mpirxx.lib $$MESHLAB_DISTRIB_DIRECTORY/win32-msvc2008/mpir.lib

# Linux can use GMP instead, which is widely available
linux:DEFINES += BUILD_WITH_GMP
linux:LIBS += -lgmp
