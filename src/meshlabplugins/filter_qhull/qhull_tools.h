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

#include <common/meshmodel.h>
#include <math.h>


#if defined(__cplusplus)
extern "C"
{
#endif
#include <stdio.h>
#include <stdlib.h>
#include "../../external/qhull-2003.1/src/qhull.h"
#include "../../external/qhull-2003.1/src/mem.h"
#include "../../external/qhull-2003.1/src/qset.h"
#include "../../external/qhull-2003.1/src/geom.h"
#include "../../external/qhull-2003.1/src/merge.h"
#include "../../external/qhull-2003.1/src/poly.h"
#include "../../external/qhull-2003.1/src/io.h"
#include "../../external/qhull-2003.1/src/stat.h"
#if defined(__cplusplus)
}
#endif

facetT *compute_convex_hull(int dim, int numpoints, MeshModel &m);
facetT *compute_delaunay(int dim, int numpoints, MeshModel &m);
bool compute_voronoi(int dim, int numpoints, MeshModel &m, MeshModel &pm,float threshold);
bool compute_alpha_shapes(int dim, int numpoints, MeshModel &m, MeshModel &pm,double alpha, bool alphashape);
int visible_points(int dim, int numpoints, MeshModel &m, MeshModel &pm,MeshModel &pm2, vcg::Point3f viewpointP,float threshold,bool convex_hullFP,bool triangVP);