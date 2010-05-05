TEMPLATE = lib
TARGET = levmar
CONFIG += staticlib
DEPENDPATH += .
INCLUDEPATH += .

macx:DESTDIR       = ../lib/macx
win32-g++:DESTDIR       = ../lib/win32-gcc  
win32-msvc2005:DESTDIR       = ../lib/win32-msvc2005
win32-msvc2008:DESTDIR       = ../lib/win32-msvc2008
linux-g++:DESTDIR = ../lib/linux-g++
linux-g++-32:DESTDIR = ../lib/linux-g++-32
linux-g++-64:DESTDIR = ../lib/linux-g++-64

# Input
HEADERS += compiler.h \
		   lm.h\
		   misc.h
SOURCES += Axb.c \
           lm.c \
           lmbc.c \
           lmblec.c \
           lmlec.c \
           misc.c\
 

