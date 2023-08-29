# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES filter_colorproc.cpp)

set(HEADERS filter_colorproc.h)

add_meshlab_plugin(filter_colorproc ${SOURCES} ${HEADERS})
