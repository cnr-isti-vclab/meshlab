# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES filter_curvature_from_sliding.cpp)

set(HEADERS curvature_from_sliding.h filter_curvature_from_sliding.h frame.h)

add_library(filter_curvature_from_sliding MODULE ${SOURCES} ${HEADERS})

target_include_directories(filter_curvature_from_sliding
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filter_curvature_from_sliding PUBLIC common)

set_property(TARGET filter_curvature_from_sliding PROPERTY FOLDER Plugins)

set_property(TARGET filter_curvature_from_sliding
             PROPERTY RUNTIME_OUTPUT_DIRECTORY ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET filter_curvature_from_sliding
             PROPERTY LIBRARY_OUTPUT_DIRECTORY ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS filter_curvature_from_sliding
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
