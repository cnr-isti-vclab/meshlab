TEMPLATE      = lib
CONFIG       += plugin
INCLUDEPATH  += ../.. ../../../../sf ../../../../code/lib/glew/include 
HEADERS       = meshio.h ui_savemask.h savemaskdialog.h
SOURCES       = meshio.cpp savemaskdialog.cpp
TARGET        = meshio
DESTDIR       = ../../meshlab/plugins

# the following line is needed to avoid mismatch between 
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX
unix{
	QMAKE_CC	 = gcc-3.3
	QMAKE_CXX	 = g++-3.3
	QMAKE_LINK	 = gcc-3.3
	CONFIG		+= debug warn_off
}


contains(TEMPLATE,lib) {
   CONFIG(debug, debug|release) {
      unix:TARGET = $$member(TARGET, 0)_debug
      else:TARGET = $$member(TARGET, 0)d
   }
}
