# Copyright 2019 Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates then re-run ./make-cmake.py

set(SOURCES TexturePainter.cpp VisibilityCheck.cpp VisibleSet.cpp
            filter_img_patch_param.cpp)

set(HEADERS Patch.h TexturePainter.h VisibilityCheck.h VisibleSet.h
            filter_img_patch_param.h)

add_library(filter_img_patch_param MODULE ${SOURCES} ${HEADERS})

target_include_directories(filter_img_patch_param
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filter_img_patch_param PUBLIC common)

set_property(TARGET filter_img_patch_param PROPERTY FOLDER Plugins)

set_property(TARGET filter_img_patch_param
             PROPERTY RUNTIME_OUTPUT_DIRECTORY ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET filter_img_patch_param
             PROPERTY LIBRARY_OUTPUT_DIRECTORY ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(TARGETS filter_img_patch_param DESTINATION
                ${MESHLAB_PLUGIN_INSTALL_DIR} COMPONENT Plugins)
