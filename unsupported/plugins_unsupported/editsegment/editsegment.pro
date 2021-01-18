include (../../shared.pri)

HEADERS = edit_segment_factory.h \ 	
		editsegment.h \
		cutting.h \
		curvaturetensor.h \
		meshcutdialog.h \
		colorpicker.h
		
SOURCES = edit_segment_factory.cpp \
		editsegment.cpp \
		meshcutdialog.cpp \
		colorpiker.cpp

TARGET = editsegment
QT += opengl
RESOURCES = meshlab.qrc
FORMS+=meshcutdialog.ui

