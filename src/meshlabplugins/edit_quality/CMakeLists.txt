# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/edit_quality.cmake (custom for this directory),
### then re-run ./make-cmake.py

set(SOURCES
    common/meshmethods.cpp
    common/transferfunction.cpp
    common/util.cpp
    edit_quality_factory.cpp
    eqhandle.cpp
    handle.cpp
    qualitymapper.cpp
    qualitymapperdialog.cpp
    tfhandle.cpp
    common/meshmethods.cpp
    common/transferfunction.cpp
    common/util.cpp)

set(HEADERS
    common/const_types.h
    common/meshmethods.h
    common/transferfunction.h
    common/util.h
    edit_quality_factory.h
    eqhandle.h
    handle.h
    qualitymapper.h
    qualitymapperdialog.h
    tfhandle.h
    common/const_types.h
    common/meshmethods.h
    common/transferfunction.h
    common/util.h)

set(RESOURCES qualitymapper.qrc)

set(UI qualitymapperdialog.ui)

add_library(edit_quality MODULE ${SOURCES} ${HEADERS} ${RESOURCES} ${UI})

target_include_directories(edit_quality PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(edit_quality PUBLIC meshlab-common)

set_property(TARGET edit_quality PROPERTY FOLDER Plugins)

set_property(TARGET edit_quality PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                          ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET edit_quality PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                          ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS edit_quality
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
