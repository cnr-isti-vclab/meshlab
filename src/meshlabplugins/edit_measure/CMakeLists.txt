# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES edit_measure.cpp edit_measure_factory.cpp
            ${VCGDIR}/wrap/gui/rubberband.cpp)

set(HEADERS edit_measure.h edit_measure_factory.h)

set(RESOURCES edit_measure.qrc)

add_meshlab_plugin(edit_measure ${SOURCES} ${HEADERS} ${RESOURCES})

target_link_libraries(edit_measure PRIVATE OpenGL::GLU)
