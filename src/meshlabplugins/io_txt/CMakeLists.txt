# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES io_txt.cpp)

set(HEADERS io_txt.h)

add_meshlab_plugin(io_txt ${SOURCES} ${HEADERS})
