include (../../shared.pri)

HEADERS       += baseio.h \
		$$VCGDIR/wrap/io_trimesh/import_obj.h \
		$$VCGDIR/wrap/io_trimesh/import_off.h \
		$$VCGDIR/wrap/io_trimesh/export_off.h \
		$$VCGDIR/wrap/ply/plylib.h \
		$$VCGDIR/wrap/io_trimesh/io_material.h \
				
SOURCES       += baseio.cpp \
		$$VCGDIR//wrap/ply/plylib.cpp\ 
		

TARGET        = io_base
