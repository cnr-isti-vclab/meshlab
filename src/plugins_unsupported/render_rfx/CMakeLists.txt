# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/CMakeLists.template.cmake (shared with all other directories),
### or create a derived template in src/templates/render_rfx.cmake,
### then re-run ./make-cmake.py

set(SOURCES
    plugins/rfx_dds.cpp
    plugins/rfx_qimage.cpp
    plugins/rfx_tga.cpp
    render_rfx.cpp
    rfx_colorbox.cpp
    rfx_dialog.cpp
    rfx_glpass.cpp
    rfx_parser.cpp
    rfx_rendertarget.cpp
    rfx_shader.cpp
    rfx_specialattribute.cpp
    rfx_specialuniform.cpp
    rfx_state.cpp
    rfx_textureloader.cpp
    rfx_uniform.cpp)

set(HEADERS
    plugins/rfx_dds.h
    plugins/rfx_qimage.h
    plugins/rfx_tga.h
    render_rfx.h
    rfx_colorbox.h
    rfx_dialog.h
    rfx_glpass.h
    rfx_parser.h
    rfx_rendertarget.h
    rfx_shader.h
    rfx_specialattribute.h
    rfx_specialuniform.h
    rfx_state.h
    rfx_textureloader.h
    rfx_uniform.h)

set(RESOURCES render_rfx.qrc)

set(UI rfx_dialog.ui)

add_library(render_rfx MODULE ${SOURCES} ${HEADERS} ${RESOURCES} ${UI})

target_include_directories(render_rfx PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(render_rfx PUBLIC common)

set_property(TARGET render_rfx PROPERTY FOLDER Plugins)

set_property(TARGET render_rfx PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                        ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET render_rfx PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                        ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS render_rfx
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
