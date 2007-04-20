TEMPLATE      = lib
CONFIG       += plugin
INCLUDEPATH  += ../.. ../../../../sf ../../../../code/lib/glew/include 
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

win32-msvc.net:LIBS	+= ../../../../code/lib/lib3ds-1.3.0/lib3ds-130s.lib
win32-msvc2005:LIBS	+= ../../../../code/lib/lib3ds-1.3.0/lib3ds-130s.lib
win32-msvc2005:INCLUDEPATH += ../../../../code/lib/lib3ds-1.3.0
win32-g++:LIBS	+= ../../../../code/lib/lib3ds-1.2.0/lib3ds/lib3ds.a
mac:INCLUDEPATH  += ../../../../code/lib/lib3ds-1.2.0
win32-g++:INCLUDEPATH  += ../../../../code/lib/lib3ds-1.2.0

CONFIG		+= debug_and_release
mac:CONFIG += x86 ppc

unix{
	CONFIG		+= warn_off debug_and_release
#	LIBS		+= -L../../../../code/lib/lib3ds-1.3.0/lib3ds/ -l3ds
}

mac:LIBS +=  -L../../../../code/lib/lib3ds-1.2.0/macos/ -l3ds 

contains(TEMPLATE,lib) {
   CONFIG(debug, debug|release) {
      unix:TARGET = $$member(TARGET, 0)_debug
      else:TARGET = $$member(TARGET, 0)d
   }
}

# the following line is needed to avoid mismatch between 
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX
