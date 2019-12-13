# Copyright 2019 Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates then re-run ./make-cmake.py

set(SOURCES io_pdb.cpp)

set(HEADERS io_pdb.h)

add_library(io_pdb MODULE ${SOURCES} ${HEADERS})

target_include_directories(io_pdb PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(io_pdb PUBLIC common)

set_property(TARGET io_pdb PROPERTY FOLDER Plugins)

set_property(TARGET io_pdb PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                    ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET io_pdb PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                    ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(TARGETS io_pdb DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR} COMPONENT
                Plugins)
