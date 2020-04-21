# Copyright 2019 Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates then re-run ./make-cmake.py

set(SOURCES edit_sample_factory.cpp sampleedit.cpp)

set(HEADERS edit_sample_factory.h sampleedit.h)

set(RESOURCES sampleedit.qrc)

add_library(sampleedit MODULE ${SOURCES} ${HEADERS} ${RESOURCES})

target_include_directories(sampleedit PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(sampleedit PUBLIC common)

target_link_libraries(sampleedit PRIVATE OpenGL::GLU)

set_property(TARGET sampleedit PROPERTY FOLDER Plugins)

set_property(TARGET sampleedit PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                        ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET sampleedit PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                        ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(TARGETS sampleedit DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR} COMPONENT
                Plugins)
