# Copyright 2019, 2020, Collabora, Ltd.
# Copyright 2019, 2020, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

option(MESHLAB_ALLOW_DOWNLOAD_SOURCE_EASYEXIF "Allow download and use of EasyExif source" ON)

if(MESHLAB_ALLOW_DOWNLOAD_SOURCE_EASYEXIF)
	set(EASYEXIF_DIR ${MESHLAB_EXTERNAL_DOWNLOAD_DIR}/easyexif-1.0)

	if (NOT EXISTS "${EASYEXIF_DIR}/exif.h")
		set(EASYEXIF_LINK https://github.com/mayanklahiri/easyexif/archive/refs/tags/v1.0.zip)
		download_and_unzip(${EASYEXIF_LINK} ${MESHLAB_EXTERNAL_DOWNLOAD_DIR} "EasyExif")
	endif()

	message(STATUS "- EasyExif - using downloaded source")
	add_library(external-easyexif STATIC ${EASYEXIF_DIR}/exif.h ${EASYEXIF_DIR}/exif.cpp)
	target_include_directories(external-easyexif PUBLIC ${EASYEXIF_DIR})
else()
	message(
		FATAL_ERROR
			"EeasyExif is required - MESHLAB_ALLOW_DOWNLOAD_SOURCE_EASYEXIF must be ON.")
endif()
