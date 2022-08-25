# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES decorate_base.cpp ${VCGDIR}/wrap/gui/coordinateframe.cpp)

set(HEADERS colorhistogram.h decorate_base.h)

set(RESOURCES decorate_base.qrc)

add_meshlab_plugin(decorate_base ${SOURCES} ${HEADERS} ${RESOURCES})

target_link_libraries(decorate_base PRIVATE OpenGL::GLU)
