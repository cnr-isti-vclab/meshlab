TEMPLATE = lib
CONFIG += plugin
INCLUDEPATH += ../.. ../../../../sf ../../../../code/lib/glew/include
HEADERS = editsegment.h cutting.h curvaturetensor.h meshcutdialog.h ui_meshcutdialog.h
SOURCES = editsegment.cpp ../../../../code/lib/glew/src/glew.c meshcutdialog.cpp
TARGET = editsegment
DESTDIR = ../../meshlab/plugins
DEFINES += GLEW_STATIC
QT += opengl
#RESOURCES = meshlab.qrc
unix{
  QMAKE_CC = gcc
  QMAKE_CXX = g++
  QMAKE_LINK = gcc
  CONFIG += warn_off debug_and_release
}
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
#win32{
#  SOURCES += paintbox.cpp 
#  DEFINES += NOMINMAX
#}
