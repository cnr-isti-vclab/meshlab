# Copyright 2019, 2020, Collabora, Ltd.
# Copyright 2019, 2021, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

option(MESHLAB_ALLOW_DOWNLOAD_SOURCE_TINYGLTF "Allow download and use of tinyglTF source" ON)

if(MESHLAB_ALLOW_DOWNLOAD_SOURCE_TINYGLTF)
	set(TINYGLTF_DIR "${MESHLAB_EXTERNAL_DOWNLOAD_DIR}/tinygltf-2.6.3")
	set(TINYGLTF_CHECK "${TINYGLTF_DIR}/tiny_gltf.h")

	if (NOT EXISTS ${TINYGLTF_CHECK})
		set(TINYGLTF_LINK
			https://github.com/syoyo/tinygltf/archive/refs/tags/v2.6.3.zip
			https://www.meshlab.net/data/libs/tinygltf-2.6.3.zip)
		set(TINYGLTF_MD5 f63ab0fb59e5de059d9719cc14057dc8)
		download_and_unzip(
			NAME "tinygltf"
			LINK ${TINYGLTF_LINK}
			MD5 ${TINYGLTF_MD5}
			DIR ${MESHLAB_EXTERNAL_DOWNLOAD_DIR})
		if (NOT download_and_unzip_SUCCESS)
			message(STATUS "- tinygltf - download failed.")
		endif()
	endif()

	if(EXISTS ${TINYGLTF_CHECK})
		message(STATUS "- tiny glTF - using downloaded source")
		add_library(external-tinygltf INTERFACE)
		target_include_directories(external-tinygltf INTERFACE ${TINYGLTF_DIR})
	endif()
endif()
