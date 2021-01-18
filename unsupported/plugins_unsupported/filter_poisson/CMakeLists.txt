# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES filter_poisson.cpp src/Factor.cpp src/Geometry.cpp
            src/MarchingCubes.cpp src/MultiGridOctest.cpp)

set(HEADERS
    filter_poisson.h
    src/Allocator.h
    src/BinaryNode.h
    src/Factor.h
    src/FunctionData.h
    src/Geometry.h
    src/Hash.h
    src/MarchingCubes.h
    src/MultiGridOctreeData.h
    src/Octree.h
    src/PPolynomial.h
    src/PoissonParam.h
    src/Polynomial.h
    src/SparseMatrix.h
    src/Vector.h)

add_library(filter_poisson MODULE ${SOURCES} ${HEADERS})

target_include_directories(filter_poisson PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filter_poisson PUBLIC common)

set_property(TARGET filter_poisson PROPERTY FOLDER Plugins)

set_property(TARGET filter_poisson PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                            ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET filter_poisson PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                            ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS filter_poisson
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
