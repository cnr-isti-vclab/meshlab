cmake_minimum_required(VERSION 3.8)

# https://github.com/libigl/libigl/issues/751
# http://lists.llvm.org/pipermail/llvm-commits/Week-of-Mon-20160425/351643.html
if(APPLE)
  if(NOT CMAKE_LIBTOOL)
    find_program(CMAKE_LIBTOOL NAMES libtool)
  endif()
  if(CMAKE_LIBTOOL)
    set(CMAKE_LIBTOOL ${CMAKE_LIBTOOL} CACHE PATH "libtool executable")
    message(STATUS "Found libtool - ${CMAKE_LIBTOOL}")
    get_property(languages GLOBAL PROPERTY ENABLED_LANGUAGES)
    foreach(lang ${languages})
      # Added -c
      set(CMAKE_${lang}_CREATE_STATIC_LIBRARY
        "${CMAKE_LIBTOOL} -c -static -o <TARGET> <LINK_FLAGS> <OBJECTS> ")
    endforeach()
  endif()
endif()

### Available options ###
option(LIBIGL_USE_STATIC_LIBRARY     "Use libigl as static library" OFF)
option(LIBIGL_WITH_CGAL              "Use CGAL"                     OFF)
option(LIBIGL_WITH_COMISO            "Use CoMiso"                   OFF)
option(LIBIGL_WITH_CORK              "Use Cork"                     OFF)
option(LIBIGL_WITH_EMBREE            "Use Embree"                   OFF)
option(LIBIGL_WITH_MATLAB            "Use Matlab"                   OFF)
option(LIBIGL_WITH_MOSEK             "Use MOSEK"                    OFF)
option(LIBIGL_WITH_OPENGL            "Use OpenGL"                   OFF)
option(LIBIGL_WITH_OPENGL_GLFW       "Use GLFW"                     OFF)
option(LIBIGL_WITH_OPENGL_GLFW_IMGUI "Use ImGui"                    OFF)
option(LIBIGL_WITH_PNG               "Use PNG"                      OFF)
option(LIBIGL_WITH_TETGEN            "Use Tetgen"                   OFF)
option(LIBIGL_WITH_TRIANGLE          "Use Triangle"                 OFF)
option(LIBIGL_WITH_PREDICATES        "Use exact predicates"         OFF)
option(LIBIGL_WITH_XML               "Use XML"                      OFF)
option(LIBIGL_WITHOUT_COPYLEFT       "Disable Copyleft libraries"   OFF)
option(LIBIGL_EXPORT_TARGETS         "Export libigl CMake targets"  OFF)

if(LIBIGL_BUILD_PYTHON)
  message(FATAL_ERROR "Python bindings have been removed in this version. Please use an older version of libigl, or wait for the new bindings to be released.")
endif()

################################################################################

