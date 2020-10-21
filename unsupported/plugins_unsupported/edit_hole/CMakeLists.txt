# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/CMakeLists.template.cmake (shared with all other directories),
### or create a derived template in src/templates/edit_hole.cmake,
### then re-run ./make-cmake.py

set(SOURCES edit_hole.cpp edit_hole_factory.cpp fillerDialog.cpp
            holeListModel.cpp)

set(HEADERS
    edit_hole.h
    edit_hole_factory.h
    fgtBridge.h
    fgtHole.h
    fillerDialog.h
    holeListModel.h
    holeSetManager.h)

set(RESOURCES edit_hole.qrc)

set(UI edit_hole.ui)

add_library(edit_hole MODULE ${SOURCES} ${HEADERS} ${RESOURCES} ${UI})

target_include_directories(edit_hole PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(edit_hole PUBLIC common)

set_property(TARGET edit_hole PROPERTY FOLDER Plugins)

set_property(TARGET edit_hole PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                       ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET edit_hole PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                       ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS edit_hole
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
