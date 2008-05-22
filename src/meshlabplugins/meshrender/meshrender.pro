include (../../shared.pri)

HEADERS       = meshrender.h textfile.h shaderStructs.h shaderDialog.h
SOURCES       = meshrender.cpp \
		textfile.cpp \  
		shaderDialog.cpp \
		$$GLEWCODE

TARGET        = meshrender

FORMS		= shaderDialog.ui

QT           += opengl xml
