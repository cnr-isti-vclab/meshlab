# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/io_base.cmake (custom for this directory),
### then re-run ./make-cmake.py

set(SOURCES baseio.cpp ${VCGDIR}/wrap/openfbx/src/miniz.c
            ${VCGDIR}/wrap/openfbx/src/ofbx.cpp ${VCGDIR}/wrap/ply/plylib.cpp)

set(HEADERS
    baseio.h
    ${VCGDIR}/wrap/io_trimesh/export_obj.h
    ${VCGDIR}/wrap/io_trimesh/export_off.h
    ${VCGDIR}/wrap/io_trimesh/export_ply.h
    ${VCGDIR}/wrap/io_trimesh/import_fbx.h
    ${VCGDIR}/wrap/io_trimesh/import_obj.h
    ${VCGDIR}/wrap/io_trimesh/import_off.h
    ${VCGDIR}/wrap/io_trimesh/import_ply.h
    ${VCGDIR}/wrap/io_trimesh/import_ptx.h
    ${VCGDIR}/wrap/io_trimesh/import_stl.h
    ${VCGDIR}/wrap/io_trimesh/io_material.h
    ${VCGDIR}/wrap/ply/plylib.h)

add_library(io_base MODULE ${SOURCES} ${HEADERS})

target_include_directories(io_base PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(io_base PUBLIC meshlab-common)

target_link_libraries(io_base PRIVATE OpenGL::GLU)

set_property(TARGET io_base PROPERTY FOLDER Plugins)

set_property(TARGET io_base PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                     ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET io_base PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                     ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS io_base
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
