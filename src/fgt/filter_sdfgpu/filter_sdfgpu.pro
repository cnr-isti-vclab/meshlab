include (../../shared.pri)

HEADERS       += filter_sdfgpu.h \
    filterinterface.h \
    vscan.h \
    ../../meshlabplugins/render_radiance_scaling/gpuProgram.h \
    ../../meshlabplugins/render_radiance_scaling/framebufferObject.h \
    ../../meshlabplugins/render_radiance_scaling/gpuShader.h
SOURCES       += filter_sdfgpu.cpp \
    vscan.cpp \
    ../../meshlabplugins/render_radiance_scaling/gpuProgram.cpp \
    ../../meshlabplugins/render_radiance_scaling/framebufferObject.cpp \
    ../../meshlabplugins/render_radiance_scaling/gpuShader.cpp
TARGET        = filter_sdfgpu
TEMPLATE      = lib
QT           += opengl
CONFIG       += plugin

RESOURCES += \
    filter_sdfgpu.qrc
