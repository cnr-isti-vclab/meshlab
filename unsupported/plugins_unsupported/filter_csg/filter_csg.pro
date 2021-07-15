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

macx:LIBS += $$MESHLAB_DISTRIB_EXT_DIRECTORY/libmpirxx.a $$MESHLAB_DISTRIB_EXT_DIRECTORY/libmpir.a
win32-g++:LIBS += $$MESHLAB_DISTRIB_EXT_DIRECTORY/libmpirxx.a $$MESHLAB_DISTRIB_EXT_DIRECTORY/libmpir.a
win32-msvc:LIBS		 += $$MESHLAB_DISTRIB_EXT_DIRECTORY/mpirxx.lib $$MESHLAB_DISTRIB_EXT_DIRECTORY/mpir.lib

# Linux can use GMP instead, which is widely available
linux:DEFINES += BUILD_WITH_GMP
linux:LIBS += -lgmp
