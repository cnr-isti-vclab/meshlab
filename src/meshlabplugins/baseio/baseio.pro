include (../../shared.pri)

TEMPLATE      = lib
CONFIG       += plugin

INCLUDEPATH  += ../.. $$VCGDIR ../../../../code/lib/glew/include 
HEADERS       = baseio.h \
		$$VCGDIR/wrap/ply/plylib.h \
		$$VCGDIR/wrap/io_trimesh/io_material.h \
				
SOURCES       = baseio.cpp \
		$$VCGDIR//wrap/ply/plylib.cpp
TARGET        = baseio