### Configuration
set(LIBIGL_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")
set(LIBIGL_SOURCE_DIR "${LIBIGL_ROOT}/include")
set(LIBIGL_EXTERNAL "${LIBIGL_ROOT}/external")

# Dependencies are linked as INTERFACE targets unless libigl is compiled as a static library
if(LIBIGL_USE_STATIC_LIBRARY)
  set(IGL_SCOPE PUBLIC)
else()
  set(IGL_SCOPE INTERFACE)
endif()

# Download and update 3rdparty libraries
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
include(LibiglDownloadExternal)

# Provides igl_set_folders() to set folders for Visual Studio/Xcode
include(LibiglFolders)

################################################################################
### IGL Common
################################################################################

add_library(igl_common INTERFACE)
target_include_directories(igl_common SYSTEM INTERFACE
  $<BUILD_INTERFACE:${LIBIGL_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>
)
# Export igl_common as igl::common
set_property(TARGET igl_common PROPERTY EXPORT_NAME igl::common)
if(LIBIGL_USE_STATIC_LIBRARY)
  target_compile_definitions(igl_common INTERFACE -DIGL_STATIC_LIBRARY)
endif()

# Transitive C++11 flags
include(CXXFeatures)
target_compile_features(igl_common INTERFACE ${CXX11_FEATURES})

# Other compilation flags
if(MSVC)
  # Enable parallel compilation for Visual Studio
  target_compile_options(igl_common INTERFACE /MP /bigobj)
  target_compile_definitions(igl_common INTERFACE -DNOMINMAX)
endif()

# Controls whether to use the static MSVC runtime or not
include(LibiglWindows)

if(BUILD_SHARED_LIBS)
  # Generate position independent code
  set_target_properties(igl_common PROPERTIES INTERFACE_POSITION_INDEPENDENT_CODE ON)
endif()

if(UNIX AND NOT HUNTER_ENABLED)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
endif()

if(HUNTER_ENABLED)
  hunter_add_package(Eigen)
  find_package(Eigen3 CONFIG REQUIRED)
endif()

# Eigen
if(NOT TARGET Eigen3::Eigen)
  igl_download_eigen()
  add_library(igl_eigen INTERFACE)
  target_include_directories(igl_eigen SYSTEM INTERFACE
    $<BUILD_INTERFACE:${LIBIGL_EXTERNAL}/eigen>
    $<INSTALL_INTERFACE:include>
  )
  set_property(TARGET igl_eigen PROPERTY EXPORT_NAME Eigen3::Eigen)
  add_library(Eigen3::Eigen ALIAS igl_eigen)
endif()
target_link_libraries(igl_common INTERFACE Eigen3::Eigen)

# C++11 Thread library
find_package(Threads REQUIRED)
target_link_libraries(igl_common INTERFACE ${CMAKE_THREAD_LIBS_INIT})

################################################################################

## CGAL dependencies on Windows: GMP & MPFR
function(igl_download_cgal_deps)
  if(WIN32)
    igl_download_project(gmp
        URL     https://cgal.geometryfactory.com/CGAL/precompiled_libs/auxiliary/x64/GMP/5.0.1/gmp-all-CGAL-3.9.zip
        URL_MD5 508c1292319c832609329116a8234c9f
    )
    igl_download_project(mpfr
        URL https://cgal.geometryfactory.com/CGAL/precompiled_libs/auxiliary/x64/MPFR/3.0.0/mpfr-all-CGAL-3.9.zip
        URL_MD5 48840454eef0ff18730050c05028734b
    )
    set(ENV{GMP_DIR} "${LIBIGL_EXTERNAL}/gmp")
    set(ENV{MPFR_DIR} "${LIBIGL_EXTERNAL}/mpfr")
  endif()
endfunction()

################################################################################

function(compile_igl_module module_dir)
  string(REPLACE "/" "_" module_name "${module_dir}")
  if(module_name STREQUAL "core")
    set(module_libname "igl")
  else()
    set(module_libname "igl_${module_name}")
  endif()
  if(LIBIGL_USE_STATIC_LIBRARY)
    file(GLOB SOURCES_IGL_${module_name}
      "${LIBIGL_SOURCE_DIR}/igl/${module_dir}/*.cpp"
      "${LIBIGL_SOURCE_DIR}/igl/${module_dir}/*.h*"
    )
    if(NOT LIBIGL_WITHOUT_COPYLEFT)
      file(GLOB COPYLEFT_SOURCES_IGL_${module_name}
        "${LIBIGL_SOURCE_DIR}/igl/copyleft/${module_dir}/*.cpp"
        "${LIBIGL_SOURCE_DIR}/igl/copyleft/${module_dir}/*.h*"
      )
      list(APPEND SOURCES_IGL_${module_name} ${COPYLEFT_SOURCES_IGL_${module_name}})
    endif()
    add_library(${module_libname} STATIC ${SOURCES_IGL_${module_name}} ${ARGN})
    if(MSVC)
      # Silencing some compile warnings
      target_compile_options(${module_libname} PRIVATE
        # Type conversion warnings. These can be fixed with some effort and possibly more verbose code.
        /wd4267 # conversion from 'size_t' to 'type', possible loss of data
        /wd4244 # conversion from 'type1' to 'type2', possible loss of data
        /wd4018 # signed/unsigned mismatch
        /wd4305 # truncation from 'double' to 'float'
        # This one is from template instantiations generated by autoexplicit.sh:
        /wd4667 # no function template defined that matches forced instantiation ()
        # This one is easy to fix, just need to switch to safe version of C functions
        /wd4996 # this function or variable may be unsafe
        # This one is when using bools in adjacency matrices
        /wd4804 #'+=': unsafe use of type 'bool' in operation
      )
    endif()
  else()
    add_library(${module_libname} INTERFACE)
  endif()

  target_link_libraries(${module_libname} ${IGL_SCOPE} igl_common)
  if(NOT module_name STREQUAL "core")
    target_link_libraries(${module_libname} ${IGL_SCOPE} igl)
  endif()

  # Alias target because it looks nicer
  message(STATUS "Creating target: igl::${module_name} (${module_libname})")
  add_library(igl::${module_name} ALIAS ${module_libname})
  # Export as igl::${module_name}
  set_property(TARGET ${module_libname} PROPERTY EXPORT_NAME igl::${module_name})
endfunction()

################################################################################
### IGL Core
################################################################################

if(LIBIGL_USE_STATIC_LIBRARY)
  file(GLOB SOURCES_IGL
    "${LIBIGL_SOURCE_DIR}/igl/*.cpp"
    "${LIBIGL_SOURCE_DIR}/igl/*.h*"
    "${LIBIGL_SOURCE_DIR}/igl/copyleft/*.cpp"
    "${LIBIGL_SOURCE_DIR}/igl/copyleft/*.h*"
  )
endif()
compile_igl_module("core" ${SOURCES_IGL})

################################################################################
### Download the python part ###
if(LIBIGL_WITH_PYTHON)
endif()

################################################################################
### Compile the CGAL part ###
if(LIBIGL_WITH_CGAL)
  # Try to find the CGAL library
  # CGAL Core is needed for
  # `Exact_predicates_exact_constructions_kernel_with_sqrt`
  if(NOT TARGET CGAL::CGAL)
    set(CGAL_DIR "${LIBIGL_EXTERNAL}/cgal")
    igl_download_cgal()
    igl_download_cgal_deps()
    message("BOOST_ROOT: ${BOOST_ROOT}")
    if(EXISTS ${LIBIGL_EXTERNAL}/boost)
      set(BOOST_ROOT "${LIBIGL_EXTERNAL}/boost")
    endif()
    option(CGAL_Boost_USE_STATIC_LIBS "Use static Boost libs with CGAL" ON)

    find_package(CGAL CONFIG COMPONENTS Core PATHS ${CGAL_DIR} NO_DEFAULT_PATH)
  endif()

  # If CGAL has been found, then build the libigl module
  if(TARGET CGAL::CGAL AND TARGET CGAL::CGAL_Core)
    compile_igl_module("cgal")
    target_link_libraries(igl_cgal ${IGL_SCOPE} CGAL::CGAL CGAL::CGAL_Core)
  else()
    message(FATAL_ERROR "Could not define CGAL::CGAL and CGAL::CGAL_Core.")
  endif()
endif()

# Helper function for `igl_copy_cgal_dll()`
function(igl_copy_imported_dll src_target dst_target)
  get_target_property(other_libs ${src_target} INTERFACE_LINK_LIBRARIES)
  set(locations)
  list(APPEND locations ${main_lib} ${other_libs})
  foreach(location ${locations})
    string(REGEX MATCH "^(.*)\\.[^.]*$" dummy ${location})
    set(location "${CMAKE_MATCH_1}.dll")
    if(EXISTS "${location}" AND location MATCHES "^.*\\.dll$")
      add_custom_command(TARGET ${dst_target} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${location}" $<TARGET_FILE_DIR:${dst_target}>)
    endif()
  endforeach()
endfunction()

# Convenient functions to copy CGAL dlls into a target (executable) destination folder (for Windows)
function(igl_copy_cgal_dll target)
  if(WIN32 AND LIBIGL_WITH_CGAL)
    igl_copy_imported_dll(CGAL::CGAL ${target})
    igl_copy_imported_dll(CGAL::CGAL_Core ${target})
  endif()
endfunction()

################################################################################
### Compile the CoMISo part ###
# NOTE: this cmakefile works only with the
# comiso available here: https://github.com/libigl/CoMISo
if(LIBIGL_WITH_COMISO)
  compile_igl_module("comiso")
  if(NOT TARGET CoMISo)
    igl_download_comiso()
    add_subdirectory("${LIBIGL_EXTERNAL}/CoMISo" CoMISo)
  endif()
  target_link_libraries(igl_comiso ${IGL_SCOPE} CoMISo)
endif()

################################################################################
### Compile the cork part ###
if(LIBIGL_WITH_CORK)
  set(CORK_DIR "${LIBIGL_EXTERNAL}/cork")
  if(NOT TARGET cork)
    # call this "lib-cork" instead of "cork", otherwise cmake gets confused about
    # "cork" executable
    igl_download_cork()
    add_subdirectory("${CORK_DIR}" "lib-cork")
  endif()
  compile_igl_module("cork")
  target_include_directories(igl_cork ${IGL_SCOPE} cork)
  target_include_directories(igl_cork ${IGL_SCOPE} "${CORK_DIR}/src")
  target_link_libraries(igl_cork ${IGL_SCOPE} cork)
endif()

################################################################################
### Compile the embree part ###
if(LIBIGL_WITH_EMBREE)
  set(EMBREE_DIR "${LIBIGL_EXTERNAL}/embree")

  if(NOT TARGET embree)
    igl_download_embree()

    # Note: On macOS, building embree as a static lib can only be done with a single ISA target.
    set(EMBREE_MAX_ISA "DEFAULT" CACHE STRING "Selects highest ISA to support.")
    set(EMBREE_TESTING_INTENSITY 0 CACHE STRING "")
    set(EMBREE_ISPC_SUPPORT OFF CACHE BOOL " ")
    set(EMBREE_TASKING_SYSTEM "INTERNAL" CACHE BOOL " ")
    set(EMBREE_TUTORIALS OFF CACHE BOOL " ")
    set(EMBREE_STATIC_LIB ON CACHE BOOL " ")
    if(MSVC)
      set(EMBREE_STATIC_RUNTIME ${IGL_STATIC_RUNTIME} CACHE BOOL "Use the static version of the C/C++ runtime library.")
    endif()

    add_subdirectory("${EMBREE_DIR}" "embree" EXCLUDE_FROM_ALL)
  endif()

  compile_igl_module("embree")
  target_link_libraries(igl_embree ${IGL_SCOPE} embree)
  target_include_directories(igl_embree ${IGL_SCOPE} ${EMBREE_DIR}/include)
  target_compile_definitions(igl_embree ${IGL_SCOPE} -DEMBREE_STATIC_LIB)
endif()

################################################################################
### Compile the matlab part ###
if(LIBIGL_WITH_MATLAB)
  find_package(Matlab REQUIRED COMPONENTS MEX_COMPILER MX_LIBRARY ENG_LIBRARY MAT_LIBRARY)
  compile_igl_module("matlab")
  target_link_libraries(igl_matlab ${IGL_SCOPE} ${Matlab_LIBRARIES})
  target_include_directories(igl_matlab ${IGL_SCOPE} ${Matlab_INCLUDE_DIRS})
endif()

################################################################################
### Compile the mosek part ###
if(LIBIGL_WITH_MOSEK)
  find_package(MOSEK REQUIRED)
  compile_igl_module("mosek")
  target_link_libraries(igl_mosek ${IGL_SCOPE} ${MOSEK_LIBRARIES})
  target_include_directories(igl_mosek ${IGL_SCOPE} ${MOSEK_INCLUDE_DIRS})
  target_compile_definitions(igl_mosek ${IGL_SCOPE} -DLIBIGL_WITH_MOSEK)
endif()

################################################################################
### Compile the opengl part ###
if(LIBIGL_WITH_OPENGL)
  # OpenGL module
  compile_igl_module("opengl")

  # OpenGL library
  if (NOT CMAKE_VERSION VERSION_LESS "3.11")
    cmake_policy(SET CMP0072 NEW)
  endif()
  find_package(OpenGL REQUIRED)
  if(TARGET OpenGL::GL)
    target_link_libraries(igl_opengl ${IGL_SCOPE} OpenGL::GL)
  else()
    target_link_libraries(igl_opengl ${IGL_SCOPE} ${OPENGL_gl_LIBRARY})
    target_include_directories(igl_opengl SYSTEM ${IGL_SCOPE} ${OPENGL_INCLUDE_DIR})
  endif()

  # glad module
  if(NOT TARGET glad)
    igl_download_glad()
    add_subdirectory(${LIBIGL_EXTERNAL}/glad glad)
  endif()
  target_link_libraries(igl_opengl ${IGL_SCOPE} glad)
endif()

################################################################################
### Compile the GLFW part ###
if(LIBIGL_WITH_OPENGL_GLFW)
  if(TARGET igl::opengl)
    # GLFW module
    compile_igl_module("opengl/glfw")
    if(NOT TARGET glfw)
      igl_download_glfw()
      option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
      option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
      option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
      option(GLFW_INSTALL "Generate installation target" OFF)
      if(IGL_STATIC_RUNTIME)
        set(USE_MSVC_RUNTIME_LIBRARY_DLL OFF CACHE BOOL "Use MSVC runtime library DLL" FORCE)
      else()
        set(USE_MSVC_RUNTIME_LIBRARY_DLL ON CACHE BOOL "Use MSVC runtime library DLL" FORCE)
      endif()
      add_subdirectory(${LIBIGL_EXTERNAL}/glfw glfw)
    endif()
    target_link_libraries(igl_opengl_glfw ${IGL_SCOPE} igl_opengl glfw)
  endif()
endif()

################################################################################
### Compile the ImGui part ###
if(LIBIGL_WITH_OPENGL_GLFW_IMGUI)
  if(TARGET igl::opengl_glfw)
    # ImGui module
    compile_igl_module("opengl/glfw/imgui")
    if(NOT TARGET imgui)
      igl_download_imgui()
      add_subdirectory(${LIBIGL_EXTERNAL}/libigl-imgui imgui)
    endif()
    if(NOT TARGET imguizmo)
      igl_download_imguizmo()
      add_library(imguizmo ${LIBIGL_EXTERNAL}/imguizmo/ImGuizmo.cpp ${LIBIGL_EXTERNAL}/imguizmo/ImGuizmo.h)
      target_compile_features(imguizmo PUBLIC cxx_std_11)
      target_link_libraries(imguizmo PUBLIC imgui)
    endif()
    target_link_libraries(igl_opengl_glfw_imgui ${IGL_SCOPE} igl_opengl_glfw imgui imguizmo)
  endif()
endif()

################################################################################
### Compile the png part ###
if(LIBIGL_WITH_PNG)
  # png/ module is anomalous because it also depends on opengl it really should
  # be moved into the opengl/ directory and namespace ...
  if(TARGET igl_opengl)
    if(NOT TARGET stb_image)
      igl_download_stb()
      add_subdirectory(${LIBIGL_EXTERNAL}/stb stb_image)
    endif()
    compile_igl_module("png" "")
    target_link_libraries(igl_png ${IGL_SCOPE} igl_stb_image igl_opengl)
  endif()
endif()

################################################################################
### Compile the tetgen part ###
if(LIBIGL_WITH_TETGEN)
  set(TETGEN_DIR "${LIBIGL_EXTERNAL}/tetgen")
  if(NOT TARGET tetgen)
    igl_download_tetgen()
    add_subdirectory("${TETGEN_DIR}" "tetgen")
  endif()
  compile_igl_module("tetgen")
  target_link_libraries(igl_tetgen ${IGL_SCOPE} tetgen)
  target_include_directories(igl_tetgen ${IGL_SCOPE} ${TETGEN_DIR})
endif()

################################################################################
### Compile the triangle part ###
if(LIBIGL_WITH_TRIANGLE)
  set(TRIANGLE_DIR "${LIBIGL_EXTERNAL}/triangle")
  if(NOT TARGET triangle)
    igl_download_triangle()
    add_subdirectory("${TRIANGLE_DIR}" "triangle")
  endif()
  compile_igl_module("triangle")
  target_link_libraries(igl_triangle ${IGL_SCOPE} triangle)
  target_include_directories(igl_triangle ${IGL_SCOPE} ${TRIANGLE_DIR})
endif()

################################################################################
### Compile the predicates part ###
if(LIBIGL_WITH_PREDICATES)
  set(PREDICATES_DIR "${LIBIGL_EXTERNAL}/predicates")
  if(NOT TARGET predicates)
    igl_download_predicates()
    add_subdirectory("${PREDICATES_DIR}" "predicates")
  endif()
  compile_igl_module("predicates")
  target_link_libraries(igl_predicates ${IGL_SCOPE} predicates)
  target_include_directories(igl_predicates ${IGL_SCOPE} ${PREDICATES_DIR})
  target_compile_definitions(igl_predicates ${IGL_SCOPE} -DLIBIGL_WITH_PREDICATES)
endif()

################################################################################
### Compile the xml part ###
if(LIBIGL_WITH_XML)
  set(TINYXML2_DIR "${LIBIGL_EXTERNAL}/tinyxml2")
  if(NOT TARGET tinyxml2)
    igl_download_tinyxml2()
    add_library(tinyxml2 STATIC ${TINYXML2_DIR}/tinyxml2.cpp ${TINYXML2_DIR}/tinyxml2.h)
    target_include_directories(tinyxml2 PUBLIC ${TINYXML2_DIR})
    set_target_properties(tinyxml2 PROPERTIES
            COMPILE_DEFINITIONS "TINYXML2_EXPORT"
            VERSION "3.0.0"
            SOVERSION "3")
  endif()
  compile_igl_module("xml")
  target_link_libraries(igl_xml ${IGL_SCOPE} tinyxml2)
  target_include_directories(igl_xml ${IGL_SCOPE} ${TINYXML2_DIR})
endif()

################################################################################
### Install and export all modules

if(NOT LIBIGL_EXPORT_TARGETS)
  return()
endif()

function(install_dir_files dir_name)
  if (dir_name STREQUAL "core")
    set(subpath "")
  else()
    set(subpath "/${dir_name}")
  endif()

  file(GLOB public_headers
    ${CMAKE_CURRENT_SOURCE_DIR}/include/igl${subpath}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/igl${subpath}/*.hpp
  )

  set(files_to_install ${public_headers})

  if(NOT LIBIGL_USE_STATIC_LIBRARY)
    file(GLOB public_sources
      ${CMAKE_CURRENT_SOURCE_DIR}/include/igl${subpath}/*.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/include/igl${subpath}/*.c
    )
  endif()
  list(APPEND files_to_install ${public_sources})

  install(
    FILES ${files_to_install}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/igl${subpath}
  )
endfunction()

################################################################################

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

if(TARGET igl_eigen)
  set(IGL_EIGEN igl_eigen)
else()
  set(IGL_EIGEN)
  message(WARNING "Trying to export igl targets while using an imported target for Eigen.")
endif()

# Install and export core library
install(
  TARGETS
    igl
    igl_common
    ${IGL_EIGEN}
  EXPORT igl-export
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
export(
  TARGETS
    igl
    igl_common
    ${IGL_EIGEN}
  FILE libigl-export.cmake
)

# Install headers for core library
install_dir_files(core)
install_dir_files(copyleft)

# Write package configuration file
configure_package_config_file(
  ${CMAKE_CURRENT_LIST_DIR}/libigl-config.cmake.in
  ${CMAKE_BINARY_DIR}/libigl-config.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/libigl/cmake
)
install(
  FILES
    ${CMAKE_BINARY_DIR}/libigl-config.cmake
  DESTINATION
    ${CMAKE_INSTALL_DATADIR}/libigl/cmake
)

# Write export file
export(EXPORT igl-export
  FILE "${CMAKE_BINARY_DIR}/libigl-export.cmake"
)
install(EXPORT igl-export DESTINATION ${CMAKE_INSTALL_DATADIR}/libigl/cmake FILE libigl-export.cmake)


export(PACKAGE libigl)

