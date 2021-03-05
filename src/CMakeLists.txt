# Copyright 2019, 2020, Collabora, Ltd.
# Copyright 2019, 2020, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

cmake_minimum_required(VERSION 3.9)
project(MeshLab)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/meshlab_global_settings.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/meshlab_tools.cmake")

### Build options
option(BUILD_MINI "Build only a minimal set of plugins" OFF)
option(BUILD_SERVER "Build a command-line server application" OFF)
option(BUILD_STRICT "Strictly enforce resolution of all symbols" ON)
option(BUILD_WITH_DOUBLE_SCALAR "Use double type instead of float type for scalars" OFF)

### Dependencies
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
message(STATUS "Searching for required components")
find_package(OpenGL REQUIRED)

find_package(
	Qt5
	COMPONENTS OpenGL Xml Network
	REQUIRED)
find_package(OpenMP)

message(STATUS "Searching for required components with bundled fallback")
find_package(GLEW)
find_package(Eigen3)

### Build directories
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
	set(CMAKE_INSTALL_RPATH $ORIGIN/../${MESHLAB_LIB_INSTALL_DIR};$ORIGIN/../${CMAKE_INSTALL_LIBDIR})
else()
	SET(CMAKE_INSTALL_RPATH $ORIGIN/../Frameworks)
endif()

### Enter subdirectories

# VCGLib -- required
if(NOT VCGDIR)
	get_filename_component(VCGDIR "${CMAKE_CURRENT_LIST_DIR}/vcglib" ABSOLUTE)
	if(NOT EXISTS ${VCGDIR})
		set(VCGDIR NOTFOUND)
	endif()
endif()
set(VCGDIR "${VCGDIR}")

if(NOT VCGDIR)
	message(FATAL_ERROR "VCGLib not found. Please clone recursively the MeshLab repo.")
endif()


# External
set(EXTERNAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external)
include(${EXTERNAL_DIR}/external_common.cmake)
add_subdirectory(${EXTERNAL_DIR})

add_subdirectory(common)
add_subdirectory(meshlab)
if(BUILD_SERVER)
	add_subdirectory(meshlabserver)
endif()
if(WIN32 AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/use_cpu_opengl")
	add_subdirectory(use_cpu_opengl)
endif()

# Clear RPATH before adding plugins
set(CMAKE_INSTALL_RPATH)

### Plugin subdirectories
if(BUILD_MINI)
	set(MESHLAB_PLUGINS
		meshlabplugins/io_base
		meshlabplugins/filter_meshing
		meshlabplugins/decorate_base
		meshlabplugins/filter_measure
	)
else()
	set(MESHLAB_PLUGINS
		# IOMesh plugins
		meshlabplugins/io_3ds
		meshlabplugins/io_base
		meshlabplugins/io_bre
		meshlabplugins/io_collada
		meshlabplugins/io_ctm
		meshlabplugins/io_expe
		meshlabplugins/io_json
		meshlabplugins/io_pdb
		meshlabplugins/io_tri
		meshlabplugins/io_txt
		meshlabplugins/io_u3d
		meshlabplugins/io_x3d

		# IORaster plugins
		meshlabplugins/ioraster_base

		# Filter plugins
		meshlabplugins/filter_sample
		meshlabplugins/filter_sample_dyn
		meshlabplugins/filter_createiso
		meshlabplugins/filter_geodesic
		meshlabplugins/filter_sample_gpu
		meshlabplugins/filter_ao
		meshlabplugins/filter_camera
		meshlabplugins/filter_clean
		meshlabplugins/filter_color_projection
		meshlabplugins/filter_colorproc
#		meshlabplugins/filter_create
#		meshlabplugins/filter_csg
#		meshlabplugins/filter_dirt
#		meshlabplugins/filter_fractal
#		meshlabplugins/filter_func
#		meshlabplugins/filter_img_patch_param
#		meshlabplugins/filter_isoparametrization
#		meshlabplugins/filter_layer
#		meshlabplugins/filter_measure
#		meshlabplugins/filter_meshing
#		meshlabplugins/filter_mls
#		meshlabplugins/filter_mutualglobal
#		meshlabplugins/filter_mutualinfo
#		meshlabplugins/filter_plymc
#		meshlabplugins/filter_qhull
#		meshlabplugins/filter_quality
#		meshlabplugins/filter_sampling
#		meshlabplugins/filter_screened_poisson
#		meshlabplugins/filter_sdfgpu
#		meshlabplugins/filter_select
#		meshlabplugins/filter_sketchfab
#		meshlabplugins/filter_ssynth
#		meshlabplugins/filter_texture
#		meshlabplugins/filter_trioptimize
#		meshlabplugins/filter_unsharp
#		meshlabplugins/filter_voronoi

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

# shaders
set(SHADER_BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../distrib/shaders")
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

if(NOT WIN32 AND NOT APPLE)
	install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/../scripts/Linux/resources/meshlab.desktop" DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications)
	install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/../scripts/meshlab.png" DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pixmaps)
endif()
