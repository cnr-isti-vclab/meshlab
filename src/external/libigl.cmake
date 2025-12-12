# Copyright 2019, 2021, Collabora, Ltd.
# Copyright 2019, 2021, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

option(MESHLAB_ALLOW_DOWNLOAD_SOURCE_LIBIGL "Allow download and use of LibIGL source" ON)
option(MESHLAB_ALLOW_SYSTEM_LIBIGL "Allow use of system-provided LibIGL" ON)

set(LIBIGL_VER 2.6.0)

find_path(libigl_INCLUDE_DIR NAMES igl_inline.h PATH_SUFFIXES igl PATHS ${libigl_PREFIX})

if(MESHLAB_ALLOW_SYSTEM_LIBIGL AND libigl_INCLUDE_DIR)
	message(STATUS "- LibIGL - using system-provided library")
	add_library(external-libigl INTERFACE)
	target_include_directories(external-libigl INTERFACE ${libigl_INCLUDE_DIR})
elseif(MESHLAB_ALLOW_DOWNLOAD_SOURCE_LIBIGL)
	set(LIBIGL_DIR "${MESHLAB_EXTERNAL_DOWNLOAD_DIR}/libigl-${LIBIGL_VER}")
	set(LIBIGL_CHECK "${LIBIGL_DIR}/include/igl/igl_inline.h")

	if (NOT EXISTS ${LIBIGL_CHECK})
		set(LIBIGL_LINK
			https://github.com/libigl/libigl/archive/refs/tags/v${LIBIGL_VER}.zip
			https://www.meshlab.net/data/libs/libigl-${LIBIGL_VER}.zip)
		set(LIBIGL_MD5 0b4fea5dba2117b8db85c99a39a71f83)
		download_and_unzip(
			NAME "LibIGL"
			LINK ${LIBIGL_LINK}
			MD5 ${LIBIGL_MD5}
			DIR ${MESHLAB_EXTERNAL_DOWNLOAD_DIR})
		if (NOT download_and_unzip_SUCCESS)
			message(STATUS "- LibIGL - download failed.")
		endif()
	endif()

	if (EXISTS ${LIBIGL_CHECK})
		message(STATUS "- libigl - using downloaded source")
		add_library(external-libigl INTERFACE)
		target_include_directories(external-libigl INTERFACE "${LIBIGL_DIR}/include/")
	endif()
endif()
