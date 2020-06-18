# Copyright 2019 Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

cmake_minimum_required(VERSION 3.9)
project(MeshLab)

# Prefer GLVND
if(POLICY CMP0072)
    cmake_policy(SET CMP0072 NEW)
endif()

### vcglib
if(NOT VCGDIR)
    get_filename_component(VCGDIR "${CMAKE_CURRENT_SOURCE_DIR}/../vcglib" ABSOLUTE)
    if(NOT EXISTS ${VCGDIR})
        set(VCGDIR NOTFOUND)
    endif()
endif()
set(VCGDIR
    "${VCGDIR}"
    CACHE PATH "The location of the vcglib source tree - defaults to sibling of meshlab directory.")

if(NOT VCGDIR)
    message(
        FATAL_ERROR
            "vcglib is required to build MeshLab. Please get the source and set VCGDIR to its location: default location is as a sibling to the meshlab directory above this directory."
    )
endif()

### Build options

option(BUILD_MINI "Build only a minimal set of plugins" OFF)
option(BUILD_STRICT "Strictly enforce resolution of all symbols" ON)
set(BUILD_ADDITIONAL_PLUGINS
    ${BUILD_ADDITIONAL_PLUGINS}
    CACHE
        STRING
        "A list of other plugin subdirectories to recurse into (e.g. select experimental or unsupported plugins) - relative to meshlab/src"
)

option(ALLOW_BUNDLED_EIGEN "Allow use of bundled Eigen source" ON)
option(ALLOW_BUNDLED_GLEW "Allow use of bundled GLEW source" ON)
option(ALLOW_BUNDLED_NEWUOA "Allow use of bundled newuoa source" ON)
option(ALLOW_BUNDLED_LEVMAR "Allow use of bundled levmar source" ON)
option(ALLOW_BUNDLED_LIB3DS "Allow use of bundled lib3ds source" ON)
# option(ALLOW_BUNDLED_MPIR "Allow use of bundled MPIR binaries" ON) # TODO
option(ALLOW_BUNDLED_MUPARSER "Allow use of bundled muparser source" ON)
option(ALLOW_BUNDLED_OPENCTM "Allow use of bundled OpenCTM source" ON)
option(ALLOW_BUNDLED_SSYNTH "Allow use of bundled structure-synth source" ON)
option(ALLOW_BUNDLED_QHULL "Allow use of bundled Qhull source" ON)
option(ALLOW_BUNDLED_U3D "Allow use of bundled u3d source" ON)

option(ALLOW_SYSTEM_EIGEN "Allow use of system-provided Eigen" ON)
option(ALLOW_SYSTEM_GLEW "Allow use of system-provided GLEW" ON)
option(ALLOW_SYSTEM_LIB3DS "Allow use of system-provided lib3ds" ON)
option(ALLOW_SYSTEM_GMP "Allow use of system-provided GMP" ON)
option(ALLOW_SYSTEM_MUPARSER "Allow use of system-provided muparser" ON)
option(ALLOW_SYSTEM_OPENCTM "Allow use of system-provided OpenCTM" ON)
option(ALLOW_SYSTEM_QHULL "Allow use of system-provided QHull" ON)

