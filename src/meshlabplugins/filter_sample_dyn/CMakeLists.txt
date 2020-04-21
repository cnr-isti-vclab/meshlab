# Copyright 2019 Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates then re-run ./make-cmake.py

set(SOURCES samplefilterdyn.cpp)

set(HEADERS samplefilterdyn.h)

add_library(samplefilterdyn MODULE ${SOURCES} ${HEADERS})

target_include_directories(samplefilterdyn PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(samplefilterdyn PUBLIC common)

set_property(TARGET samplefilterdyn PROPERTY FOLDER Plugins)

set_property(TARGET samplefilterdyn PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                             ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET samplefilterdyn PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                             ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(TARGETS samplefilterdyn DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
                COMPONENT Plugins)
