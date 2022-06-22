# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES filter_embree.cpp)

set(HEADERS filter_embree.h)

add_meshlab_plugin(filter_embree ${SOURCES} ${HEADERS})

target_link_libraries(filter_embree PRIVATE embree)