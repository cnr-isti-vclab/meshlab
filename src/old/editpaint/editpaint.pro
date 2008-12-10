include (../../shared.pri)

FORMS += PaintToolbox.ui

HEADERS   = edit_paint_factory.h \
			editpaint.h \
			colorwid.h
			 
SOURCES   = edit_paint_factory.h \
			editpaint.cpp \
			$$GLEWCODE  \
			paintbox.cpp
	
TARGET = editpaint

QT += opengl

RESOURCES = meshlab.qrc
