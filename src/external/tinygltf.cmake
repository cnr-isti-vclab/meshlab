# Copyright 2019, 2020, Collabora, Ltd.
# Copyright 2019, 2021, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

option(ALLOW_BUNDLED_TINY_GLTF "Allow use of bundled tiny glTF source" ON)

set(TINYGLTF_DIR ${CMAKE_CURRENT_LIST_DIR}/tinygltf)

if(ALLOW_BUNDLED_TINY_GLTF AND EXISTS "${TINYGLTF_DIR}/tiny_gltf.h")
	message(STATUS "- tiny glTF - using bundled source")
	add_library(external-tinygltf INTERFACE)
	target_include_directories(external-tinygltf INTERFACE ${TINYGLTF_DIR})
endif()
