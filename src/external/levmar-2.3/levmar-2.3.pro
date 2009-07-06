TEMPLATE = lib
TARGET = levmar
CONFIG += staticlib
DEPENDPATH += .
INCLUDEPATH += .

macx:DESTDIR       = ../lib/macx
win32-g++:DESTDIR       = ../lib/win32-gcc  
win32-msvc2005:DESTDIR       = ../lib/win32-msvc2005
win32-msvc2008:DESTDIR       = ../lib/win32-msvc2008
linux-g++-32:DESTDIR = ../lib/linux-g++-32
linux-g++-64:DESTDIR = ../lib/linux-g++-64
linux-g++:DESTDIR = ../lib/linux-g++

# Input
HEADERS += compiler.h \
		   lm.h\
		   misc.h
SOURCES += lmdemo.c\
		   Axb.c \
           Axb_core.c \
           expfit.c \
           lm.c \
           lm_core.c \
           lmbc.c \
           lmbc_core.c \
           lmblec.c \
           lmblec_core.c \
           lmlec.c \
           lmlec_core.c \
           misc.c\
           misc_core.c
 

