# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES
    decorate_shadow.cpp
    shadow_mapping.cpp
    ssao.cpp
    variance_shadow_mapping.cpp
    variance_shadow_mapping_blur.cpp
    ${VCGDIR}/wrap/gui/trackball.cpp
    ${VCGDIR}/wrap/gui/trackmode.cpp)

set(HEADERS
    decorate_shader.h
    decorate_shadow.h
    shadow_mapping.h
    ssao.h
    variance_shadow_mapping.h
    variance_shadow_mapping_blur.h
    ${VCGDIR}/wrap/gui/trackball.h)

set(RESOURCES noise.qrc)

add_meshlab_plugin(decorate_shadow ${SOURCES} ${HEADERS} ${RESOURCES})

target_link_libraries(decorate_shadow PRIVATE OpenGL::GLU)
