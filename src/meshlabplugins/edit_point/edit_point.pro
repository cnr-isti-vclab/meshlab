include (../../shared.pri)


HEADERS       = \
                edit_point.h \
                edit_point_factory.h \
                connectedComponent.h \
                knnGraph.h
				 
SOURCES       = \
                edit_point.cpp \
                edit_point_factory.cpp \

TARGET        = edit_point

QT           += opengl

RESOURCES     = edit_point.qrc




