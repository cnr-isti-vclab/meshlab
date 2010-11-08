include (../../shared.pri)

HEADERS       = splatrenderer_plugin.h
SOURCES       = splatrenderer_plugin.cpp
TARGET        = render_splatting

QT            += opengl
RESOURCES     = ../../../../vcglib/wrap/gl/splatting_apss/splatrenderer.qrc

# CONFIG += debug
