# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES filter_trioptimize.cpp)

set(HEADERS curvdata.h curvedgeflip.h filter_trioptimize.h)

add_meshlab_plugin(filter_trioptimize ${SOURCES} ${HEADERS})
