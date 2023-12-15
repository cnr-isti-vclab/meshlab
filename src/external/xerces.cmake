# Copyright 2019, 2020, Collabora, Ltd.
# Copyright 2019, 2020, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

option(MESHLAB_ALLOW_DOWNLOAD_SOURCE_XERCES "Allow download and use of Xerces-C sources" ON)
option(MESHLAB_ALLOW_SYSTEM_XERCES "Allow use of system-provided Xerces-C" ON)

set(XERCES_C_VER 3.2.4)

find_package(XercesC)

if(MESHLAB_ALLOW_SYSTEM_XERCES AND TARGET XercesC::XercesC)
	message(STATUS "- Xerces-C - using system-provided library")
	add_library(external-xerces INTERFACE)
	target_link_libraries(external-xerces INTERFACE XercesC::XercesC)
elseif(MESHLAB_ALLOW_DOWNLOAD_SOURCE_XERCES)
	set(XERCES_C_DIR "${MESHLAB_EXTERNAL_DOWNLOAD_DIR}/xerces-c-${XERCES_C_VER}")
	set(XERCES_C_CHECK "${XERCES_C_DIR}/CMakeLists.txt")

	if(NOT EXISTS ${XERCES_C_CHECK})
		set(XERCES_C_LINK
			https://dlcdn.apache.org//xerces/c/3/sources/xerces-c-${XERCES_C_VER}.zip
			https://www.meshlab.net/data/libs/xerces-c-${XERCES_C_VER}.zip)
		set(XERCES_C_MD5 f84488fc2b8f62c4afca2f9943a42c00)
		download_and_unzip(
			NAME "Xerces-C"
			LINK ${XERCES_C_LINK}
			MD5 ${XERCES_C_MD5}
			DIR ${MESHLAB_EXTERNAL_DOWNLOAD_DIR})
		if (NOT download_and_unzip_SUCCESS)
			message(STATUS "- Xerces-C - download failed.")
		endif()
	endif()

	if(EXISTS ${XERCES_C_CHECK})
		message(STATUS "- Xerces-C - using downloaded source")

		set(MESSAGE_QUIET ON)
		add_subdirectory(${XERCES_C_DIR} EXCLUDE_FROM_ALL)
		unset(MESSAGE_QUIET)

		add_library(external-xerces INTERFACE)
		target_link_libraries(external-xerces INTERFACE xerces-c)
		target_include_directories(
			external-xerces INTERFACE
			${XERCES_C_DIR}/src
			${MESHLAB_EXTERNAL_BINARY_DIR}/xerces-c-${XERCES_C_VER}/src)
		add_library(XercesC::XercesC ALIAS external-xerces)
		install(TARGETS xerces-c DESTINATION ${MESHLAB_LIB_INSTALL_DIR})
	endif()
endif()
