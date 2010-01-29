include (../../shared.pri)

HEADERS      += filter_csg.h \
                bigint.h \
                fixed.h \
                intercept.h 

SOURCES      += filter_csg.cpp 

TARGET        = filter_csg
TEMPLATE      = lib
QT           += opengl
CONFIG       += plugin
