/*
 * Connect3D - Reconstruction of water-tight triangulated meshes from unstructured point clouds
 *
 * please credit the following article: Stefan Ohrhallinger, Sudhir Mudur and Michael Wimmer:
 * 'Minimizing Edge Length to Connect Sparsely Sampled Unstructured Point Sets',
 * Shape Modeling International 2013, published in Computers & Graphics Journal, 2013.
 *
 * Copyright (C) 2013 Stefan Ohrhallinger, Daniel Prieler
 * This program is free software; you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#ifdef __cplusplus

#include <vector>
#include <string>
#include <array>
#include <iostream>
#include <ostream>
#include <fstream>
#include <memory>
#include <list>
#include <map>
#include <unordered_set>
#include <chrono>
#include <cstdint>


#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include <CGAL/Triangulation_vertex_base_with_info_3.h>
#include <CGAL/Triangulation_cell_base_with_info_3.h>


#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4100)
#endif

typedef int64_t ssize_t;
#include "btree_set.h"
#include "btree_map.h"
#ifdef __MSC_VER
#pragma warning(pop)
#endif

#include "Vector3D.h"


#endif
