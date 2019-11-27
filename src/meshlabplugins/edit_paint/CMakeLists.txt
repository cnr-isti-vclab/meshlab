# Copyright 2019 Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates then re-run ./make-cmake.py

set(SOURCES edit_paint.cpp edit_paint_factory.cpp paintbox.cpp)

set(HEADERS cloneview.h colorframe.h edit_paint.h edit_paint_factory.h
            paintbox.h)

set(RESOURCES edit_paint.qrc)

set(UI paintbox.ui)

add_library(edit_paint MODULE ${SOURCES} ${HEADERS} ${RESOURCES} ${UI})

target_include_directories(edit_paint PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(edit_paint PUBLIC common)

target_link_libraries(edit_paint PRIVATE OpenGL::GLU)

set_property(TARGET edit_paint PROPERTY FOLDER Plugins)

set_property(TARGET edit_paint PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                        ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET edit_paint PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                        ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(TARGETS edit_paint DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR} COMPONENT
                Plugins)
