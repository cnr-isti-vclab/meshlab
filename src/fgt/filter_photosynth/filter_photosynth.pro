include (../../shared.pri)
include(../../external/qtsoap-2.7_1/src/qtsoap.pri)

#INCLUDEPATH += C:\Emanuele\Uni\FGT\devel\qjson\qjson\src
#LIBS += C:\Emanuele\Uni\FGT\devel\meshlab\src\external\lib
#INCLUDEPATH += ../../external/qjson/src
#win32-g++:LIBS += ../../external/lib/win32-gcc/libqjson.a
#win32-g++:LIBS += -L../../external/lib/win32-gcc/ -lqjson

HEADERS       += filter_photosynth.h \
    synthData.h
SOURCES       += filter_photosynth.cpp \ 
    downloader.cpp
TARGET        = filter_photosynth
TEMPLATE      = lib
CONFIG       += plugin
