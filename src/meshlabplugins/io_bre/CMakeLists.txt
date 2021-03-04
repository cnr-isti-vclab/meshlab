# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES io_bre.cpp)

set(HEADERS io_bre.h)

add_meshlab_plugin(io_bre ${SOURCES} ${HEADERS})
