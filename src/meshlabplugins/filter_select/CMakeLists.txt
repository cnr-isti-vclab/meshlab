# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES meshselect.cpp)

set(HEADERS meshselect.h)

set(RESOURCES meshlab.qrc)

add_library(filter_select MODULE ${SOURCES} ${HEADERS} ${RESOURCES})

target_include_directories(filter_select PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filter_select PUBLIC meshlab-common)

set_property(TARGET filter_select PROPERTY FOLDER Plugins)

set_property(TARGET filter_select PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                           ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET filter_select PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                           ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS filter_select
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
