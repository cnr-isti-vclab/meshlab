include (../../shared.pri)
QT += opengl
HEADERS = decorate_shadow.h \
    shadow_mapping.h
SOURCES = decorate_shadow.cpp \
    shadow_mapping.cpp \
    ../../meshlab/filterparameter.cpp \
    $$GLEWCODE \
    ../../../../vcglib/wrap/gui/trackball.cpp \
    ../../../../vcglib/wrap/gui/trackmode.cpp
TARGET = decorate_shadow
