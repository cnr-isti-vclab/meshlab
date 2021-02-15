# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0




set(SOURCES filter_sketchfab.cpp ${VCGDIR}/wrap/ply/plylib.cpp)

set(HEADERS filter_sketchfab.h miniz.h)

add_library(filter_sketchfab MODULE ${SOURCES} ${HEADERS})

target_include_directories(filter_sketchfab
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filter_sketchfab PUBLIC meshlab-common)

set_property(TARGET filter_sketchfab PROPERTY FOLDER Plugins)

set_property(TARGET filter_sketchfab 
	PROPERTY RUNTIME_OUTPUT_DIRECTORY ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET filter_sketchfab 
	PROPERTY LIBRARY_OUTPUT_DIRECTORY ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
	TARGETS filter_sketchfab
	DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
	COMPONENT Plugins)
