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
	set(MUPARSER_DIR "${MESHLAB_EXTERNAL_DOWNLOAD_DIR}/muparser-2.3.3-1")
	set(MUPARSER_CHECK "${MUPARSER_DIR}/src/muParser.cpp")

	if (NOT EXISTS ${MUPARSER_CHECK})
		set(MUPARSER_LINK https://github.com/beltoforion/muparser/archive/refs/tags/v2.3.3-1.zip)
		download_and_unzip(
			LINK ${MUPARSER_LINK}
			DIR ${MESHLAB_EXTERNAL_DOWNLOAD_DIR}
			NAME "muparser")
		if (NOT download_and_unzip_SUCCESS)
			message(STATUS "- muparser - download failed.")
		endif()
	endif()

	if (EXISTS ${MUPARSER_CHECK})
		message(STATUS "- muparser - using downloaded source")

		set(ENABLE_SAMPLES OFF)
		set(MESSAGE_QUIET ON)
		add_subdirectory(${MUPARSER_DIR})
		unset(MESSAGE_QUIET)
		unset(ENABLE_SAMPLES)

		add_library(external-muparser INTERFACE)
		target_link_libraries(external-muparser INTERFACE muparser)
		install(TARGETS muparser DESTINATION ${MESHLAB_LIB_INSTALL_DIR})
	endif()
endif()
