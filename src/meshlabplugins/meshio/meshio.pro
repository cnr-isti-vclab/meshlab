include (../../shared.pri)

HEADERS		= meshio.h \
			$$VCGDIR/wrap/ply/plylib.h \
			$$VCGDIR/wrap/io_trimesh/export_obj.h \
			$$VCGDIR/wrap/io_trimesh/export_3ds.h \
			$$VCGDIR/wrap/io_trimesh/import_obj.h \
			$$VCGDIR/wrap/io_trimesh/import_off.h \
			$$VCGDIR/wrap/io_trimesh/export_off.h \
			$$VCGDIR/wrap/io_trimesh/io_material.h \
			import_3ds.h \
			io_3ds.h 
				
SOURCES       = 	meshio.cpp \
			../../meshlab/filterparameter.cpp\
			$$VCGDIR/wrap/ply/plylib.cpp

TARGET        = meshio

INCLUDEPATH += ../../../../code/lib/lib3ds-1.3.0

win32-msvc.net:LIBS	+= ../../../../code/lib/lib3ds-1.3.0/lib3ds-130s.lib
win32-msvc2005:LIBS	+= ../../../../code/lib/lib3ds-1.3.0/msvc7/build/release/lib3ds-130s.lib
win32-g++:LIBS		+= ../../../../code/lib/lib3ds-1.3.0/lib3ds/.libs/lib3ds.a

unix:CONFIG	+= warn_off
unix:LIBS		+= -L../../../../code/lib/lib3ds-1.3.0/lib3ds/ -l3ds

mac:LIBS -= -l3ds 
mac:LIBS += ../../../../code/lib/lib3ds-1.3.0/lib3ds/lib3ds.a

