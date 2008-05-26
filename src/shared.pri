# this is the common include for all the plugins 

CONFIG *= debug_and_release
TEMPLATE      = lib
CONFIG       += plugin

VCGDIR  = ../../../../vcglib
GLEWDIR = ../../../../code/lib/glew 
GLEWCODE = $$GLEWDIR/src/glew.c
DEFINES *= GLEW_STATIC

INCLUDEPATH  *= ../.. $$VCGDIR $$GLEWDIR/include


# the following line is needed to avoid mismatch between 
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX

DESTDIR       = ../../meshlab/plugins

contains(TEMPLATE,lib) {
   CONFIG(debug, debug|release) {
      unix:TARGET = $$member(TARGET, 0)_debug
      else:TARGET = $$member(TARGET, 0)d
   }
}
