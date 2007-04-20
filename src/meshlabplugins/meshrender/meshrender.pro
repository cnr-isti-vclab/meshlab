TEMPLATE      = lib
CONFIG       += plugin
INCLUDEPATH  += ../.. ../../../../sf ../../../../code/lib/glew/include
#LIBPATH			 += ../../../../code/lib/glew/lib/
HEADERS       = meshrender.h textfile.h shaderStructs.h shaderDialog.h
SOURCES       = meshrender.cpp textfile.cpp shaderDialog.cpp  ../../../../code/lib/glew/src/glew.c

TARGET        = meshrender
DESTDIR       = ../../meshlab/plugins
# the following line is needed to avoid mismatch between 
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX
//win32:LIBS += QtXml4.lib
FORMS		  = shaderDialog.ui
DEFINES += GLEW_STATIC
CONFIG		+= debug_and_release
mac:CONFIG += x86 ppc


contains(TEMPLATE,lib) {
   CONFIG(debug, debug|release) {
      unix:TARGET = $$member(TARGET, 0)_debug
      else:TARGET = $$member(TARGET, 0)d
   }
}
QT           += opengl xml
