# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/CMakeLists.template.cmake (shared with all other directories),
### or create a derived template in src/templates/filter_screened_poisson_xml.cmake,
### then re-run ./make-cmake.py

set(SOURCES
    Src/CmdLineParser.cpp
    Src/Factor.cpp
    Src/Geometry.cpp
    Src/MarchingCubes.cpp
    Src/PoissonRecon.cpp
    Src/SSDRecon.cpp
    Src/SurfaceTrimmer.cpp
    Src/Time.cpp
    filter_screened_poisson.cpp)

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
    Src/PlyVertexMini.h
    Src/PointStream.h
    Src/Polynomial.h
    Src/SparseMatrix.h
    Src/Time.h
    Src/Vector.h
    filter_screened_poisson.h)

add_library(filter_screened_poisson_xml MODULE ${SOURCES} ${HEADERS})

target_include_directories(filter_screened_poisson_xml
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filter_screened_poisson_xml PUBLIC common)

set_property(TARGET filter_screened_poisson_xml PROPERTY FOLDER Plugins)

set_property(TARGET filter_screened_poisson_xml
             PROPERTY RUNTIME_OUTPUT_DIRECTORY ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET filter_screened_poisson_xml
             PROPERTY LIBRARY_OUTPUT_DIRECTORY ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS filter_screened_poisson_xml
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
