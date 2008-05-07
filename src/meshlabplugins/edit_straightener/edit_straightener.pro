# Base options
TEMPLATE = lib
CONFIG += plugin

# QT modules
QT += opengl

# Options
CONFIG += debug_and_release
#mac:CONFIG += x86 ppc
contains(TEMPLATE,lib) {
   CONFIG(debug, debug|release) {
      unix:TARGET = $$member(TARGET, 0)_debug
      else:TARGET = $$member(TARGET, 0)d
   }
}

# Lib name
TARGET = editstraightener

# Directories
DESTDIR = ../../meshlab/plugins

# Lib headers
INCLUDEPATH += .
INCLUDEPATH += ../..
INCLUDEPATH += ../../../../sf

# Lib sources
SOURCES += ../../../../sf/wrap/gui/trackball.cpp
SOURCES += ../../../../sf/wrap/gui/trackmode.cpp 
SOURCES += ../../../../sf/wrap/gui/coordinateframe.cpp 
SOURCES += ../../../../sf/wrap/gui/activecoordinateframe.cpp 
SOURCES += ../../../../sf/wrap/gui/rubberband.cpp 

# Compile glew
DEFINES += GLEW_STATIC
INCLUDEPATH += ../../../../code/lib/glew/include
SOURCES += ../../../../code/lib/glew/src/glew.c

# Awful..
win32{
  DEFINES += NOMINMAX
}

# Input
HEADERS += editstraightener.h
HEADERS += editstraightenerdialog.h

SOURCES += editstraightener.cpp
SOURCES += editstraightenerdialog.cpp

FORMS += editstraightener.ui

RESOURCES += editstraightener.qrc
