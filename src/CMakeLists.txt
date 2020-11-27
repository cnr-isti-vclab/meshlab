# Copyright 2019 Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

cmake_minimum_required(VERSION 3.9)
project(MeshLab)

# Prefer GLVND
if(POLICY CMP0072)
	cmake_policy(SET CMP0072 NEW)
endif()

### Build options
option(BUILD_MINI "Build only a minimal set of plugins" OFF)
option(BUILD_STRICT "Strictly enforce resolution of all symbols" ON)
option(BUILD_WITH_DOUBLE_SCALAR "Use double type instead of float type for scalars" OFF)

### Dependencies
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
message(STATUS "Searching for required components")
find_package(OpenGL REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
find_package(
	Qt5
	COMPONENTS OpenGL Xml Network
	REQUIRED)
find_package(OpenMP)

message(STATUS "Searching for required components with bundled fallback")
find_package(GLEW)
find_package(Eigen3)

message(STATUS "Searching for optional components")
find_package(Lib3ds)
find_package(GMP)
find_package(muparser)
find_package(OpenCTM)
find_package(Qhull COMPONENTS libqhull)

### Build directories
set(MESHLAB_BUILD_DISTRIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/distrib)
set(MESHLAB_LIB_OUTPUT_DIR ${MESHLAB_BUILD_DISTRIB_DIR})
set(MESHLAB_PLUGIN_OUTPUT_DIR ${MESHLAB_BUILD_DISTRIB_DIR}/plugins)
set(MESHLAB_SHADER_OUTPUT_DIR ${MESHLAB_BUILD_DISTRIB_DIR}/shaders)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${MESHLAB_BUILD_DISTRIB_DIR})

### Build settings
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.11" CACHE STRING "Minimum OS X deployment version" FORCE)

### Settings needed for both "external" and internal code
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

### Build Settings
if (BUILD_WITH_DOUBLE_SCALAR)
	message(STATUS "Building with double precision")
	add_definitions(-DMESHLAB_SCALAR=double)
else()
	message(STATUS "Building with single precision")
	add_definitions(-DMESHLAB_SCALAR=float)
endif()

if(WIN32)
	add_definitions(-DNOMINMAX)
	if(MSVC)
		add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
	endif()
endif()

if(BUILD_STRICT AND NOT MSVC AND NOT APPLE)
	# Make sure that our shared libraries were appropriately linked
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")
	set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,--no-undefined")
endif()

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

#external
set(EXTERNAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external)
include(${CMAKE_CURRENT_SOURCE_DIR}/external_common.cmake)
if (NOT BUILD_MINI)
	include(${EXTERNAL_DIR}/external.cmake)
endif()

add_subdirectory(common)
add_subdirectory(meshlab)
#add_subdirectory(meshlabserver)
set(CMAKE_INSTALL_RPATH)
if (WIN32)
	add_subdirectory(use_cpu_opengl)
endif()

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

		#IORaster plugins
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
		meshlabplugins/filter_create
		meshlabplugins/filter_csg
		meshlabplugins/filter_dirt
		meshlabplugins/filter_fractal
		meshlabplugins/filter_func
		meshlabplugins/filter_globalregistration
		meshlabplugins/filter_img_patch_param
		meshlabplugins/filter_isoparametrization
		meshlabplugins/filter_layer
		meshlabplugins/filter_measure
		meshlabplugins/filter_meshing
		meshlabplugins/filter_mls
		meshlabplugins/filter_mutualglobal
		meshlabplugins/filter_mutualinfo
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

# shaders
file(
	GLOB SHADERS
	LIST_DIRECTORIES false
	"${CMAKE_CURRENT_SOURCE_DIR}/../distrib/shaders/*.vert"
	"${CMAKE_CURRENT_SOURCE_DIR}/../distrib/shaders/*.frag"
	"${CMAKE_CURRENT_SOURCE_DIR}/../distrib/shaders/*.gdp")
foreach(FN ${SHADERS})
	get_filename_component(NAME_ONLY ${FN} NAME)
	set(OUTFN ${MESHLAB_SHADER_OUTPUT_DIR}/${NAME_ONLY})
	add_custom_command(
		OUTPUT ${OUTFN}
		COMMAND ${CMAKE_COMMAND} -E make_directory "${MESHLAB_SHADER_OUTPUT_DIR}"
		COMMAND ${CMAKE_COMMAND} -E copy_if_different "${FN}" "${OUTFN}"
		COMMENT "Copying ${NAME_ONLY} to shader build directory"
		VERBATIM
	)
	install(
		FILES ${FN}
		DESTINATION ${MESHLAB_SHADER_INSTALL_DIR}
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
