# Copyright 2019 Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates then re-run ./make-cmake.py

set(SOURCES)

add_library(shaders MODULE ${SOURCES})

target_include_directories(shaders PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(shaders PUBLIC common)

set_property(TARGET shaders PROPERTY FOLDER Plugins)

set_property(TARGET shaders PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                     ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET shaders PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                     ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(TARGETS shaders DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR} COMPONENT
                Plugins)
