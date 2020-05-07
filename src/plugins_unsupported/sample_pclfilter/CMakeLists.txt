# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/CMakeLists.template.cmake (shared with all other directories),
### or create a derived template in src/templates/sample_pclfilter.cmake,
### then re-run ./make-cmake.py

set(SOURCES sample_pclfilter.cpp)

set(HEADERS sample_pclfilter.h)

add_library(sample_pclfilter MODULE ${SOURCES} ${HEADERS})

target_include_directories(sample_pclfilter PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(sample_pclfilter PUBLIC common)

set_property(TARGET sample_pclfilter PROPERTY FOLDER Plugins)

set_property(TARGET sample_pclfilter PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                              ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET sample_pclfilter PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                              ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS sample_pclfilter
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
