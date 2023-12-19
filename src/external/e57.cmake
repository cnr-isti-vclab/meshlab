# Copyright 2019, 2020, Collabora, Ltd.
# Copyright 2019, 2020, 2021 Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

option(MESHLAB_ALLOW_DOWNLOAD_SOURCE_LIBE57 "Allow download and use of libE57Format source" ON)

set(LIBE57_VER 2.3.0)

if (TARGET XercesC::XercesC)
	if(MESHLAB_ALLOW_DOWNLOAD_SOURCE_LIBE57)
		set(LIBE57_DIR "${MESHLAB_EXTERNAL_DOWNLOAD_DIR}/libE57Format-${LIBE57_VER}")
		set(LIBE57_CHECK "${LIBE57_DIR}/CMakeLists.txt")

		if (NOT EXISTS ${LIBE57_CHECK})
			set(LIBE57_LINK
				https://github.com/asmaloney/libE57Format/archive/refs/tags/v${LIBE57_VER}.zip
				https://www.meshlab.net/data/libs/libE57Format-${LIBE57_VER}.zip)
			set(LIBE57_MD5 958ada3883f9b60195f79bbab156f3e3)
			download_and_unzip(
				NAME "LibE57"
				MD5 ${LIBE57_MD5}
				LINK ${LIBE57_LINK}
				DIR ${MESHLAB_EXTERNAL_DOWNLOAD_DIR})
			if (NOT download_and_unzip_SUCCESS)
				message(STATUS "- LibE57 - download failed.")
			endif()
		endif()

		if (EXISTS ${LIBE57_CHECK})
			message(STATUS "- libE57 - using downloaded source")

			set(MESSAGE_QUIET ON)
			if (APPLE)
				set(E57_BUILD_SHARED OFF)
			endif()
			add_subdirectory(${LIBE57_DIR} EXCLUDE_FROM_ALL)
			unset(MESSAGE_QUIET)

			add_library(external-libE57 INTERFACE)
			target_link_libraries(external-libE57 INTERFACE E57Format)
			if (NOT APPLE)
				install(TARGETS E57Format DESTINATION ${MESHLAB_LIB_INSTALL_DIR})
			endif()
		endif()
	endif()
else()
	message(STATUS - "Cannot build e57 because Xerces dependence not satisfied.")
endif()
