# Copyright 2019, 2020, Collabora, Ltd.
# Copyright 2019, 2021, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

### Dependencies
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/meshlab_global_settings.cmake" NO_POLICY_SCOPE)
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/meshlab_tools.cmake")

#message(STATUS "Searching for required components")
find_package(OpenGL REQUIRED)

find_package(
	Qt5
	COMPONENTS OpenGL Xml Network
	REQUIRED)
if (Qt5_VERSION VERSION_LESS 5.15.0)
	message(FATAL_ERROR "Minimum supported Qt5 version is 5.15!")
endif()
find_package(OpenMP)
find_package(Threads)

message(STATUS "Searching for required components with bundled fallback")
find_package(GLEW QUIET)
find_package(Eigen3 QUIET)

set(MESHLAB_RESOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../resources)

### Build directories
if (MESHLAB_USE_DEFAULT_BUILD_AND_INSTALL_DIRS) # otherwise, we assume that all these dirs are defined in parent dir
	set(MESHLAB_BUILD_DISTRIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/distrib)
	set(MESHLAB_LIB_OUTPUT_DIR ${MESHLAB_BUILD_DISTRIB_DIR})
	set(MESHLAB_PLUGIN_OUTPUT_DIR ${MESHLAB_BUILD_DISTRIB_DIR}/plugins)
	set(MESHLAB_SHADER_OUTPUT_DIR ${MESHLAB_BUILD_DISTRIB_DIR}/shaders)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${MESHLAB_BUILD_DISTRIB_DIR})

	### Install directories
	if(WIN32 OR APPLE)
		set(INSTALL_TO_UNIX_LAYOUT OFF)
	else()
		set(INSTALL_TO_UNIX_LAYOUT ON)
	endif()
	include(GNUInstallDirs)
	if(INSTALL_TO_UNIX_LAYOUT)
		set(MESHLAB_BIN_INSTALL_DIR ${CMAKE_INSTALL_BINDIR})
		set(MESHLAB_LIB_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/meshlab)
		set(MESHLAB_PLUGIN_INSTALL_DIR ${MESHLAB_LIB_INSTALL_DIR}/plugins)
		set(MESHLAB_SHADER_INSTALL_DIR ${CMAKE_INSTALL_DATAROOTDIR}/meshlab/shaders)
	elseif(APPLE)
		set(MESHLAB_BIN_INSTALL_DIR .)
		set(MESHLAB_LIB_INSTALL_DIR meshlab.app/Contents/Frameworks)
		set(MESHLAB_PLUGIN_INSTALL_DIR meshlab.app/Contents/PlugIns)
		set(MESHLAB_SHADER_INSTALL_DIR meshlab.app/Contents/shaders)
	else()
		set(MESHLAB_BIN_INSTALL_DIR .)
		set(MESHLAB_LIB_INSTALL_DIR .)
		set(MESHLAB_PLUGIN_INSTALL_DIR plugins)
		set(MESHLAB_SHADER_INSTALL_DIR shaders)
	endif()

	### Install Settings
	if (NOT APPLE)
		set(CMAKE_INSTALL_RPATH $ORIGIN/../${MESHLAB_LIB_INSTALL_DIR})
	else()
		SET(CMAKE_INSTALL_RPATH @executable_path/../Frameworks)
	endif()

	get_directory_property(hasParent PARENT_DIRECTORY)
	if(hasParent)
		set(MESHLAB_PLUGIN_OUTPUT_DIR ${MESHLAB_PLUGIN_OUTPUT_DIR} PARENT_SCOPE)
		set(MESHLAB_PLUGIN_INSTALL_DIR ${MESHLAB_PLUGIN_INSTALL_DIR} PARENT_SCOPE)
	endif()
endif()

### Enter subdirectories

# VCGLib -- required
if (VCGDIR) # VCGDIR exists - using custom user vcglib path
	if(EXISTS ${VCGDIR})
		add_subdirectory(${VCGDIR} {CMAKE_CURRENT_BINARY_DIR}/vcglib)
		message(STATUS "- VCGLib - using custom VCGDIR path library")
	else()
		set(VCGDIR NOTFOUND)
	endif()
