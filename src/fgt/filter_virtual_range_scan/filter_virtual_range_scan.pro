include (../../shared.pri)

VCG_PATH = $$VCGDIR

TEMPLATE = lib
TARGET = filter_virtual_range_scan
QT += opengl
CONFIG += plugin

HEADERS += filter_virtual_range_scan.h \
           vrs/compactor.h \
           vrs/resources.h \
           vrs/sampler.h \
           vrs/shaders.h \
           vrs/stage.h \
           vrs/stages.h \
           vrs/start.h \
           vrs/utils.h
SOURCES += filter_virtual_range_scan.cpp





