include (../../shared.pri)

HEADERS += \
    filter_qhull.h \
    qhull_tools.h

SOURCES += \
    filter_qhull.cpp \
    qhull_tools.cpp

INCLUDEPATH += \
    $$MESHLAB_EXTERNAL_DIRECTORY/qhull-2003.1/src

#TODO: compile qhull in external and then link it here
#      instead of including all the sources
SOURCES += \
    $$MESHLAB_EXTERNAL_DIRECTORY/qhull-2003.1/src/geom.c \
    $$MESHLAB_EXTERNAL_DIRECTORY/qhull-2003.1/src/geom2.c \
    $$MESHLAB_EXTERNAL_DIRECTORY/qhull-2003.1/src/global.c \
    $$MESHLAB_EXTERNAL_DIRECTORY/qhull-2003.1/src/io.c \
    $$MESHLAB_EXTERNAL_DIRECTORY/qhull-2003.1/src/mem.c \
    $$MESHLAB_EXTERNAL_DIRECTORY/qhull-2003.1/src/merge.c \
    $$MESHLAB_EXTERNAL_DIRECTORY/qhull-2003.1/src/poly.c \
    $$MESHLAB_EXTERNAL_DIRECTORY/qhull-2003.1/src/poly2.c \
    $$MESHLAB_EXTERNAL_DIRECTORY/qhull-2003.1/src/qconvex.c \
    $$MESHLAB_EXTERNAL_DIRECTORY/qhull-2003.1/src/qhull.c \
    $$MESHLAB_EXTERNAL_DIRECTORY/qhull-2003.1/src/qset.c \
    $$MESHLAB_EXTERNAL_DIRECTORY/qhull-2003.1/src/stat.c \
    $$MESHLAB_EXTERNAL_DIRECTORY/qhull-2003.1/src/user.c

TARGET = filter_qhull
