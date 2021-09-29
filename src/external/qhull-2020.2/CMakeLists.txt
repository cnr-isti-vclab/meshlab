# CMakeLists.txt -- CMake configuration file for qhull, qhull6, and related programs
#
# CMake options
#   BUILD_SHARED_LIBS=OFF -- Do not build libqhull_r.so
#   BUILD_STATIC_LIBS=OFF -- Do not build libqhullcpp.a, libqhullstatic_r.a, libqhullstatic.a, and user_eg3
#   CMAKE_INSTALL_PREFIX=path -- Prefix for the install directories
#   LINK_APPS_SHARED=ON -- Link qconvex,etc. to libqhull_r instead of libqhullstatic_r
#
# 32-bit vs. 64-bit
#   Qhull uses less memory when built as 32-bit code
#   On 64-bit hosts, consider building with -DCMAKE_CXX_FLAGS=-m32 -DCMAKE_C_FLAGS=-m32
#
# Additional build targets
#   libqhull -- Deprecated, shared library for non-reentrant Qhull (use libqhull_r or libqhullstatic)
#   qhull_p -- Deprecated, shared library for qh_QHpointer and non-reentrant Qhull (use libqhull_r)
#   qhullp -- Same as qhull using qh_QHpointer and libqhull_p
#   user_egp -- Same as user_eg using qh_QHpointer and libqhull_p
#
# For qhulltest, use the Qt build (src/qhull-all.pro)
#
# To install CMake
#   Download from http://www.cmake.org/download/
#
# To find the available targets for CMake -G "..."
#   cmake --help
#
# To build with MSYS, MSYS2, mingw, mingw-w64, or GitForWindows
#   cd build && cmake -G "MSYS Makefiles" .. && cmake ..
#   make
#   make install
#
# To build Qhull with Visual Studio projects, run cmake twice
#   To install bin/doc/include/lib in the current directory
#      mkdir -p build-cmake && cd build-cmake && cmake -G "Visual Studio 11 2012" .. && cmake -DCMAKE_INSTALL_PREFIX=.. ..
#      mkdir -p build-cmake && cd build-cmake && cmake -G "Visual Studio 11 2012 Win64" .. && cmake -DCMAKE_INSTALL_PREFIX=.. ..
#   To install into Program Files/qhull
#      mkdir -p build-cmake && cd build-cmake && cmake -G "Visual Studio 11 2012" .. && cmake ..
#      mkdir -p build-cmake && cd build-cmake && cmake -G "Visual Studio 11 2012 Win64" .. && cmake ..
#   To build for Visual Studio 2005 and install into Program Files/qhull
#      mkdir -p build-cmake && cd build-cmake && cmake -G "Visual Studio 8 2005" .. && cmake  ..
#      mkdir -p build-cmake && cd build-cmake && cmake -G "Visual Studio 8 2005 Win64" .. && cmake  ..
#   Double click build-cmake/qhull-all.sln
#   Build INSTALL to copy files into C:/Program Files/qhull
#
# To uninstall on unix or MSYS/mingw
#   make uninstall
#
# Notes on Visual Studio projects
#   You may need to copy bin/msvcr80.dll into C:/Program Files/qhull/bin
#   If using library debug targets, please rename with '_d' (e.g., qhullstatic_d.lib)
# 
# Troubleshooting
#   "No CMAKE_C_COMPILER could be found"
#     cmake was not able to find the build environment specified (e.g., Visual Studio 11)
# 
# Qhull ships with CMake-derived sln and proj files for DevStudio 8 2005
#   See eg/make-vcproj.sh
#   Change to relative paths
#   Remove ZERO_CHECK, ALL_BUILD, and INSTALL projects
#   Change targets to bin/ and lib/ directories
#   Disable incremental linking and ilk files (LinkIncremental="1")
#   Disable Run-Time Type Info (rtti)
#   Remove src/libqhullcpp from most of the AdditionalIncludeDirectories
#   Remove CMAKE_INTDIR from PreprocessorDefinitions
#   Adjust target names and destinations (e.g., lib/libqhullstatic_rd.a)
#  
# $Id: //main/2019/qhull/CMakeLists.txt#21 $$Change: 3039 $
# $DateTime: 2020/09/03 21:26:22 $$Author: bbarber $

project(qhull)
cmake_minimum_required(VERSION 3.0)

