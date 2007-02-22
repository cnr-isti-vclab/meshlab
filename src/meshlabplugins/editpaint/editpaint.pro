
TEMPLATE = lib
CONFIG += plugin
INCLUDEPATH += ../.. ../../../../sf ../../../../code/lib/glew/include
HEADERS = editpaint.h \
	colorwid.h \
	ui_painttoolbox.h
SOURCES = editpaint.cpp ../../../../code/lib/glew/src/glew.c \
	paintbox.cpp
TARGET = editpaint
DESTDIR = ../../meshlab/plugins
DEFINES += GLEW_STATIC
QT += opengl
RESOURCES = meshlab.qrc

contains(TEMPLATE,lib){
  CONFIG(debug, debug|release){
    unix{
      TARGET = $$member(TARGET, 0)_debug
    }
    else{
      TARGET = $$member(TARGET, 0)d
    }
  }
}
win32{
  SOURCES += paintbox.cpp 
  DEFINES += NOMINMAX
}
