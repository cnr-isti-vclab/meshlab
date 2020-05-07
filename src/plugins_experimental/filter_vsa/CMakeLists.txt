# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/CMakeLists.template.cmake (shared with all other directories),
### or create a derived template in src/templates/filter_vsa.cmake,
### then re-run ./make-cmake.py

set(SOURCES vsa.cpp)

set(HEADERS planar_region.h region_growing.h vsa.h)

add_library(filter_vsa MODULE ${SOURCES} ${HEADERS})

target_include_directories(filter_vsa PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filter_vsa PUBLIC common)

set_property(TARGET filter_vsa PROPERTY FOLDER Plugins)

set_property(TARGET filter_vsa PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                        ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET filter_vsa PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                        ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS filter_vsa
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
