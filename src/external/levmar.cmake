# Copyright 2019, 2020, Collabora, Ltd.
# Copyright 2019, 2020, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

option(MESHLAB_ALLOW_DOWNLOAD_SOURCE_LEVMAR "Allow download and use of levmar source" ON)
option(MESHLAB_ALLOW_SYSTEM_LEVMAR "Allow use of system-provided levmar" ON)

find_path(levmar_INCLUDE_DIR NAMES levmar.h PATHS ${levmar_PREFIX})
find_library(levmar_LIBRARY NAMES levmar PATHS ${levmar_PREFIX})
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
	levmar
	DEFAULT_MSG
	levmar_LIBRARY
	levmar_INCLUDE_DIR
)
mark_as_advanced(levmar_INCLUDE_DIR levmar_LIBRARY)

if(MESHLAB_ALLOW_SYSTEM_LEVMAR AND levmar_FOUND)
	message(STATUS "- levmar - using system-provided library")
	add_library(external-levmar INTERFACE)
	target_link_libraries(external-levmar INTERFACE ${levmar_LIBRARY})
	target_include_directories(external-levmar INTERFACE ${levmar_INCLUDE_DIR})
elseif(MESHLAB_ALLOW_DOWNLOAD_SOURCE_LEVMAR)
	set(LEVMAR_VERSION "2.6.1")

	set(LEVMAR_DIR "${MESHLAB_EXTERNAL_DOWNLOAD_DIR}/levmar-${LEVMAR_VERSION}")
	set(LEVMAR_CHECK "${LEVMAR_DIR}/lm.h")

	if (NOT EXISTS ${LEVMAR_CHECK})
		set(LEVMAR_LINK
			https://github.com/alemuntoni/levmar/archive/refs/tags/${LEVMAR_VERSION}.zip
			https://www.meshlab.net/data/libs/levmar-${LEVMAR_VERSION}.zip)
		set(LEVMAR_MD5 ecc22109fcad6575645de6e1a87d2ea3)
		download_and_unzip(
			NAME "Levmar"
			LINK ${LEVMAR_LINK}
			MD5 ${LEVMAR_MD5}
			DIR ${MESHLAB_EXTERNAL_DOWNLOAD_DIR})
		if (NOT download_and_unzip_SUCCESS)
			message(STATUS "- Levmar - download failed.")
		endif()
	endif()

	if (EXISTS ${LEVMAR_CHECK})
		message(STATUS "- levmar - using downloaded source")

		set(HAVE_LAPACK 0 CACHE BOOL "Do we have LAPACK/BLAS?")
		set(BUILD_DEMO OFF)
		set(MESSAGE_QUIET ON)
		set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
		add_subdirectory(${LEVMAR_DIR})
		unset(CMAKE_POLICY_VERSION_MINIMUM)
		unset(MESSAGE_QUIET)
		unset(HAVE_LAPACK)
		unset(BUILD_DEMO)

		add_library(external-levmar INTERFACE)
		target_link_libraries(external-levmar INTERFACE levmar)
		target_include_directories(external-levmar INTERFACE ${LEVMAR_DIR})
	endif()
endif()
