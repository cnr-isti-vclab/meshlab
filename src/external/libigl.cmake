# Copyright 2019, 2021, Collabora, Ltd.
# Copyright 2019, 2021, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

option(MESHLAB_ALLOW_DOWNLOAD_SOURCE_LIBIGL "Allow download and use of LibIGL source" ON)

if(MESHLAB_ALLOW_DOWNLOAD_SOURCE_LIBIGL)
	set(LIBIGL_DIR "${MESHLAB_EXTERNAL_DOWNLOAD_DIR}/libigl-2.4.0")
	set(LIBIGL_CHECK "${LIBIGL_DIR}/include/igl/igl_inline.h")

	if (NOT EXISTS ${LIBIGL_CHECK})
		set(LIBIGL_LINK https://github.com/libigl/libigl/archive/refs/tags/v2.4.0.zip)
		download_and_unzip(
			LINK ${LIBIGL_LINK}
			DIR ${MESHLAB_EXTERNAL_DOWNLOAD_DIR}
			NAME "LibIGL")
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
