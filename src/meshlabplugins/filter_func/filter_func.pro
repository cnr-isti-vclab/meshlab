include (../../shared.pri)

HEADERS       += filter_func.h

SOURCES       += filter_func.cpp

TARGET        = filter_func


DEFINES += _UNICODE
INCLUDEPATH += ../../external/muparser_v225/include

# Note: we need static libs so when building muparser lib use 
# ./configure --enable-shared=no

win32-msvc:LIBS += ../../external/lib/win32-msvc/muparser.lib

macx:LIBS            += $$MACLIBDIR/libmuparser.a
linux-g++:LIBS       += -lmuparser
win32-g++:LIBS		 += ../../external/lib/win32-gcc/libmuparser.a
