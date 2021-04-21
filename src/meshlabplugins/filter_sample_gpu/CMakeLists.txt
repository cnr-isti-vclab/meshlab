# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES filter_sample_gpu.cpp)

set(HEADERS filter_sample_gpu.h)

add_meshlab_plugin(filter_sample_gpu ${SOURCES} ${HEADERS})

target_link_libraries(filter_sample_gpu PRIVATE OpenGL::GLU)
