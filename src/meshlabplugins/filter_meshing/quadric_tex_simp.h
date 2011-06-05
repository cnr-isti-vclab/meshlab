/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2009                                                \/)\/    *
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
#ifndef __QUADRICTEXSIMP_H
#define __QUADRICTEXSIMP_H

#include <vcg/complex/algorithms/local_optimization/tri_edge_collapse_quadric_tex.h>

namespace vcg
{
namespace tri
{

typedef BasicVertexPair<CVertexO> VertexPair;

class MyTriEdgeCollapseQTex: public TriEdgeCollapseQuadricTex< CMeshO, VertexPair, MyTriEdgeCollapseQTex, QuadricTexHelper<CMeshO> > {
						public:
            typedef  TriEdgeCollapseQuadricTex< CMeshO,  VertexPair, MyTriEdgeCollapseQTex, QuadricTexHelper<CMeshO> > TECQ;
            inline MyTriEdgeCollapseQTex(  const VertexPair &p, int i,BaseParameterClass *pp) :TECQ(p,i,pp){}
};

} // end namespace tri
} // end namespace vcg

#endif
