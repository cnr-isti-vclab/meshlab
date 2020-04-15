# Copyright 2019 Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates then re-run ./make-cmake.py

set(SOURCES decorate_raster_proj.cpp)

set(HEADERS decorate_raster_proj.h)

add_library(decorate_raster_proj MODULE ${SOURCES} ${HEADERS})

target_include_directories(decorate_raster_proj
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(decorate_raster_proj PUBLIC common)

target_link_libraries(decorate_raster_proj PRIVATE vcglib_trackball)

set_property(TARGET decorate_raster_proj PROPERTY FOLDER Plugins)

set_property(TARGET decorate_raster_proj PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                                  ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET decorate_raster_proj PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                                  ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(TARGETS decorate_raster_proj DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
                COMPONENT Plugins)
