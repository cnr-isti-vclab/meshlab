# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES dialog.cpp)

set(HEADERS
    dialog.h
    materials.h
    object.h
    point_based_renderer.h
    pyramid_point_renderer.h
    pyramid_point_renderer_base.h
    pyramid_point_renderer_color.h
    shaderStructs.h
    splat_pyramid.h)

set(UI dialog.ui)

add_library(render_splatpyramid MODULE ${SOURCES} ${HEADERS} ${UI})

target_include_directories(render_splatpyramid
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(render_splatpyramid PUBLIC common)

set_property(TARGET render_splatpyramid PROPERTY FOLDER Plugins)

set_property(TARGET render_splatpyramid PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                                 ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET render_splatpyramid PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                                 ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS render_splatpyramid
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
