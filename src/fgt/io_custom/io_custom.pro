include (../../shared.pri)

HEADERS       = io_custom.h
SOURCES       = io_custom.cpp \
                ../../meshlab/filterparameter.cpp\
		$$GLEWCODE
TARGET        = io_custom
TEMPLATE      = lib
CONFIG       += plugin