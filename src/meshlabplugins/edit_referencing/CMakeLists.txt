# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/edit_referencing.cmake (custom for this directory),
### then re-run ./make-cmake.py

set(SOURCES
	edit_referencing.cpp
	edit_referencingDialog.cpp
	edit_referencing_factory.cpp)

set(HEADERS
	edit_referencing.h
	edit_referencingDialog.h
	edit_referencing_factory.h)

set(RESOURCES
	edit_referencing.qrc)

set(UI
	edit_referencingDialog.ui)

add_library(edit_referencing MODULE ${SOURCES} ${HEADERS} ${RESOURCES} ${UI})

target_include_directories(edit_referencing PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(edit_referencing PUBLIC meshlab-common)

target_link_libraries(edit_referencing PRIVATE OpenGL::GLU)

set_property(TARGET edit_referencing PROPERTY FOLDER Plugins)

set_property(TARGET edit_referencing PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                              ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET edit_referencing PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                              ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS edit_referencing
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
