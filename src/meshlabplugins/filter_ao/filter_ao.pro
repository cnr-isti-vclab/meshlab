
TEMPLATE = lib
CONFIG += plugin
TARGET = filter_ao
DESTDIR = ../../meshlab/plugins
QT += opengl
CONFIG += debug_and_release


INCLUDEPATH += ./../.. \
    ./../../../../sf \
    ./../../../../code/lib/glew/include \
    .
DEFINES += GLEW_STATIC

win32{
  DEFINES += NOMINMAX
}

OBJECTS_DIR += debug

HEADERS += ./filter_ao.h AOGLWidget.h
SOURCES += ./filter_ao.cpp AOGLWidget.cpp \
    ../../meshlab/filterparameter.cpp \
    ../../../../code/lib/glew/src/glew.c

