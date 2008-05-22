include (../../shared.pri)


QT           += opengl

HEADERS       = sampledecorate.h cubemap.h

SOURCES       = sampledecorate.cpp\ 
		cubemap.cpp \
		../../meshlab/filterparameter.cpp \
		$$GLEWCODE

TARGET        = sampledecoration

