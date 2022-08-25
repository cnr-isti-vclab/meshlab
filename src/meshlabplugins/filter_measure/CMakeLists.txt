# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES filter_measure.cpp)

set(HEADERS filter_measure.h)

add_meshlab_plugin(filter_measure ${SOURCES} ${HEADERS})
