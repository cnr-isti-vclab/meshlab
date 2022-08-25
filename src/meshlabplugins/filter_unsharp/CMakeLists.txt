# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES filter_unsharp.cpp)

set(HEADERS filter_unsharp.h)

add_meshlab_plugin(filter_unsharp ${SOURCES} ${HEADERS})
