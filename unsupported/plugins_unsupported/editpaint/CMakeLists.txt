# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/CMakeLists.template.cmake (shared with all other directories),
### or create a derived template in src/templates/editpaint.cmake,
### then re-run ./make-cmake.py

set(SOURCES edit_paint_factory.cpp editpaint.cpp paintbox.cpp)

set(HEADERS colorwid.h edit_paint_factory.h editpaint.h)

set(RESOURCES meshlab.qrc)

set(UI PaintToolbox.ui)

add_library(editpaint MODULE ${SOURCES} ${HEADERS} ${RESOURCES} ${UI})

target_include_directories(editpaint PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(editpaint PUBLIC common)

set_property(TARGET editpaint PROPERTY FOLDER Plugins)

set_property(TARGET editpaint PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                       ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET editpaint PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                       ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS editpaint
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
