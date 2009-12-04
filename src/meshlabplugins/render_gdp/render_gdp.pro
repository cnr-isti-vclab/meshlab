include (../../shared.pri)

HEADERS       = meshrender.h \
                textfile.h \
                shaderStructs.h \
                shaderDialog.h \
                ../../common/meshmodel.h

SOURCES       = meshrender.cpp \
		textfile.cpp \  
        shaderDialog.cpp #\
#		$$GLEWCODE \
#        ../../common/meshmodel.cpp

TARGET        = render_gdp

FORMS		= shaderDialog.ui

QT           += opengl xml
