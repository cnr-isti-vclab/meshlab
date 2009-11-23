include (../../sharedfilter.pri)

HEADERS       = filter_dirt.h
SOURCES       = filter_dirt.cpp 
                
		$$GLEWCODE
TARGET        = filter_dirt
TEMPLATE      = lib
QT           += opengl
CONFIG       += plugin