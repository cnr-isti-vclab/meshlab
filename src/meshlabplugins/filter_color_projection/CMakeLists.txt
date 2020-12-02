# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES filter_color_projection.cpp)

set(HEADERS filter_color_projection.h floatbuffer.h pushpull.h rastering.h
            render_helper.h)

add_library(filter_color_projection MODULE ${SOURCES} ${HEADERS})

target_include_directories(filter_color_projection
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filter_color_projection PUBLIC meshlab-common)

set_property(TARGET filter_color_projection PROPERTY FOLDER Plugins)

set_property(TARGET filter_color_projection
             PROPERTY RUNTIME_OUTPUT_DIRECTORY ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET filter_color_projection
             PROPERTY LIBRARY_OUTPUT_DIRECTORY ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS filter_color_projection
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
