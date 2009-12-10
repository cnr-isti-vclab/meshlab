include (../../shared.pri)
HEADERS += filter_shape.h \
    platonic.h \
    archimedean.h \
    knot.h \
    extrude.h
SOURCES += filter_shape.cpp
TARGET = filter_shape
TEMPLATE = lib
QT += opengl
CONFIG += plugin
