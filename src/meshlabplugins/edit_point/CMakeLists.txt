# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES edit_point.cpp edit_point_factory.cpp)

set(HEADERS connectedComponent.h edit_point.h edit_point_factory.h knnGraph.h)

set(RESOURCES edit_point.qrc)

add_meshlab_plugin(edit_point ${SOURCES} ${HEADERS} ${RESOURCES})

target_link_libraries(edit_point PUBLIC OpenGL::GLU)
