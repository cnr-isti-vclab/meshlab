# Copyright 2019, 2020, Collabora, Ltd.
# Copyright 2019, 2020, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

option(MESHLAB_ALLOW_DOWNLOAD_SOURCE_NEXUS "Allow download and use of nexus source" ON)

set(CORTO_VER 2025.07)
set(NEXUS_VER 2025.05)

if(MESHLAB_ALLOW_DOWNLOAD_SOURCE_NEXUS)
	set(NEXUS_DIR "${MESHLAB_EXTERNAL_DOWNLOAD_DIR}/nexus-${NEXUS_VER}")
	set(NEXUS_CHECK "${NEXUS_DIR}/CMakeLists.txt")
	set(CORTO_DIR "${NEXUS_DIR}/src/corto")
	set(CORTO_CHECK "${CORTO_DIR}/CMakeLists.txt")

	if (NOT EXISTS ${NEXUS_CHECK})
		set(NEXUS_LINK
			https://github.com/cnr-isti-vclab/nexus/archive/refs/tags/v${NEXUS_VER}.zip
			https://www.meshlab.net/data/libs/nexus-${NEXUS_VER}.zip)
		set(NEXUS_MD5 e2be7636fb4c901c6ba12b3e4a0a3633)
		download_and_unzip(
			NAME "nexus"
			LINK ${NEXUS_LINK}
			MD5 ${NEXUS_MD5}
			DIR ${MESHLAB_EXTERNAL_DOWNLOAD_DIR})
		if (NOT download_and_unzip_SUCCESS)
			message(STATUS "- nexus - download failed.")
		endif()
	endif()

	if (NOT EXISTS ${CORTO_CHECK})
		file(REMOVE_RECURSE ${NEXUS_DIR}/src/corto)
		set(CORTO_LINK
			https://github.com/cnr-isti-vclab/corto/archive/refs/tags/v${CORTO_VER}.zip
			https://www.meshlab.net/data/libs/corto-${CORTO_VER}.zip)
		set(CORTO_MD5 93c78a0b5e238e0e7c35bfbd1ac4249c)
		download_and_unzip(
			NAME "corto"
			LINK ${CORTO_LINK}
			MD5 ${CORTO_MD5}
			DIR "${NEXUS_DIR}/src/")
		if (NOT download_and_unzip_SUCCESS)
			message(STATUS "- corto - download failed.")
		else()
			file(RENAME ${NEXUS_DIR}/src/corto-${CORTO_VER} ${NEXUS_DIR}/src/corto)
		endif()
	endif()

	if (EXISTS ${NEXUS_CHECK} AND EXISTS ${CORTO_CHECK})
		message(STATUS "- nexus - using downloaded source")

		set (BUILD_NXS_BUILD OFF)
		set (BUILD_NXS_EDIT OFF)
		set (BUILD_NXS_VIEW OFF)
		set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
		add_subdirectory(${NEXUS_DIR} EXCLUDE_FROM_ALL)
		unset(CMAKE_POLICY_VERSION_MINIMUM)
		add_library(external-nexus INTERFACE)
		target_link_libraries(external-nexus INTERFACE nexus)
	endif()
endif()
