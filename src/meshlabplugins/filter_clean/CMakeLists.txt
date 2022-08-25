# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES cleanfilter.cpp)

set(HEADERS cleanfilter.h)

add_meshlab_plugin(filter_clean ${SOURCES} ${HEADERS})
