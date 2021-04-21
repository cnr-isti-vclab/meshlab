# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES filter_camera.cpp)

set(HEADERS filter_camera.h)

add_meshlab_plugin(filter_camera ${SOURCES} ${HEADERS})