### Dependencies
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
message(STATUS "Searching for required components")
find_package(OpenGL REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
find_package(
    Qt5
    COMPONENTS OpenGL Xml XmlPatterns
    REQUIRED)

message(STATUS "Searching for required components with bundled fallback")
find_package(GLEW)
find_package(Eigen3)

message(STATUS "Searching for optional components")
find_package(Lib3ds)
find_package(GMP)
find_package(muparser)
find_package(OpenCTM)
# TODO dynamic qhull that isn't qhull_r is deprecated: port to qhull_r
find_package(Qhull COMPONENTS qhull)
find_package(OpenMP)

if(WIN32)
    option(INSTALL_TO_UNIX_LAYOUT
           "Should the files be installed to a unix layout? If false, they will be installed more like the build tree."
           OFF)

    get_target_property(Qt5_qmake_EXECUTABLE Qt5::qmake IMPORTED_LOCATION)
    get_filename_component(Qt5_BIN_DIR "${Qt5_qmake_EXECUTABLE}" DIRECTORY)
    find_program(
        Qt5_windeployqt_EXECUTABLE
        NAMES windeployqt
        HINTS ${Qt5_BIN_DIR})
    if(Qt5_windeployqt_EXECUTABLE)
        option(BUILD_WITH_WINDEPLOYQT_POST_BUILD
               "Should we run windeployqt after the build to copy the qt parts to the build tree?" ON)
    endif()
else()
    set(INSTALL_TO_UNIX_LAYOUT ON)
endif()

option(INSTALL_SAMPLE_MESHES
       "Should the sample meshes in src/distrib/samples be installed? It will increase the installed size by 14 MiB."
       OFF)
option(
    INSTALL_SAMPLE_RANGEMAPS
    "Should the sample rangemaps in src/distrib/samples/rangemaps be installed? It will increase the installed size by 67 MiB."
    OFF)

### Settings needed for both "external" and internal code
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

### Bundled dependencies in the "external" directory
set(EXTERNAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external)
include("${CMAKE_CURRENT_SOURCE_DIR}/external.cmake")

### Install directories and build/staging directories
include(GNUInstallDirs)
if(INSTALL_TO_UNIX_LAYOUT)
    set(MESHLAB_BIN_INSTALL_DIR ${CMAKE_INSTALL_BINDIR})
    set(MESHLAB_LIB_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/meshlab)
    set(MESHLAB_PLUGIN_INSTALL_DIR ${MESHLAB_LIB_INSTALL_DIR}/plugins)
    set(MESHLAB_SHADER_INSTALL_DIR ${CMAKE_INSTALL_DATAROOTDIR}/meshlab/shaders)
    set(MESHLAB_SAMPLE_INSTALL_DIR ${CMAKE_INSTALL_DATAROOTDIR}/meshlab)
else()
    set(MESHLAB_BIN_INSTALL_DIR .)
    set(MESHLAB_LIB_INSTALL_DIR .)
    set(MESHLAB_PLUGIN_INSTALL_DIR plugins)
    set(MESHLAB_SHADER_INSTALL_DIR shaders)
    set(MESHLAB_SAMPLE_INSTALL_DIR .)
endif()

set(MESHLAB_BUILD_DISTRIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/distrib)
set(MESHLAB_PLUGIN_OUTPUT_DIR ${MESHLAB_BUILD_DISTRIB_DIR}/plugins)
set(MESHLAB_SHADER_OUTPUT_DIR ${MESHLAB_BUILD_DISTRIB_DIR}/shaders)
set(MESHLAB_SAMPLE_OUTPUT_DIR ${MESHLAB_BUILD_DISTRIB_DIR}/sample)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${MESHLAB_BUILD_DISTRIB_DIR})

### Common build settings for internal code
include_directories(${VCGDIR} ${CMAKE_CURRENT_SOURCE_DIR})

# This gets set to system or local, as appropriate, by external.cmake
include_directories(${EIGEN_INCLUDE_DIRS})

add_definitions(-DMESHLAB_SCALAR=float)

if(BUILD_STRICT AND NOT MSVC)
    # Make sure that our shared libraries were appropriately linked
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,--no-undefined")
endif()

if(WIN32)
    add_definitions(-DNOMINMAX)
    if(MSVC)
        add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
    endif()
endif()


### Enter subdirectories
add_subdirectory(common)

set(CMAKE_INSTALL_RPATH $ORIGIN/../${MESHLAB_LIB_INSTALL_DIR};$ORIGIN/../${CMAKE_INSTALL_LIBDIR})
add_subdirectory(meshlab)
add_subdirectory(meshlabserver)
if (WIN32)
    add_subdirectory(use_cpu_opengl)
endif()
set(CMAKE_INSTALL_RPATH)

### Plugin subdirectories
if(BUILD_MINI)
    # mini
    set(POSSIBLE_PLUGINS
        meshlabplugins/io_base
        meshlabplugins/filter_meshing
        meshlabplugins/decorate_base
        meshlabplugins/filter_measure)
