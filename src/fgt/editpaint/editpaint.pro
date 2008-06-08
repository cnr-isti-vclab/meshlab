include (../../shared.pri)
QT += opengl
HEADERS = cloneview.h \
    colorframe.h \
    paintbox.h \
    editpaint.h
SOURCES = paintbox.cpp \
    editpaint.cpp \
    $$GLEWCODE
TARGET = editpaint
RESOURCES = meshlab.qrc
FORMS = paintbox.ui
