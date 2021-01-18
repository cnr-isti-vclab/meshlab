# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES edit_point.cpp edit_point_factory.cpp)

set(HEADERS connectedComponent.h edit_point.h edit_point_factory.h knnGraph.h)

set(RESOURCES edit_point.qrc)

add_library(edit_point MODULE ${SOURCES} ${HEADERS} ${RESOURCES})

target_include_directories(edit_point PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(edit_point PUBLIC meshlab-common OpenGL::GLU)

set_property(TARGET edit_point PROPERTY FOLDER Plugins)

set_property(TARGET edit_point PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                        ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET edit_point PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                        ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS edit_point
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
