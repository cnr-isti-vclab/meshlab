include (../../sharedfilter.pri)

HEADERS = filter_dirt.h \
    dustparticle.h \
    dustsampler.h \
    ../../meshlab/meshmodel.h \
    $$VCGDIR/vcg/complex/trimesh/point_sampling.h
SOURCES = filter_dirt.cpp \
     ../../common/filterparameter.cpp \
    ../../meshlab/meshmodel.cpp \
    $$GLEWCODE
TARGET = filter_dirt
TEMPLATE = lib
QT += opengl
CONFIG += plugin
