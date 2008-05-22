include (../../shared.pri)

HEADERS       = colladaio.h 

SOURCES       = colladaio.cpp \
				$$VCGDIR/wrap/dae/xmldocumentmanaging.cpp
TARGET        = colladaio

QT           += xml opengl

