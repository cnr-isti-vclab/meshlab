# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES filter_fractal.cpp)

set(HEADERS craters_utils.h filter_fractal.h filter_functors.h fractal_utils.h)

add_meshlab_plugin(filter_fractal ${SOURCES} ${HEADERS})
