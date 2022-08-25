# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES cubemap.cpp decorate_background.cpp)

set(HEADERS cubemap.h decorate_background.h)

set(RESOURCES decorate_background.qrc)

add_meshlab_plugin(decorate_background ${SOURCES} ${HEADERS} ${RESOURCES})
