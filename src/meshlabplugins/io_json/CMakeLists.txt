# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES io_json.cpp)

set(HEADERS io_json.h)

add_meshlab_plugin(io_json ${SOURCES} ${HEADERS})
