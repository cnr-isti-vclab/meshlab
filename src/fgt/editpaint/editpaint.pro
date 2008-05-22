include (../../shared.pri)

QT += opengl

HEADERS = colorframe.h \
    paintbox.h \
    editpaint.h
SOURCES = paintbox.cpp \
    editpaint.cpp \
	$$GLEWCODE
TARGET = zpaint

RESOURCES = meshlab.qrc

FORMS = paintbox.ui
