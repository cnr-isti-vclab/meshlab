TEMPLATE = lib
CONFIG += plugin
INCLUDEPATH += ../.. ../../../../sf ../../../../code/lib/glew/include
HEADERS = editsegment.h cutting.h curvaturetensor.h meshcutdialog.h colorpicker.h
SOURCES = editsegment.cpp ../../../../code/lib/glew/src/glew.c meshcutdialog.cpp colorpiker.cpp
TARGET = editsegment
DESTDIR = ../../meshlab/plugins
DEFINES += GLEW_STATIC
QT += opengl
RESOURCES = meshlab.qrc
FORMS+=meshcutdialog.ui
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
#  DEFINES += NOMINMAX
#}
