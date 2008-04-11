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

#ifndef __LAPLACIANADJUST_H
#define __LAPLACIANADJUST_H

#include <vcg/container/simple_temporary_data.h>
#include <vcg/space/point3.h>
#include <vcg/simplex/face/topology.h>
#include <vcg/complex/trimesh/update/flag.h>

#include <vcg/space/plane3.h>

namespace vcg
{
namespace tri
{

template<class FLT> class LaplacianAdjustInfo
{
public:
	Point3<FLT> sum;
	FLT dsp; // sum of angles between old and new face normals around a vertex
	FLT cnt;
};

// Laplacian Smoothing that praserve surface shape. Each vertex can be moved
// onto the average of the adjacent vertices, only if still (almost) lies on 
// the original surface. Smooth only selected vertices, if selection = true

template<class MESH_TYPE>void LaplacianAdjust(MESH_TYPE &m, int step,
                                               float thr = 1.0,
                                               bool selection = false)
{
	typedef typename MESH_TYPE::VertContainer VertContainer;
	typedef LaplacianAdjustInfo<typename MESH_TYPE::ScalarType> LInfo;
	
	SimpleTempData<VertContainer, LInfo > TD(m.vert);
	LInfo lpz;
	lpz.sum = typename MESH_TYPE::CoordType(0,0,0);
	lpz.cnt = 1;
	lpz.dsp = 0;
	TD.Start(lpz);

	// to preserve surface shape, we do not move border vertices - mark them
	//vcg::tri::UpdateFlags<MESH_TYPE>::VertexBorderFromFace(m);

	for (int i = 0; i < step; ++i) {
		typename MESH_TYPE::VertexIterator vi;
		for (vi = m.vert.begin(); vi != m.vert.end(); ++vi)
			TD[*vi].sum = (*vi).P();

		typename MESH_TYPE::FaceIterator fi;
		for (fi = m.face.begin(); fi != m.face.end(); ++fi)
			if (!(*fi).IsD() && (!selection || (*fi).IsS()))
				for (int j = 0; j < 3; ++j)
					if (!(*fi).IsB(j)) {
						TD[(*fi).V(j)].sum += (*fi).V1(j)->P();
						TD[(*fi).V1(j)].sum += (*fi).V(j)->P();
						++TD[(*fi).V(j)].cnt;
						++TD[(*fi).V1(j)].cnt;
					} else {
						TD[(*fi).V0(j)].cnt = 1;
						TD[(*fi).V1(j)].cnt = 1;
					}

		// compute sum of normal diplacement from old to new faces  
		for (fi = m.face.begin(); fi != m.face.end(); ++fi) {
			if (!(*fi).IsD() && (!selection || (*fi).IsS())) {
				for (int j = 0; j < 3; ++j) {
					typename MESH_TYPE::CoordType newPoint = 
						TD[(*fi).V(j)].sum / (float) TD[(*fi).V(j)].cnt;
					typename MESH_TYPE::CoordType newNormal = 
						(newPoint - (*fi).P1(j)) ^ (newPoint - (*fi).P2(j));
					TD[(*fi).V(j)].dsp += Angle((*fi).N(), newNormal);
				}
			}
		}

		/*if(QualityWeight>0)
		 { // quality weighted smoothing
		 // We assume that weights are in the 0..1 range.
		 assert(tri::HasPerVertexQuality(m));
		 for(vi=m.vert.begin();vi!=m.vert.end();++vi)
		 if(!(*vi).IsD() && TD[*vi].cnt>0 )
		 if(!selection || (*vi).IsS())
		 {
		 float q=1.0-(*vi).Q();
		 (*vi).P()=(*vi).P()*(1.0-q) + (TD[*vi].sum/TD[*vi].cnt)*q;
		 }
		 }*/

		for (vi = m.vert.begin(); vi != m.vert.end(); ++vi)
			if (!(*vi).IsD() && TD[*vi].cnt>0)
				if (!selection || (*vi).IsS())
					if (math::ToDeg(TD[*vi].dsp / (float) TD[*vi].cnt) < thr)
						(*vi).P() = TD[*vi].sum / (float) TD[*vi].cnt;
	}

	TD.Stop();
}

} // end tri namespace
} // end vcg namespace

#endif 
