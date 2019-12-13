# Copyright 2019 Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates then re-run ./make-cmake.py

set(SOURCES meshcolorize.cpp)

set(HEADERS meshcolorize.h)

add_library(filter_colorize MODULE ${SOURCES} ${HEADERS})

target_include_directories(filter_colorize PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filter_colorize PUBLIC common)

set_property(TARGET filter_colorize PROPERTY FOLDER Plugins)

set_property(TARGET filter_colorize PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                             ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET filter_colorize PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                             ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(TARGETS filter_colorize DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
                COMPONENT Plugins)
