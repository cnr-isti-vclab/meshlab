# Copyright 2019 Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates then re-run ./make-cmake.py

set(SOURCES edit_manipulators.cpp edit_manipulators_factory.cpp)

set(HEADERS edit_manipulators.h edit_manipulators_factory.h)

set(RESOURCES edit_manipulators.qrc)

add_library(edit_manipulators MODULE ${SOURCES} ${HEADERS} ${RESOURCES})

target_include_directories(edit_manipulators
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(edit_manipulators PUBLIC common)

target_link_libraries(edit_manipulators PRIVATE OpenGL::GLU)

set_property(TARGET edit_manipulators PROPERTY FOLDER Plugins)

set_property(TARGET edit_manipulators PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                               ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET edit_manipulators PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                               ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(TARGETS edit_manipulators DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
                COMPONENT Plugins)
