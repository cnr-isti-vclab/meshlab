# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES edit_manipulators.cpp edit_manipulators_factory.cpp)

set(HEADERS edit_manipulators.h edit_manipulators_factory.h)

set(RESOURCES edit_manipulators.qrc)

add_meshlab_plugin(edit_manipulators ${SOURCES} ${HEADERS} ${RESOURCES})

target_link_libraries(edit_manipulators PRIVATE OpenGL::GLU)
