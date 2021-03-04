# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES io_pdb.cpp)

set(HEADERS io_pdb.h)

add_meshlab_plugin(io_pdb ${SOURCES} ${HEADERS})
