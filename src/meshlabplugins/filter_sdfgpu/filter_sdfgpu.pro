include (../../shared.pri)

INCLUDEPATH += ../render_radiance_scaling
DEPENDPATH += shaders

HEADERS += \
    filter_sdfgpu.h \
    filterinterface.h

SOURCES += \
    filter_sdfgpu.cpp \
    ../render_radiance_scaling/gpuProgram.cpp \
    ../render_radiance_scaling/framebufferObject.cpp \
    ../render_radiance_scaling/gpuShader.cpp \
    ../render_radiance_scaling/textureParams.cpp \
    ../render_radiance_scaling/textureFormat.cpp

RESOURCES += \
    filter_sdfgpu.qrc

TARGET = filter_sdfgpu


