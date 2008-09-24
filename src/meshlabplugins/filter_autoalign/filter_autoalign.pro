include (../../shared.pri)


QT           += opengl

HEADERS       = filter_autoalign.h

SOURCES       = filter_autoalign.cpp \ 
		../../meshlab/filterparameter.cpp \
		../../../../code/lib/glew/src/glew.c

TARGET        = filter_autoalign
