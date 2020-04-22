# Copyright 2019 Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates then re-run ./make-cmake.py

set(SOURCES edit_sample_factory.cpp edit_sample.cpp)

set(HEADERS edit_sample_factory.h edit_sample.h)

set(RESOURCES edit_sample.qrc)

add_library(edit_sample MODULE ${SOURCES} ${HEADERS} ${RESOURCES})

target_include_directories(edit_sample PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(edit_sample PUBLIC common)

target_link_libraries(edit_sample PRIVATE OpenGL::GLU)

set_property(TARGET edit_sample PROPERTY FOLDER Plugins)

set_property(TARGET edit_sample PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                        ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET edit_sample PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                        ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(TARGETS edit_sample DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR} COMPONENT
                Plugins)
