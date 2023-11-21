# Copyright 2023, PTC

if (TARGET external-boost AND TARGET external-cgal AND TARGET external-libigl)
	set(SOURCES filter_mesh_alpha_wrap.cpp)

	set(HEADERS filter_mesh_alpha_wrap.h)

	add_meshlab_plugin(filter_mesh_alpha_wrap ${SOURCES} ${HEADERS})

	target_link_libraries(filter_mesh_alpha_wrap PRIVATE external-boost external-cgal external-libigl)
else()
	message(
		STATUS "Skipping filter_mesh_alpha_wrap - don't know about boost, cgal or libigl on this system.")
endif()
