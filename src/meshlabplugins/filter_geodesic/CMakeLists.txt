# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES filter_geodesic.cpp)

set(HEADERS filter_geodesic.h)

add_meshlab_plugin(filter_geodesic ${SOURCES} ${HEADERS})
