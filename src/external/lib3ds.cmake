# Copyright 2019, 2020, Collabora, Ltd.
# Copyright 2019, 2020, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0


option(MESHLAB_ALLOW_DOWNLOAD_SOURCE_LIB3DS "Allow download and use of lib3ds source" ON)
option(MESHLAB_ALLOW_SYSTEM_LIB3DS "Allow use of system-provided lib3ds" ON)

find_package(Lib3ds QUIET)

if(MESHLAB_ALLOW_SYSTEM_LIB3DS AND TARGET Lib3ds::Lib3ds)
	message(STATUS "- lib3ds - using system-provided library")
	add_library(external-lib3ds INTERFACE)
	target_link_libraries(external-lib3ds INTERFACE Lib3ds::Lib3ds)
elseif(MESHLAB_ALLOW_DOWNLOAD_SOURCE_LIB3DS)
	set(LIB3DS_DIR "${MESHLAB_EXTERNAL_DOWNLOAD_DIR}/lib3ds-1.3.0")
	set(LIB3DS_CHECK "${LIB3DS_DIR}/lib3ds/types.h")

	if (NOT EXISTS ${LIB3DS_CHECK})
		set(LIB3DS_LINK
			http://storage.googleapis.com/google-code-archive-downloads/v2/code.google.com/lib3ds/lib3ds-1.3.0.zip
			https://www.meshlab.net/data/libs/lib3ds-1.3.0.zip)
		set(LIB3DS_MD5 2572f7b0f29b591d494c1a0658b35c86)
		download_and_unzip(
			NAME "Lib3DS"
			LINK ${LIB3DS_LINK}
			MD5 ${LIB3DS_MD5}
			DIR ${MESHLAB_EXTERNAL_DOWNLOAD_DIR})
		if (NOT download_and_unzip_SUCCESS)
			message(STATUS "- Lib3DS - download failed.")
		endif()
	endif()

	if (EXISTS ${LIB3DS_CHECK})
		message(STATUS "- lib3ds - using downloaded source")
		set(MODE SHARED)
		if (APPLE)
			set(MODE STATIC)
		endif()
		add_library(
			external-lib3ds ${MODE}
			"${LIB3DS_DIR}/lib3ds/atmosphere.c"
			"${LIB3DS_DIR}/lib3ds/atmosphere.h"
			"${LIB3DS_DIR}/lib3ds/background.c"
			"${LIB3DS_DIR}/lib3ds/background.h"
			"${LIB3DS_DIR}/lib3ds/camera.c"
			"${LIB3DS_DIR}/lib3ds/camera.h"
			"${LIB3DS_DIR}/lib3ds/chunk.c"
			"${LIB3DS_DIR}/lib3ds/chunk.h"
			"${LIB3DS_DIR}/lib3ds/chunktable.h"
			"${LIB3DS_DIR}/lib3ds/ease.c"
			"${LIB3DS_DIR}/lib3ds/ease.h"
			"${LIB3DS_DIR}/lib3ds/file.c"
			"${LIB3DS_DIR}/lib3ds/file.h"
			"${LIB3DS_DIR}/lib3ds/io.c"
			"${LIB3DS_DIR}/lib3ds/io.h"
			"${LIB3DS_DIR}/lib3ds/light.c"
			"${LIB3DS_DIR}/lib3ds/light.h"
			"${LIB3DS_DIR}/lib3ds/material.c"
			"${LIB3DS_DIR}/lib3ds/material.h"
			"${LIB3DS_DIR}/lib3ds/matrix.c"
			"${LIB3DS_DIR}/lib3ds/matrix.h"
			"${LIB3DS_DIR}/lib3ds/mesh.c"
			"${LIB3DS_DIR}/lib3ds/mesh.h"
			"${LIB3DS_DIR}/lib3ds/node.c"
			"${LIB3DS_DIR}/lib3ds/node.h"
			"${LIB3DS_DIR}/lib3ds/quat.c"
			"${LIB3DS_DIR}/lib3ds/quat.h"
			"${LIB3DS_DIR}/lib3ds/shadow.c"
			"${LIB3DS_DIR}/lib3ds/shadow.h"
			"${LIB3DS_DIR}/lib3ds/tcb.c"
			"${LIB3DS_DIR}/lib3ds/tcb.h"
			"${LIB3DS_DIR}/lib3ds/tracks.c"
			"${LIB3DS_DIR}/lib3ds/tracks.h"
			"${LIB3DS_DIR}/lib3ds/types.h"
			"${LIB3DS_DIR}/lib3ds/vector.c"
			"${LIB3DS_DIR}/lib3ds/vector.h"
			"${LIB3DS_DIR}/lib3ds/viewport.c"
			"${LIB3DS_DIR}/lib3ds/viewport.h")

		target_include_directories(external-lib3ds SYSTEM PUBLIC "${LIB3DS_DIR}")
		target_link_libraries(external-lib3ds PRIVATE external-disable-warnings)
		if (NOT APPLE)
			install(TARGETS external-lib3ds DESTINATION ${MESHLAB_LIB_INSTALL_DIR})
		endif()
	endif()
endif()
