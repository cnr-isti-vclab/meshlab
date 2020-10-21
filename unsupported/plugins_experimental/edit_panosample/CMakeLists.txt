# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/CMakeLists.template.cmake (shared with all other directories),
### or create a derived template in src/templates/edit_panosample.cmake,
### then re-run ./make-cmake.py

set(SOURCES edit_panosample.cpp edit_panosample_factory.cpp qualitychecker.cpp)

set(HEADERS cloneview.h colorframe.h edit_panosample.h
            edit_panosample_factory.h qualitychecker.h resource.h)

set(UI QualityChecker.ui paintbox.ui)

add_library(edit_panosample MODULE ${SOURCES} ${HEADERS} ${UI})

target_include_directories(edit_panosample PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(edit_panosample PUBLIC common)

set_property(TARGET edit_panosample PROPERTY FOLDER Plugins)

set_property(TARGET edit_panosample PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                             ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET edit_panosample PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                             ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS edit_panosample
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
