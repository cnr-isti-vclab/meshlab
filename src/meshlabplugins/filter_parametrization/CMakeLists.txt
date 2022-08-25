# Copyright 2019, 2020, Visual Computing Lab, ISTI - Italian National Research Council

if (TARGET external-libigl AND TARGET Threads::Threads)

	set(SOURCES filter_parametrization.cpp)

	set(HEADERS filter_parametrization.h)

	add_meshlab_plugin(filter_parametrization ${SOURCES} ${HEADERS})

	target_link_libraries(filter_parametrization PRIVATE external-libigl Threads::Threads)
else()
	message(
		STATUS "Skipping filter_parametrization - don't know about libigl or Threads::Threads on this system.")
endif()