else()
    # full

    set(POSSIBLE_PLUGINS
        # IO plugins
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


        # Filter plugins
        # meshlabplugins/filter_aging # not in qmake file?
        # meshlabplugins/filter_bnpts # not in qmake file?
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
        meshlabplugins/edit_align
        # meshlabplugins/edit_hole # not in qmake file?
        meshlabplugins/edit_manipulators
        meshlabplugins/edit_measure
        meshlabplugins/edit_mutualcorrs
        meshlabplugins/edit_paint
        meshlabplugins/edit_pickpoints
        meshlabplugins/edit_point
        meshlabplugins/edit_referencing
        meshlabplugins/edit_quality
        meshlabplugins/edit_select
        # meshlabplugins/edit_slice # not in qmake file?
        # meshlabplugins/edit_texture # not in qmake file?

        # Sample Plugins
        meshlabplugins/edit_sample
        meshlabplugins/filter_sample
        meshlabplugins/filter_sample_dyn
        meshlabplugins/filter_createiso
        meshlabplugins/filter_geodesic
        meshlabplugins/filter_sample_gpu)
endif()

message(STATUS "\nConfiguring plugins")
foreach(PLUGIN ${POSSIBLE_PLUGINS})
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${PLUGIN}/CMakeLists.txt)
        message(STATUS "- ${PLUGIN}")
        add_subdirectory(${PLUGIN})
    else()
        message(STATUS "  - ${PLUGIN} - Skipping, plugin or build system not found.")
    endif()
endforeach()
if(BUILD_ADDITIONAL_PLUGINS)
    message(STATUS "\nConfiguring plugins specified in BUILD_ADDITIONAL_PLUGINS")
endif()
foreach(PLUGIN ${BUILD_ADDITIONAL_PLUGINS})
    get_filename_component(PLUGIN_DIR ${PLUGIN} ABSOLUTE BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
    if(EXISTS ${PLUGIN_DIR}/CMakeLists.txt)
        message(STATUS "- ${PLUGIN}")
        add_subdirectory(${PLUGIN_DIR})
    else()
        message(FATAL_ERROR "${PLUGIN} - Additional plugin specified in BUILD_ADDITIONAL_PLUGINS but not found.")
    endif()
endforeach()

### Copy/install other files

# This variable keeps track of the output filenames that need to be copied at build time
set(COPIED_FILES)

# shaders
# TODO subdirs?
file(
    GLOB SHADERS
    LIST_DIRECTORIES false
    "${CMAKE_CURRENT_SOURCE_DIR}/../distrib/shaders/*.vert" "${CMAKE_CURRENT_SOURCE_DIR}/../distrib/shaders/*.frag"
    "${CMAKE_CURRENT_SOURCE_DIR}/../distrib/shaders/*.gdp")
foreach(FN ${SHADERS})
    get_filename_component(NAME_ONLY ${FN} NAME)
    set(OUTFN ${MESHLAB_SHADER_OUTPUT_DIR}/${NAME_ONLY})
    add_custom_command(
        OUTPUT ${OUTFN}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${MESHLAB_SHADER_OUTPUT_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${FN}" "${OUTFN}"
        COMMENT "Copying ${NAME_ONLY} to shader build directory"
        VERBATIM)
    install(
        FILES ${FN}
        DESTINATION ${MESHLAB_SHADER_INSTALL_DIR}
        COMPONENT Shaders)
    list(APPEND COPIED_FILES "${OUTFN}")
endforeach()

# Custom target - to trigger the execution of the custom commands above.
add_custom_target(copy-distrib-files ALL DEPENDS ${COPIED_FILES})
set_property(TARGET copy-distrib-files PROPERTY FOLDER Core)

if(INSTALL_SAMPLE_MESHES)
    # Install sample files from distrib/sample directory (except for the rangemaps)
    install(
        DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../sample
        DESTINATION ${MESHLAB_SAMPLE_INSTALL_DIR}
        COMPONENT Samples
        PATTERN "*/rangemaps" EXCLUDE)
endif()
if(INSTALL_SAMPLE_RANGEMAPS)
    # Install sample rangemaps
    install(
        DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../sample/rangemaps
        DESTINATION ${MESHLAB_SAMPLE_INSTALL_DIR}/sample
        COMPONENT Samples)
endif()

if(NOT WIN32 AND NOT APPLE)
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/../install/linux/resources/meshlab.desktop" DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications)
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/../install/meshlab.png" DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/pixmaps)
endif()

if(Qt5_windeployqt_EXECUTABLE AND BUILD_WITH_WINDEPLOYQT_POST_BUILD)
    install(CODE "execute_process(COMMAND \"${Qt5_windeployqt_EXECUTABLE}\" --no-translations meshlab.exe WORKING_DIRECTORY \${CMAKE_INSTALL_PREFIX})")
endif()
