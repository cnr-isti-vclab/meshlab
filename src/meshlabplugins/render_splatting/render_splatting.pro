include (../../shared.pri)

HEADERS       = splatrenderer.h
SOURCES       = splatrenderer.cpp $$GLEWCODE

TARGET        = render_splatting

QT            += opengl
RESOURCES     = splatrenderer.qrc

# CONFIG += debug
