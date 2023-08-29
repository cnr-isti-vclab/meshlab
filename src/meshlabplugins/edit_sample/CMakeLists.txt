# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES edit_sample.cpp edit_sample_factory.cpp)

set(HEADERS edit_sample.h edit_sample_factory.h)

set(RESOURCES edit_sample.qrc)

add_meshlab_plugin(edit_sample ${SOURCES} ${HEADERS} ${RESOURCES})

target_link_libraries(edit_sample PRIVATE OpenGL::GLU)
