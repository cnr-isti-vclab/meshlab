# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES io_tri.cpp)

set(HEADERS io_tri.h)

add_library(io_tri MODULE ${SOURCES} ${HEADERS})

target_include_directories(io_tri PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(io_tri PUBLIC meshlab-common)

set_property(TARGET io_tri PROPERTY FOLDER Plugins)

set_property(TARGET io_tri PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                    ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET io_tri PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                    ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS io_tri
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
