include (../../shared.pri)

HEADERS       = colladaio.h \
		$$VCGDIR/wrap/io_trimesh/export_dae.h \
		$$VCGDIR/wrap/io_trimesh/import_dae.h \
		$$VCGDIR/wrap/io_trimesh/util_dae.h


SOURCES       = colladaio.cpp \
				$$VCGDIR/wrap/dae/xmldocumentmanaging.cpp
TARGET        = colladaio

QT           += xml opengl

