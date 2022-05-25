# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES filter_ao.cpp)

set(HEADERS filter_ao.h)

add_meshlab_plugin(filter_ao ${SOURCES} ${HEADERS})

target_link_libraries(filter_ao PRIVATE OpenGL::GLU)
