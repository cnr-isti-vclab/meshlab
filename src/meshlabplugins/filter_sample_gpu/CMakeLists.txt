# Copyright 2019 Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates then re-run ./make-cmake.py

set(SOURCES sample_filtergpu.cpp)

set(HEADERS sample_filtergpu.h)

add_library(sample_filtergpu MODULE ${SOURCES} ${HEADERS})

target_include_directories(sample_filtergpu PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(sample_filtergpu PUBLIC common)

target_link_libraries(sample_filtergpu PRIVATE OpenGL::GLU)

set_property(TARGET sample_filtergpu PROPERTY FOLDER Plugins)

set_property(TARGET sample_filtergpu PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                              ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET sample_filtergpu PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                              ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(TARGETS sample_filtergpu DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
                COMPONENT Plugins)
