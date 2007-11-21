TEMPLATE      = lib
CONFIG       += plugin
INCLUDEPATH  += ../.. ../../../../sf ../../../../code/lib/glew/include 

FORMS         = ui/param.ui \
				ui/cameraparametergui.ui \
				ui/converterdirectory.ui 				
HEADERS       = u3d_io.h \
				angleeditwidget.h \
				cameraparametergui.h \
				converterdirectory.h \
				framecontainer.h \
				param.h 
SOURCES       = u3d_io.cpp \
				cameraparametergui.cpp \
				converterdirectory.cpp \
				param.cpp 
			
TARGET        = u3d_io
DESTDIR       = ../../meshlab/plugins
CONFIG		+= debug_and_release


# the following line is needed to avoid mismatch between 
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX


				
CONFIG		+= debug_and_release
# mac:CONFIG += x86 ppc

contains(TEMPLATE,lib) {
   CONFIG(debug, debug|release) {
      unix:TARGET = $$member(TARGET, 0)_debug
      else:TARGET = $$member(TARGET, 0)d
   }
}

# mac:CONFIG += x86 ppc
