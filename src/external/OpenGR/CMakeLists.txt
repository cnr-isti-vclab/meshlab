cmake_minimum_required (VERSION 3.3)
project (OpenGR LANGUAGES CXX VERSION 1.1.3)

################################################################################
## User options
################################################################################
# Set option to enable timing recording
OPTION (OpenGR_COMPILE_TESTS "Enable testing" TRUE)
OPTION (OpenGR_COMPILE_DEMOS "Compile demo applications (including the Super4PCS standalone)" TRUE)
OPTION (IO_USE_BOOST "Use boost::filesystem for texture loading" TRUE)
OPTION (ENABLE_TIMING "Enable computation time recording" FALSE)
OPTION (OpenGR_USE_CHEALPIX "Use Chealpix for orientation filtering (deprecated)" FALSE)
OPTION (DL_DATASETS "Download demo datasets and associated run scripts" FALSE)
OPTION (OpenGR_USE_WEIGHTED_LCP "Use gaussian weights for point samples when computing LCP" FALSE)

################################################################################
## DO NOT EDIT AFTER THIS LINE
##
## Variables configuration
################################################################################
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src/)
set(ASSETS_DIR ${PROJECT_SOURCE_DIR}/assets)
set(SCRIPTS_DIR ${PROJECT_SOURCE_DIR}/scripts)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

################################################################################
## GUARDS
################################################################################

# guard against in-source builds (source: Eigen)
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there. You may need to remove CMakeCache.txt. ")
endif()

# guard against bad build-type strings (source: Eigen)
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()
string(TOLOWER "${CMAKE_BUILD_TYPE}" cmake_build_type_tolower)
if(    NOT cmake_build_type_tolower STREQUAL "debug"
   AND NOT cmake_build_type_tolower STREQUAL "release"
   AND NOT cmake_build_type_tolower STREQUAL "relwithdebinfo")
  message(FATAL_ERROR "Unknown build type \"${CMAKE_BUILD_TYPE}\". Allowed values are Debug, Release, RelWithDebInfo (case-insensitive).")
endif()

################################################################################
## Compiler configuration
################################################################################
include(cmake/ConfigureCompiler.cmake)

include(cmake/ConfigureRPath.cmake)

################################################################################
## Apply user-defined configuration
################################################################################
if (ENABLE_TIMING)
    ADD_DEFINITIONS( -DTEST_GLOBAL_TIMINGS )
    message (STATUS "OPT: Enable timing recording")
endif (ENABLE_TIMING)

if (OpenGR_USE_WEIGHTED_LCP)
    ADD_DEFINITIONS( -DOPENGR_USE_WEIGHTED_LCP )
    message (STATUS "Enable Weighted LCP")
endif (OpenGR_USE_WEIGHTED_LCP)

if (DL_DATASETS)
    add_custom_target(dl-datasets)
    include(ExternalProject)

    ExternalProject_Add(dl-dataset-demo1
        SOURCE_DIR ${ASSETS_DIR}/demo1
        URL https://www.irit.fr/~Nicolas.Mellado/dl/datasets/point-clouds/Super4PCS-dataset-demo1.zip
        URL_MD5 ad1e172902b41a3f17e9b4901adf3ba5
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/"
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<CMAKE_INSTALL_PREFIX>
        INSTALL_COMMAND ""
    )
    add_dependencies(dl-datasets dl-dataset-demo1)
endif (DL_DATASETS)

################################################################################
## Dependencies
################################################################################

if(OpenGR_USE_CHEALPIX)
    include(cmake/ConfigureChealpix.cmake)
endif(OpenGR_USE_CHEALPIX)

