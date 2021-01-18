# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES decorate_raster_proj.cpp ${VCGDIR}/wrap/gui/trackball.cpp
            ${VCGDIR}/wrap/gui/trackmode.cpp)

set(HEADERS decorate_raster_proj.h ${VCGDIR}/wrap/gui/trackball.h)

add_library(decorate_raster_proj MODULE ${SOURCES} ${HEADERS})

target_include_directories(decorate_raster_proj
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(decorate_raster_proj PUBLIC meshlab-common)

set_property(TARGET decorate_raster_proj PROPERTY FOLDER Plugins)

set_property(TARGET decorate_raster_proj PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                                  ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET decorate_raster_proj PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                                  ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS decorate_raster_proj
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
