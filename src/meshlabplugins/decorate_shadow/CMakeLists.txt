# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/decorate_shadow.cmake (custom for this directory),
### then re-run ./make-cmake.py

set(SOURCES
    decorate_shadow.cpp
    shadow_mapping.cpp
    ssao.cpp
    variance_shadow_mapping.cpp
    variance_shadow_mapping_blur.cpp
    ${VCGDIR}/wrap/gui/trackball.cpp
    ${VCGDIR}/wrap/gui/trackmode.cpp)

set(HEADERS
    decorate_shader.h
    decorate_shadow.h
    shadow_mapping.h
    ssao.h
    variance_shadow_mapping.h
    variance_shadow_mapping_blur.h
    ${VCGDIR}/wrap/gui/trackball.h)

set(RESOURCES noise.qrc)

add_library(decorate_shadow MODULE ${SOURCES} ${HEADERS} ${RESOURCES})

target_include_directories(decorate_shadow PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(decorate_shadow PUBLIC meshlab-common)

target_link_libraries(decorate_shadow PRIVATE OpenGL::GLU)

set_property(TARGET decorate_shadow PROPERTY FOLDER Plugins)

set_property(TARGET decorate_shadow PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                             ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET decorate_shadow PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                             ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS decorate_shadow
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
