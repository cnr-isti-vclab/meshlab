# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/CMakeLists.template.cmake (shared with all other directories),
### or create a derived template in src/templates/sampleio.cmake,
### then re-run ./make-cmake.py

set(SOURCES sampleio.cpp)

set(HEADERS sampleio.h)

add_library(sampleio MODULE ${SOURCES} ${HEADERS})

target_include_directories(sampleio PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(sampleio PUBLIC common)

set_property(TARGET sampleio PROPERTY FOLDER Plugins)

set_property(TARGET sampleio PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                      ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET sampleio PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                      ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS sampleio
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
