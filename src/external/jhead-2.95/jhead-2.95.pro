TEMPLATE = lib
TARGET = jhead
CONFIG += staticlib
DEPENDPATH += .
INCLUDEPATH += .


macx:DESTDIR       = ../lib/macx
macx32:DESTDIR       = ../lib/macx32
macx64:DESTDIR       = ../lib/macx64
win32-g++:DESTDIR       = ../lib/win32-gcc  
win32-msvc2005:DESTDIR       = ../lib/win32-msvc2005
win32-msvc2008:DESTDIR       = ../lib/win32-msvc2008
linux-g++:DESTDIR = ../lib/linux-g++
linux-g++-32:DESTDIR = ../lib/linux-g++-32
linux-g++-64:DESTDIR = ../lib/linux-g++-64




# Input
HEADERS += jhead.h
win32:SOURCES   +=  myglob.c
SOURCES         +=  jpgfile.c \
                    jhead.c  \
                    paths.c \
                    exif.c \
                    iptc.c \
                    gpsinfo.c \
                    makernote.c
