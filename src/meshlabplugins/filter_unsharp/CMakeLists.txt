# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES filter_unsharp.cpp)

set(HEADERS filter_unsharp.h)

add_library(filter_unsharp MODULE ${SOURCES} ${HEADERS})

target_include_directories(filter_unsharp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filter_unsharp PUBLIC meshlab-common)

set_property(TARGET filter_unsharp PROPERTY FOLDER Plugins)

set_property(TARGET filter_unsharp PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                            ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET filter_unsharp PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                            ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS filter_unsharp
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
