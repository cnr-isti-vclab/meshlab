# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES
	filter_screened_poisson.cpp Src/MarchingCubes.cpp
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
	Src/PlyVertexMini.h
	Src/PointStream.h
	Src/Polynomial.h
	Src/SparseMatrix.h
	Src/Time.h
	Src/Vector.h
	filter_screened_poisson.h
	poisson_utils.h)

set(INL_HEADERS
	Src/Array.inl
	Src/BSplineData.inl
	Src/CmdLineParser.inl
	Src/FunctionData.inl
	Src/Geometry.inl
	Src/MAT.inl
	Src/MultiGridOctreeData.Evaluation.inl
	Src/MultiGridOctreeData.inl
	Src/MultiGridOctreeData.IsoSurface.inl
	Src/MultiGridOctreeData.SortedTreeNodes.inl
	Src/MultiGridOctreeData.System.inl
	Src/MultiGridOctreeData.WeightedSamples.inl
	Src/Octree.inl
	Src/PointStream.inl
	Src/Polynomial.inl
	Src/PPolynomial.inl
	Src/SparseMatrix.inl
	Src/Vector.inl)

add_meshlab_plugin(filter_screened_poisson ${SOURCES} ${HEADERS} ${INL_HEADERS})

target_compile_definitions(filter_screened_poisson PRIVATE BRUNO_LEVY_FIX
														   FOR_RELEASE)

if(OpenMP_CXX_FOUND)
	target_link_libraries(filter_screened_poisson PRIVATE OpenMP::OpenMP_CXX)
endif()
