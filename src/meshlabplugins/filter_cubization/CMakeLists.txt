# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

if (TARGET external-libigl AND TARGET Threads::Threads)
	set(SOURCES
			filter_cubization.cpp
			CubicStylizationFiles/src/cube_style_precomputation.cpp
			CubicStylizationFiles/src/cube_style_single_iteration.cpp
			CubicStylizationFiles/src/fit_rotations_l1.cpp
			CubicStylizationFiles/src/normalize_unitbox.cpp
			CubicStylizationFiles/src/orthogonal_procrustes.cpp
			CubicStylizationFiles/src/shrinkage.cpp)

	set(HEADERS
			filter_cubization.h
			curvdata.h
			curvedgeflip.h
			CubicStylizationFiles/conversionMeshes.h
			CubicStylizationFiles/cubic_stylizing.h
			CubicStylizationFiles/src/cube_style_data.h
			CubicStylizationFiles/src/cube_style_precomputation.h
			CubicStylizationFiles/src/cube_style_single_iteration.h
			CubicStylizationFiles/src/fit_rotations_l1.h
			CubicStylizationFiles/src/normalize_unitbox.h
			CubicStylizationFiles/src/orthogonal_procrustes.h
			CubicStylizationFiles/src/shrinkage.h)

	add_meshlab_plugin(filter_cubization ${SOURCES} ${HEADERS})
	target_link_libraries(filter_cubization PRIVATE external-libigl Threads::Threads)
	
else()
	message(
		STATUS "Skipping filter_cubization - don't know about libigl or Threads::Threads on this system.")
endif()
