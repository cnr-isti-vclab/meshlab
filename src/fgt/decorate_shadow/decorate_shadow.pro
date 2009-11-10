include (../../shared.pri)
QT += opengl
HEADERS = decorate_shadow.h \
    shadow_mapping.h \
    ../../../../vcglib/wrap/gui/trackball.h \
    decorate_shader.h \
    variance_shadow_mapping.h \
    variance_shadow_mapping_blur.h \
    ssao.h
SOURCES = decorate_shadow.cpp \
    shadow_mapping.cpp \
    ../../meshlab/filterparameter.cpp \
    $$GLEWCODE \
    ../../../../vcglib/wrap/gui/trackball.cpp \
    ../../../../vcglib/wrap/gui/trackmode.cpp \
    variance_shadow_mapping.cpp \
    variance_shadow_mapping_blur.cpp \
    ssao.cpp
TARGET = decorate_shadow
RESOURCES    += noise.qrc