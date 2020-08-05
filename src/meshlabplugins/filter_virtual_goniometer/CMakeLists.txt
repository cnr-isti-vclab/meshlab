# Copyright 2019 Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates then re-run ./make-cmake.py

set(SOURCES virtual_goniometer.cpp)

set(HEADERS virtual_goniometer.h)

set(RESOURCES meshlab.qrc)

add_library(filter_virtual_goniometer MODULE ${SOURCES} ${HEADERS} ${RESOURCES})

target_include_directories(filter_virtual_goniometer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filter_virtual_goniometer PUBLIC common)

set_property(TARGET filter_virtual_goniometer PROPERTY FOLDER Plugins)

set_property(TARGET filter_virtual_goniometer PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                           ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET filter_virtual_goniometer PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                           ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(TARGETS filter_virtual_goniometer DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
                COMPONENT Plugins)
