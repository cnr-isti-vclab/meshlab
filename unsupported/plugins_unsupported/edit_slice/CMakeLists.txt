# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES edit_slice_factory.cpp editslice.cpp slicedialog.cpp svgpro.cpp)

set(HEADERS edit_slice_factory.h editslice.h slicedialog.h svgpro.h)

set(RESOURCES meshlab.qrc)

set(UI slicedialog.ui svgpro.ui)

add_library(edit_slice MODULE ${SOURCES} ${HEADERS} ${RESOURCES} ${UI})

target_include_directories(edit_slice PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(edit_slice PUBLIC common)

set_property(TARGET edit_slice PROPERTY FOLDER Plugins)

set_property(TARGET edit_slice PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                        ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET edit_slice PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                        ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS edit_slice
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
