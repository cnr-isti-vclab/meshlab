include (../../shared.pri)


QT           += opengl

HEADERS       = samplefilterdoc.h \
		../../meshlab/filterparameter.h \
		../../meshlab/meshmodel.h

SOURCES       = samplefilterdoc.cpp \ 
		../../meshlab/filterparameter.cpp \
		../../meshlab/meshmodel.cpp \
		../../../../code/lib/glew/src/glew.c

TARGET        = samplefilterdoc
