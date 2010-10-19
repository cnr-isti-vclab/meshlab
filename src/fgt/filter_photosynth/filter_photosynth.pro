include (../../shared.pri)

#INCLUDEPATH += C:\Emanuele\Uni\FGT\devel\qjson\qjson\src
#LIBS += C:\Emanuele\Uni\FGT\devel\meshlab\src\external\lib
#INCLUDEPATH += ../../external/qjson/src
#win32-g++:LIBS += ../../external/lib/win32-gcc/libqjson.a
#win32-g++:LIBS += -L../../external/lib/win32-gcc/ -lqjson

QTSOAPDIR = ../../external/qtsoap-2.7_1/src

INCLUDEPATH += $$QTSOAPDIR

HEADERS       += filter_photosynth.h \
                 synthData.h \
                 $$QTSOAPDIR/qtsoap.h

SOURCES       += filter_photosynth.cpp \
                 downloader.cpp \
                 $$QTSOAPDIR/qtsoap.cpp

TARGET         = filter_photosynth

QT += network
