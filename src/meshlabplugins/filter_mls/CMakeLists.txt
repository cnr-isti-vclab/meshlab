# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/CMakeLists.template.cmake (shared with all other directories),
### or create a derived template in src/templates/filter_mls.cmake,
### then re-run ./make-cmake.py

set(SOURCES 
	apss.cpp 
	balltree.cpp 
	mlsplugin.cpp 
	rimls.cpp)

set(HEADERS
	apss.h
	balltree.h
	implicits.h
	mlsmarchingcube.h
	mlsplugin.h
	mlssurface.h
	priorityqueue.h
	rimls.h
	smallcomponentselection.h)

SET(TPP_HEADERS
	apss.tpp
	mlssurface.tpp
	rimls.tpp)

add_library(filter_mls MODULE ${SOURCES} ${HEADERS} ${TPP_HEADERS})

target_include_directories(filter_mls PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filter_mls PUBLIC common)

set_property(TARGET filter_mls PROPERTY FOLDER Plugins)

set_property(TARGET filter_mls PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                        ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET filter_mls PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                        ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS filter_mls
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
