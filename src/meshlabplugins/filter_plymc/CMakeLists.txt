# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES filter_plymc.cpp ${VCGDIR}/wrap/ply/plylib.cpp)

set(HEADERS filter_plymc.h)

add_meshlab_plugin(filter_plymc ${SOURCES} ${HEADERS})

target_link_libraries(filter_plymc PRIVATE OpenGL::GLU)
