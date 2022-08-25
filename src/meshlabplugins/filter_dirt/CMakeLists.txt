# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES dirt_utils.cpp filter_dirt.cpp)

set(HEADERS dirt_utils.h dustparticle.h dustsampler.h filter_dirt.h particle.h)

add_meshlab_plugin(filter_dirt ${SOURCES} ${HEADERS})