# Define qhull_VERSION in README.txt, Announce.txt, qh-get.htm, CMakeLists.txt
#   qhull-zip.sh (twice), qhull-wiki.md, qhull-news.htm, File_id.diz, index.htm
#   qhull-warn.pri (VERSION), qhull-exports.def (VERSION), qhull_p-exports.def, qhull_r-exports.def
set(qhull_VERSION2 "2020.2  2020/08/31")  # not used, See global.c, global_r.c, rbox.c, rbox_r.c
set(qhull_VERSION  "8.0.2")  # Advance every release

# SOVERSION -- qhull 2003 = empty, 2009 = 5, 2010-2012 = 6, 2015-2019 = 7, 2020 = 8.0
# Increase SOVERSION if ABI breaks (abi-compliance-checker)
set(qhull_SOVERSION 8.0) # For SOVERSION 

# If MSYS, set CMAKE_INSTALL_PREFIX to /usr/local; if WINDOWS, '..'
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    if(MSYS)
    	execute_process(
            COMMAND sh -c 'cd / && pwd -W'
            OUTPUT_VARIABLE MSYS_PWD
            ERROR_VARIABLE MSYS_ERR
        )
        if(MSYS_ERR STREQUAL "")
            string(REPLACE "\n" "" MSYS_ROOT ${MSYS_PWD})
            set(CMAKE_INSTALL_PREFIX "${MSYS_ROOT}/usr/local" CACHE PATH "Install to ${MSYS_ROOT}/usr/local" FORCE)
        else()
            message(STATUS "Use default MAKE_INSTALL_PREFIX due to ${MSYS_ERR}")
        endif()
    endif()
endif()

include(build/CMakeModules/CheckLFS.cmake)
option(WITH_LFS "Enable Large File Support" ON)
check_lfs(WITH_LFS)

# Build shared and static libs by default
# If LINK_APPS_SHARED=ON, link applications to libqhull_r.so

include(CMakeDependentOption)
option(BUILD_STATIC_LIBS "Build static libraries" ON)
option(BUILD_SHARED_LIBS "Build shared library" ON)
set(_NO_STATIC_LIBS NOCACHE INTERNAL (NOT ${BUILD_STATIC_LIBS}))
cmake_dependent_option(LINK_APPS_SHARED "Use shared library for linking applications"
    _NO_STATIC_LIBS
    "BUILD_SHARED_LIBS;BUILD_STATIC_LIBS"
    ${BUILD_SHARED_LIBS}
)

if(INCLUDE_INSTALL_DIR)
else()
    set(INCLUDE_INSTALL_DIR include)
endif()
if(LIB_INSTALL_DIR)
else()
    set(LIB_INSTALL_DIR lib)
endif()
if(BIN_INSTALL_DIR)
else()
    set(BIN_INSTALL_DIR bin)
endif()
if(MAN_INSTALL_DIR)
else()
    set(MAN_INSTALL_DIR share/man/man1)
endif()
if(DOC_INSTALL_DIR)
else()
    set(DOC_INSTALL_DIR share/doc/qhull)
endif()
message(STATUS)
message(STATUS "========== qhull Build Information ==========")
message(STATUS "Build Version:                             ${qhull_VERSION}")
message(STATUS "Install Prefix (CMAKE_INSTALL_PREFIX):     ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Binary Directory (BIN_INSTALL_DIR):        ${BIN_INSTALL_DIR}")
message(STATUS "Library Directory (LIB_INSTALL_DIR):       ${LIB_INSTALL_DIR}")
message(STATUS "Include Directory (INCLUDE_INSTALL_DIR):   ${INCLUDE_INSTALL_DIR}")
message(STATUS "Documentation Directory (DOC_INSTALL_DIR): ${DOC_INSTALL_DIR}")
message(STATUS "Man Pages Directory (MAN_INSTALL_DIR):     ${MAN_INSTALL_DIR}")
message(STATUS "Build Type (CMAKE_BUILD_TYPE):             ${CMAKE_BUILD_TYPE}")
message(STATUS "Build static libraries:                    ${BUILD_STATIC_LIBS}")
message(STATUS "Build shared library:                      ${BUILD_SHARED_LIBS}")
message(STATUS "Use shared library for linking apps:       ${LINK_APPS_SHARED}")
message(STATUS "To override these options, add -D{OPTION_NAME}=... to the cmake command")
message(STATUS "  Build the debug targets                  -DCMAKE_BUILD_TYPE=Debug")
message(STATUS)
message(STATUS "To build and install qhull, enter \"make\" and \"make install\"")
message(STATUS "To smoketest qhull, enter \"ctest\"")
message(STATUS)

