# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/filter_sdfgpu.cmake (custom for this directory),
### then re-run ./make-cmake.py

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

add_library(filter_sdfgpu MODULE ${SOURCES} ${HEADERS} ${RESOURCES})

target_include_directories(filter_sdfgpu PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filter_sdfgpu PUBLIC meshlab-common)

target_link_libraries(filter_sdfgpu PRIVATE OpenGL::GLU)

target_include_directories(
    filter_sdfgpu
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../render_radiance_scaling)

set_property(TARGET filter_sdfgpu PROPERTY FOLDER Plugins)

set_property(TARGET filter_sdfgpu PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                           ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET filter_sdfgpu PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                           ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS filter_sdfgpu
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
