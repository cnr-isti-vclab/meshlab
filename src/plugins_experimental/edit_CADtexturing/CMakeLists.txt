# Copyright 2019 Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates then re-run ./make-cmake.py

set(SOURCES
    CADtexturingControl.cpp CADtexturingedit.cpp edit_CADtexturing_factory.cpp
    hidden/image_edge_detection.cpp image_edge_detection.cpp)

set(HEADERS
    CADtexturingControl.h CADtexturingedit.h edit_CADtexturing_factory.h
    hidden/image_edge_detection.h image_edge_detection.h resource.h)

set(RESOURCES CADtexturingedit.qrc)

set(UI CADtexturingControl.ui)

add_library(edit_CADtexturing MODULE ${SOURCES} ${HEADERS} ${RESOURCES} ${UI})

target_include_directories(edit_CADtexturing
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(edit_CADtexturing PUBLIC common)

set_property(TARGET edit_CADtexturing PROPERTY FOLDER Plugins)

set_property(TARGET edit_CADtexturing PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                               ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET edit_CADtexturing PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                               ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(TARGETS edit_CADtexturing DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
                COMPONENT Plugins)
