# Copyright 2019 Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates then re-run ./make-cmake.py

set(SOURCES coform_interface.cpp filter_3dcoform.cpp metadatafile_generator.cpp)

set(HEADERS coform_interface.h filter_3dcoform.h metadatafile_generator.h)

set(UI ui/coform_interface.ui)

add_library(filter_3dcoform MODULE ${SOURCES} ${HEADERS} ${UI})

target_include_directories(filter_3dcoform PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filter_3dcoform PUBLIC common)

set_property(TARGET filter_3dcoform PROPERTY FOLDER Plugins)

set_property(TARGET filter_3dcoform PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                             ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET filter_3dcoform PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                             ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(TARGETS filter_3dcoform DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
                COMPONENT Plugins)
