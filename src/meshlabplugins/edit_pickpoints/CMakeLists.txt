# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES
	edit_pickpoints_factory.cpp
	editpickpoints.cpp
	pickPointsTemplate.cpp
	pickedPoints.cpp
	pickpointsDialog.cpp)

set(HEADERS
	edit_pickpoints_factory.h
	editpickpoints.h
	pickPointsTemplate.h
	pickedPoints.h
	pickpointsDialog.h)

set(RESOURCES editpickpoints.qrc)

set(UI pickpointsDialog.ui)

add_meshlab_plugin(edit_pickpoints ${SOURCES} ${HEADERS} ${RESOURCES} ${UI})

target_link_libraries(edit_pickpoints PRIVATE meshlab-common-gui OpenGL::GLU)
