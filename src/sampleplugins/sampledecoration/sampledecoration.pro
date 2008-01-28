TEMPLATE      = lib
QT           += opengl
CONFIG       += plugin
INCLUDEPATH  += ../.. ../../../../sf ../../../../code/lib/glew/

HEADERS       = sampledecorate.h cubemap.h

SOURCES       = sampledecorate.cpp\ 
		cubemap.cpp \
		../../meshlab/filterparameter.cpp \
		../../../../code/lib/glew/src/glew.c

TARGET        = sampledecoration
DESTDIR       = ../../meshlab/plugins

# the following line is needed to avoid mismatch between 
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX
CONFIG		+= debug_and_release
DEFINES += GLEW_STATIC


contains(TEMPLATE,lib) {
   CONFIG(debug, debug|release) {
      unix:TARGET = $$member(TARGET, 0)_debug
      else:TARGET = $$member(TARGET, 0)d
   }
}