## Eigen: automatic configuration:
##  1. if EIGEN3_INCLUDE_DIR is set, use it directly
##  2. if any, and version >= 3.3.x, use system version
##  3. otherwise, download (if required) and use submodule
if( NOT EIGEN3_INCLUDE_DIR )
    find_package(Eigen3 QUIET)
    if( (NOT Eigen3_FOUND) OR EIGEN_VERSION_NUMBER VERSION_LESS 3.3 )

        set(EIGEN3_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/3rdparty/Eigen")

        if( NOT EXISTS ${EIGEN3_INCLUDE_DIR}/signature_of_eigen3_matrix_library )
            execute_process(COMMAND git submodule update --init -- ${EIGEN3_INCLUDE_DIR}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
        endif( NOT EXISTS ${EIGEN3_INCLUDE_DIR}/signature_of_eigen3_matrix_library )
    endif()
endif( NOT EIGEN3_INCLUDE_DIR )
include_directories(${EIGEN3_INCLUDE_DIR})
message(STATUS "Eigen3 root path: ${EIGEN3_INCLUDE_DIR}")

## Eigen: automatic configuration:
##  1. if EIGEN3_INCLUDE_DIR is set, use it directly
##  2. if any, and version >= 3.3.x, use system version
##  3. otherwise, download (if required) and use submodule
if( NOT STB_INCLUDE_DIR )
    set(STB_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/3rdparty/stb")

    if( NOT EXISTS ${STB_INCLUDE_DIR}/stb.h )
        execute_process(COMMAND git submodule update --init -- ${STB_INCLUDE_DIR}
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    endif( NOT EXISTS ${STB_INCLUDE_DIR}/stb.h )
endif( NOT STB_INCLUDE_DIR )
include_directories(${STB_INCLUDE_DIR})
message(STATUS "STB root path: ${STB_INCLUDE_DIR}")

################################################################################
## OPENGR algo
################################################################################

include_directories(${SRC_DIR})

add_subdirectory(${SRC_DIR}/gr/io)
add_subdirectory(${SRC_DIR}/gr/utils)
add_subdirectory(${SRC_DIR}/gr/accelerators)
add_subdirectory(${SRC_DIR}/gr/algorithms)

# Add main target
add_custom_target(opengr DEPENDS opengr_utils opengr_io opengr_accel opengr_algo )
if(OpenGR_USE_CHEALPIX)
    add_dependencies(opengr chealpix)
endif(OpenGR_USE_CHEALPIX)

################################################################################
## Demos, tests, assets and scripts
################################################################################

if(OpenGR_COMPILE_TESTS)
        include(cmake/ConfigureTesting.cmake)
        add_subdirectory(tests EXCLUDE_FROM_ALL)
endif(OpenGR_COMPILE_TESTS)

include(cmake/AssetsAndScripts.cmake)

if(OpenGR_COMPILE_DEMOS)
        add_subdirectory(demos)
endif(OpenGR_COMPILE_DEMOS)

################################################################################
# API documentation (generated with Doxygen)                                   #
################################################################################

find_package(Git)

if(GIT_FOUND)
    execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_CHANGESET)
    if( GIT_CHANGESET )
        # remove new line sometime appearing in git changeset
        string(REGEX REPLACE "\n$" "" GIT_CHANGESET "${GIT_CHANGESET}")
        string(SUBSTRING ${GIT_CHANGESET} 0 7 GIT_CHANGESET_SHORT)
    endif()
else()
    set(GIT_CHANGESET "")
    set(GIT_CHANGESET_SHORT "")
endif()

find_package(Doxygen 1.8.0)
if(DOXYGEN_FOUND)

    set (OpenGR_PROJECT_NUMBER "V${OpenGR_VERSION} (${GIT_CHANGESET_SHORT})")
    set (OpenGR_DOC_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc)
    file (GLOB_RECURSE OpenGR_MD_FILES ${CMAKE_CURRENT_SOURCE_DIR}/doc/*.md)

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/dox/Doxyfile.in ${OpenGR_DOC_DIRECTORY}/Doxyfile @ONLY)

    ### Doxygen bootstrap
    file(MAKE_DIRECTORY ${OpenGR_DOC_DIRECTORY}/)
    file(MAKE_DIRECTORY ${OpenGR_DOC_DIRECTORY}/html/)
    file ( DOWNLOAD https://raw.githubusercontent.com/Velron/doxygen-bootstrapped/master/customdoxygen.css
           ${OpenGR_DOC_DIRECTORY}/html/customdoxygen.css SHOW_PROGRESS)
    file ( DOWNLOAD https://raw.githubusercontent.com/Velron/doxygen-bootstrapped/master/doxy-boot.js
           ${OpenGR_DOC_DIRECTORY}/html/doxy-boot.js SHOW_PROGRESS)

    add_custom_target(doc
        ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/doc/img/
        ${OpenGR_DOC_DIRECTORY}/html/img
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/doc/dox/html/
        ${OpenGR_DOC_DIRECTORY}/html/
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/CHANGELOG
        ${OpenGR_DOC_DIRECTORY}/
        COMMAND ${DOXYGEN_EXECUTABLE} ${OpenGR_DOC_DIRECTORY}/Doxyfile
        DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/doc/dox/Doxyfile.in
        ${CMAKE_CURRENT_SOURCE_DIR}/CHANGELOG
        ${OpenGR_MD_FILES}
        SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/doc/dox/Doxyfile.in
        ${CMAKE_CURRENT_SOURCE_DIR}/CHANGELOG
        ${OpenGR_MD_FILES}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen" VERBATIM
        )
    install( DIRECTORY ${OpenGR_DOC_DIRECTORY}/ DESTINATION ${CMAKE_INSTALL_PREFIX}/doc )
endif(DOXYGEN_FOUND)

################################################################################
# Installation (source: https://github.com/forexample/package-example)
################################################################################

# add a target allowing to force the installation of the library

set(ForceInstallCommutators "")
if (MSVC)
        set(ForceInstallCommutators "/p:Configuration=${CMAKE_BUILD_TYPE}")
endif(MSVC)
add_custom_target(ForceOpenGRInstallation
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target install -- ${ForceInstallCommutators}
    COMMENT "Forcing OpenGR installation")


set(config_install_dir "${CMAKE_INSTALL_PREFIX}/lib/cmake/")
set(include_install_dir "${CMAKE_INSTALL_PREFIX}/include")
set(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")

# Configuration
set(version_config "${generated_dir}/${PROJECT_NAME}ConfigVersion.cmake")
set(project_config "${generated_dir}/${PROJECT_NAME}Config.cmake")
set(targets_export_name "${PROJECT_NAME}Targets")
set(namespace "${PROJECT_NAME}::")

# Include module with fuction 'write_basic_package_version_file'
include(CMakePackageConfigHelpers)

# Configure '<PROJECT-NAME>ConfigVersion.cmake'
# Note: PROJECT_VERSION is used as a VERSION
write_basic_package_version_file(
    "${version_config}"
                VERSION ${OpenGR_VERSION}
                COMPATIBILITY AnyNewerVersion
)

# Configure '<PROJECT-NAME>Config.cmake'
# version to be installed
# Use variables:
#   * targets_export_name
#   * PROJECT_NAME
# Used by Config.cmake.in
set(OpenGR_INSTALL_INC_DIR "${CMAKE_INSTALL_PREFIX}/include")
set(OpenGR_INSTALL_LIB_DIR "${CMAKE_INSTALL_PREFIX}/lib")
set (OpenGR_EXPORT_TARGET_DIR \${CMAKE_CURRENT_LIST_DIR})
configure_package_config_file(
    "cmake/Config.cmake.in"
    "${project_config}"
    INSTALL_DESTINATION "${config_install_dir}"
)

# Configure '<PROJECT-NAME>Config.cmake'
# version to be used with the current build directory
# Use variables:
#   * targets_export_name
#   * PROJECT_NAME
# Used by Config.cmake.in
set(OpenGR_INSTALL_INC_DIR "${CMAKE_SOURCE_DIR}/src")
set(OpenGR_INSTALL_LIB_DIR "${CMAKE_BINARY_DIR}/src/gr/io")
set (OpenGR_EXPORT_TARGET_DIR "${CMAKE_BINARY_DIR}/cmake")
configure_file(
    "cmake/Config.cmake.in"
    "${CMAKE_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
)
export(TARGETS opengr_io FILE "${CMAKE_BINARY_DIR}/cmake/OpenGR-IOLibTargets.cmake")
export(TARGETS opengr_algo FILE "${CMAKE_BINARY_DIR}/cmake/OpenGR-AlgoLibTargets.cmake")
export(TARGETS opengr_accel FILE "${CMAKE_BINARY_DIR}/cmake/OpenGR-AccelLibTargets.cmake")
export(TARGETS opengr_utils FILE "${CMAKE_BINARY_DIR}/cmake/OpenGR-UtilsLibTargets.cmake")
if(OpenGR_USE_CHEALPIX)
    export(TARGETS chealpix FILE "${CMAKE_BINARY_DIR}/cmake/chealpixTargets.cmake")
endif(OpenGR_USE_CHEALPIX)
file(COPY "${version_config}" DESTINATION "${CMAKE_BINARY_DIR}/cmake"
)


install(EXPORT OpenGR-IOLibTargets    DESTINATION ${config_install_dir})
install(EXPORT OpenGR-AlgoLibTargets  DESTINATION ${config_install_dir})
install(EXPORT OpenGR-AccelLibTargets DESTINATION ${config_install_dir})
install(EXPORT OpenGR-UtilsLibTargets DESTINATION ${config_install_dir})

if(OpenGR_USE_CHEALPIX)
    install(EXPORT chealpixTargets DESTINATION ${config_install_dir})
endif(OpenGR_USE_CHEALPIX)

# Headers:
#   * Source/foo/Bar.hpp -> <prefix>/include/foo/Bar.hpp
install(DIRECTORY ${SRC_DIR}/
        DESTINATION ${include_install_dir}
        FILES_MATCHING REGEX ".*(h|hpp)$")

# Config
#   * <prefix>/lib/cmake/Foo/FooConfig.cmake
#   * <prefix>/lib/cmake/Foo/FooConfigVersion.cmake
install(
    FILES "${project_config}" "${version_config}"
    DESTINATION "${config_install_dir}"
)
