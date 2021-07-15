# Copyright 2019-2020, Collabora, Ltd.
# Copyright 2019, 2020, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

#function used to add plugins
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

# make quiet some portions of cmake
# usage
#
# set(MESSAGE_QUIET ON)
# #everything here will be quiet
# unset(MESSAGE_QUIET)
function(message)
	if (NOT MESSAGE_QUIET)
		_message(${ARGN})
	endif()

function(add_file_format_info_plist)
	cmake_parse_arguments(ARG "" "TARGET;FILE;FORMAT" "" ${ARGN})
	
	string(TOUPPER ${ARG_FORMAT} FORMAT_UPPER)
	string(TOLOWER ${ARG_FORMAT} FORMAT_LOWER)
	
	add_custom_command(
		TARGET ${ARG_TARGET}
		POST_BUILD
		COMMAND plutil -insert CFBundleDocumentTypes.0 -xml '<dict><key>CFBundleTypeName</key><string>${FORMAT_UPPER} 3D File</string><key>CFBundleTypeIconFile</key><string>meshlab.icns</string><key>CFBundleTypeRole</key><string>Editor</string><key>LSHandlerRank</key><string>Default</string></dict>' ${ARG_FILE}
		COMMAND plutil -insert CFBundleDocumentTypes.0.CFBundleTypeExtensions -xml '<array/>' ${ARG_FILE}
		COMMAND plutil -insert CFBundleDocumentTypes.0.CFBundleTypeExtensions.0 -xml '<string>${FORMAT_LOWER}</string>' ${ARG_FILE}
		COMMAND plutil -insert CFBundleDocumentTypes.0.CFBundleTypeOSTypes -xml '<array/>' ${ARG_FILE}
		COMMAND plutil -insert CFBundleDocumentTypes.0.CFBundleTypeOSTypes.0 -xml '<string>${FORMAT_UPPER}</string>' ${ARG_FILE}
	)
endfunction()

function(set_additional_settings_info_plist)
	cmake_parse_arguments(ARG "" "TARGET;FILE" "" ${ARGN})
	add_custom_command(
		TARGET ${ARG_TARGET}
		POST_BUILD
		COMMAND plutil -replace NSHighResolutionCapable -bool True ${ARG_FILE}
		COMMAND plutil -insert CFBundleDocumentTypes -xml '<array/>' ${ARG_FILE}
	)

	add_file_format_info_plist(
		TARGET ${ARG_TARGET}
		FILE ${ARG_FILE}
		FORMAT OBJ)
	add_file_format_info_plist(
		TARGET ${ARG_TARGET}
		FILE ${ARG_FILE}
		FORMAT STL)
	add_file_format_info_plist(
		TARGET ${ARG_TARGET}
		FILE ${ARG_FILE}
		FORMAT PLY)
>>>>>>> origin/master
endfunction()
