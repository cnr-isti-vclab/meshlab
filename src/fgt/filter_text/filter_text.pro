include (../../sharedfilter.pri)

HEADERS       = filter_text.h
SOURCES       = filter_text.cpp \
		$$GLEWCODE
TARGET        = filter_text
TEMPLATE      = lib
QT           += opengl
CONFIG       += plugin