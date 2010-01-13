VCGDIR  = ../../../vcglib
GLEWDIR = ../external/glew-1.5.1
GLEWCODE = $$GLEWDIR/src/glew.c


CONFIG(debug) {
	win32-msvc2005:DESTDIR     = ./debug
	win32-msvc2008:DESTDIR     = ./debug
    win32-g++:DLLDESTDIR     = ./debug
}

CONFIG(release) {
	win32-msvc2005:DESTDIR     = ./release
	win32-msvc2008:DESTDIR     = ./release
    win32-g++:DLLDESTDIR     = ./release
}

mac:DLLDESTDIR       = .


INCLUDEPATH  *= ../.. $$VCGDIR $$GLEWDIR/include

TEMPLATE = lib
win32-g++:CONFIG       += dll
macx-g++:CONFIG        += dll
unix:CONFIG            += dll
win32-msvc2005:CONFIG  += staticlib
win32-msvc2008:CONFIG  += staticlib

QT += opengl
QT += xml

TARGET = 
DEPENDPATH += .
INCLUDEPATH += .
DEFINES += GLEW_STATIC
win32-msvc2005:DEFINES	+= _CRT_SECURE_NO_WARNINGS
win32-msvc2008:DEFINES	+= _CRT_SECURE_NO_WARNINGS

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

