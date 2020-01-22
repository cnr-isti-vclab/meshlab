EXTERNAL_BASE_PATH = ../../
include($$EXTERNAL_BASE_PATH/ext_common.pri)
TEMPLATE = lib
TARGET = 3ds
CONFIG += staticlib
DEPENDPATH += .
INCLUDEPATH += ..
DEFINES += LIB3DS_STATIC

# Input
HEADERS += \
    atmosphere.h \
    background.h \
    camera.h \
    chunk.h \
    chunktable.h \
    ease.h \
    file.h \
    io.h \
    light.h \
    material.h \
    matrix.h \
    mesh.h \
    node.h \
    quat.h \
    shadow.h \
    tcb.h \
    tracks.h \
    types.h \
    vector.h \
    viewport.h

SOURCES += \
    atmosphere.c \
    background.c \
    camera.c \
    chunk.c \
    ease.c \
    file.c \
    io.c \
    light.c \
    material.c \
    matrix.c \
    mesh.c \
    node.c \
    quat.c \
    shadow.c \
    tcb.c \
    tracks.c \
    vector.c \
    viewport.c
