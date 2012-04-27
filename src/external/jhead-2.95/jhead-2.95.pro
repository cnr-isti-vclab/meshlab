EXTERNAL_BASE_PATH = ../
include($$EXTERNAL_BASE_PATH/ext_common.pri)
TEMPLATE = lib
TARGET = jhead
CONFIG += staticlib
DEPENDPATH += .
INCLUDEPATH += .

# Input
HEADERS += jhead.h

SOURCES += 		jpgfile.c \
				jhead.c  \
                myglob.c \
                paths.c \
                exif.c \
                iptc.c \
                gpsinfo.c \
                makernote.c
