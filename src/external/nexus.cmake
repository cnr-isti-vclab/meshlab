# Copyright 2019, 2020, Collabora, Ltd.
# Copyright 2019, 2020, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

option(MESHLAB_ALLOW_DOWNLOAD_SOURCE_NEXUS "Allow download and use of nexus source" ON)

if(MESHLAB_ALLOW_DOWNLOAD_SOURCE_NEXUS)
	# todo - make release of nexus and corto
	set(NEXUS_DIR "${MESHLAB_EXTERNAL_DOWNLOAD_DIR}/nexus-master")
	set(NEXUS_CHECK "${NEXUS_DIR}/CMakeLists.txt")
	set(CORTO_DIR "${NEXUS_DIR}/src/corto")
	set(CORTO_CHECK "${CORTO_DIR}/CMakeLists.txt")

	if (NOT EXISTS ${NEXUS_CHECK})
		set(NEXUS_LINK
			https://github.com/cnr-isti-vclab/nexus/archive/refs/heads/master.zip
			https://www.meshlab.net/data/libs/nexus-master.zip)
		#set(NEXUS_MD5 3e50878dbaedd140f3e8c34d5af9a9d9)
		download_and_unzip(
			NAME "nexus"
			LINK ${NEXUS_LINK}
			#MD5 ${NEXUS_MD5}
			DIR ${MESHLAB_EXTERNAL_DOWNLOAD_DIR})
		if (NOT download_and_unzip_SUCCESS)
			message(STATUS "- nexus - download failed.")
		endif()
	endif()

	if (NOT EXISTS ${CORTO_CHECK})
		file(REMOVE_RECURSE ${NEXUS_DIR}/src/corto)
		set(CORTO_LINK
			https://github.com/cnr-isti-vclab/corto/archive/refs/heads/master.zip
			https://www.meshlab.net/data/libs/corto-master.zip)
		#set(CORTO_MD5 ede1b41e369a8117d8f2f46fba89e11d)
		download_and_unzip(
			NAME "corto"
			LINK ${CORTO_LINK}
			#MD5 ${CORTO_MD5}
			DIR "${NEXUS_DIR}/src/")
		if (NOT download_and_unzip_SUCCESS)
			message(STATUS "- corto - download failed.")
		else()
			file(RENAME ${NEXUS_DIR}/src/corto-master ${NEXUS_DIR}/src/corto)
		endif()
	endif()

	if (EXISTS ${NEXUS_CHECK} AND EXISTS ${CORTO_CHECK})
		message(STATUS "- nexus - using downloaded source")

		set (BUILD_NXS_BUILD OFF)
		set (BUILD_NXS_EDIT OFF)
		set (BUILD_NXS_VIEW OFF)
		add_subdirectory(${NEXUS_DIR} EXCLUDE_FROM_ALL)
		add_library(external-nexus INTERFACE)
		target_link_libraries(external-nexus INTERFACE nexus)
	endif()
endif()
