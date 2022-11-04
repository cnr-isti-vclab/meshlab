# Copyright 2019, 2020, Collabora, Ltd.
# Copyright 2019, 2020, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

option(MESHLAB_ALLOW_DOWNLOAD_SOURCE_NEXUS "Allow download and use of nexus source" ON)

if(MESHLAB_ALLOW_DOWNLOAD_SOURCE_NEXUS)
	# todo - make release of nexus and corto
	set(NEXUS_DIR ${MESHLAB_EXTERNAL_DOWNLOAD_DIR}/nexus-master)
	set(CORTO_DIR ${NEXUS_DIR}/src/corto)

	if (NOT EXISTS "${NEXUS_DIR}/CMakeLists.txt")
		set(NEXUS_LINK https://github.com/cnr-isti-vclab/nexus/archive/refs/heads/master.zip)
		download_and_unzip(${NEXUS_LINK} ${MESHLAB_EXTERNAL_DOWNLOAD_DIR} "nexus")
	endif()

	if (NOT EXISTS "${CORTO_DIR}/CMakeLists.txt")
		file(REMOVE_RECURSE ${NEXUS_DIR}/src/corto)
		set(CORTO_LINK https://github.com/cnr-isti-vclab/corto/archive/refs/heads/master.zip)
		download_and_unzip(${CORTO_LINK} "${NEXUS_DIR}/src/" "corto")
		file(RENAME ${NEXUS_DIR}/src/corto-master ${NEXUS_DIR}/src/corto)
	endif()

	message(STATUS "- nexus - using downloaded source")

	set (BUILD_NXS_BUILD OFF)
	set (BUILD_NXS_EDIT OFF)
	set (BUILD_NXS_VIEW OFF)
	add_subdirectory(${NEXUS_DIR} EXCLUDE_FROM_ALL)
	add_library(external-nexus INTERFACE)
	target_link_libraries(external-nexus INTERFACE nexus)
endif()
