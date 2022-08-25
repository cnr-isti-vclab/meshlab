# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES meshselect.cpp)

set(HEADERS meshselect.h)

set(RESOURCES meshlab.qrc)

add_meshlab_plugin(filter_select ${SOURCES} ${HEADERS} ${RESOURCES})
