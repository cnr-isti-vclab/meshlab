# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES filter_voronoi.cpp)

set(HEADERS filter_voronoi.h)

add_meshlab_plugin(filter_voronoi ${SOURCES} ${HEADERS})
