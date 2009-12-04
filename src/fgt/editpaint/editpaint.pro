include (../../shared.pri)

QT += opengl

HEADERS = edit_paint_factory.h \
	cloneview.h \
    colorframe.h \
    paintbox.h \
    editpaint.h

SOURCES = edit_paint_factory.cpp \
	paintbox.cpp \
    editpaint.cpp

TARGET = editpaint
RESOURCES = meshlab.qrc
FORMS = paintbox.ui
