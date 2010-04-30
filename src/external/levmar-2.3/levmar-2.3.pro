TEMPLATE = lib
TARGET = levmar
CONFIG += staticlib
DEPENDPATH += .
INCLUDEPATH += .

macx:DESTDIR       = ../lib/macx
win32-g++:DESTDIR       = ../lib/win32-gcc  
win32-msvc2005:DESTDIR       = ../lib/win32-msvc2005
win32-msvc2008:DESTDIR       = ../lib/win32-msvc2008
unix:DESTDIR = ../lib/unix

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
 

