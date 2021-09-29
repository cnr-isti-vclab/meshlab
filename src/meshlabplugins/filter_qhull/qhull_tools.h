/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/

//--- Include qhull, so it works from with in a C++ source file
//---
//--- In MVC one cannot just do:
//---
//---    extern "C"
//---    {
//---      #include "qhull_a.h"
//---    }
//---
//--- Because qhull_a.h includes math.h, which can not appear
//--- inside a extern "C" declaration.
//---
//--- Maybe that why Numerical recipes in C avoid this problem, by removing
//--- standard include headers from its header files and add them in the
//--- respective source files instead.
//---
//--- [K. Erleben]

/****************************************************************************
  History


****************************************************************************/

#include <common/ml_document/mesh_model.h>

#include <libqhull_r/libqhull_r.h>
#include <libqhull_r/geom_r.h>
#include <libqhull_r/io_r.h>
#include <libqhull_r/merge_r.h>

facetT *compute_convex_hull(qhT* qh, int dim, int numpoints, MeshModel &m);
bool compute_delaunay(qhT* qh, int dim, int numpoints, MeshModel &m);
bool compute_voronoi(qhT* qh, int dim, int numpoints, MeshModel &m, MeshModel &pm, Scalarm threshold);
bool compute_alpha_shapes(qhT* qh, int dim, int numpoints, MeshModel &m, MeshModel &pm,double alpha, bool alphashape);
int visible_points(qhT* qh, int dim, int numpoints, MeshModel &m, MeshModel &pm,MeshModel &pm2, Point3m viewpointP,float threshold,bool convex_hullFP,bool triangVP);
