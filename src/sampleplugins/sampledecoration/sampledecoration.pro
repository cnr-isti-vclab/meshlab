include (../../shared.pri)


QT           += opengl

HEADERS       = sampledecorate.h cubemap.h

SOURCES       = sampledecorate.cpp\ 
		cubemap.cpp \
		../../common/filterparameter.cpp \
		$$GLEWCODE

TARGET        = sampledecoration

