TEMPLATE      = lib
CONFIG       += plugin
INCLUDEPATH  += ../.. ../../../../sf ../../../../code/lib/glew/include
HEADERS       = colladaio.h 
SOURCES       = colladaio.cpp
TARGET        = colladaio
DESTDIR       = ../../meshlab/plugins
CONFIG		+= debug_and_release
mac:CONFIG += x86 ppc


win32-msvc.net:LIBS     -= LIBCMTD.lib

QT           += opengl 
QT           += xml

# the following line is needed to avoid mismatch between 
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX


contains(TEMPLATE,lib) {
   CONFIG(debug, debug|release) {
      unix:TARGET = $$member(TARGET, 0)_debug
      else:TARGET = $$member(TARGET, 0)d
   }
}
