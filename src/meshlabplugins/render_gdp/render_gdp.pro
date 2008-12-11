include (../../shared.pri)

HEADERS       = meshrender.h textfile.h shaderStructs.h shaderDialog.h ../../meshlab/meshmodel.h
SOURCES       = meshrender.cpp \
		textfile.cpp \  
		shaderDialog.cpp \
		$$GLEWCODE \
		../../meshlab/meshmodel.cpp

TARGET        = render_gdp

FORMS		= shaderDialog.ui

QT           += opengl xml
