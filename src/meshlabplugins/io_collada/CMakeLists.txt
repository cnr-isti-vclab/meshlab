# Copyright 2019 Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates then re-run ./make-cmake.py

set(SOURCES io_collada.cpp ${VCGDIR}/wrap/dae/xmldocumentmanaging.cpp)

set(HEADERS
    io_collada.h
    ${VCGDIR}/wrap/dae/colladaformat.h
    ${VCGDIR}/wrap/dae/util_dae.h
    ${VCGDIR}/wrap/dae/xmldocumentmanaging.h
    ${VCGDIR}/wrap/io_trimesh/export_dae.h
    ${VCGDIR}/wrap/io_trimesh/import_dae.h)

add_library(io_collada MODULE ${SOURCES} ${HEADERS})

target_include_directories(io_collada PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(io_collada PUBLIC common)

set_property(TARGET io_collada PROPERTY FOLDER Plugins)

set_property(TARGET io_collada PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                        ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET io_collada PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                        ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(TARGETS io_collada DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR} COMPONENT
                Plugins)
