include (../../shared.pri)
HEADERS = filter_shape.h \
    platonic.h
SOURCES = filter_shape.cpp \
    ../../meshlab/filterparameter.cpp \
    $$GLEWCODE
TARGET = filter_shape
TEMPLATE = lib
QT += opengl
CONFIG += plugin
