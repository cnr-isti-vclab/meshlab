# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES
    filter_developability.cpp)

set(HEADERS
    filter_developability.h
    remeshing.h
    mesh_utils.h
    energy.h
    energy_grad.h
    opt.h)

add_meshlab_plugin(filter_developability ${SOURCES} ${HEADERS})
