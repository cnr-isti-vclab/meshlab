# Copyright 2019, 2020, Collabora, Ltd.
# Copyright 2019, 2020, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

option(MESHLAB_ALLOW_DOWNLOAD_SOURCE_QHULL "Allow download and use of Qhull source" ON)
option(MESHLAB_ALLOW_SYSTEM_QHULL "Allow use of system-provided QHull" ON)

find_package(Qhull 8 COMPONENTS qhull_r CONFIG)

if(MESHLAB_ALLOW_SYSTEM_QHULL AND TARGET Qhull::qhull_r)
	message(STATUS "- qhull - using system-provided library")
	add_library(external-qhull INTERFACE)
	target_link_libraries(external-qhull INTERFACE Qhull::qhull_r)
elseif(MESHLAB_ALLOW_DOWNLOAD_SOURCE_QHULL)
	set(QHULL_VER 2020.2)
	set(QHULL_DIR ${MESHLAB_EXTERNAL_DOWNLOAD_DIR}/qhull-${QHULL_VER})

	if (NOT EXISTS "${QHULL_DIR}/src/libqhull_r/libqhull_r.h")
		set(QHULL_LINK https://github.com/qhull/qhull/archive/refs/tags/${QHULL_VER}.zip)
		download_and_unzip(${QHULL_LINK} ${MESHLAB_EXTERNAL_DOWNLOAD_DIR} "Qhull")
	endif()

	message(STATUS "- qhull - using downloaded source")

	set(MESSAGE_QUIET ON)
	add_subdirectory(${QHULL_DIR} EXCLUDE_FROM_ALL)
	unset(MESSAGE_QUIET)

	if (WIN32)
		set(QHULL_TARGET qhullstatic_r)
	else()
		set(QHULL_TARGET qhull_r)
	endif()

	add_library(external-qhull INTERFACE)
	target_link_libraries(external-qhull INTERFACE ${QHULL_TARGET})
	target_include_directories(external-qhull INTERFACE "${QHULL_DIR}/src")
	if (NOT WIN32)
		install(TARGETS ${QHULL_TARGET} DESTINATION ${MESHLAB_LIB_INSTALL_DIR})
	endif()
endif()
