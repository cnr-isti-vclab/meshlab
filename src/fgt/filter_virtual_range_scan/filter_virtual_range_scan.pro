include (../../shared.pri)
VCG_PATH = $$VCGDIR
TEMPLATE = lib
TARGET = filter_virtual_range_scan
QT += opengl
CONFIG += plugin
HEADERS += filter_virtual_range_scan.h \
    vs/compactor.h \
    vs/resources.h \
    vs/sampler.h \
    vs/shaders.h \
    vs/stage.h \
    vs/stages.h \
    vs/start.h \
    vs/utils.h \
    my_gl_widget.h
SOURCES += filter_virtual_range_scan.cpp
