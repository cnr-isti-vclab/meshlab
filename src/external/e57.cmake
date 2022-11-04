# Copyright 2019, 2020, Collabora, Ltd.
# Copyright 2019, 2020, 2021 Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

option(MESHLAB_ALLOW_DOWNLOAD_SOURCE_LIBE57 "Allow download and use of libE57Format source" ON)

if (TARGET XercesC::XercesC)
	if(MESHLAB_ALLOW_DOWNLOAD_SOURCE_LIBE57)
		set(LIBE57_DIR "${MESHLAB_EXTERNAL_DOWNLOAD_DIR}/libE57Format-2.3.0")
		set(LIBE57_CHECK "${LIBE57_DIR}/CMakeLists.txt")

		if (NOT EXISTS ${LIBE57_CHECK})
			set(LIBE57_LINK https://github.com/asmaloney/libE57Format/archive/refs/tags/v2.3.0.zip)
			download_and_unzip(
				LINK ${LIBE57_LINK}
				DIR ${MESHLAB_EXTERNAL_DOWNLOAD_DIR}
				NAME "LibE57")
			if (NOT download_and_unzip_SUCCESS)
				message(STATUS "- LibE57 - download failed.")
			endif()
		endif()

		if (EXISTS ${LIBE57_CHECK})
			message(STATUS "- libE57 - using downloaded source")

			set(MESSAGE_QUIET ON)
			add_subdirectory(${LIBE57_DIR} EXCLUDE_FROM_ALL)
			unset(MESSAGE_QUIET)

			add_library(external-libE57 INTERFACE)
			target_link_libraries(external-libE57 INTERFACE E57Format)
			install(TARGETS E57Format DESTINATION ${MESHLAB_LIB_INSTALL_DIR})
		endif()
	endif()
else()
	message(STATUS - "Cannot build e57 because Xerces dependence not satisfied.")
endif()
