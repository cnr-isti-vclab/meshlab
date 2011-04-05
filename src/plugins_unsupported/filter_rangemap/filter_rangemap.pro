include (../../shared.pri)

HEADERS       = filter_rangemap.h

SOURCES       = filter_rangemap.cpp \ 
		../../meshlab/filterparameter.cpp \
		$$GLEWCODE

TARGET        = filter_rangemap

CONFIG       += opengl



