# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

set(SOURCES filter_sketchfab.cpp ${VCGDIR}/wrap/ply/plylib.cpp)

set(HEADERS filter_sketchfab.h miniz.h)

add_meshlab_plugin(filter_sketchfab ${SOURCES} ${HEADERS})
