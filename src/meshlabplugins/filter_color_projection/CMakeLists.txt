# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES filter_color_projection.cpp)

set(HEADERS filter_color_projection.h floatbuffer.h pushpull.h rastering.h
            render_helper.h)

add_meshlab_plugin(filter_color_projection ${SOURCES} ${HEADERS})
