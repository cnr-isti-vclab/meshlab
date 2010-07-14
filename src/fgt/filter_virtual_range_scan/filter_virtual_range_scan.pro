AGL_PATH = agl/

include (../../shared.pri)

VCG_PATH = $$VCGDIR

include ( agl/agl.pri )

TEMPLATE = lib
TARGET = filter_virtual_range_scan
QT += opengl
CONFIG += plugin
INCLUDEPATH += $$AGL_PATH

HEADERS += filter_virtual_range_scan.h
SOURCES += filter_virtual_range_scan.cpp