# ---------------------------------------
# Define library source files and variables
#
# Files for individual targets are defined with the target
# ---------------------------------------

# Order libqhull object files by frequency of execution.  Small files at end.

# Non-reentrant Qhull
set(
    libqhull_HEADERS
        src/libqhull/libqhull.h
        src/libqhull/geom.h
        src/libqhull/io.h
        src/libqhull/mem.h
        src/libqhull/merge.h
        src/libqhull/poly.h
        src/libqhull/qhull_a.h
        src/libqhull/qset.h
        src/libqhull/random.h
        src/libqhull/stat.h
        src/libqhull/user.h
)
set(
    libqhull_SOURCES
        src/libqhull/global.c
        src/libqhull/stat.c
        src/libqhull/geom2.c
        src/libqhull/poly2.c
        src/libqhull/merge.c
        src/libqhull/libqhull.c
        src/libqhull/geom.c
        src/libqhull/poly.c
        src/libqhull/qset.c
        src/libqhull/mem.c
        src/libqhull/random.c
        src/libqhull/usermem.c
        src/libqhull/userprintf.c
        src/libqhull/io.c
        src/libqhull/user.c
        src/libqhull/rboxlib.c
        src/libqhull/userprintf_rbox.c
        ${libqhull_HEADERS}
)

set(
    testqset_HEADERS
        src/libqhull/mem.h
        src/libqhull/qset.h
)
set(
    testqset_SOURCES
        src/libqhull/qset.c
        src/libqhull/mem.c
        src/libqhull/usermem.c
        src/testqset/testqset.c
        ${testqset_HEADERS}
)

# Reeentrant Qhull

set(
    libqhullr_HEADERS
        src/libqhull_r/libqhull_r.h
        src/libqhull_r/geom_r.h
        src/libqhull_r/io_r.h
        src/libqhull_r/mem_r.h
        src/libqhull_r/merge_r.h
        src/libqhull_r/poly_r.h
        src/libqhull_r/qhull_ra.h
        src/libqhull_r/qset_r.h
        src/libqhull_r/random_r.h
        src/libqhull_r/stat_r.h
        src/libqhull_r/user_r.h
)
set(
    libqhullr_SOURCES
        src/libqhull_r/global_r.c
        src/libqhull_r/stat_r.c
        src/libqhull_r/geom2_r.c
        src/libqhull_r/poly2_r.c
        src/libqhull_r/merge_r.c
        src/libqhull_r/libqhull_r.c
        src/libqhull_r/geom_r.c
        src/libqhull_r/poly_r.c
        src/libqhull_r/qset_r.c
        src/libqhull_r/mem_r.c
        src/libqhull_r/random_r.c
        src/libqhull_r/usermem_r.c
        src/libqhull_r/userprintf_r.c
        src/libqhull_r/io_r.c
        src/libqhull_r/user_r.c
        src/libqhull_r/rboxlib_r.c
        src/libqhull_r/userprintf_rbox_r.c
        ${libqhullr_HEADERS}
)

set(
    testqsetr_HEADERS
        src/libqhull_r/mem_r.h
        src/libqhull_r/qset_r.h
)
set(
    testqsetr_SOURCES
        src/libqhull_r/qset_r.c
        src/libqhull_r/mem_r.c
        src/libqhull_r/usermem_r.c
        src/testqset_r/testqset_r.c
        ${testqsetr_HEADERS}
)

# C++ interface to reentrant Qhull

