# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/CMakeLists.template.cmake (shared with all other directories),
### or create a derived template in src/templates/filter_web_export_vmust.cmake,
### then re-run ./make-cmake.py

set(SOURCES filter_web_export.cpp)

set(HEADERS filter_web_export.h)

add_library(filter_web_export_vmust MODULE ${SOURCES} ${HEADERS})

target_include_directories(filter_web_export_vmust
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filter_web_export_vmust PUBLIC common)

set_property(TARGET filter_web_export_vmust PROPERTY FOLDER Plugins)

set_property(TARGET filter_web_export_vmust
             PROPERTY RUNTIME_OUTPUT_DIRECTORY ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET filter_web_export_vmust
             PROPERTY LIBRARY_OUTPUT_DIRECTORY ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS filter_web_export_vmust
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
