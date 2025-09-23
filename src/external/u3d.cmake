# Copyright 2019, 2020, Collabora, Ltd.
# Copyright 2019, 2020, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

option(MESHLAB_ALLOW_DOWNLOAD_SOURCE_U3D "Allow download and use of u3d source" ON)

set(U3D_VERSION 1.5.2)

if(MESHLAB_ALLOW_DOWNLOAD_SOURCE_U3D)
	set(U3D_DIR "${MESHLAB_EXTERNAL_DOWNLOAD_DIR}/u3d-${U3D_VERSION}")
	set(U3D_CHECK "${U3D_DIR}/CMakeLists.txt")

	if (NOT EXISTS ${U3D_CHECK})
		set(U3D_LINK
			https://github.com/alemuntoni/u3d/archive/refs/tags/${U3D_VERSION}.zip
			https://www.meshlab.net/data/libs/u3d-${U3D_VERSION}.zip)
		set(U3D_MD5 28d28c03e39166bc2f756e7f6d769c02)
		download_and_unzip(
			NAME "u3d"
			LINK ${U3D_LINK}
			MD5 ${U3D_MD5}
			DIR ${MESHLAB_EXTERNAL_DOWNLOAD_DIR})
		if (NOT download_and_unzip_SUCCESS)
			message(STATUS "- u3d - download failed.")
		endif()
	endif()

	if (EXISTS ${U3D_CHECK})
		message(STATUS "- u3d - using downloaded source")

		set(U3D_BUILD_LIBIDTF_TEST OFF)
		set(U3D_BUILD_IDTF_TO_U3D_BIN OFF)
		set(U3D_BUILD_STATIC_IDTF_LIB ON)
		set(U3D_INSTALL_LIBS OFF)
		set(MESSAGE_QUIET ON)
		add_subdirectory(${U3D_DIR})
		unset(MESSAGE_QUIET)
		unset(U3D_BUILD_LIBIDTF_TEST)
		unset(U3D_BUILD_IDTF_TO_U3D_BIN)
		install(
			TARGETS
				IDTF
				IFXCore
				IFXExporting
				IFXScheduling
			DESTINATION
				${MESHLAB_LIB_INSTALL_DIR})
	endif()
endif()