set(
    libqhullcpp_HEADERS
        src/libqhullcpp/Coordinates.h
        src/libqhullcpp/functionObjects.h
        src/libqhullcpp/PointCoordinates.h
        src/libqhullcpp/Qhull.h
        src/libqhullcpp/QhullError.h
        src/libqhullcpp/QhullFacet.h
        src/libqhullcpp/QhullFacetList.h
        src/libqhullcpp/QhullFacetSet.h
        src/libqhullcpp/QhullHyperplane.h
        src/libqhullcpp/QhullIterator.h
        src/libqhullcpp/QhullLinkedList.h
        src/libqhullcpp/QhullPoint.h
        src/libqhullcpp/QhullPoints.h
        src/libqhullcpp/QhullPointSet.h
        src/libqhullcpp/QhullQh.h
        src/libqhullcpp/QhullRidge.h
        src/libqhullcpp/QhullSet.h
        src/libqhullcpp/QhullSets.h
        src/libqhullcpp/QhullStat.h
        src/libqhullcpp/QhullUser.h
        src/libqhullcpp/QhullVertex.h
        src/libqhullcpp/QhullVertexSet.h
        src/libqhullcpp/RboxPoints.h
        src/libqhullcpp/RoadError.h
        src/libqhullcpp/RoadLogEvent.h
        src/qhulltest/RoadTest.h
)

set(
    libqhullcpp_SOURCES
        src/libqhullcpp/Coordinates.cpp
        src/libqhullcpp/PointCoordinates.cpp
        src/libqhullcpp/Qhull.cpp
        src/libqhullcpp/QhullFacet.cpp
        src/libqhullcpp/QhullFacetList.cpp
        src/libqhullcpp/QhullFacetSet.cpp
        src/libqhullcpp/QhullHyperplane.cpp
        src/libqhullcpp/QhullPoint.cpp
        src/libqhullcpp/QhullPointSet.cpp
        src/libqhullcpp/QhullPoints.cpp
        src/libqhullcpp/QhullQh.cpp
        src/libqhullcpp/QhullRidge.cpp
        src/libqhullcpp/QhullSet.cpp
        src/libqhullcpp/QhullStat.cpp
        src/libqhullcpp/QhullUser.cpp
        src/libqhullcpp/QhullVertex.cpp
        src/libqhullcpp/QhullVertexSet.cpp
        src/libqhullcpp/RboxPoints.cpp
        src/libqhullcpp/RoadError.cpp
        src/libqhullcpp/RoadLogEvent.cpp
        ${libqhullcpp_HEADERS}
)

# Documentation files (index.htm refers to html/...)

