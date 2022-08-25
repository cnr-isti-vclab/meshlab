# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES colorpiker.cpp edit_segment_factory.cpp editsegment.cpp
            meshcutdialog.cpp)

set(HEADERS colorpicker.h curvaturetensor.h cutting.h edit_segment_factory.h
            editsegment.h meshcutdialog.h)

set(RESOURCES meshlab.qrc)

set(UI meshcutdialog.ui)

add_library(editsegment MODULE ${SOURCES} ${HEADERS} ${RESOURCES} ${UI})

target_include_directories(editsegment PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(editsegment PUBLIC common)

set_property(TARGET editsegment PROPERTY FOLDER Plugins)

set_property(TARGET editsegment PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                         ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET editsegment PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                         ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS editsegment
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
