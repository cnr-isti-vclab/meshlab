include (../../shared.pri)

HEADERS       = filter_dirt.h
SOURCES       = filter_dirt.cpp \
                ../../meshlab/filterparameter.cpp\
		$$GLEWCODE
TARGET        = filter_dirt
TEMPLATE      = lib
QT           += opengl
CONFIG       += plugin