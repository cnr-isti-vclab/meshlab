TEMPLATE      = lib
CONFIG       += plugin
INCLUDEPATH  += ../.. ../../../../sf ../../../../code/lib/glew/include
HEADERS       = meshselect.h 
SOURCES       = meshselect.cpp
TARGET        = meshselect
DESTDIR       = ../../meshlab/plugins
RESOURCES     = meshlab.qrc

# the following line is needed to avoid mismatch between 
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX
CONFIG		+= debug_and_release
mac:CONFIG += x86 ppc

contains(TEMPLATE,lib) {
   CONFIG(debug, debug|release) {
      unix:TARGET = $$member(TARGET, 0)_debug
      else:TARGET = $$member(TARGET, 0)d
   }
}
