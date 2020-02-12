include (../../shared.pri)

HEADERS += \
    filter_func.h

SOURCES += \
    filter_func.cpp

TARGET = filter_func

DEFINES += _UNICODE

INCLUDEPATH += $$MESHLAB_EXTERNAL_DIRECTORY/muparser_v225/include

# Note: we need static libs so when building muparser lib use 
# ./configure --enable-shared=no

win32-msvc:LIBS += $$MESHLAB_DISTRIB_DIRECTORY/lib/win32-msvc/muparser.lib
win32-g++:LIBS += $$MESHLAB_DISTRIB_DIRECTORY/lib/win32-gcc/libmuparser.a
macx:LIBS += $$MESHLAB_DISTRIB_DIRECTORY/lib/macx64/libmuparser.a
linux:LIBS += -lmuparser
