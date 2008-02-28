TEMPLATE      = lib
QT           += opengl
CONFIG       += plugin
INCLUDEPATH  +=  ../.. ../../../../sf ../../../../code/lib/glew/include
HEADERS       = filter_trioptimize.h

SOURCES       = filter_trioptimize.cpp \ 
		../../meshlab/filterparameter.cpp \
		../../../../code/lib/glew/src/glew.c

TARGET        = filter_trioptimize

DESTDIR       = ../../meshlab/plugins

# the following line is needed to avoid mismatch between 
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX
# mac:CONFIG += x86 ppc

DEFINES += GLEW_STATIC

contains(TEMPLATE,lib) {
   CONFIG(debug, debug|release) {
      unix:TARGET = $$member(TARGET, 0)_debug
      else:TARGET = $$member(TARGET, 0)d
   }
}
