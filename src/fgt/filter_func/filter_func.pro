include (../../shared.pri)

HEADERS       += filter_func.h

SOURCES       += filter_func.cpp

TARGET        = filter_func

INCLUDEPATH += ../../external/muparser_v130/include

# Note: we need static libs so when building muparser lib use 
# ./configure --enable-shared=no

win32-msvc2005:LIBS += ../../external/lib/win32-msvc2005/muparser.lib
win32-msvc2008:LIBS += ../../external/lib/win32-msvc2008/muparser.lib
macx:LIBS            += ../../external/lib/macx/libmuparser.a
linux-g++:LIBS       += ../../external/lib/linux-g++/libmuparser.a
linux-g++-32:LIBS       += ../../external/lib/linux-g++-32/libmuparser.a
linux-g++-64:LIBS       += ../../external/lib/linux-g++-64/libmuparser.a
win32-g++:LIBS		 += ../../external/lib/win32-gcc/libmuparser.a
