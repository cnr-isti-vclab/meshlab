# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES filterborder.cpp)

set(HEADERS filterborder.h)

add_library(filterborder MODULE ${SOURCES} ${HEADERS})

target_include_directories(filterborder PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filterborder PUBLIC common)

set_property(TARGET filterborder PROPERTY FOLDER Plugins)

set_property(TARGET filterborder PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                          ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET filterborder PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                          ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS filterborder
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
