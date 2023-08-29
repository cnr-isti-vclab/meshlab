# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES filter_create.cpp)

set(HEADERS filter_create.h)

add_meshlab_plugin(filter_create ${SOURCES} ${HEADERS})
