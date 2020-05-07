# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/CMakeLists.template.cmake (shared with all other directories),
### or create a derived template in src/templates/decorate_background.cmake,
### then re-run ./make-cmake.py

set(SOURCES cubemap.cpp decorate_background.cpp)

set(HEADERS cubemap.h decorate_background.h)

set(RESOURCES decorate_background.qrc)

add_library(decorate_background MODULE ${SOURCES} ${HEADERS} ${RESOURCES})

target_include_directories(decorate_background
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(decorate_background PUBLIC common)

set_property(TARGET decorate_background PROPERTY FOLDER Plugins)

set_property(TARGET decorate_background PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                                 ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET decorate_background PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                                 ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS decorate_background
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
