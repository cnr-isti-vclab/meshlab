# Copyright 2019 Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates then re-run ./make-cmake.py

set(SOURCES
    controlPoint.cpp
    edit_rgbtri_factory.cpp
    interactiveEdit.cpp
    modButterfly.cpp
    rgbPrimitives.cpp
    rgbt.cpp
    subDialog.cpp
    widgetRgbT.cpp)

set(HEADERS
    controlPoint.h
    edit_rgbtri_factory.h
    interactiveEdit.h
    modButterfly.h
    rgbInfo.h
    rgbPrimitives.h
    rgbt.h
    subDialog.h
    topologicalOp.h
    widgetRgbT.h)

set(RESOURCES editrgbtri.qrc)

set(UI subDialog.ui widgetRgbT.ui)

add_library(editrgbtri MODULE ${SOURCES} ${HEADERS} ${RESOURCES} ${UI})

target_include_directories(editrgbtri PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(editrgbtri PUBLIC common)

set_property(TARGET editrgbtri PROPERTY FOLDER Plugins)

set_property(TARGET editrgbtri PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                        ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET editrgbtri PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                        ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(TARGETS editrgbtri DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR} COMPONENT
                Plugins)
