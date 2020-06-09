# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/CMakeLists.template.cmake (shared with all other directories),
### or create a derived template in src/templates/io_u3d.cmake,
### then re-run ./make-cmake.py

# Only build if we have u3d
if(TARGET external-IDTFConverter)

	set(SOURCES io_u3d.cpp)

	set(HEADERS io_u3d.h)

	add_library(io_u3d MODULE ${SOURCES} ${HEADERS})

	target_include_directories(io_u3d PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
	target_link_libraries(io_u3d PUBLIC common)

	target_link_libraries(io_u3d PRIVATE external-IDTFConverter)

	set_property(TARGET io_u3d PROPERTY FOLDER Plugins)

	set_property(TARGET io_u3d PROPERTY RUNTIME_OUTPUT_DIRECTORY
										${MESHLAB_PLUGIN_OUTPUT_DIR})

	set_property(TARGET io_u3d PROPERTY LIBRARY_OUTPUT_DIRECTORY
										${MESHLAB_PLUGIN_OUTPUT_DIR})

	install(
		TARGETS io_u3d
		DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
		COMPONENT Plugins)

else()
	message(STATUS "Skipping io_u3d - missing u3d")
endif()
