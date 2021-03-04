# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES filter_layer.cpp)

set(HEADERS filter_layer.h)

add_meshlab_plugin(filter_layer ${SOURCES} ${HEADERS})
