TEMPLATE      = lib
CONFIG       += plugin
INCLUDEPATH  += ../.. ../../../../sf ../../../../code/lib/glew/include
HEADERS       = editslice.h \
		slicedialog.h\

SOURCES       = editslice.cpp \
		slicedialog.cpp\
		 ../../../../code/lib/glew/src/glew.c \
                 ../../../../sf/wrap/gui/trackball.cpp\
                 ../../../../sf/wrap/gui/trackmode.cpp \
		
TARGET        = editslice
DESTDIR       = ../../meshlab/plugins
DEFINES += GLEW_STATIC
QT           += opengl
RESOURCES     = meshlab.qrc
FORMS         = slicedialog.ui
# the following line is needed to avoid mismatch between 
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX
CONFIG		+= debug_and_release

contains(TEMPLATE,lib) {
   CONFIG(debug, debug|release) {
      unix:TARGET = $$member(TARGET, 0)_debug
      else:TARGET = $$member(TARGET, 0)d
   }
}



