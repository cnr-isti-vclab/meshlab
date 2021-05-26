# Copyright 2019, 2020, Visual Computing Lab, ISTI - Italian National Research Council

if (TARGET external-boost AND TARGET external-cgal AND TARGET external-libigl)
	set(SOURCES filter_mesh_booleans.cpp)

	set(HEADERS filter_mesh_booleans.h)

	add_meshlab_plugin(filter_mesh_booleans ${SOURCES} ${HEADERS})

	target_link_libraries(filter_mesh_booleans PRIVATE external-boost external-cgal external-libigl)
else()
	message(
		STATUS "Skipping filter_mesh_booleans - don't know about boost, cgal or libigl on this system.")
endif()
