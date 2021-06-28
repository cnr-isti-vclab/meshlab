# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

# Only build if we have tiny glTF
if(TARGET external-tinygltf)

	set(SOURCES
		io_gltf.cpp)

	set(HEADERS
		io_gltf.h)

	add_meshlab_plugin(io_gltf MODULE ${SOURCES} ${HEADERS})

	target_link_libraries(io_gltf PUBLIC external-tinygltf)

else()
	message(STATUS "Skipping io_gltf - missing tiny glTF in external directory.")
endif()
