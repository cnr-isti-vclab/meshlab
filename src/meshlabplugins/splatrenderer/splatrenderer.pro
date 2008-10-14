include (../../shared.pri)

HEADERS       = splatrenderer.h  ../../meshlab/meshmodel.h
SOURCES       = splatrenderer.cpp $$GLEWCODE ../../meshlab/meshmodel.cpp

TARGET        = splatrenderer

QT            += opengl
RESOURCES     = splatrenderer.qrc

CONFIG += debug
