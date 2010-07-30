include (../../shared.pri)
HEADERS = filter_dirt.h \
          dirt_utils.h \
          dustparticle.h \
          dustsampler.h \
          $$VCGDIR/vcg/complex/trimesh/point_sampling.h

SOURCES = filter_dirt.cpp

TARGET  = filter_dirt

INCLUDEPATH += ../../external/muparser_v132/include

win32-msvc2005:LIBS += ../../external/lib/win32-msvc2005/muparser.lib
win32-msvc2008:LIBS += ../../external/lib/win32-msvc2008/muparser.lib
macx:LIBS += ../../external/lib/macx/libmuparser.a
linux-g++:LIBS += ../../external/lib/linux-g++/libmuparser.a
win32-g++:LIBS += ../../external/lib/win32-gcc/libmuparser.a