set(
    doc_FILES
        README.txt 
        REGISTER.txt 
        Announce.txt 
        COPYING.txt 
        index.htm
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

set(qhull_CPP qhullcpp)
set(qhull_STATIC qhullstatic)
set(qhull_STATICR qhullstatic_r)
set(qhull_SHAREDR qhull_r)
set(qhull_SHARED libqhull)  # Temporarily avoid name conflict with qhull executable
set(qhull_SHAREDP qhull_p)  # libqhull and qhull_p are deprecated, use qhull_r instead

set(qhull_TARGETS_APPLICATIONS qhull rbox qconvex qdelaunay qvoronoi qhalf)
set(qhull_TARGETS_STATIC ${qhull_CPP} ${qhull_STATIC} ${qhull_STATICR})
set(qhull_TARGETS_SHARED ${qhull_SHAREDR})

set(
    qhull_TARGETS_TEST   # Unused
        testqset testqset_r user_eg user_eg2 user_eg3
)
set(
    qhull_TARGETS_DEPRECATED   # Unused
        $(qhull_SHARED) $(qhull_SHAREDP) qhullp user_egp 
)

# ---------------------------------------
# Define shared library for reentrant qhull (installed)
# ---------------------------------------

add_library(${qhull_SHAREDR} SHARED 
        ${libqhullr_SOURCES}
        src/libqhull_r/qhull_r-exports.def)
set_target_properties(${qhull_SHAREDR} PROPERTIES
    SOVERSION ${qhull_SOVERSION}
    VERSION ${qhull_VERSION}
    OUTPUT_NAME "${qhull_SHAREDR}$<$<CONFIG:Debug>:d>")

if(UNIX)
    target_link_libraries(${qhull_SHAREDR} m)
    if(APPLE)
        set_target_properties(${qhull_SHAREDR} PROPERTIES 
            INSTALL_NAME_DIR "${LIB_INSTALL_DIR}")
    else()
        set_target_properties(${qhull_SHAREDR} PROPERTIES 
            INSTALL_RPATH "${LIB_INSTALL_DIR}"
            INSTALL_RPATH_USE_LINK_PATH TRUE
            BUILD_WITH_INSTALL_RPATH FALSE)
    endif()
endif(UNIX)

# ---------------------------------------
# Define deprecated shared library for non-reentrant qhull
# ---------------------------------------

add_library(${qhull_SHARED} SHARED 
        ${libqhull_SOURCES}
        src/libqhull/qhull-exports.def)

set_target_properties(${qhull_SHARED} PROPERTIES
    EXCLUDE_FROM_ALL TRUE
    SOVERSION ${qhull_SOVERSION}
    VERSION ${qhull_VERSION}
    OUTPUT_NAME "qhull$<$<CONFIG:Debug>:_d>")

if(UNIX)
    target_link_libraries(${qhull_SHARED} m)
    if(APPLE)
        set_target_properties(${qhull_SHARED} PROPERTIES 
            INSTALL_NAME_DIR "${LIB_INSTALL_DIR}")
    else()
        set_target_properties(${qhull_SHARED} PROPERTIES 
            INSTALL_RPATH "${LIB_INSTALL_DIR}"
            INSTALL_RPATH_USE_LINK_PATH TRUE
            BUILD_WITH_INSTALL_RPATH FALSE)
    endif()
endif(UNIX)

# ---------------------------------------
# Define deprecated shared library for non-reentrant qhull with qh_QHpointer
# ---------------------------------------

add_library(${qhull_SHAREDP} SHARED
        ${libqhull_SOURCES}
        src/libqhull/qhull_p-exports.def)

set_target_properties(${qhull_SHAREDP} PROPERTIES
    EXCLUDE_FROM_ALL TRUE
    COMPILE_DEFINITIONS "qh_QHpointer"
    SOVERSION ${qhull_SOVERSION}
    VERSION ${qhull_VERSION}
    OUTPUT_NAME "${qhull_SHAREDP}$<$<CONFIG:Debug>:d>")

if(UNIX)
    target_link_libraries(${qhull_SHAREDP} m)
    if(APPLE)
        set_target_properties(${qhull_SHAREDP} PROPERTIES 
            INSTALL_NAME_DIR "${LIB_INSTALL_DIR}")
    else()
        set_target_properties(${qhull_SHAREDP} PROPERTIES 
            INSTALL_RPATH "${LIB_INSTALL_DIR}"
            INSTALL_RPATH_USE_LINK_PATH TRUE
            BUILD_WITH_INSTALL_RPATH FALSE)
    endif()
endif(UNIX)

# ---------------------------------------
# Define static libraries qhullstatic (non-reentrant) and qhullstatic_r (reentrant)
# ---------------------------------------

add_library(${qhull_STATIC} STATIC ${libqhull_SOURCES})
set_target_properties(${qhull_STATIC} PROPERTIES
    VERSION ${qhull_VERSION}
    OUTPUT_NAME "${qhull_STATIC}$<$<CONFIG:Debug>:_d>")

add_library(${qhull_STATICR} STATIC ${libqhullr_SOURCES})
set_target_properties(${qhull_STATICR} PROPERTIES
    VERSION ${qhull_VERSION}
    OUTPUT_NAME "${qhull_STATICR}$<$<CONFIG:Debug>:d>")

if(UNIX)
    target_link_libraries(${qhull_STATIC} m)
    target_link_libraries(${qhull_STATICR} m)
endif(UNIX)

# ---------------------------------------
# Define C++ static library qhullcpp
#     Do not create libqhullcpp as a shared library.  
#     Qhull C++ classes may change layout and size. 
#     Qhull C programs use setjmp/longjmp for error handling.
# ---------------------------------------

add_library(${qhull_CPP} STATIC ${libqhullcpp_SOURCES})
set_target_properties(${qhull_CPP} PROPERTIES
    VERSION ${qhull_VERSION}
    OUTPUT_NAME "${qhull_CPP}$<$<CONFIG:Debug>:_d>"
    POSITION_INDEPENDENT_CODE "TRUE")

# ---------------------------------------
# if BUILD_STATIC_LIBS=OFF or BUILD_SHARED_LIBS=OFF
#   Exclude library builds from "make all" 
# ---------------------------------------
if(NOT ${BUILD_STATIC_LIBS})
    set_target_properties(${qhull_STATIC} PROPERTIES EXCLUDE_FROM_ALL TRUE)
    set_target_properties(${qhull_STATICR} PROPERTIES EXCLUDE_FROM_ALL TRUE)
    set_target_properties(${qhull_CPP} PROPERTIES EXCLUDE_FROM_ALL TRUE)
endif()
if(NOT ${BUILD_SHARED_LIBS})
    set_target_properties(${qhull_SHARED} PROPERTIES EXCLUDE_FROM_ALL TRUE)
    set_target_properties(${qhull_SHAREDR} PROPERTIES EXCLUDE_FROM_ALL TRUE)
    set_target_properties(${qhull_SHAREDP} PROPERTIES EXCLUDE_FROM_ALL TRUE)
endif()

# ---------------------------------------
# Define options for linking to shared libaries
# ---------------------------------------
if(MSVC) 
    set(qconvex_DEFINES  qh_dllimport)
    set(qdelaunay_DEFINES qh_dllimport)
    set(qhalf_DEFINES    qh_dllimport)
    set(qhull_DEFINES    qh_dllimport)
    set(qvoronoi_DEFINES qh_dllimport)
    set(rbox_DEFINES 	 qh_dllimport)
    set(user_eg_DEFINES  qh_dllimport)
    set(user_eg2_DEFINES qh_dllimport)
    set(user_egp_DEFINES qh_QHpointer_dllimport qh_QHpointer)
    set(qhullp_DEFINES   qh_QHpointer_dllimport qh_QHpointer)
else()
    set(qconvex_DEFINES )
    set(qdelaunay_DEFINES )
    set(qhalf_DEFINES )
    set(qhull_DEFINES )
    set(qvoronoi_DEFINES )
    set(rbox_DEFINES )
    set(user_eg_DEFINES )
    set(user_eg2_DEFINES )
    set(user_egp_DEFINES qh_QHpointer)
    set(qhullp_DEFINES qh_QHpointer)
endif()

# ---------------------------------------
# Define qhull applications
# 
# By default, qhull is linked to reentrant qhull (more flexible)
# the other applications are linked to non-reentrant qhull (somewhat faster)
#
# If LINK_APPS_SHARED, applications are linked to reentrant qhull
# ---------------------------------------

if(${LINK_APPS_SHARED})
    add_executable(qconvex src/qconvex/qconvex_r.c)
    target_link_libraries(qconvex ${qhull_SHAREDR})
    set_target_properties(qconvex PROPERTIES
        COMPILE_DEFINITIONS "${qconvex_DEFINES}")

    add_executable(qdelaunay src/qdelaunay/qdelaun_r.c)
    target_link_libraries(qdelaunay ${qhull_SHAREDR})
    set_target_properties(qdelaunay PROPERTIES
        COMPILE_DEFINITIONS "${qdelaunay_DEFINES}")

    add_executable(qhalf src/qhalf/qhalf_r.c)
    target_link_libraries(qhalf ${qhull_SHAREDR})
    set_target_properties(qhalf PROPERTIES
        COMPILE_DEFINITIONS "${qhalf_DEFINES}")

    add_executable(qhull src/qhull/unix_r.c)
    target_link_libraries(qhull ${qhull_SHAREDR})
    set_target_properties(qhull PROPERTIES
        COMPILE_DEFINITIONS "${qhull_DEFINES}")

    add_executable(qvoronoi src/qvoronoi/qvoronoi_r.c)
    target_link_libraries(qvoronoi ${qhull_SHAREDR})
    set_target_properties(qvoronoi PROPERTIES
        COMPILE_DEFINITIONS "${qvoronoi_DEFINES}")

    add_executable(rbox src/rbox/rbox_r.c)
    target_link_libraries(rbox ${qhull_SHAREDR})
    set_target_properties(rbox PROPERTIES
        COMPILE_DEFINITIONS "${rbox_DEFINES}")
else()
    if(NOT ${BUILD_STATIC_LIBS})
        message(FATAL_ERROR, " Nothing to build -- BUILD_SHARED_LIBS=OFF and BUILD_STATIC_LIBS=OFF")
    endif()

    add_executable(qconvex src/qconvex/qconvex.c)
    target_link_libraries(qconvex ${qhull_STATIC})

    add_executable(qdelaunay src/qdelaunay/qdelaun.c)
    target_link_libraries(qdelaunay ${qhull_STATIC})

    add_executable(qhalf src/qhalf/qhalf.c)
    target_link_libraries(qhalf ${qhull_STATIC})

    add_executable(qhull src/qhull/unix_r.c)
    target_link_libraries(qhull ${qhull_STATICR})

    add_executable(qvoronoi src/qvoronoi/qvoronoi.c)
    target_link_libraries(qvoronoi ${qhull_STATIC})

    add_executable(rbox src/rbox/rbox.c)
    target_link_libraries(rbox ${qhull_STATIC})
endif()

# #@# 20
# ---------------------------------------
# Define testqset linked to qset.o, mem.o, and usermem.o
# Define testqset_r linked to qset_r.o, mem_r.o, and usermem.o
# ---------------------------------------

add_executable(testqset ${testqset_SOURCES})
add_executable(testqset_r ${testqsetr_SOURCES})

# ---------------------------------------
# Define user_eg linked to reentrant qhull shared library
# ---------------------------------------

add_executable(user_eg src/user_eg/user_eg_r.c)

if(${BUILD_SHARED_LIBS})
    target_link_libraries(user_eg ${qhull_SHAREDR})
    set_target_properties(user_eg PROPERTIES
	COMPILE_DEFINITIONS "${user_eg_DEFINES}")
else()
    target_link_libraries(user_eg ${qhull_STATICR})
endif()

# ---------------------------------------
# Define user_eg2 linked to reentrant qhull static library
# ---------------------------------------

add_executable(user_eg2 src/user_eg2/user_eg2_r.c)

if(${BUILD_STATIC_LIBS})
    target_link_libraries(user_eg2 ${qhull_STATICR})
else()
    target_link_libraries(user_eg2 ${qhull_SHAREDR})
    set_target_properties(user_eg2 PROPERTIES
	COMPILE_DEFINITIONS "${user_eg2_DEFINES}")
endif()

# ---------------------------------------
# Define user_eg3 linked to qhullcpp and qhullstatic_r static libraries
# 
# user_eg3 is not defined for shared libraries
#   user_eg3 and qhullcpp must be compiled with the same compiler for setjmp/longjmp
# ---------------------------------------

if(${BUILD_STATIC_LIBS})
    add_executable(user_eg3 src/user_eg3/user_eg3_r.cpp)
    # qhull_STATICR must be last, otherwise qh_fprintf,etc. are not loaded from qhull_CPP
    target_link_libraries(user_eg3 ${qhull_CPP} ${qhull_STATICR})
endif()

# ---------------------------------------
# qhullp is qhull/unix.c linked to unsuported qh_QHpointer libqhull_p
# Included for testing qh_QHpointer 
# ---------------------------------------

add_executable(qhullp EXCLUDE_FROM_ALL src/qhull/unix.c)
target_link_libraries(qhullp ${qhull_SHAREDP})
set_target_properties(qhullp PROPERTIES
    COMPILE_DEFINITIONS "${qhullp_DEFINES}")

# ---------------------------------------
# user_egp is user_eg/user_eg.c linked to unsuported qh_QHpointer libqhull_p
# Included for compatibility with qhull-2012.1 
# ---------------------------------------

add_executable(user_egp EXCLUDE_FROM_ALL src/user_eg/user_eg.c)
target_link_libraries(user_egp ${qhull_SHAREDP})
set_target_properties(user_egp PROPERTIES
    COMPILE_DEFINITIONS "${user_egp_DEFINES}")

# ---------------------------------------
# Define test
# ---------------------------------------

enable_testing()
add_test(NAME testqset
   COMMAND ./testqset 10000)
add_test(NAME testqset_r
   COMMAND ./testqset_r 10000)
add_test(NAME smoketest
   COMMAND sh -c "./rbox D4 | ./qhull Tv")
add_test(NAME rbox-10-qhull
   COMMAND sh -c "./rbox 10 | ./qhull Tv")
add_test(NAME rbox-10-qconvex
   COMMAND sh -c "./rbox 10 | ./qconvex Tv")
add_test(NAME rbox-10-qdelaunay
   COMMAND sh -c "./rbox 10 | ./qdelaunay Tv")
add_test(NAME rbox-10-qhalf
   COMMAND sh -c "./rbox 10 | ./qconvex FQ FV n Tv | ./qhalf Tv")
add_test(NAME rbox-10-qvoronoi
   COMMAND sh -c "./rbox 10 | ./qvoronoi Tv")
add_test(NAME user_eg
   COMMAND sh -c "./user_eg")
add_test(NAME user_eg2
   COMMAND sh -c "./user_eg2")
   
if(${BUILD_STATIC_LIBS})
    add_test(NAME user_eg3
       COMMAND sh -c "./user_eg3 rbox '10 D2' '2 D2' qhull 's p' facets")
endif()

# ---------------------------------------
# Define install
# ---------------------------------------

set(qhull_TARGETS_INSTALL ${qhull_TARGETS_APPLICATIONS})
if (BUILD_SHARED_LIBS)
    list(APPEND qhull_TARGETS_INSTALL ${qhull_TARGETS_SHARED})
endif()
if (BUILD_STATIC_LIBS)
    list(APPEND qhull_TARGETS_INSTALL ${qhull_TARGETS_STATIC})
endif()

install(TARGETS ${qhull_TARGETS_INSTALL} EXPORT QhullTargets
        RUNTIME DESTINATION ${BIN_INSTALL_DIR}
        LIBRARY DESTINATION ${LIB_INSTALL_DIR}
        ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
        INCLUDES DESTINATION include)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/QhullExport/QhullConfigVersion.cmake"
    VERSION ${qhull_VERSION}
    COMPATIBILITY AnyNewerVersion
)

