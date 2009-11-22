include (../../shared.pri)
HEADERS = filter_fractal.h
SOURCES = filter_fractal.cpp \
    ../../meshlab/filterparameter.cpp \
    $$GLEWCODE
TARGET = filter_fractal
TEMPLATE = lib
QT += opengl
CONFIG += plugin
OTHER_FILES += ff_description.txt
RESOURCES += ff_res.qrc
