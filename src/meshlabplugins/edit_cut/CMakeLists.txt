# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

set(SOURCES edit_cut.cpp edit_cut_factory.cpp)

set(HEADERS edit_cut.h edit_cut_factory.h)

set(RESOURCES edit_cut.qrc)

add_meshlab_plugin(edit_cut ${SOURCES} ${HEADERS} ${RESOURCES})

target_link_libraries(edit_cut PRIVATE OpenGL::GLU)
