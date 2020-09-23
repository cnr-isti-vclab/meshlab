# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/CMakeLists.template.cmake (shared with all other directories),
### or create a derived template in src/templates/edit_texture.cmake,
### then re-run ./make-cmake.py

set(SOURCES edit_texture_factory.cpp edittexture.cpp renderarea.cpp
            textureeditor.cpp)

set(HEADERS edit_texture_factory.h edittexture.h renderarea.h textureeditor.h)

set(RESOURCES meshlab.qrc)

set(UI textureeditor.ui)

add_library(edit_texture MODULE ${SOURCES} ${HEADERS} ${RESOURCES} ${UI})

target_include_directories(edit_texture PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(edit_texture PUBLIC common)

set_property(TARGET edit_texture PROPERTY FOLDER Plugins)

set_property(TARGET edit_texture PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                          ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET edit_texture PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                          ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS edit_texture
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
