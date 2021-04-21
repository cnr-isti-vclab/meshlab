# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES filter_ao.cpp)

set(HEADERS filter_ao.h)

set(RESOURCES filter_ao.qrc)

add_meshlab_plugin(filter_ao ${SOURCES} ${HEADERS} ${RESOURCES})

target_link_libraries(filter_ao PRIVATE OpenGL::GLU)
