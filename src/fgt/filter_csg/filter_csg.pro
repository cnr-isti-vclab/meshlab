include (../../sharedfilter.pri)

HEADERS      += filter_csg.h \
                intercept.h

SOURCES      += filter_csg.cpp \
                $$GLEWCODE

TARGET        = filter_csg
TEMPLATE      = lib
QT           += opengl
CONFIG       += plugin
