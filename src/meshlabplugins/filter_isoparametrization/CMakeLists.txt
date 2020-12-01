# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/filter_isoparametrization.cmake (custom for this directory),
### then re-run ./make-cmake.py

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

	add_library(filter_isoparametrization MODULE ${SOURCES} ${HEADERS})

	target_include_directories(filter_isoparametrization PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
	target_link_libraries(filter_isoparametrization PUBLIC common)

	target_link_libraries(filter_isoparametrization PRIVATE external-newuoa external-levmar)

	if(OpenMP_CXX_FOUND)
		target_link_libraries(filter_isoparametrization PRIVATE OpenMP::OpenMP_CXX)
		target_compile_definitions(filter_isoparametrization PRIVATE _USE_OMP)
	endif()

	set_property(TARGET filter_isoparametrization PROPERTY FOLDER Plugins)

	set_property(TARGET filter_isoparametrization PROPERTY RUNTIME_OUTPUT_DIRECTORY ${MESHLAB_PLUGIN_OUTPUT_DIR})

	set_property(TARGET filter_isoparametrization PROPERTY LIBRARY_OUTPUT_DIRECTORY ${MESHLAB_PLUGIN_OUTPUT_DIR})

	install(
		TARGETS filter_isoparametrization
		DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
		COMPONENT Plugins)

else()
	message(STATUS "Skipping filter_isoparametrization - missing either newuoa or levmar in external directory.")
endif()
