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
/****************************************************************************
  History
$Log$
Revision 1.1  2006/10/10 21:13:08  cignoni
Added remove non manifold and quadric simplification filter.

****************************************************************************/
#include <QtGui>

#include <math.h>
#include <stdlib.h>

#include <limits>

#include "meshfilter.h"
#include <vcg/complex/trimesh/update/position.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/local_optimization.h>
#include <vcg/complex/local_optimization/tri_edge_collapse_quadric.h>
#include <vcg/container/simple_temporary_data.h>
using namespace vcg;
using namespace std;


typedef	SimpleTempData<typename CMeshO::VertContainer, math::Quadric<double> > QuadricTemp;


class QHelper
		{
		public:
			QHelper(){};
      static void Init(){};
      static math::Quadric<double> &Qd(CVertexO &v) {return TD()[v];}
      static math::Quadric<double> &Qd(CVertexO *v) {return TD()[*v];}
      static CVertexO::ScalarType W(CVertexO *v) {return 1.0;};
      static CVertexO::ScalarType W(CVertexO &v) {return 1.0;};
      static void Merge(CVertexO & /*v_dest*/, CVertexO const & /*v_del*/){};
      static QuadricTemp* &TDp() {static QuadricTemp *td; return td;}
      static QuadricTemp &TD() {return *TDp();}
		};


class MyTriEdgeCollapse: public vcg::tri::TriEdgeCollapseQuadric< CMeshO, MyTriEdgeCollapse, QHelper > {
						public:
						typedef  vcg::tri::TriEdgeCollapseQuadric< CMeshO,  MyTriEdgeCollapse, QHelper> TECQ;
            typedef  CMeshO::VertexType::EdgeType EdgeType;
            inline MyTriEdgeCollapse(  const EdgeType &p, int i) :TECQ(p,i){}
};


void QuadricSimplification(CMeshO &cm,float threshold)
{
  math::Quadric<double> QZero;
  QZero.Zero();
  QuadricTemp TD(cm.vert);
  QHelper::TDp()=&TD;

  TD.Start(QZero);
  tri::TriEdgeCollapseQuadricParameter qparams;
  MyTriEdgeCollapse::SetDefaultParams();
  qparams.QualityThr  =.3;
  int FinalSize=cm.fn/2;
  vcg::LocalOptimization<CMeshO> DeciSession(cm);
	
	int t1=clock();		
	DeciSession.Init<MyTriEdgeCollapse >();
  int t2=clock();	
  printf("Initial Heap Size %i\n",DeciSession.h.size());

	DeciSession.SetTargetSimplices(FinalSize);
	DeciSession.SetTimeBudget(0.5f);
//  if(TargetError< numeric_limits<double>::max() ) DeciSession.SetTargetMetric(TargetError);

 while( DeciSession.DoOptimization() && cm.fn>FinalSize );

 
}