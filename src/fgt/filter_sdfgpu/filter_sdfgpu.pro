include (../../shared.pri)


INCLUDEPATH += ../../meshlabplugins/render_radiance_scaling
DEPENDPATH += shaders

HEADERS       += filter_sdfgpu.h \
    filterinterface.h \
    ../../meshlabplugins/render_radiance_scaling/gpuProgram.h \
    ../../meshlabplugins/render_radiance_scaling/framebufferObject.h \
    ../../meshlabplugins/render_radiance_scaling/gpuShader.h \
    ../../meshlabplugins/render_radiance_scaling/textureParams.h \
    ../../meshlabplugins/render_radiance_scaling/textureFormat.h \
    ../../meshlabplugins/render_radiance_scaling/texture2D.h
SOURCES       += filter_sdfgpu.cpp \
    ../../meshlabplugins/render_radiance_scaling/gpuProgram.cpp \
    ../../meshlabplugins/render_radiance_scaling/framebufferObject.cpp \
    ../../meshlabplugins/render_radiance_scaling/gpuShader.cpp \
    ../../meshlabplugins/render_radiance_scaling/textureParams.cpp \
    ../../meshlabplugins/render_radiance_scaling/textureFormat.cpp
TARGET        = filter_sdfgpu
TEMPLATE      = lib
QT           += opengl
CONFIG       += plugin

DEPENDPATH += ./shaders

RESOURCES += \
    filter_sdfgpu.qrc
