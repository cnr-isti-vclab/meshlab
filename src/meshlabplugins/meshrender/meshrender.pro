TEMPLATE      = lib
CONFIG       += plugin
INCLUDEPATH  += ../.. ../../../../sf ../../../../code/lib/glew/include
#LIBPATH			 += ../../../../code/lib/glew/lib/
HEADERS       = meshrender.h textfile.h shaderStructs.h shaderDialog.h
SOURCES       = meshrender.cpp textfile.cpp shaderDialog.cpp
TARGET        = meshrender
DESTDIR       = ../../meshlab/plugins
# the following line is needed to avoid mismatch between 
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX
win32:LIBS += ../../../../code/lib/glew/lib/glew32.lib QtXml4.lib
FORMS		  = shaderDialog.ui

unix{
	QMAKE_CC	 = gcc
	QMAKE_CXX	 = g++
	QMAKE_LINK	 = gcc
	CONFIG		+= warn_off debug_and_release
	QT		+= xml
}

contains(TEMPLATE,lib) {
   CONFIG(debug, debug|release) {
      unix:TARGET = $$member(TARGET, 0)_debug
      else:TARGET = $$member(TARGET, 0)d
   }
}
QT           += opengl
