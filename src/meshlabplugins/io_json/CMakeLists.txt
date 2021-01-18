# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES io_json.cpp)

set(HEADERS io_json.h)

add_library(io_json MODULE ${SOURCES} ${HEADERS})

target_include_directories(io_json PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(io_json PUBLIC meshlab-common)

set_property(TARGET io_json PROPERTY FOLDER Plugins)

set_property(TARGET io_json PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                     ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET io_json PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                     ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS io_json
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
