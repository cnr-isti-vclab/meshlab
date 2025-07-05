# Copyright 2019, 2020, Collabora, Ltd.
# Copyright 2019, 2020, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

option(MESHLAB_ALLOW_DOWNLOAD_SOURCE_MUPARSER "Allow download and use of muparser source" ON)
option(MESHLAB_ALLOW_SYSTEM_MUPARSER "Allow use of system-provided muparser" ON)

find_package(muparser)

if(MESHLAB_ALLOW_SYSTEM_MUPARSER AND TARGET muparser::muparser)
	message(STATUS "- muparser - using system-provided library")
	add_library(external-muparser INTERFACE)
	target_link_libraries(external-muparser INTERFACE muparser::muparser)
elseif(MESHLAB_ALLOW_DOWNLOAD_SOURCE_MUPARSER)
	set(MUPARSER_VERSION "2.3.5")
	set(MUPARSER_DIR "${MESHLAB_EXTERNAL_DOWNLOAD_DIR}/muparser-${MUPARSER_VERSION}")
	set(MUPARSER_CHECK "${MUPARSER_DIR}/src/muParser.cpp")

	if (NOT EXISTS ${MUPARSER_CHECK})
		set(MUPARSER_LINK
			https://github.com/beltoforion/muparser/archive/refs/tags/v${MUPARSER_VERSION}.zip
			https://www.meshlab.net/data/libs/muparser-${MUPARSER_VERSION}.zip)
		set(MUPARSER_MD5 274d3e3c8b9a48b94f3e281820a00c57)
		download_and_unzip(
			NAME "muparser"
			LINK ${MUPARSER_LINK}
			MD5 ${MUPARSER_MD5}
			DIR ${MESHLAB_EXTERNAL_DOWNLOAD_DIR})
		if (NOT download_and_unzip_SUCCESS)
			message(STATUS "- muparser - download failed.")
		endif()
	endif()

	if (EXISTS ${MUPARSER_CHECK})
		message(STATUS "- muparser - using downloaded source")

		set(ENABLE_SAMPLES OFF)
		set(MESSAGE_QUIET ON)
		if(NOT OpenMP_CXX_FOUND)
			set(ENABLE_OPENMP OFF)
		endif()
		if(APPLE)
			set(BUILD_SHARED_LIBS OFF)
		endif()
		add_subdirectory(${MUPARSER_DIR} EXCLUDE_FROM_ALL)
		if (APPLE AND OpenMP_CXX_FOUND)
			target_link_libraries(muparser PRIVATE OpenMP::OpenMP_CXX)
		endif()
		if(NOT OpenMP_CXX_FOUND)
			unset(ENABLE_OPENMP)
		endif()
		unset(MESSAGE_QUIET)
		unset(ENABLE_SAMPLES)
		

		add_library(external-muparser INTERFACE)
		target_link_libraries(external-muparser INTERFACE muparser)
		if (NOT APPLE)
			install(TARGETS muparser DESTINATION ${MESHLAB_LIB_INSTALL_DIR})
		endif()
	endif()
endif()
