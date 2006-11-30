TEMPLATE      = lib
CONFIG       += plugin
INCLUDEPATH  += ../.. ../../../../sf ../../../../code/lib/glew/include 
HEADERS       = baseio.h \
				../../../../sf/wrap/ply/plylib.h \
				../../../../sf/wrap/io_trimesh/io_material.h \
				
SOURCES       = baseio.cpp \
				../../../../sf/wrap/ply/plylib.cpp
TARGET        = baseio
DESTDIR       = ../../meshlab/plugins


# the following line is needed to avoid mismatch between 
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX
unix{
	QMAKE_CC	 = gcc
	QMAKE_CXX	 = g++
	QMAKE_LINK	 = gcc
	CONFIG		+= warn_off debug_and_release
}


contains(TEMPLATE,lib) {
   CONFIG(debug, debug|release) {
      unix:TARGET = $$member(TARGET, 0)_debug
      else:TARGET = $$member(TARGET, 0)d
   }
}
