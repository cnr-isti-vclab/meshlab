EXTERNAL_BASE_PATH = ../
include($$EXTERNAL_BASE_PATH/ext_common.pri)
TEMPLATE = lib
TARGET = jhead
CONFIG += staticlib
DEPENDPATH += .
INCLUDEPATH += .
macx:QMAKE_CXXFLAGS += -Wno-unused-parameter -Wno-format -Wno-pointer-sign
macx:QMAKE_CXXFLAGS_WARN_ON = 

# Input
HEADERS += jhead.h

SOURCES += 		jpgfile.c \
				jhead.c  \
                paths.c \
                exif.c \
                iptc.c \
                gpsinfo.c \
                makernote.c

win32:SOURCES += myglob.c
