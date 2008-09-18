include (../../shared.pri)

HEADERS       = filter_func.h

SOURCES       = filter_func.cpp\ 
		../../meshlab/filterparameter.cpp

TARGET        = filter_func

INCLUDEPATH += ../../../../code/lib/muparser/include

# Note: we need static libs so when building muparser lib use 
# ./configure --enable-shared=no

win32-msvc2005:LIBS += ../../../../code/lib/muparser/lib/muparser.lib
mac:LIBS            += ../../../../code/lib/muparser/lib/libmuparser.a
