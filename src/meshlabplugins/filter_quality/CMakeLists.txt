# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES
    filterqualitymapper.cpp ../edit_quality/common/meshmethods.cpp
    ../edit_quality/common/transferfunction.cpp ../edit_quality/common/util.cpp)

set(HEADERS
    filterqualitymapper.h ../edit_quality/common/const_types.h
    ../edit_quality/common/meshmethods.h
    ../edit_quality/common/transferfunction.h ../edit_quality/common/util.h)

add_library(filter_quality MODULE ${SOURCES} ${HEADERS})

target_include_directories(filter_quality PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filter_quality PUBLIC meshlab-common)

set_property(TARGET filter_quality PROPERTY FOLDER Plugins)

set_property(TARGET filter_quality PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                            ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET filter_quality PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                            ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS filter_quality
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
