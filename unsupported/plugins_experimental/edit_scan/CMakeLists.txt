# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES edit_scan.cpp)

set(HEADERS edit_scan.h widget.h)

set(RESOURCES meshlab.qrc)

set(UI widget.ui)

add_library(edit_scan MODULE ${SOURCES} ${HEADERS} ${RESOURCES} ${UI})

target_include_directories(edit_scan PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(edit_scan PUBLIC common)

set_property(TARGET edit_scan PROPERTY FOLDER Plugins)

set_property(TARGET edit_scan PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                       ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET edit_scan PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                       ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS edit_scan
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
