# Copyright 2019 Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates then re-run ./make-cmake.py

set(SOURCES io_u3d.cpp)

set(HEADERS io_u3d.h)

add_library(io_u3d MODULE ${SOURCES} ${HEADERS})

target_include_directories(io_u3d PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(io_u3d PUBLIC common)

set_property(TARGET io_u3d PROPERTY FOLDER Plugins)

set_property(TARGET io_u3d PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                    ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET io_u3d PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                    ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(TARGETS io_u3d DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR} COMPONENT
                Plugins)
