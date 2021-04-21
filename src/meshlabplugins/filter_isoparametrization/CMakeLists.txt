# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


# Only build if we have newuoa and levmar
if(TARGET external-newuoa AND TARGET external-levmar)

	set(SOURCES filter_isoparametrization.cpp)

	set(HEADERS
		defines.h
		diam_parametrization.h
		diam_topology.h
		diamond_sampler.h
		dual_coord_optimization.h
		filter_isoparametrization.h
		iso_parametrization.h
		iso_transfer.h
		local_optimization.h
		local_parametrization.h
		mesh_operators.h
		opt_patch.h
		param_collapse.h
		param_flip.h
		param_mesh.h
		parametrizator.h
		stat_remeshing.h
		statistics.h
		tangent_space.h
		texcoord_optimization.h
		uv_grid.h)

	add_meshlab_plugin(filter_isoparametrization ${SOURCES} ${HEADERS})

	target_link_libraries(filter_isoparametrization PRIVATE external-newuoa external-levmar)

	if(OpenMP_CXX_FOUND)
		target_link_libraries(filter_isoparametrization PRIVATE OpenMP::OpenMP_CXX)
		target_compile_definitions(filter_isoparametrization PRIVATE _USE_OMP)
	endif()

else()
	message(STATUS "Skipping filter_isoparametrization - missing either newuoa or levmar in external directory.")
endif()
