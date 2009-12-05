include (../../shared.pri)

HEADERS = filter_dirt.h \
    dustparticle.h \
    dustsampler.h \
    $$VCGDIR/vcg/complex/trimesh/point_sampling.h
SOURCES = filter_dirt.cpp 
TARGET = filter_dirt
TEMPLATE = lib
QT += opengl
CONFIG += plugin
