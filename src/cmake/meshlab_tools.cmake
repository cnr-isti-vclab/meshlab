# Copyright 2019-2020, Collabora, Ltd.
# Copyright 2019, 2020, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

#function used to add plugins
function(add_meshlab_plugin target_name)
	set(EXCLUDE_ALL_ARG "")
	if (MESHLAB_BUILD_MINI)
		set(EXCLUDE_ALL_ARG "EXCLUDE_FROM_ALL")
	endif()

	cmake_parse_arguments(ARG "" "" "" ${ARGN})

	add_library(${target_name} MODULE ${EXCLUDE_ALL_ARG} ${ARG_UNPARSED_ARGUMENTS})

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

# function that dowloads and unzips an archive
# params:
# - link: download link
# - dir: directory where the archive will be extracted
# - name: a name used only for log
function(download_and_unzip)
	set(download_and_unzip_SUCCESS FALSE PARENT_SCOPE)
	set(options)
	set(oneValueArgs MD5 NAME DIR)
	set(multiValueArgs LINK)
	cmake_parse_arguments(DAU
		"${options}" "${oneValueArgs}"
		"${multiValueArgs}" ${ARGN})

	list(LENGTH DAU_LINK NUMBER_OF_LINKS)
	if(NUMBER_OF_LINKS LESS 1)
		message(FATAL_ERROR "Need to specify at least a download link.")
	endif()

	set(ZIP ${CMAKE_CURRENT_LIST_DIR}/tmp.zip)

	foreach(LINK ${DAU_LINK})
		message(STATUS "Downloading ${DAU_NAME} from ${LINK}")

		file(DOWNLOAD ${LINK} ${ZIP})

		file(MD5 ${ZIP} MD5SUM)

		# no md5 argument given, no checksum needed...
		if (NOT DEFINED DAU_MD5)
			set(DAU_MD5 ${MD5SUM})
		endif()

		# manual md5 check, avoid to fail when a download fails
		if(MD5SUM STREQUAL ${DAU_MD5}) # download successful
			message(STATUS "${DAU_NAME} downloaded.")
			message(STATUS "Extracting ${DAU_NAME} archive...")
			file(ARCHIVE_EXTRACT
				INPUT ${ZIP}
				DESTINATION ${DAU_DIR})
			message(STATUS "${DAU_NAME} archive extracted.")
			file(REMOVE ${ZIP})
			set(download_and_unzip_SUCCESS TRUE PARENT_SCOPE)
			break() # done
		else()
			file(REMOVE ${ZIP})
			message(STATUS "${DAU_NAME} download failed...")
			# next link in DAU_LINK list...
		endif()
	endforeach()
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
endfunction()

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
		COMMAND plutil -replace CFBundleDocumentTypes -xml '<array/>' ${ARG_FILE}
		COMMAND plutil -replace CFBundleIdentifier -string 'com.vcg.meshlab' ${ARG_FILE}
		COMMAND plutil -insert NSRequiresAquaSystemAppearance -bool True ${ARG_FILE} || (exit 0)
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
endfunction()
