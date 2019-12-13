# Copyright 2019 Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates then re-run ./make-cmake.py

set(SOURCES samplefilter.cpp)

set(HEADERS samplefilter.h)

add_library(samplefilter MODULE ${SOURCES} ${HEADERS})

target_include_directories(samplefilter PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(samplefilter PUBLIC common)

set_property(TARGET samplefilter PROPERTY FOLDER Plugins)

set_property(TARGET samplefilter PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                          ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET samplefilter PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                          ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(TARGETS samplefilter DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
                COMPONENT Plugins)
