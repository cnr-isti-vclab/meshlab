# Only build if we have tiny glTF
if(TARGET external-tinygltf)

	set(SOURCES
		io_gltf.cpp
		gltf_loader.cpp)

	set(HEADERS
		io_gltf.h
		tinygltf_include.h
		gltf_loader.h)

	add_meshlab_plugin(io_gltf MODULE ${SOURCES} ${HEADERS})

	target_link_libraries(io_gltf PUBLIC external-tinygltf)

else()
	message(STATUS "Skipping io_gltf - missing tiny glTF in external directory.")
endif()
