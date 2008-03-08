TEMPLATE = lib
CONFIG += plugin
INCLUDEPATH += ../.. \
    ../../../../sf \
    ../../../../code/lib/glew/include
HEADERS = colorframe.h \
    paintbox.h \
    editpaint.h
SOURCES = paintbox.cpp \
    editpaint.cpp \
    ../../../../code/lib/glew/src/glew.c
TARGET = zpaint
DESTDIR = ../../meshlab/plugins
DEFINES += GLEW_STATIC
QT += opengl
RESOURCES = meshlab.qrc
CONFIG += debug_and_release

# mac:CONFIG += x86 ppc
contains(TEMPLATE,lib):CONFIG(debug, debug|release) { 
    unix:TARGET = $$member(TARGET, 0)_debug
    else:TARGET = $$member(TARGET, 0)d
}
win32:DEFINES += NOMINMAX
FORMS = paintbox.ui
