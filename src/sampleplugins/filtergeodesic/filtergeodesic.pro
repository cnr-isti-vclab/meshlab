TEMPLATE      = lib
CONFIG       += plugin 
INCLUDEPATH  += ../.. ../../../../sf ../../../../code/lib/glew/include
HEADERS       = filtergeodesic.h 
				
SOURCES       = filtergeodesic.cpp\ 
		../../meshlab/filterparameter.cpp
TARGET        = filtergeodesic
DESTDIR       = ../../meshlab/plugins

CONFIG		+= debug_and_release

# the following line is needed to avoid mismatch between 
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX



contains(TEMPLATE,lib) {
   CONFIG(debug, debug|release) {
      unix:TARGET = $$member(TARGET, 0)_debug
      else:TARGET = $$member(TARGET, 0)d
   }
}
