# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/CMakeLists.template.cmake (shared with all other directories),
### or create a derived template in src/templates/filter_camera.cmake,
### then re-run ./make-cmake.py

set(SOURCES filter_camera.cpp)

set(HEADERS filter_camera.h)

add_library(filter_camera MODULE ${SOURCES} ${HEADERS})

target_include_directories(filter_camera PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filter_camera PUBLIC meshlab-common)

set_property(TARGET filter_camera PROPERTY FOLDER Plugins)

set_property(TARGET filter_camera PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                           ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET filter_camera PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                           ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS filter_camera
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
