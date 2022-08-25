# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES
    filter_sdfgpu.cpp
    ../render_radiance_scaling/gpuProgram.cpp
    ../render_radiance_scaling/framebufferObject.cpp
    ../render_radiance_scaling/gpuShader.cpp
    ../render_radiance_scaling/textureParams.cpp
    ../render_radiance_scaling/textureFormat.cpp)

set(HEADERS
    filter_sdfgpu.h
    filterinterface.h
    ../render_radiance_scaling/gpuProgram.h
    ../render_radiance_scaling/framebufferObject.h
    ../render_radiance_scaling/gpuShader.h
    ../render_radiance_scaling/textureParams.h
    ../render_radiance_scaling/textureFormat.h
    ../render_radiance_scaling/texture2D.h)

set(RESOURCES filter_sdfgpu.qrc)

add_meshlab_plugin(filter_sdfgpu ${SOURCES} ${HEADERS} ${RESOURCES})

target_link_libraries(filter_sdfgpu PRIVATE OpenGL::GLU)

target_include_directories(
    filter_sdfgpu
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../render_radiance_scaling)
