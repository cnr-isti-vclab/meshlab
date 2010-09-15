include (../../shared.pri)
VCG_PATH = $$VCGDIR
TEMPLATE = lib
TARGET = filter_virtual_range_scan
QT += opengl
CONFIG += plugin
HEADERS += filter_virtual_range_scan.h \
    vs/resources.h \
    vs/sampler.h \
    vs/stages.h \
    vs/utils.h \
    vs/povs_generator.h \
    my_gl_widget.h \
    vs/simple_renderer.h
SOURCES += filter_virtual_range_scan.cpp
