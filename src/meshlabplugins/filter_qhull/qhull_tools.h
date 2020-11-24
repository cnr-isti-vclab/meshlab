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
#include <math.h>


extern "C"
{
#include <stdio.h>
#include <stdlib.h>

// qhull includes - qhull/ omitted to support both system and bundled libs.
#ifdef SYSTEM_QHULL
#include "libqhull.h"
#else
#include "qhull.h"
#include "qset.h"
#endif
#include "mem.h"
#include "geom.h"
#include "merge.h"
#include "poly.h"
#include "io.h"
#include "stat.h"
}

facetT *compute_convex_hull(int dim, int numpoints, MeshModel &m);
facetT *compute_delaunay(int dim, int numpoints, MeshModel &m);
bool compute_voronoi(int dim, int numpoints, MeshModel &m, MeshModel &pm,float threshold);
bool compute_alpha_shapes(int dim, int numpoints, MeshModel &m, MeshModel &pm,double alpha, bool alphashape);
int visible_points(int dim, int numpoints, MeshModel &m, MeshModel &pm,MeshModel &pm2, Point3m viewpointP,float threshold,bool convex_hullFP,bool triangVP);
