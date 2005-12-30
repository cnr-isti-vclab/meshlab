TEMPLATE      = lib
CONFIG       += plugin
INCLUDEPATH  += ../.. ../../../../sf ../../../../code/lib/glew/include
HEADERS       = meshfilter.h refinedialog.h decimatordialog.h ../../meshlab/interfaces.h
SOURCES       = meshfilter.cpp 
TARGET        = meshfilter
DESTDIR       = ../../meshlab/plugins
# the following line is needed to avoid mismatch between 
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX
FORMS         = refine.ui decimator.ui
QT += opengl
unix{
	QMAKE_CC	 = gcc-3.3
	QMAKE_CXX	 = g++-3.3
	QMAKE_LINK	 = gcc-3.3
	CONFIG		+= warn_off debug_and_release
}

contains(TEMPLATE,lib) {
   CONFIG(debug, debug|release) {
      unix:TARGET = $$member(TARGET, 0)_debug
      else:TARGET = $$member(TARGET, 0)d
   }
}



