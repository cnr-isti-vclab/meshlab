TEMPLATE      = lib
CONFIG       += plugin 
INCLUDEPATH  += ../.. ../../../../sf ../../../../code/lib/glew/include
HEADERS       = filter_texture.h 
				
SOURCES       = filter_texture.cpp\ 
		../../meshlab/filterparameter.cpp
TARGET        = filter_texture
DESTDIR       = ../../meshlab/plugins

CONFIG		+= debug_and_release

# the following line is needed to avoid mismatch between 
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX

