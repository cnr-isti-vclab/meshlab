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
	set(QHULL_DIR "${MESHLAB_EXTERNAL_DOWNLOAD_DIR}/qhull-2020.2")
	set(QHULL_CHECK "${QHULL_DIR}/src/libqhull_r/libqhull_r.h")

	if (NOT EXISTS ${QHULL_CHECK})
		set(QHULL_LINK
			https://github.com/qhull/qhull/archive/refs/tags/2020.2.zip
			https://www.meshlab.net/data/libs/qhull-2020.2.zip)
		set(QHULL_MD5 a0a9b0e69bdbd9461319b8d2ac3d2f2e)
		download_and_unzip(
			NAME "Qhull"
			LINK ${QHULL_LINK}
			MD5 ${QHULL_MD5}
			DIR ${MESHLAB_EXTERNAL_DOWNLOAD_DIR})
		if (NOT download_and_unzip_SUCCESS)
			message(STATUS "- Qhull - download failed.")
		else()
			# remove target uninstall to avoid duplicates
			# todo - make PR to Qhull to solve this
			file(READ "${QHULL_DIR}/CMakeLists.txt" FILE_CONTENTS)
			string(REPLACE "uninstall" "qhull-uninstall" FILE_CONTENTS "${FILE_CONTENTS}")
			file(WRITE "${QHULL_DIR}/CMakeLists.txt" "${FILE_CONTENTS}")
		endif()
	endif()

	if (EXISTS ${QHULL_CHECK})
		message(STATUS "- qhull - using downloaded source")

		set(MESSAGE_QUIET ON)
		add_subdirectory(${QHULL_DIR} EXCLUDE_FROM_ALL)
		unset(MESSAGE_QUIET)

		add_library(external-qhull INTERFACE)
		target_link_libraries(external-qhull INTERFACE qhullstatic_r)
		target_include_directories(external-qhull INTERFACE "${QHULL_DIR}/src")
	endif()
endif()
