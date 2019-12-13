# Copyright 2019 Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates then re-run ./make-cmake.py

set(SOURCES meshfilter.cpp quadric_simp.cpp)

set(HEADERS meshfilter.h quadric_simp.h)

add_library(filter_meshing MODULE ${SOURCES} ${HEADERS})

target_include_directories(filter_meshing PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filter_meshing PUBLIC common)

target_link_libraries(filter_meshing PRIVATE OpenGL::GLU)

set_property(TARGET filter_meshing PROPERTY FOLDER Plugins)

set_property(TARGET filter_meshing PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                            ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET filter_meshing PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                            ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(TARGETS filter_meshing DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
                COMPONENT Plugins)
