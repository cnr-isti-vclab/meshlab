# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES filter_sample_dyn.cpp)

set(HEADERS filter_sample_dyn.h)

add_meshlab_plugin(filter_sample_dyn ${SOURCES} ${HEADERS})
