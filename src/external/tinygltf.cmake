# Copyright 2019, 2020, Collabora, Ltd.
# Copyright 2019, 2021, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

option(MESHLAB_ALLOW_DOWNLOAD_SOURCE_TINYGLTF "Allow download and use of tinyglTF source" ON)

if(MESHLAB_ALLOW_DOWNLOAD_SOURCE_TINYGLTF)
	set(TINYGLTF_DIR ${MESHLAB_EXTERNAL_DOWNLOAD_DIR}/tinygltf-2.6.3)

	if (NOT EXISTS "${TINYGLTF_DIR}/tiny_gltf.h")
		set(TINYGLTF_LINK https://github.com/syoyo/tinygltf/archive/refs/tags/v2.6.3.zip)
		download_and_unzip(${TINYGLTF_LINK} ${MESHLAB_EXTERNAL_DOWNLOAD_DIR} "tinygltf")
	endif()

	message(STATUS "- tiny glTF - using downloaded source")
	add_library(external-tinygltf INTERFACE)
	target_include_directories(external-tinygltf INTERFACE ${TINYGLTF_DIR})
endif()
