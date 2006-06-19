TEMPLATE      = lib
CONFIG       += plugin
INCLUDEPATH  +=  ../.. ../../../../sf ../../../../code/lib/glew/include
HEADERS       = transformDialog.h \
                ../../../../sf/vcg/complex/trimesh/clean.h \
                ../../meshlab/GenericELDialog.h \
                ../../meshlab/interfaces.h \
                meshfilter.h 

SOURCES       = transformDialog.cpp \
				meshfilter.cpp \ 
				../../meshlab/GLLogStream.cpp 

TARGET        = meshfilter
DESTDIR       = ../../meshlab/plugins
# the following line is needed to avoid mismatch between 
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX
# all dialog are stored in meshlab/ui directory
FORMS         = ui/transformDialog.ui ../../meshlab/ui/genericELDialog.ui

QT += opengl
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



