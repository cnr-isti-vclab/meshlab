# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


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
