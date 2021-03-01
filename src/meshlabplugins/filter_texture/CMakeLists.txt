# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES filter_texture.cpp ${VCGDIR}/wrap/ply/plylib.cpp
            ${VCGDIR}/wrap/qt/outline2_rasterizer.cpp)

set(HEADERS rastering.h filter_texture.h pushpull.h
            ${VCGDIR}/vcg/complex/algorithms/parametrization/voronoi_atlas.h)

add_meshlab_plugin(filter_texture ${SOURCES} ${HEADERS})

if(MSVC)
    target_compile_definitions(filter_texture PRIVATE _USE_MATH_DEFINES)
endif()
