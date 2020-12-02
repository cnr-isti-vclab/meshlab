# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES filter_measure.cpp)

set(HEADERS filter_measure.h)

add_library(filter_measure MODULE ${SOURCES} ${HEADERS})

target_include_directories(filter_measure PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filter_measure PUBLIC meshlab-common)

set_property(TARGET filter_measure PROPERTY FOLDER Plugins)

set_property(TARGET filter_measure PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                            ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET filter_measure PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                            ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS filter_measure
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
