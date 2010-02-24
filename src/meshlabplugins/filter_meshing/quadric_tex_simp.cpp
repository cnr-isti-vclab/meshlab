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

#include "meshfilter.h"   
#include "quadric_tex_simp.h"

using namespace vcg;
using namespace std;
using namespace vcg::tri;

void QuadricTexSimplification(CMeshO &m,int  TargetFaceNum, bool Selected, CallBackPos *cb)
{
	math::Quadric<double> QZero;
	QZero.SetZero();
	QuadricTemp TD3(m.vert,QZero);
	QuadricTexHelper::TDp3()=&TD3;

	QVector <QPair<vcg::TexCoord2<float>,Quadric5<double> > > qv;

	Quadric5Temp TD(m.vert,qv);
	QuadricTexHelper::TDp()=&TD;

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
	
	vcg::LocalOptimization<CMeshO> DeciSession(m);
	cb(1,"Initializing simplification");
	DeciSession.Init<MyTriEdgeCollapseQTex>();

	if(Selected)
		TargetFaceNum= m.fn - (m.sfn-TargetFaceNum);
	DeciSession.SetTargetSimplices(TargetFaceNum);
	DeciSession.SetTimeBudget(0.1f);
//	int startFn=m.fn;
  
	int faceToDel=m.fn-TargetFaceNum;
	
	while( DeciSession.DoOptimization() && m.fn>TargetFaceNum )
	{
    char buf[256];
    sprintf(buf,"Simplifing heap size %i ops %i\n",int(DeciSession.h.size()),DeciSession.nPerfmormedOps);
	   cb(100-100*(m.fn-TargetFaceNum)/(faceToDel), buf);
	};

	DeciSession.Finalize<MyTriEdgeCollapseQTex>();
	
	if(Selected) // Clear Writable flags 
  {
    CMeshO::VertexIterator  vi;
    for(vi=m.vert.begin();vi!=m.vert.end();++vi) 
      if(!(*vi).IsD()) (*vi).SetW();
  }
	

}
