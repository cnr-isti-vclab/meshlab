include (../../shared.pri)

HEADERS       = colladaio.h \
		$$VCGDIR/wrap/io_trimesh/export_dae.h \
		$$VCGDIR/wrap/io_trimesh/import_dae.h \
		$$VCGDIR/wrap/dae/util_dae.h \
		$$VCGDIR/wrap/dae/colladaformat.h \
		$$VCGDIR/wrap/dae/xmldocumentmanaging.h


SOURCES       = colladaio.cpp \
		../../meshlab/filterparameter.cpp\
		$$VCGDIR/wrap/dae/xmldocumentmanaging.cpp

TARGET        = colladaio

QT           += xml opengl

