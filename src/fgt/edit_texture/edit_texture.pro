TEMPLATE      = lib
CONFIG       += plugin
INCLUDEPATH  += ../.. ../../../../sf ../../../../code/lib/glew/include
HEADERS       = edittexture.h textureeditor.h renderarea.h ui_textureeditor.h ../../../../sf/wrap/gui/trackball.h
SOURCES       = edittexture.cpp ../../../../code/lib/glew/src/glew.c textureeditor.cpp renderarea.cpp ../../../../sf/wrap/gui/trackball.cpp
TARGET        = edit_texture
DESTDIR       = ../../meshlab/plugins
DEFINES += GLEW_STATIC
QT           += opengl
RESOURCES     = meshlab.qrc
FORMS         = textureeditor.ui

# the following line is needed to avoid mismatch between 
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX
CONFIG		+= debug_and_release

# mac:CONFIG += x86 ppc

contains(TEMPLATE,lib) {
   CONFIG(debug, debug|release) {
      unix:TARGET = $$member(TARGET, 0)_debug
      else:TARGET = $$member(TARGET, 0)d
   }
}



