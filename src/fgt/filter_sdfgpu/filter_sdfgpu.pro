include (../../shared.pri)

HEADERS       += filter_sdfgpu.h \
    filterinterface.h \
    vscan.h
SOURCES       += filter_sdfgpu.cpp \
    vscan.cpp
TARGET        = filter_sdfgpu
TEMPLATE      = lib
QT           += opengl
CONFIG       += plugin
