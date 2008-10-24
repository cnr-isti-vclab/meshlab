include (../../shared.pri)

HEADERS       = splatrenderer.h
SOURCES       = splatrenderer.cpp $$GLEWCODE

TARGET        = splatrenderer

QT            += opengl
RESOURCES     = splatrenderer.qrc

# CONFIG += debug
