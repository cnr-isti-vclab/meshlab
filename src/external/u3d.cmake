# Copyright 2019, 2020, Collabora, Ltd.
# Copyright 2019, 2020, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

option(MESHLAB_ALLOW_DOWNLOAD_SOURCE_U3D "Allow download and use of u3d source" ON)

if(MESHLAB_ALLOW_DOWNLOAD_SOURCE_U3D)
	# todo - make release of u3d
	set(U3D_DIR ${MESHLAB_EXTERNAL_DOWNLOAD_DIR}/u3d-master)

	if (NOT EXISTS ${U3D_DIR}/CMakeLists.txt)
		set(U3D_LINK https://github.com/alemuntoni/u3d/archive/refs/heads/master.zip)
		download_and_unzip(${U3D_LINK} ${MESHLAB_EXTERNAL_DOWNLOAD_DIR} "u3d")
	endif()

	message(STATUS "- u3d - using bundled source")

	set(MESSAGE_QUIET ON)
	add_subdirectory(${U3D_DIR})
	unset(MESSAGE_QUIET)
endif()
