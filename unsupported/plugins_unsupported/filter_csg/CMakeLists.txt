# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


# Only build if we have gmp/gmpxx or mpir
if(TARGET external-gmp OR TARGET external-mpir)

	set(SOURCES filter_csg.cpp)

	set(HEADERS filter_csg.h gmpfrac.h intercept.h)

	add_meshlab_plugin(filter_csg ${SOURCES} ${HEADERS})

	if(TARGET external-gmp)
		target_compile_definitions(filter_csg PRIVATE BUILD_WITH_GMP)
		target_link_libraries(filter_csg PRIVATE external-gmp)
	else()
		target_link_libraries(filter_csg PRIVATE external-mpir)
	endif()

else()
	message(
		STATUS "Skipping filter_csg - don't know about gmp or mpir on this system.")
endif()
