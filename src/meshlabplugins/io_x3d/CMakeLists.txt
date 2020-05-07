# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/io_x3d.cmake (custom for this directory),
### then re-run ./make-cmake.py

set(SOURCES io_x3d.cpp vrml/Parser.cpp vrml/Scanner.cpp)

set(HEADERS export_x3d.h import_x3d.h io_x3d.h util_x3d.h vrml/Parser.h
            vrml/Scanner.h)

add_library(io_x3d MODULE ${SOURCES} ${HEADERS})

target_include_directories(io_x3d PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(io_x3d PUBLIC common)

target_link_libraries(io_x3d PRIVATE OpenGL::GLU)

set_property(TARGET io_x3d PROPERTY FOLDER Plugins)

set_property(TARGET io_x3d PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                    ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET io_x3d PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                    ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS io_x3d
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
