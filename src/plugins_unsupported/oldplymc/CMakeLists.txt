# Copyright 2019 Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates then re-run ./make-cmake.py

set(SOURCES)

set(HEADERS
    plymc.h
    simplemeshprovider.h
    svoxel.h
    tri_edge_collapse_mc.h
    trivial_walker.h
    volume.h
    voxel.h)

add_library(oldplymc MODULE ${SOURCES} ${HEADERS})

target_include_directories(oldplymc PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(oldplymc PUBLIC common)

set_property(TARGET oldplymc PROPERTY FOLDER Plugins)

set_property(TARGET oldplymc PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                      ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET oldplymc PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                      ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(TARGETS oldplymc DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR} COMPONENT
                Plugins)
