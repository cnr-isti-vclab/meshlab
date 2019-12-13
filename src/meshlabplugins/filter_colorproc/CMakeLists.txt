# Copyright 2019 Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates then re-run ./make-cmake.py

set(SOURCES filter_colorproc.cpp)

set(HEADERS filter_colorproc.h)

add_library(filter_colorproc MODULE ${SOURCES} ${HEADERS})

target_include_directories(filter_colorproc PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filter_colorproc PUBLIC common)

set_property(TARGET filter_colorproc PROPERTY FOLDER Plugins)

set_property(TARGET filter_colorproc PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                              ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET filter_colorproc PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                              ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(TARGETS filter_colorproc DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
                COMPONENT Plugins)
