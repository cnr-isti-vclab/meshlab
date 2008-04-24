TEMPLATE      = lib
CONFIG       += plugin
INCLUDEPATH  += ../.. ../../../../sf ../../../../code/lib/glew/include 
HEADERS       = meshio.h \
				../../../../sf/wrap/ply/plylib.h \
				../../../../sf/wrap/io_trimesh/export_obj.h \
				../../../../sf/wrap/io_trimesh/export_3ds.h \
				../../../../sf/wrap/io_trimesh/import_obj.h \
				../../../../sf/wrap/io_trimesh/import_off.h \
				../../../../sf/wrap/io_trimesh/export_off.h \
				../../../../sf/wrap/io_trimesh/io_material.h \
				import_3ds.h \
				io_3ds.h 
				
SOURCES       = meshio.cpp \
				../../../../sf/wrap/ply/plylib.cpp
TARGET        = meshio
DESTDIR       = ../../meshlab/plugins	

INCLUDEPATH += ../../../../code/lib/lib3ds-1.3.0


win32-msvc.net:LIBS	+= ../../../../code/lib/lib3ds-1.3.0/lib3ds-130s.lib
win32-msvc2005:LIBS	+= ../../../../code/lib/lib3ds-1.3.0/msvc8/release/lib3ds-1_3.lib
win32-g++:LIBS	+= ../../../../code/lib/lib3ds-1.3.0/lib3ds/.libs/lib3ds.a

CONFIG		+= debug_and_release


unix{
	CONFIG		+= warn_off
	LIBS		+= -L../../../../code/lib/lib3ds-1.3.0/lib3ds/ -l3ds
}

# mac:LIBS +=  -L../../../../code/lib/lib3ds-1.3.0/ -l3ds 
mac:LIBS += ../../../../code/lib/lib3ds-1.3.0/lib3ds/lib3ds.a

contains(TEMPLATE,lib) {
   CONFIG(debug, debug|release) {
      unix:TARGET = $$member(TARGET, 0)_debug
      else:TARGET = $$member(TARGET, 0)d
   }
}

# the following line is needed to avoid mismatch between 
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX
