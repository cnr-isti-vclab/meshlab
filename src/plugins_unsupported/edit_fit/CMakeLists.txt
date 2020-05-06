# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/CMakeLists.template.cmake (shared with all other directories),
### or create a derived template in src/templates/edit_fit.cmake,
### then re-run ./make-cmake.py

set(SOURCES edit_fit_factory.cpp editfit.cpp fittoolbox.cpp primitive.cpp)

set(HEADERS box.h cylinder.h edit_fit_factory.h editfit.h fittoolbox.h
            primitive.h)

set(UI fittoolbox.ui)

add_library(edit_fit MODULE ${SOURCES} ${HEADERS} ${UI})

target_include_directories(edit_fit PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(edit_fit PUBLIC common)

set_property(TARGET edit_fit PROPERTY FOLDER Plugins)

set_property(TARGET edit_fit PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                      ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET edit_fit PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                      ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS edit_fit
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
