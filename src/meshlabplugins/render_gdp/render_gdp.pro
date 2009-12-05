include (../../shared.pri)

HEADERS       = meshrender.h \
                textfile.h \
                shaderStructs.h \
                shaderDialog.h 

SOURCES       = meshrender.cpp \
		textfile.cpp \  
        shaderDialog.cpp 

TARGET        = render_gdp

FORMS		= shaderDialog.ui

QT           += opengl xml
