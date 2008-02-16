TEMPLATE      = lib
CONFIG       += plugin
INCLUDEPATH  += ../.. ../../../../sf ../../../../code/lib/glew/include
HEADERS       = editmeasure.h 
SOURCES       = editmeasure.cpp ../../../../code/lib/glew/src/glew.c ../../../../sf/wrap/gui/rubberband.cpp
TARGET        = editmeasure
DESTDIR       = ../../meshlab/plugins
DEFINES += GLEW_STATIC
QT           += opengl
RESOURCES     = editmeasure.qrc

# the following line is needed to avoid mismatch between 
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX

CONFIG		+= debug_and_release
#mac:CONFIG += x86 ppc

contains(TEMPLATE,lib) {
   CONFIG(debug, debug|release) {
      unix:TARGET = $$member(TARGET, 0)_debug
      else:TARGET = $$member(TARGET, 0)d
   }
}



