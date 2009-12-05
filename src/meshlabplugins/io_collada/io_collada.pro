include (../../shared.pri)

HEADERS       += io_collada.h \
		$$VCGDIR/wrap/io_trimesh/export_dae.h \
		$$VCGDIR/wrap/io_trimesh/import_dae.h \
		$$VCGDIR/wrap/dae/util_dae.h \
		$$VCGDIR/wrap/dae/colladaformat.h \
		$$VCGDIR/wrap/dae/xmldocumentmanaging.h


SOURCES       += io_collada.cpp \
        $$VCGDIR/wrap/dae/xmldocumentmanaging.cpp

TARGET        = io_collada

QT           += xml opengl

