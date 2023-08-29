# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES edit_paint.cpp edit_paint_factory.cpp paintbox.cpp)

set(HEADERS cloneview.h colorframe.h edit_paint.h edit_paint_factory.h
            paintbox.h)

set(RESOURCES edit_paint.qrc)

set(UI paintbox.ui)

add_meshlab_plugin(edit_paint ${SOURCES} ${HEADERS} ${RESOURCES} ${UI})

target_link_libraries(edit_paint PRIVATE OpenGL::GLU)
