# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/CMakeLists.template.cmake (shared with all other directories),
### or create a derived template in src/templates/filterborder.cmake,
### then re-run ./make-cmake.py

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
