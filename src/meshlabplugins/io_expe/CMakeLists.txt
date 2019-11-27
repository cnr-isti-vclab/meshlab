# Copyright 2019 Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates then re-run ./make-cmake.py

set(SOURCES io_expe.cpp)

set(HEADERS export_xyz.h import_expe.h import_xyz.h io_expe.h)

add_library(io_expe MODULE ${SOURCES} ${HEADERS})

target_include_directories(io_expe PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(io_expe PUBLIC common)

set_property(TARGET io_expe PROPERTY FOLDER Plugins)

set_property(TARGET io_expe PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                     ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET io_expe PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                     ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(TARGETS io_expe DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR} COMPONENT
                Plugins)
