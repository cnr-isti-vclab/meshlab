# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES edit_select.cpp edit_select_factory.cpp)

set(HEADERS edit_select.h edit_select_factory.h)

set(RESOURCES edit_select.qrc)

add_meshlab_plugin(edit_select ${SOURCES} ${HEADERS} ${RESOURCES})
