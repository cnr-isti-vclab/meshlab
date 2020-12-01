# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES meshrender.cpp shaderDialog.cpp textfile.cpp)

set(HEADERS meshrender.h shaderDialog.h shaderStructs.h textfile.h)

set(UI shaderDialog.ui)

add_library(render_gdp MODULE ${SOURCES} ${HEADERS} ${UI})

target_include_directories(render_gdp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(render_gdp PUBLIC meshlab-common)

set_property(TARGET render_gdp PROPERTY FOLDER Plugins)

set_property(TARGET render_gdp PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                        ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET render_gdp PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                        ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS render_gdp
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
