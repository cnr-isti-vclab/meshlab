# Copyright 2019 Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates then re-run ./make-cmake.py

set(SOURCES filter_geodesic.cpp)

set(HEADERS filter_geodesic.h)

add_library(filter_geodesic MODULE ${SOURCES} ${HEADERS})

target_include_directories(filter_geodesic PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filter_geodesic PUBLIC common)

set_property(TARGET filter_geodesic PROPERTY FOLDER Plugins)

set_property(TARGET filter_geodesic PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                             ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET filter_geodesic PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                             ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(TARGETS filter_geodesic DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
                COMPONENT Plugins)
