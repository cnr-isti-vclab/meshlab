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
#include "meshfilter.h"
#include <vcg/complex/local_optimization.h>
#include <vcg/complex/local_optimization/tri_edge_collapse_quadric.h>
#include <vcg/container/simple_temporary_data.h>
#include "quadric_simp.h"

using namespace vcg;
using namespace std;

void QuadricSimplification(CMeshO &m,int  TargetFaceNum, bool Selected, CallBackPos *cb)
{
  math::Quadric<double> QZero;
  QZero.SetZero();
  tri::QuadricTemp TD(m.vert,QZero);
  tri::QHelper::TDp()=&TD;

	// we assume that the caller has already set up the tri::MyTriEdgeCollapse::Params() class
	tri::TriEdgeCollapseQuadricParameter & pp = tri::MyTriEdgeCollapse::Params();
  
  if(Selected) // simplify only inside selected faces
  {
    // select only the vertices having ALL incident faces selected
    tri::UpdateSelection<CMeshO>::VertexFromFaceStrict(m);

    // Mark not writable un-selected vertices
    CMeshO::VertexIterator  vi;
    for(vi=m.vert.begin();vi!=m.vert.end();++vi) if(!(*vi).IsD())
          if(!(*vi).IsS()) (*vi).ClearW();
                      else (*vi).SetW();
  }

  if(pp.PreserveBoundary && !Selected) 
	{
    pp.FastPreserveBoundary=true;
		pp.PreserveBoundary = false;
	}
		
  if(pp.NormalCheck) pp.NormalThrRad = M_PI/4.0;
	
	
  vcg::LocalOptimization<CMeshO> DeciSession(m);
	cb(1,"Initializing simplification");
	DeciSession.Init<tri::MyTriEdgeCollapse >();

	if(Selected)
		TargetFaceNum= m.fn - (m.sfn-TargetFaceNum);
	DeciSession.SetTargetSimplices(TargetFaceNum);
	DeciSession.SetTimeBudget(0.1f); // this allow to update the progress bar 10 time for sec...
//  if(TargetError< numeric_limits<double>::max() ) DeciSession.SetTargetMetric(TargetError);
  //int startFn=m.fn;
  int faceToDel=m.fn-TargetFaceNum;
 while( DeciSession.DoOptimization() && m.fn>TargetFaceNum )
 {
   cb(100-100*(m.fn-TargetFaceNum)/(faceToDel), "Simplifying...");
 };

	DeciSession.Finalize<tri::MyTriEdgeCollapse >();
  
  if(Selected) // Clear Writable flags 
  {
    CMeshO::VertexIterator  vi;
    for(vi=m.vert.begin();vi!=m.vert.end();++vi) 
      if(!(*vi).IsD()) (*vi).SetW();
  }
}
