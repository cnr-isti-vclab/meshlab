# Copyright 2019, 2020, Collabora, Ltd.
# Copyright 2019, 2020, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

option(MESHLAB_ALLOW_DOWNLOAD_SOURCE_OPENCTM "Allow download and use of OpenCTM source" ON)
option(MESHLAB_ALLOW_SYSTEM_OPENCTM "Allow use of system-provided OpenCTM" ON)

find_package(OpenCTM)

if(MESHLAB_ALLOW_SYSTEM_OPENCTM AND TARGET OpenCTM::OpenCTM)
	message(STATUS "- OpenCTM - using system-provided library")
	add_library(external-openctm INTERFACE)
	target_link_libraries(external-openctm INTERFACE OpenCTM::OpenCTM)
elseif(MESHLAB_ALLOW_DOWNLOAD_SOURCE_OPENCTM)
	set(OPENCTM_DIR "${MESHLAB_EXTERNAL_DOWNLOAD_DIR}/OpenCTM-1.0.3")
	set(OPENCTM_CHECK "${OPENCTM_DIR}/lib/openctm.c")

	if (NOT EXISTS ${OPENCTM_CHECK})
		set(OPENCTM_LINK
			https://sourceforge.net/projects/openctm/files/OpenCTM-1.0.3/OpenCTM-1.0.3-src.zip/download
			https://www.meshlab.net/data/libs/OpenCTM-1.0.3-src.zip)
		set(OPENCTM_MD5 a87b6a6509ae0712b4a55f8887dc6a54)
		download_and_unzip(
			NAME "OpenCTM"
			LINK ${OPENCTM_LINK}
			MD5 ${OPENCTM_MD5}
			DIR ${MESHLAB_EXTERNAL_DOWNLOAD_DIR})
		if (NOT download_and_unzip_SUCCESS)
			message(STATUS "- OpenCTM - download failed.")
		endif()
	endif()

	if (EXISTS ${OPENCTM_CHECK})
		message(STATUS "- OpenCTM - using downloaded source")
		add_library(
			external-openctm STATIC
			"${OPENCTM_DIR}/lib/openctm.c"
			"${OPENCTM_DIR}/lib/stream.c"
			"${OPENCTM_DIR}/lib/compressRAW.c"
			"${OPENCTM_DIR}/lib/compressMG1.c"
			"${OPENCTM_DIR}/lib/compressMG2.c"
			"${OPENCTM_DIR}/lib/liblzma/Alloc.c"
			"${OPENCTM_DIR}/lib/liblzma/LzFind.c"
			"${OPENCTM_DIR}/lib/liblzma/LzmaDec.c"
			"${OPENCTM_DIR}/lib/liblzma/LzmaEnc.c"
			"${OPENCTM_DIR}/lib/liblzma/LzmaLib.c")
		target_compile_definitions(external-openctm PUBLIC OPENCTM_STATIC)
		target_include_directories(
			external-openctm SYSTEM
			PUBLIC "${OPENCTM_DIR}/lib" SYSTEM
			PRIVATE "${OPENCTM_DIR}/lib/liblzma")
		if(MSVC)
			target_compile_definitions(external-openctm PRIVATE _CRT_SECURE_NO_WARNINGS)
		endif()
		target_link_libraries(external-openctm PRIVATE external-disable-warnings)
	endif()
endif()
