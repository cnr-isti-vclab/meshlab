include (../../shared.pri)

FORMS += PaintToolbox.ui

HEADERS   = edit_paint_factory.h \
			editpaint.h \
			colorwid.h
			 
SOURCES   = edit_paint_factory.h \
			editpaint.cpp \
			paintbox.cpp
!CONFIG(system_glew): SOURCES += $$GLEWCODE
CONFIG(system_glew) {
    linux: LIBS += -lGLEW
}
	
TARGET = editpaint

QT += opengl

RESOURCES = meshlab.qrc
