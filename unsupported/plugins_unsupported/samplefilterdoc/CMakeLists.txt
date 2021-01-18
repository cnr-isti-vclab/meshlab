# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


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

install(
    TARGETS samplefilterdoc
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
