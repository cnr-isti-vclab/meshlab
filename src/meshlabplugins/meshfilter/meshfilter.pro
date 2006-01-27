TEMPLATE      = lib
CONFIG       += plugin
INCLUDEPATH  += ../.. ../../../../sf ../../../../code/lib/glew/include
HEADERS       = 	../../meshlab/refineDialog.h \
../../meshlab/transformDialog.h \
../../meshlab/decimatorDialog.h \
detacher.h \
../../meshlab/detacherDialog.h \
../../meshlab/interfaces.h \
meshfilter.h 
SOURCES       = 		../../meshlab/transformDialog.cpp \
meshfilter.cpp \ 
../../meshlab/GLLogStream.cpp 

TARGET        = meshfilter
DESTDIR       = ../../meshlab/plugins
# the following line is needed to avoid mismatch between 
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX
# all dialog are stored in meshlab/ui directory
FORMS         = ../../meshlab/ui/refineDialog.ui \
 ../../meshlab/ui/decimatorDialog.ui \
 ../../meshlab/ui/transformDialog.ui \
 ../../meshlab/ui/detacherDialog.ui
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



