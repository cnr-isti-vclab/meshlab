include (../../shared.pri)

HEADERS += \
    filter_func.h

SOURCES += \
    filter_func.cpp

TARGET = filter_func

DEFINES += _UNICODE

!CONFIG(system_muparser) INCLUDEPATH += $$MESHLAB_EXTERNAL_DIRECTORY/muparser_v225/include

# Note: we need static libs so when building muparser lib use 
# ./configure --enable-shared=no

win32-msvc:LIBS += $$MESHLAB_DISTRIB_EXT_DIRECTORY/muparser.lib
win32-g++:LIBS += $$MESHLAB_DISTRIB_EXT_DIRECTORY/libmuparser.a
macx:LIBS += $$MESHLAB_DISTRIB_EXT_DIRECTORY/libmuparser.a
linux:LIBS += -L$$MESHLAB_DISTRIB_EXT_DIRECTORY -lmuparser
