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

add_library(render_radiance_scaling MODULE ${SOURCES} ${HEADERS} ${RESOURCES}
                                           ${UI})

target_include_directories(render_radiance_scaling
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(render_radiance_scaling PUBLIC meshlab-common)

target_link_libraries(render_radiance_scaling PRIVATE OpenGL::GLU)

set_property(TARGET render_radiance_scaling PROPERTY FOLDER Plugins)

set_property(TARGET render_radiance_scaling
             PROPERTY RUNTIME_OUTPUT_DIRECTORY ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET render_radiance_scaling
             PROPERTY LIBRARY_OUTPUT_DIRECTORY ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS render_radiance_scaling
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
