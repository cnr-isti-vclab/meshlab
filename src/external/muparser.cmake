# Copyright 2019, 2020, Collabora, Ltd.
# Copyright 2019, 2020, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

option(MESHLAB_ALLOW_DOWNLOAD_SOURCE_MUPARSER "Allow use of bundled muparser source" ON)
option(MESHLAB_ALLOW_SYSTEM_MUPARSER "Allow use of system-provided muparser" ON)

find_package(muparser)

if(MESHLAB_ALLOW_SYSTEM_MUPARSER AND TARGET muparser::muparser)
	message(STATUS "- muparser - using system-provided library")
	add_library(external-muparser INTERFACE)
	target_link_libraries(external-muparser INTERFACE muparser::muparser)
elseif(MESHLAB_ALLOW_DOWNLOAD_SOURCE_MUPARSER)
	set(MUPARSER_VER 2.3.3-1)
	set(MUPARSER_DIR ${MESHLAB_EXTERNAL_DOWNLOAD_DIR}/muparser-${MUPARSER_VER})

	if (NOT EXISTS "${MUPARSER_DIR}/src/muParser.cpp")
		set(MUPARSER_LINK https://github.com/beltoforion/muparser/archive/refs/tags/v${MUPARSER_VER}.zip)
		download_and_unzip(${MUPARSER_LINK} ${MESHLAB_EXTERNAL_DOWNLOAD_DIR} "muparser")
	endif()

	message(STATUS "- muparser - using downloaded source")

	set(BUILD_SHARED_LIBS OFF)
	set(ENABLE_SAMPLES OFF)
	set(MESSAGE_QUIET ON)
	add_subdirectory(${MUPARSER_DIR})
	unset(MESSAGE_QUIET)
	unset(BUILD_SHARED_LIBS)
	unset(ENABLE_SAMPLES)

	add_library(external-muparser INTERFACE)
	target_link_libraries(external-muparser INTERFACE muparser)
endif()
