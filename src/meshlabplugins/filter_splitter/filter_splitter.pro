include (../../shared.pri)

HEADERS       = filter_splitter.h

SOURCES       = filter_splitter.cpp \ 
		../../meshlab/filterparameter.cpp \
		$$GLEWCODE

TARGET        = filter_splitter

CONFIG       += opengl



