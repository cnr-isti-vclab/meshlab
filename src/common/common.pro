VCGDIR  = ../../../vcglib
GLEWDIR = ../external/glew-1.5.1
GLEWCODE = $$GLEWDIR/src/glew.c

mac:DESTDIR       = ../meshlab/meshlab.app/Contents/MacOS
win32-g++:debug:DESTDIR       = ../meshlab/debug
win32-g++:release:DESTDIR       = ../meshlab/release
INCLUDEPATH  *= ../.. $$VCGDIR $$GLEWDIR/include

TEMPLATE = lib
CONFIG += dll
QT += opengl
QT += xml

TARGET = 
DEPENDPATH += .
INCLUDEPATH += .
win32-g++:DEFINES += GLEW_STATIC
# Input
HEADERS += filterparameter.h \
           filterscript.h \
           GLLogStream.h \
           interfaces.h \
           meshmodel.h \
           pluginmanager.h
SOURCES += filterparameter.cpp \
           filterscript.cpp \
           GLLogStream.cpp \
           meshmodel.cpp \
           pluginmanager.cpp \
           $$GLEWCODE

