include (../../sharedfilter.pri)

HEADERS      += filter_csg.h \
                intercept.h \
                ../../meshlab/meshmodel.h

SOURCES      += filter_csg.cpp \
                ../../meshlab/meshmodel.cpp \
                $$GLEWCODE

TARGET        = filter_csg
TEMPLATE      = lib
QT           += opengl
CONFIG       += plugin
