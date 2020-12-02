# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES filter_sample.cpp)

set(HEADERS filter_sample.h)

add_library(filter_sample MODULE ${SOURCES} ${HEADERS})

target_include_directories(filter_sample PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filter_sample PUBLIC meshlab-common)

set_property(TARGET filter_sample PROPERTY FOLDER Plugins)

set_property(TARGET filter_sample PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                           ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET filter_sample PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                           ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS filter_sample
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
