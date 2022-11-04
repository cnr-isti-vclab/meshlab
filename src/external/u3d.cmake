# Copyright 2019, 2020, Collabora, Ltd.
# Copyright 2019, 2020, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

option(MESHLAB_ALLOW_DOWNLOAD_SOURCE_U3D "Allow download and use of u3d source" ON)

if(MESHLAB_ALLOW_DOWNLOAD_SOURCE_U3D)
	# todo - make release of u3d
	set(U3D_DIR "${MESHLAB_EXTERNAL_DOWNLOAD_DIR}/u3d-master")
	set(U3D_CHECK "${U3D_DIR}/CMakeLists.txt")

	if (NOT EXISTS ${U3D_CHECK})
		set(U3D_LINK https://github.com/alemuntoni/u3d/archive/refs/heads/master.zip)
		download_and_unzip(
			LINK ${U3D_LINK}
			DIR ${MESHLAB_EXTERNAL_DOWNLOAD_DIR}
			NAME "u3d")
		if (NOT download_and_unzip_SUCCESS)
			message(STATUS "- u3d - download failed.")
		endif()
	endif()

	if (EXISTS ${U3D_CHECK})
		message(STATUS "- u3d - using bundled source")

		set(MESSAGE_QUIET ON)
		add_subdirectory(${U3D_DIR})
		unset(MESSAGE_QUIET)
		install(
			TARGETS
				IFXCore
				IFXExporting
				IFXScheduling
				IDTF-to-U3D-converter
			DESTINATION
				${MESHLAB_LIB_INSTALL_DIR})
	endif()
endif()
