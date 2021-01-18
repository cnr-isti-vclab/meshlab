# Copyright 2019, 2020, Collabora, Ltd.
# Copyright 2019, 2020, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

option(ALLOW_BUNDLED_OPENCTM "Allow use of bundled OpenCTM source" ON)
option(ALLOW_SYSTEM_OPENCTM "Allow use of system-provided OpenCTM" ON)

find_package(OpenCTM)
set(OPENCTM_DIR ${EXTERNAL_DIR}/OpenCTM-1.0.3)

if(ALLOW_SYSTEM_OPENCTM AND TARGET OpenCTM::OpenCTM)
	message(STATUS "- OpenCTM - using system-provided library")
	add_library(external-openctm INTERFACE)
	target_link_libraries(external-openctm INTERFACE OpenCTM::OpenCTM)
elseif(ALLOW_BUNDLED_OPENCTM AND EXISTS "${OPENCTM_DIR}/lib/openctm.c")
	message(STATUS "- OpenCTM - using bundled source")
	# Modified liblzma included - can't build against system version
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
	set_property(TARGET external-openctm PROPERTY FOLDER External)
	target_link_libraries(external-openctm PRIVATE external-disable-warnings)
endif()
