# Copyright 2019 Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates then re-run ./make-cmake.py

set(SOURCES coformimportdialog.cpp io_3dcoform.cpp metadatafile_generator.cpp)

set(HEADERS coformimportdialog.h io_3dcoform.h metadatafile_generator.h)

set(UI ui/coformimportdialog.ui)

add_library(io_3dcoform MODULE ${SOURCES} ${HEADERS} ${UI})

target_include_directories(io_3dcoform PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(io_3dcoform PUBLIC common)

set_property(TARGET io_3dcoform PROPERTY FOLDER Plugins)

set_property(TARGET io_3dcoform PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                         ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET io_3dcoform PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                         ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(TARGETS io_3dcoform DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR} COMPONENT
                Plugins)
