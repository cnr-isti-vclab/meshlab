# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES filter_autoalign.cpp)

set(HEADERS filter_autoalign.h)

add_library(filter_autoalign MODULE ${SOURCES} ${HEADERS})

target_include_directories(filter_autoalign PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filter_autoalign PUBLIC common)

set_property(TARGET filter_autoalign PROPERTY FOLDER Plugins)

set_property(TARGET filter_autoalign PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                              ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET filter_autoalign PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                              ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS filter_autoalign
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