export(EXPORT QhullTargets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/QhullExport/QhullTargets.cmake"
  NAMESPACE Qhull:: 
)

configure_file(${PROJECT_SOURCE_DIR}/build/config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/QhullExport/QhullConfig.cmake"
  @ONLY
)

set(ConfigPackageLocation lib/cmake/Qhull)
install(EXPORT QhullTargets
  FILE
    QhullTargets.cmake
  NAMESPACE
    Qhull::
  DESTINATION
    ${ConfigPackageLocation}
)
install(
  FILES
    "${CMAKE_CURRENT_BINARY_DIR}/QhullExport/QhullConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/QhullExport/QhullConfigVersion.cmake"
  DESTINATION
    ${ConfigPackageLocation}
  COMPONENT
    Devel
)

set(PkgConfigLocation lib/pkgconfig)
foreach(pkgconfig IN ITEMS "${qhull_SHAREDR};Qhull reentrant shared library"
                           "${qhull_STATIC};Qhull static library"
                           "${qhull_STATICR};Qhull reentrant static library"
                           "${qhull_CPP};Qhull C++ library")
    list(GET pkgconfig 0 LIBRARY_NAME)
    if(LIBRARY_NAME STREQUAL "libqhull")
        set(LIBRARY_NAME "qhull")
    endif()
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        string(REGEX REPLACE "_(.)$" "_\\1d" LIBRARY_NAME ${LIBRARY_NAME})
        string(REGEX REPLACE "([^d])$" "\\1_d" LIBRARY_NAME ${LIBRARY_NAME})
    endif()
    list(GET pkgconfig 1 LIBRARY_DESCRIPTION)
    configure_file(build/qhull.pc.in ${LIBRARY_NAME}.pc @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}.pc DESTINATION ${PkgConfigLocation} COMPONENT Devel)
endforeach()

install(FILES ${libqhull_HEADERS}    DESTINATION ${INCLUDE_INSTALL_DIR}/libqhull)
install(FILES src/libqhull/DEPRECATED.txt DESTINATION ${INCLUDE_INSTALL_DIR}/libqhull)
install(FILES ${libqhullr_HEADERS}    DESTINATION ${INCLUDE_INSTALL_DIR}/libqhull_r)
install(FILES ${libqhullcpp_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/libqhullcpp)
install(FILES html/qhull.man         DESTINATION ${MAN_INSTALL_DIR} RENAME qhull.1)
install(FILES html/rbox.man          DESTINATION ${MAN_INSTALL_DIR} RENAME rbox.1)
install(FILES ${doc_FILES}           DESTINATION ${DOC_INSTALL_DIR})
install(DIRECTORY html/              DESTINATION ${DOC_INSTALL_DIR}/html)
install(FILES src/Changes.txt        DESTINATION ${DOC_INSTALL_DIR}/src)

add_custom_target(uninstall
    COMMENT "uninstall Qhull by deleting files in install_manifest.txt"
    COMMAND cat install_manifest.txt | tr -d \"\\r\" | xargs -r rm
)
