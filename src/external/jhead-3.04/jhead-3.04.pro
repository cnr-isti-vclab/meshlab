include(../ext_common.pri)
TEMPLATE = lib
TARGET = jhead
CONFIG += staticlib
DEPENDPATH += .
INCLUDEPATH += .

# Input
HEADERS += jhead.h

SOURCES += \
    jpgfile.c \
    jpgqguess.c \
    jhead.c  \
    paths.c \
    exif.c \
    iptc.c \
    gpsinfo.c \
    makernote.c

win32:SOURCES += myglob.c

DEFINES += main="jhead_main"
