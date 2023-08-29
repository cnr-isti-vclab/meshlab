# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES io_tri.cpp)

set(HEADERS io_tri.h)

add_meshlab_plugin(io_tri ${SOURCES} ${HEADERS})
