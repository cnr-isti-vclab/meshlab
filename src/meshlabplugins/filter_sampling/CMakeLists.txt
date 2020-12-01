# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES filter_sampling.cpp)

set(HEADERS filter_sampling.h)

add_library(filter_sampling MODULE ${SOURCES} ${HEADERS})

target_include_directories(filter_sampling PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filter_sampling PUBLIC meshlab-common)

set_property(TARGET filter_sampling PROPERTY FOLDER Plugins)

set_property(TARGET filter_sampling PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                             ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET filter_sampling PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                             ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS filter_sampling
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
