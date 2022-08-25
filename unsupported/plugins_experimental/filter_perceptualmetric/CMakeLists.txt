# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES filter_perceptualmetric.cpp)

set(HEADERS filter_perceptualmetric.h perceptualmetrics.h)

add_library(filter_perceptualmetric MODULE ${SOURCES} ${HEADERS})

target_include_directories(filter_perceptualmetric
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filter_perceptualmetric PUBLIC common)

set_property(TARGET filter_perceptualmetric PROPERTY FOLDER Plugins)

set_property(TARGET filter_perceptualmetric
             PROPERTY RUNTIME_OUTPUT_DIRECTORY ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET filter_perceptualmetric
             PROPERTY LIBRARY_OUTPUT_DIRECTORY ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS filter_perceptualmetric
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
