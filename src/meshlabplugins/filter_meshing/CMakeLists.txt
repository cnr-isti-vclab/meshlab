# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES meshfilter.cpp quadric_simp.cpp)

set(HEADERS meshfilter.h quadric_simp.h)

add_meshlab_plugin(filter_meshing ${SOURCES} ${HEADERS})

target_link_libraries(filter_meshing PRIVATE OpenGL::GLU)
