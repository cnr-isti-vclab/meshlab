# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES
    framebufferObject.cpp
    gpuProgram.cpp
    gpuShader.cpp
    radianceScalingRenderer.cpp
    shaderDialog.cpp
    textureFormat.cpp
    textureParams.cpp)

set(HEADERS
    framebufferObject.h
    gpuProgram.h
    gpuShader.h
    radianceScalingRenderer.h
    shaderDialog.h
    texture2D.h
    textureFormat.h
    textureParams.h)

set(RESOURCES radianceScalingRenderer.qrc)

set(UI shaderDialog.ui)

add_meshlab_plugin(render_radiance_scaling ${SOURCES} ${HEADERS} ${RESOURCES}
                                           ${UI})

target_link_libraries(render_radiance_scaling PRIVATE OpenGL::GLU)
