# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/CMakeLists.template.cmake (shared with all other directories),
### or create a derived template in src/templates/io_m.cmake,
### then re-run ./make-cmake.py

set(SOURCES io_m.cpp)

set(HEADERS export_m.h io_m.h)

add_library(io_m MODULE ${SOURCES} ${HEADERS})

target_include_directories(io_m PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(io_m PUBLIC common)

set_property(TARGET io_m PROPERTY FOLDER Plugins)

set_property(TARGET io_m PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                  ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET io_m PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                  ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS io_m
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
