# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES
    balloon.cpp
    edit_vase.cpp
    edit_vase_factory.cpp
    fieldinterpolator.cpp
    gridaccell.cpp
    vase_utils.cpp
    vasewidget.cpp
    volume.cpp)

set(HEADERS
    OLD.h
    balloon.h
    edit_vase.h
    edit_vase_factory.h
    fieldinterpolator.h
    gridaccell.h
    myheap.h
    vase_utils.h
    vasewidget.h
    volume.h)

set(RESOURCES meshlab.qrc)

set(UI vasewidget.ui)

add_library(edit_vase MODULE ${SOURCES} ${HEADERS} ${RESOURCES} ${UI})

target_include_directories(edit_vase PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(edit_vase PUBLIC common)

set_property(TARGET edit_vase PROPERTY FOLDER Plugins)

set_property(TARGET edit_vase PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                       ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET edit_vase PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                       ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS edit_vase
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
