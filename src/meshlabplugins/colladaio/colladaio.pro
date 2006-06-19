TEMPLATE      = lib
CONFIG       += plugin
INCLUDEPATH  += ../.. ../../../../sf ../../../../code/lib/glew/include ../../../../code/lib/glut/include ../../../../code/lib/FCollada/ ../../../../code/lib/FCollada/LibXML/include
HEADERS       = colladaio.h \
		    io_dae.h
SOURCES       = colladaio.cpp
TARGET        = colladaio
DESTDIR       = ../../meshlab/plugins

win32-msvc.net:LIBS	+= ../../../../code/lib/FCollada/FColladaSU_MTD.lib
win32-msvc.net:LIBS     -= LIBCMTD.lib

# the following line is needed to avoid mismatch between 
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX
unix{
	QMAKE_CC	 = gcc
	QMAKE_CXX	 = g++
	QMAKE_LINK	 = gcc
	CONFIG		+= warn_off debug_and_release
	LIBS		+= -l3ds
}


contains(TEMPLATE,lib) {
   CONFIG(debug, debug|release) {
      unix:TARGET = $$member(TARGET, 0)_debug
      else:TARGET = $$member(TARGET, 0)d
   }
}
