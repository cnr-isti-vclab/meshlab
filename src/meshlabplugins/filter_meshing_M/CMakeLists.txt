# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES meshfilter_M.cpp)

set(HEADERS meshfilter_M.h)

add_meshlab_plugin(filter_meshing_M ${SOURCES} ${HEADERS})

target_link_libraries(filter_meshing_M PRIVATE OpenGL::GLU)
