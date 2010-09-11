include (../../shared.pri)


QT           += opengl

HEADERS       +=    vsa.h \
                    planar_region.h \
                    region_growing.h
SOURCES       += vsa.cpp

TARGET        = filter_vsa
