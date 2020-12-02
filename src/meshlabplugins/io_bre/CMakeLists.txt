# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES io_bre.cpp)

set(HEADERS io_bre.h)

add_library(io_bre MODULE ${SOURCES} ${HEADERS})

target_include_directories(io_bre PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(io_bre PUBLIC meshlab-common)

set_property(TARGET io_bre PROPERTY FOLDER Plugins)

set_property(TARGET io_bre PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                    ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET io_bre PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                    ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS io_bre
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