else()
	get_filename_component(VCGDIR "${CMAKE_CURRENT_LIST_DIR}/vcglib" ABSOLUTE)
	if(EXISTS ${VCGDIR})
		add_subdirectory(${VCGDIR})
		message(STATUS "- VCGLib - using bundled source")
	else()
		set(VCGDIR NOTFOUND)
	endif()
endif()
set(VCGDIR "${VCGDIR}")

if(NOT VCGDIR)
	message(FATAL_ERROR "VCGLib not found. Please clone recursively the MeshLab repo.")
else ()
	message(STATUS "- VCGLib directory: " ${VCGDIR})
endif()

# External
set(EXTERNAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external)
add_subdirectory(${EXTERNAL_DIR})

add_subdirectory(common)
add_subdirectory(common_gui)

if (NOT MESHLAB_BUILD_ONLY_LIBRARIES)
	add_subdirectory(meshlab)
	if(WIN32 AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/use_cpu_opengl")
		add_subdirectory(use_cpu_opengl)
	endif()

	# Clear RPATH before adding plugins
	set(CMAKE_INSTALL_RPATH)
endif()



### Plugin subdirectories
if(NOT DEFINED MESHLAB_PLUGINS) # it may be already defined in parent directory
	set(MESHLAB_PLUGINS
		# IO plugins
		meshlabplugins/io_3ds
		meshlabplugins/io_base
		meshlabplugins/io_bre
		meshlabplugins/io_collada
		meshlabplugins/io_ctm
		meshlabplugins/io_e57
		meshlabplugins/io_expe
		meshlabplugins/io_gltf
		meshlabplugins/io_json
		meshlabplugins/io_pdb
		meshlabplugins/io_tri
		meshlabplugins/io_txt
		meshlabplugins/io_u3d
		meshlabplugins/io_x3d

		# Filter plugins
		meshlabplugins/filter_sample
		meshlabplugins/filter_createiso
		meshlabplugins/filter_geodesic
		meshlabplugins/filter_sample_gpu
		meshlabplugins/filter_ao
		meshlabplugins/filter_camera
		meshlabplugins/filter_clean
		meshlabplugins/filter_color_projection
		meshlabplugins/filter_colorproc
		meshlabplugins/filter_create
		meshlabplugins/filter_cubization
		meshlabplugins/filter_developability
		meshlabplugins/filter_dirt
		meshlabplugins/filter_embree
		meshlabplugins/filter_fractal
		meshlabplugins/filter_func
		meshlabplugins/filter_img_patch_param
		meshlabplugins/filter_icp
		meshlabplugins/filter_io_nxs
		meshlabplugins/filter_isoparametrization
		meshlabplugins/filter_layer
		meshlabplugins/filter_measure
		meshlabplugins/filter_mesh_booleans
		meshlabplugins/filter_mesh_alpha_wrap
		meshlabplugins/filter_meshing
		meshlabplugins/filter_mls
		meshlabplugins/filter_mutualglobal
		meshlabplugins/filter_mutualinfo
		meshlabplugins/filter_parametrization
		meshlabplugins/filter_plymc
		meshlabplugins/filter_qhull
		meshlabplugins/filter_quality
		meshlabplugins/filter_sampling
		meshlabplugins/filter_screened_poisson
		meshlabplugins/filter_sdfgpu
		meshlabplugins/filter_select
		meshlabplugins/filter_sketchfab
		meshlabplugins/filter_ssynth
		meshlabplugins/filter_texture
		meshlabplugins/filter_texture_defragmentation
		meshlabplugins/filter_trioptimize
		meshlabplugins/filter_unsharp
		meshlabplugins/filter_voronoi

		# Rendering and Decoration Plugins
		meshlabplugins/render_gdp
		meshlabplugins/render_radiance_scaling
		meshlabplugins/decorate_base
		meshlabplugins/decorate_background
		meshlabplugins/decorate_raster_proj
		meshlabplugins/decorate_shadow

		# Edit Plugins
		meshlabplugins/edit_sample
		meshlabplugins/edit_align
		meshlabplugins/edit_manipulators
		meshlabplugins/edit_measure
		meshlabplugins/edit_mutualcorrs
		meshlabplugins/edit_paint
		meshlabplugins/edit_pickpoints
		meshlabplugins/edit_point
		meshlabplugins/edit_referencing
		meshlabplugins/edit_quality
		meshlabplugins/edit_select
	)
endif()


message(STATUS "\nConfiguring plugins")
foreach(PLUGIN ${MESHLAB_PLUGINS})
	if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${PLUGIN}/CMakeLists.txt)
		message(STATUS "- ${PLUGIN}")
		add_subdirectory(${PLUGIN})
	else()
		message(STATUS "  - ${PLUGIN} - Skipping, plugin or build system not found.")
	endif()
