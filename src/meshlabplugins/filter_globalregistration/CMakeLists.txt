# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/filter_globalregistration.cmake (custom for this directory),
### then re-run ./make-cmake.py

# Only build if we have opengr
if(TARGET external-opengr)

	set(SOURCES globalregistration.cpp)

	set(HEADERS globalregistration.h)

	add_library(filter_globalregistration MODULE ${SOURCES} ${HEADERS})

	target_include_directories(filter_globalregistration PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
	target_link_libraries(filter_globalregistration PUBLIC common)

	target_link_libraries(filter_globalregistration PRIVATE external-opengr)
	if(OpenMP_CXX_FOUND)
		target_link_libraries(filter_globalregistration PRIVATE OpenMP::OpenMP_CXX)
	endif()

	set_property(TARGET filter_globalregistration PROPERTY FOLDER Plugins)

	set_property(TARGET filter_globalregistration PROPERTY RUNTIME_OUTPUT_DIRECTORY ${MESHLAB_PLUGIN_OUTPUT_DIR})

	set_property(TARGET filter_globalregistration PROPERTY LIBRARY_OUTPUT_DIRECTORY ${MESHLAB_PLUGIN_OUTPUT_DIR})

	install(
		TARGETS filter_globalregistration
		DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
		COMPONENT Plugins)

else()
	message(STATUS "Skipping filter_globalregistration - missing opengr in external directory.")
endif()
