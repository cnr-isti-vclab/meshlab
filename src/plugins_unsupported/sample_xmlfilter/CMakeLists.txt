# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/CMakeLists.template.cmake (shared with all other directories),
### or create a derived template in src/templates/sample_xmlfilter.cmake,
### then re-run ./make-cmake.py

set(SOURCES sample_xmlfilter.cpp)

set(HEADERS sample_xmlfilter.h)

add_library(sample_xmlfilter MODULE ${SOURCES} ${HEADERS})

target_include_directories(sample_xmlfilter PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(sample_xmlfilter PUBLIC common)

set_property(TARGET sample_xmlfilter PROPERTY FOLDER Plugins)

set_property(TARGET sample_xmlfilter PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                              ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET sample_xmlfilter PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                              ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS sample_xmlfilter
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
