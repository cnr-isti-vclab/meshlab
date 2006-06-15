TEMPLATE      = lib
CONFIG       += plugin
INCLUDEPATH  += ../.. ../../../../sf ../../../../code/lib/glew/include ../../../../code/lib/lib3ds-1.2.0
HEADERS       = meshio.h \
				../../../../sf/wrap/ply/plylib.h \
				../../../../sf/wrap/io_trimesh/export_obj.h \
				../../../../sf/wrap/io_trimesh/export_3ds.h \
				../../../../sf/wrap/io_trimesh/import_obj.h \
				../../../../sf/wrap/io_trimesh/io_material.h \
				import_3ds.h \
				io_3ds.h 
				
SOURCES       = meshio.cpp \
				../../../../sf/wrap/ply/plylib.cpp
TARGET        = meshio
DESTDIR       = ../../meshlab/plugins

win32-msvc.net:LIBS	+= ../../../../code/lib/lib3ds-1.2.0/lib3ds-120s.lib

win32-g++:LIBS	+= ../../../../code/lib/lib3ds-1.2.0/lib3ds/lib3ds.a

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
