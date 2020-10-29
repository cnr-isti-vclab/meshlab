# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/decorate_base.cmake (custom for this directory),
### then re-run ./make-cmake.py

set(SOURCES decorate_base.cpp ${VCGDIR}/wrap/gui/coordinateframe.cpp)

set(HEADERS colorhistogram.h decorate_base.h)

set(RESOURCES decorate_base.qrc)

add_library(decorate_base MODULE ${SOURCES} ${HEADERS} ${RESOURCES})

target_include_directories(decorate_base PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(decorate_base PUBLIC meshlab-common)

target_link_libraries(decorate_base PRIVATE OpenGL::GLU)

set_property(TARGET decorate_base PROPERTY FOLDER Plugins)

set_property(TARGET decorate_base PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                           ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET decorate_base PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                           ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS decorate_base
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
