# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


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

add_meshlab_plugin(edit_referencing ${SOURCES} ${HEADERS} ${RESOURCES} ${UI})

target_link_libraries(edit_referencing PRIVATE OpenGL::GLU)
