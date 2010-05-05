include (../../shared.pri)

HEADERS		= meshio.h \
			$$VCGDIR/wrap/ply/plylib.h \
			$$VCGDIR/wrap/io_trimesh/export_obj.h \
			$$VCGDIR/wrap/io_trimesh/export_3ds.h \
			$$VCGDIR/wrap/io_trimesh/io_material.h \
			import_3ds.h \
			io_3ds.h 
				
SOURCES       += 	meshio.cpp \
			


TARGET        = io_3ds

INCLUDEPATH += ../../external/lib3ds-1.3.0/

# Notes on the paths of lib3ds files.
# Once you have compiled the library put the lib files in a dir named lib/XXX/ 
# where XXX is the name of your system according to the QT naming style.
# Linux users can rely on their own package managers and hope that it is installing the last version of lib3ds

win32-msvc.net:LIBS	+= ../../external/lib/win32-msvc.net/3ds.lib
win32-msvc2005:LIBS	+= ../../external/lib/win32-msvc2005/3ds.lib
win32-msvc2008:LIBS	+= ../../external/lib/win32-msvc2008/3ds.lib
win32-g++:LIBS		+= ../../external/lib/win32-gcc/lib3ds.a
linux-g++:LIBS += ../../external/lib/linux-g++/lib3ds.a
linux-g++-32:LIBS += ../../external/lib/linux-g++-32/lib3ds.a
linux-g++-64:LIBS += ../../external/lib/linux-g++-64/lib3ds.a

# unix:LIBS		+= -L../../../../code/lib/lib3ds-1.3.0/lib3ds/lib/unix -l3ds

# mac:LIBS -= -l3ds 
# mac:LIBS += ../../../../code/lib/lib3ds-1.3.0/lib/mac/lib3ds.a

macx:LIBS += ../../external/lib/macx/lib3ds.a

