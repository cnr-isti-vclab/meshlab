include (../../shared.pri)


QT           += opengl

HEADERS       = decorate_shadow.h
SOURCES       = decorate_shadow.cpp\ 
		../../meshlab/filterparameter.cpp \
		$$GLEWCODE

TARGET        = decorate_shadow

