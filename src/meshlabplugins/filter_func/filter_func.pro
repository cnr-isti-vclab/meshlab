include (../../shared.pri)

HEADERS       += filter_func.h

SOURCES       += filter_func.cpp

TARGET        = filter_func


DEFINES += _UNICODE
INCLUDEPATH += ../../external/muparser_v225/include

# Note: we need static libs so when building muparser lib use 
# ./configure --enable-shared=no

win32-msvc:LIBS += ../../external/lib/win32-msvc/muparser.lib
win32-msvc2005:LIBS += ../../external/lib/win32-msvc2005/muparser.lib
win32-msvc2008:LIBS += ../../external/lib/win32-msvc2008/muparser.lib
win32-msvc2010:LIBS += ../../external/lib/win32-msvc2010/muparser.lib
win32-msvc2012:LIBS += ../../external/lib/win32-msvc2012/muparser.lib
win32-msvc2013:LIBS += ../../external/lib/win32-msvc2013/muparser.lib
win32-msvc2015:LIBS += ../../external/lib/win32-msvc2015/muparser.lib

macx:LIBS            += $$MACLIBDIR/libmuparser.a
linux-g++:LIBS       += $$PWD/../../external/lib/linux-g++/libmuparser.a
linux-g++-32:LIBS       += $$PWD/../../external/lib/linux-g++-32/libmuparser.a
linux-g++-64:LIBS       += $$PWD/../../external/lib/linux-g++-64/libmuparser.a
win32-g++:LIBS		 += ../../external/lib/win32-gcc/libmuparser.a
