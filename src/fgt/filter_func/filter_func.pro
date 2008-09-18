include (../../shared.pri)

HEADERS       = filter_func.h

SOURCES       = filter_func.cpp\ 
		../../meshlab/filterparameter.cpp

TARGET        = filter_func

INCLUDEPATH += ../../../../code/lib/muparser/include

win32-msvc2005:LIBS += ../../../../code/lib/muparser/lib/muparser.lib