include (../../shared.pri)

HEADERS = filter_dirt.h \
    dustparticle.h \
    dustsampler.h \
    $$VCGDIR/vcg/complex/trimesh/point_sampling.h
SOURCES = filter_dirt.cpp 

INCLUDEPATH += ../../external/muparser_v130/include

win32-msvc2005:LIBS += ../../external/lib/win32-msvc2005/muparser.lib
win32-msvc2008:LIBS += ../../external/lib/win32-msvc2008/muparser.lib
macx:LIBS           += ../../external/lib/macx/libmuparser.a
linux-g++:LIBS      += ../../external/lib/linux-g++/libmuparser.a
win32-g++:LIBS	    += ../../external/lib/win32-gcc/libmuparser.a

TARGET = filter_dirt
TEMPLATE = lib
QT += opengl
CONFIG += plugin

