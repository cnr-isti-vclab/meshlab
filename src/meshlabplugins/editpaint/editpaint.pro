include (../../shared.pri)

FORMS += PaintToolbox.ui
HEADERS = editpaint.h \
	colorwid.h 
SOURCES = editpaint.cpp $$GLEWCODE  \
	paintbox.cpp
TARGET = editpaint
QT += opengl
RESOURCES = meshlab.qrc
