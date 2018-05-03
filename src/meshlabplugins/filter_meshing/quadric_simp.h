/****************************************************************************
 * MeshLab                                                           o o     *
 * A versatile mesh processing toolbox                             o     o   *
 *                                                                _   O  _   *
 * Copyright(C) 2005                                                \/)\/    *
 * Visual Computing Lab                                            /\/|      *
 * ISTI - Italian National Research Council                           |      *
 *                                                                    \      *
 * All rights reserved.																											 *
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
#include <vcg/container/simple_temporary_data.h>
#include <vcg/complex/algorithms/local_optimization.h>
#include <vcg/complex/algorithms/local_optimization/tri_edge_collapse_quadric.h>
#include <vcg/complex/algorithms/local_optimization/tri_edge_collapse_quadric_tex.h>

namespace vcg {
namespace tri {

typedef	SimpleTempData<CMeshO::VertContainer, math::Quadric<double> > QuadricTemp;


class QHelper
{
public:
  QHelper(){}
  static void Init(){}
  static math::Quadric<double> &Qd(CVertexO &v) {return TD()[v];}
  static math::Quadric<double> &Qd(CVertexO *v) {return TD()[*v];}
  static CVertexO::ScalarType W(CVertexO * /*v*/) {return 1.0;}
  static CVertexO::ScalarType W(CVertexO & /*v*/) {return 1.0;}
  static void Merge(CVertexO & /*v_dest*/, CVertexO const & /*v_del*/){}
  static QuadricTemp* &TDp() {static QuadricTemp *td; return td;}
  static QuadricTemp &TD() {return *TDp();}
};

typedef BasicVertexPair<CVertexO> VertexPair;

class MyTriEdgeCollapse: public vcg::tri::TriEdgeCollapseQuadric< CMeshO, VertexPair , MyTriEdgeCollapse, QHelper > {
public:
  typedef  vcg::tri::TriEdgeCollapseQuadric< CMeshO, VertexPair,  MyTriEdgeCollapse, QHelper> TECQ;
  inline MyTriEdgeCollapse(  const VertexPair &p, int i, BaseParameterClass *pp) :TECQ(p,i,pp){}
};

class MyTriEdgeCollapseQTex: public TriEdgeCollapseQuadricTex< CMeshO, VertexPair, MyTriEdgeCollapseQTex, QuadricTexHelper<CMeshO> > {
public:
            typedef  TriEdgeCollapseQuadricTex< CMeshO,  VertexPair, MyTriEdgeCollapseQTex, QuadricTexHelper<CMeshO> > TECQ;
            inline MyTriEdgeCollapseQTex(  const VertexPair &p, int i,BaseParameterClass *pp) :TECQ(p,i,pp){}
};

} // end namespace tri
} // end namepsace vcg
void QuadricSimplification   (CMeshO &m,int  TargetFaceNum,    bool Selected, vcg::tri::TriEdgeCollapseQuadricParameter &pp,    vcg::CallBackPos *cb);
void QuadricTexSimplification(CMeshO &m,int  TargetFaceNum,    bool Selected, vcg::tri::TriEdgeCollapseQuadricTexParameter &pp, vcg::CallBackPos *cb);

