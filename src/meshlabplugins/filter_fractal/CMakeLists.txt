# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/CMakeLists.template.cmake (shared with all other directories),
### or create a derived template in src/templates/filter_fractal.cmake,
### then re-run ./make-cmake.py

set(SOURCES filter_fractal.cpp)

set(HEADERS craters_utils.h filter_fractal.h filter_functors.h fractal_utils.h)

set(RESOURCES ff_res.qrc)

add_library(filter_fractal MODULE ${SOURCES} ${HEADERS} ${RESOURCES})

target_include_directories(filter_fractal PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filter_fractal PUBLIC common)

set_property(TARGET filter_fractal PROPERTY FOLDER Plugins)

set_property(TARGET filter_fractal PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                            ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET filter_fractal PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                            ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS filter_fractal
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
