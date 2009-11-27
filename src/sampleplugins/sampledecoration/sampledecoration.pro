include (../../sharedfilter.pri)

QT           += opengl

HEADERS       += sampledecorate.h cubemap.h

SOURCES      += sampledecorate.cpp\ 
		cubemap.cpp \
		../../common/pluginmanager.cpp \
		$$GLEWCODE

TARGET        = sampledecoration

