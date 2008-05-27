include (../../shared.pri)

HEADERS = 	editsegment.h \
		cutting.h \
		curvaturetensor.h \
		meshcutdialog.h \
		colorpicker.h
SOURCES = 	editsegment.cpp \
		$$GLEWCODE \
		meshcutdialog.cpp \
		colorpiker.cpp

TARGET = editsegment
QT += opengl
RESOURCES = meshlab.qrc
FORMS+=meshcutdialog.ui

