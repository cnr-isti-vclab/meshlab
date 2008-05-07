TEMPLATE      = lib
CONFIG       += plugin
INCLUDEPATH  += ../.. ../../../../sf ../../../../code/lib/glew/include 

FORMS         = ui/u3d_gui.ui 				
HEADERS       = io_u3d.h \
				u3d_gui.h \
				../../../../sf/wrap/io_trimesh/export_u3d.h
SOURCES       = io_u3d.cpp \
				u3d_gui.cpp
TARGET        = io_u3d
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
