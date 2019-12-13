# Copyright 2019 Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates then re-run ./make-cmake.py

set(SOURCES filter_aging.cpp)

set(HEADERS edgepred.h filter_aging.h)

add_library(filter_aging MODULE ${SOURCES} ${HEADERS})

target_include_directories(filter_aging PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filter_aging PUBLIC common)

set_property(TARGET filter_aging PROPERTY FOLDER Plugins)

set_property(TARGET filter_aging PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                          ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET filter_aging PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                          ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(TARGETS filter_aging DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
                COMPONENT Plugins)
