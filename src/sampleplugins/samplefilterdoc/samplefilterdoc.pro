include (../../shared.pri)


QT           += opengl

HEADERS       = samplefilterdoc.h

SOURCES       = samplefilterdoc.cpp \ 
		../../meshlab/filterparameter.cpp \
		../../../../code/lib/glew/src/glew.c

TARGET        = samplefilterdoc
