# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES io_expe.cpp)

set(HEADERS export_xyz.h import_expe.h import_xyz.h io_expe.h)

add_meshlab_plugin(io_expe ${SOURCES} ${HEADERS})
