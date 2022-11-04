# Copyright 2019, 2020, Collabora, Ltd.
# Copyright 2019, 2020, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

option(MESHLAB_ALLOW_DOWNLOAD_SOURCE_EASYEXIF "Allow download and use of EasyExif source" ON)

if(MESHLAB_ALLOW_DOWNLOAD_SOURCE_EASYEXIF)
	set(EASYEXIF_DIR "${MESHLAB_EXTERNAL_DOWNLOAD_DIR}/easyexif-1.0")
	set(EASYEXIF_CHECK "${EASYEXIF_DIR}/exif.h")

	if (NOT EXISTS ${EASYEXIF_CHECK})
		set(EASYEXIF_LINK https://github.com/mayanklahiri/easyexif/archive/refs/tags/v1.0.zip)
		download_and_unzip(
			LINK ${EASYEXIF_LINK}
			DIR ${MESHLAB_EXTERNAL_DOWNLOAD_DIR}
			NAME "EasyExif")
		if (NOT download_and_unzip_SUCCESS)
			message(FATAL_ERROR "- EasyExif - download failed.")
		endif()
	endif()

	if (EXISTS ${EASYEXIF_CHECK})
		message(STATUS "- EasyExif - using downloaded source")
		add_library(external-easyexif STATIC ${EASYEXIF_DIR}/exif.h ${EASYEXIF_DIR}/exif.cpp)
		target_include_directories(external-easyexif PUBLIC ${EASYEXIF_DIR})
	endif()
else()
	message(
		FATAL_ERROR
			"EeasyExif is required - MESHLAB_ALLOW_DOWNLOAD_SOURCE_EASYEXIF must be ON.")
endif()
