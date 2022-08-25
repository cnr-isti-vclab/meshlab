# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES filter_info.cpp)

set(HEADERS filter_info.h)

add_library(filter_info_vmust MODULE ${SOURCES} ${HEADERS})

target_include_directories(filter_info_vmust
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filter_info_vmust PUBLIC common)

set_property(TARGET filter_info_vmust PROPERTY FOLDER Plugins)

set_property(TARGET filter_info_vmust PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                               ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET filter_info_vmust PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                               ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS filter_info_vmust
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
