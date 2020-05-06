# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/CMakeLists.template.cmake (shared with all other directories),
### or create a derived template in src/templates/filter_polygonal_polychord_collapse.cmake,
### then re-run ./make-cmake.py

set(SOURCES polygonalpolychordcollapsefilter.cpp)

set(HEADERS mesh.h polygonalpolychordcollapsefilter.h)

add_library(filter_polygonal_polychord_collapse MODULE ${SOURCES} ${HEADERS})

target_include_directories(filter_polygonal_polychord_collapse
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filter_polygonal_polychord_collapse PUBLIC common)

set_property(TARGET filter_polygonal_polychord_collapse PROPERTY FOLDER Plugins)

set_property(TARGET filter_polygonal_polychord_collapse
             PROPERTY RUNTIME_OUTPUT_DIRECTORY ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET filter_polygonal_polychord_collapse
             PROPERTY LIBRARY_OUTPUT_DIRECTORY ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS filter_polygonal_polychord_collapse
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
