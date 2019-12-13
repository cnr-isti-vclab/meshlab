# Copyright 2019 Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates then re-run ./make-cmake.py

set(SOURCES
    filter_screened_poisson.cpp Src/MarchingCubes.cpp Src/PlyFile.cpp
    # Src/CmdLineParser.cpp
    Src/Factor.cpp Src/Geometry.cpp)

set(HEADERS
    Src/Allocator.h
    Src/Array.h
    Src/BSplineData.h
    Src/BinaryNode.h
    Src/CmdLineParser.h
    Src/Factor.h
    Src/FunctionData.h
    Src/Geometry.h
    Src/Hash.h
    Src/MAT.h
    Src/MarchingCubes.h
    Src/MemoryUsage.h
    Src/MultiGridOctreeData.h
    Src/MyTime.h
    Src/Octree.h
    Src/PPolynomial.h
    Src/Ply.h
    Src/PlyFile.h
    Src/PointStream.h
    Src/Polynomial.h
    Src/SparseMatrix.h
    Src/Time.h
    Src/Vector.h
    filter_screened_poisson.h)

set(XML filter_screened_poisson.xml)
meshlab_install_plugin_xml(${CMAKE_CURRENT_SOURCE_DIR}/${XML} XML_OUT)
list(APPEND SOURCES ${XML_OUT})

add_library(filter_screened_poisson MODULE ${SOURCES} ${HEADERS})

target_compile_definitions(filter_screened_poisson PRIVATE BRUNO_LEVY_FIX
                           FOR_RELEASE)

if(OpenMP_CXX_FOUND)
    target_link_libraries(filter_screened_poisson PRIVATE OpenMP::OpenMP_CXX)
endif()

target_include_directories(filter_screened_poisson
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filter_screened_poisson PUBLIC common)

set_property(TARGET filter_screened_poisson PROPERTY FOLDER Plugins)

set_property(TARGET filter_screened_poisson
             PROPERTY RUNTIME_OUTPUT_DIRECTORY ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET filter_screened_poisson
             PROPERTY LIBRARY_OUTPUT_DIRECTORY ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(TARGETS filter_screened_poisson DESTINATION
                ${MESHLAB_PLUGIN_INSTALL_DIR} COMPONENT Plugins)
