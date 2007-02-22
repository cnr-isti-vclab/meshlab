TEMPLATE      = lib
CONFIG       += plugin
INCLUDEPATH  += ../.. ../../../../sf ../../../../code/lib/glew/include
HEADERS       = meshedit.h 
SOURCES       = meshedit.cpp ../../../../code/lib/glew/src/glew.c
TARGET        = meshedit
DESTDIR       = ../../meshlab/plugins
DEFINES += GLEW_STATIC
QT           += opengl
RESOURCES     = meshlab.qrc

# the following line is needed to avoid mismatch between 
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX

contains(TEMPLATE,lib) {
   CONFIG(debug, debug|release) {
      unix:TARGET = $$member(TARGET, 0)_debug
      else:TARGET = $$member(TARGET, 0)d
   }
}



