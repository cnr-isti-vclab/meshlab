# Copyright 2019-2020, Collabora, Ltd.
# Copyright 2019, 2020, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

function(add_meshlab_plugin target_name)
	cmake_parse_arguments(ARG "" "" "" ${ARGN})

	add_library(${target_name} MODULE ${ARG_UNPARSED_ARGUMENTS})

	target_include_directories(${target_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
	target_link_libraries(${target_name} PUBLIC meshlab-common)

	set_property(TARGET ${target_name} PROPERTY FOLDER Plugins)

	set_property(TARGET ${target_name}
		PROPERTY RUNTIME_OUTPUT_DIRECTORY ${MESHLAB_PLUGIN_OUTPUT_DIR})

	set_property(TARGET ${target_name}
		PROPERTY LIBRARY_OUTPUT_DIRECTORY ${MESHLAB_PLUGIN_OUTPUT_DIR})

	install(
		TARGETS ${target_name}
		DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
		COMPONENT Plugins)
endfunction()
