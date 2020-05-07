# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/filter_sample_gpu.cmake (custom for this directory),
### then re-run ./make-cmake.py

set(SOURCES filter_sample_gpu.cpp)

set(HEADERS filter_sample_gpu.h)

add_library(filter_sample_gpu MODULE ${SOURCES} ${HEADERS})

target_include_directories(filter_sample_gpu
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filter_sample_gpu PUBLIC common)

target_link_libraries(filter_sample_gpu PRIVATE OpenGL::GLU)

set_property(TARGET filter_sample_gpu PROPERTY FOLDER Plugins)

set_property(TARGET filter_sample_gpu PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                               ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET filter_sample_gpu PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                               ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS filter_sample_gpu
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
