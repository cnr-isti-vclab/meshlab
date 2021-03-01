# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES decorate_raster_proj.cpp ${VCGDIR}/wrap/gui/trackball.cpp
            ${VCGDIR}/wrap/gui/trackmode.cpp)

set(HEADERS decorate_raster_proj.h ${VCGDIR}/wrap/gui/trackball.h)

add_meshlab_plugin(decorate_raster_proj ${SOURCES} ${HEADERS})
