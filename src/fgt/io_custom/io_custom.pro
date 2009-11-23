include (../../sharedfilter.pri)

HEADERS       = io_custom.h
SOURCES       = io_custom.cpp \
                \
		$$GLEWCODE
TARGET        = io_custom
TEMPLATE      = lib
CONFIG       += plugin