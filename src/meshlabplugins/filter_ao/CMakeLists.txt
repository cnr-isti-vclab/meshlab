# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/filter_ao.cmake (custom for this directory),
### then re-run ./make-cmake.py

set(SOURCES filter_ao.cpp)

set(HEADERS filter_ao.h)

set(RESOURCES filter_ao.qrc)

add_library(filter_ao MODULE ${SOURCES} ${HEADERS} ${RESOURCES})

target_include_directories(filter_ao PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filter_ao PUBLIC meshlab-common)

target_link_libraries(filter_ao PRIVATE OpenGL::GLU)

set_property(TARGET filter_ao PROPERTY FOLDER Plugins)

set_property(TARGET filter_ao PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                       ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET filter_ao PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                       ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS filter_ao
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
