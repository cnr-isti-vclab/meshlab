# Copyright 2019, 2020, Collabora, Ltd.
# Copyright 2019, 2020, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

option(ALLOW_BUNDLED_EXIF "Allow use of bundled EasyExif source" ON)

set(EXIF_DIR ${CMAKE_CURRENT_LIST_DIR}/easyexif)

if(ALLOW_BUNDLED_EXIF AND EXISTS "${EXIF_DIR}/exif.h")
	message(STATUS "- exif - using bundled source")
	add_library(external-exif STATIC ${EXIF_DIR}/exif.h ${EXIF_DIR}/exif.cpp)
	target_include_directories(external-exif PUBLIC ${EXIF_DIR})
else()
	message(
		FATAL_ERROR
			"Exif is required - ALLOW_BUNDLED_EXIF must be enabled and found.")
endif()
