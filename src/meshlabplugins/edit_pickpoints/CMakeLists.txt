# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/CMakeLists.template.cmake (shared with all other directories),
### or create a derived template in src/templates/edit_pickpoints.cmake,
### then re-run ./make-cmake.py

set(SOURCES
	edit_pickpoints_factory.cpp
	editpickpoints.cpp
	pickPointsTemplate.cpp
	pickedPoints.cpp
	pickpointsDialog.cpp
	../../meshlab/rich_parameter_gui/richparameterwidgets.cpp
	../../meshlab/rich_parameter_gui/richparameterlistframe.cpp
	../../meshlab/rich_parameter_gui/richparameterlistdialog.cpp)

set(HEADERS
	edit_pickpoints_factory.h
	editpickpoints.h
	pickPointsTemplate.h
	pickedPoints.h
	pickpointsDialog.h)

set(RESOURCES editpickpoints.qrc)

set(UI pickpointsDialog.ui)

add_library(edit_pickpoints MODULE ${SOURCES} ${HEADERS} ${RESOURCES} ${UI})

target_include_directories(edit_pickpoints PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(edit_pickpoints PUBLIC common)

target_link_libraries(edit_pickpoints PRIVATE OpenGL::GLU)

set_property(TARGET edit_pickpoints PROPERTY FOLDER Plugins)

set_property(TARGET edit_pickpoints PROPERTY RUNTIME_OUTPUT_DIRECTORY
	${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET edit_pickpoints PROPERTY LIBRARY_OUTPUT_DIRECTORY
	${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
	TARGETS edit_pickpoints
	DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
	COMPONENT Plugins)
