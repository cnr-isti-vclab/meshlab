# Copyright 2019 Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates then re-run ./make-cmake.py

set(SOURCES samplefilterdoc.cpp)

set(HEADERS samplefilterdoc.h)

add_library(samplefilterdoc MODULE ${SOURCES} ${HEADERS})

target_include_directories(samplefilterdoc PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(samplefilterdoc PUBLIC common)

set_property(TARGET samplefilterdoc PROPERTY FOLDER Plugins)

set_property(TARGET samplefilterdoc PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                             ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET samplefilterdoc PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                             ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(TARGETS samplefilterdoc DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
                COMPONENT Plugins)
