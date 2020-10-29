# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/CMakeLists.template.cmake (shared with all other directories),
### or create a derived template in src/templates/edit_select.cmake,
### then re-run ./make-cmake.py

set(SOURCES edit_select.cpp edit_select_factory.cpp)

set(HEADERS edit_select.h edit_select_factory.h)

set(RESOURCES edit_select.qrc)

add_library(edit_select MODULE ${SOURCES} ${HEADERS} ${RESOURCES})

target_include_directories(edit_select PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(edit_select PUBLIC meshlab-common)

set_property(TARGET edit_select PROPERTY FOLDER Plugins)

set_property(TARGET edit_select PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                         ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET edit_select PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                         ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS edit_select
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
