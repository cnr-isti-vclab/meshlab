include (../../shared.pri)

HEADERS       += filter_waffle.h \
                filter_waffle_functors.h \
                export_svg.h

SOURCES       += filter_waffle.cpp
TARGET        = filter_waffle

QT           += opengl