endforeach()


### Copy/install other files

# This variable keeps track of the output filenames that need to be copied at build time
set(COPIED_FILES)

if (NOT MESHLAB_BUILD_ONLY_LIBRARIES)
	# shaders
	set(SHADER_BASE_DIR "${MESHLAB_RESOURCES_DIR}/shaders")
	file(
		GLOB_RECURSE SHADERS
		LIST_DIRECTORIES false
		"${SHADER_BASE_DIR}/*.vert"
		"${SHADER_BASE_DIR}/*.frag"
		"${SHADER_BASE_DIR}/*.gdp")
	foreach(FN ${SHADERS})
		get_filename_component(SRC_PATH ${FN} DIRECTORY)
		get_filename_component(NAME_ONLY ${FN} NAME)
		file(RELATIVE_PATH REL_DIR "${SHADER_BASE_DIR}" "${SRC_PATH}")
		set(OUTFN "${MESHLAB_SHADER_OUTPUT_DIR}/${REL_DIR}/${NAME_ONLY}")
		if(REL_DIR)
			set(REL_DIR_MESSAGE "${REL_DIR} in ")
		else()
			set(REL_DIR_MESSAGE "")
		endif()
		add_custom_command(
			OUTPUT ${OUTFN}
			COMMAND ${CMAKE_COMMAND} -E make_directory "${MESHLAB_SHADER_OUTPUT_DIR}/${REL_DIR}"
			COMMAND ${CMAKE_COMMAND} -E copy_if_different "${FN}" "${OUTFN}"
			COMMENT "Copying ${NAME_ONLY} to ${REL_DIR_MESSAGE}shader build directory"
			VERBATIM
		)
		install(
			FILES ${FN}
			DESTINATION ${MESHLAB_SHADER_INSTALL_DIR}/${REL_DIR}
			COMPONENT Shaders
		)
		list(APPEND COPIED_FILES "${OUTFN}")
	endforeach()

	# Custom target - to trigger the execution of the custom commands above.
	add_custom_target(copy-distrib-files ALL DEPENDS ${COPIED_FILES})

	set_property(TARGET copy-distrib-files PROPERTY FOLDER Core)

	# be sure to remove all plugins (also old one that are not target anymore) when clean
	set_property(
		TARGET copy-distrib-files
		APPEND
		PROPERTY ADDITIONAL_CLEAN_FILES ${MESHLAB_PLUGIN_OUTPUT_DIR}
	)

	if(NOT WIN32 AND NOT APPLE)
		install(FILES "${MESHLAB_RESOURCES_DIR}/linux/meshlab.desktop" DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications)
		install(FILES "${MESHLAB_RESOURCES_DIR}/icons/meshlab.png" DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/512x512/apps)
	endif()
	if(WIN32)
		install(FILES "${MESHLAB_RESOURCES_DIR}/icons/meshlab.png" DESTINATION ${MESHLAB_BIN_INSTALL_DIR}/)
		install(FILES "${MESHLAB_RESOURCES_DIR}/icons/meshlab.ico" DESTINATION ${MESHLAB_BIN_INSTALL_DIR}/)
	endif()
endif()
