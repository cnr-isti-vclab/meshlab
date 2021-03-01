# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES filter_sampling.cpp)

set(HEADERS filter_sampling.h)

add_meshlab_plugin(filter_sampling ${SOURCES} ${HEADERS})
