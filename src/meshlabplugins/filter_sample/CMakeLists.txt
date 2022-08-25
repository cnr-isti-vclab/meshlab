# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES filter_sample.cpp)

set(HEADERS filter_sample.h)

add_meshlab_plugin(filter_sample ${SOURCES} ${HEADERS})
