# Copyright 2019-2020, Collabora, Ltd.
# Copyright 2019, 2020, Visual Computing Lab, ISTI - Italian National Research Council

# Only build if we have nexus
if (TARGET nexus)

	set(SOURCES filter_nxs_builder.cpp)

	set(HEADERS filter_nxs_builder.h)

	add_meshlab_plugin(filter_nxs_builder ${SOURCES} ${HEADERS})

	target_link_libraries(filter_nxs_builder PRIVATE nexus)

else()
	message(STATUS "Skipping io_nxs - missing nexus")
endif()
