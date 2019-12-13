# Copyright 2019 Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates then re-run ./make-cmake.py

set(SOURCES filter_texture.cpp ${VCGDIR}/wrap/ply/plylib.cpp
            ${VCGDIR}/wrap/qt/outline2_rasterizer.cpp)

set(HEADERS rastering.h filter_texture.h pushpull.h
            ${VCGDIR}/vcg/complex/algorithms/parametrization/voronoi_atlas.h)

add_library(filter_texture MODULE ${SOURCES} ${HEADERS})

target_include_directories(filter_texture PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filter_texture PUBLIC common)

if(MSVC)
    target_compile_definitions(filter_texture PRIVATE _USE_MATH_DEFINES)
endif()

set_property(TARGET filter_texture PROPERTY FOLDER Plugins)

set_property(TARGET filter_texture PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                            ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET filter_texture PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                            ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(TARGETS filter_texture DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
                COMPONENT Plugins)
