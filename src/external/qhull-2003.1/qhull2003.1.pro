include(../ext_common.pri)
TEMPLATE = lib
TARGET = qhull
CONFIG += staticlib

INCLUDEPATH += src/

SOURCES += \
    src/geom.c \
    src/geom2.c \
    src/global.c \
    src/io.c \
    src/mem.c \
    src/merge.c \
    src/poly.c \
    src/poly2.c \
    src/qconvex.c \
    src/qhull.c \
    src/qset.c \
    src/stat.c \
    src/user.c
