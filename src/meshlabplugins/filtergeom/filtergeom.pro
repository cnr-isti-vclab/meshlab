TEMPLATE      = lib
CONFIG       += plugin 
INCLUDEPATH  += ../.. ../../../../sf ../../../../code/lib/glew/include

HEADERS       = filtergeom.h 
				
SOURCES       = filtergeom.cpp
TARGET        = filtergeom
DESTDIR       = ../../meshlab/plugins

# the following line is needed to avoid mismatch between 
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX
mac:CONFIG += x86 ppc

contains(TEMPLATE,lib) {
   CONFIG(debug, debug|release) {
      unix:TARGET = $$member(TARGET, 0)_debug
      else:TARGET = $$member(TARGET, 0)d
   }
}
