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

namespace vcg
{
namespace tri
{


template<class FLT> class LaplacianAdjustInfo
{
public:
	Point3<FLT> sum;
	FLT dsp; // sum of angles between old and new normals of faces around a vertex
	FLT cnt;
};

// Classical Laplacian Smoothing. Each vertex can be moved onto the average of the adjacent vertices.
// Can smooth only the selected vertices and weight the smoothing according to the quality 
// In the latter case 0 means that the vertex is not moved and 1 means that the vertex is moved onto the computed position.

template<class MESH_TYPE>
void LaplacianAdjust(MESH_TYPE &m,
                     int step,
                     float threshold = 1.0,
                     bool SmoothSelected = false)
{
	SimpleTempData<typename MESH_TYPE::VertContainer,LaplacianAdjustInfo<typename MESH_TYPE::ScalarType> >
			TD(m.vert);
	LaplacianAdjustInfo<typename MESH_TYPE::ScalarType> lpz;
	lpz.sum = typename MESH_TYPE::CoordType(0,0,0);
	lpz.cnt = 1;
	lpz.dsp = 0;
	TD.Start(lpz);
	for (int i=0; i<step; ++i) {
		typename MESH_TYPE::VertexIterator vi;
		for (vi = m.vert.begin(); vi != m.vert.end(); ++vi)
			TD[*vi].sum = (*vi).P();
		
		typename MESH_TYPE::FaceIterator fi;
		for (fi = m.face.begin(); fi != m.face.end(); ++fi)
			if (!(*fi).IsD())
				for (int j = 0; j < 3; ++j)
					if (!(*fi).IsB(j)) {
						TD[(*fi).V(j)].sum += (*fi).V1(j)->P();
						TD[(*fi).V1(j)].sum += (*fi).V(j)->P();
						++TD[(*fi).V(j)].cnt;
						++TD[(*fi).V1(j)].cnt;
					}
		
		
		// si azzaera i dati per i vertici di bordo
		for (fi = m.face.begin(); fi != m.face.end(); ++fi)
			if (!(*fi).IsD())
				for (int j = 0; j <3; ++j)
					if ((*fi).IsB(j)) {
						//TD[(*fi).V(j)]=lpz;
						//TD[(*fi).V1(j)]=lpz;
						TD[(*fi).V0(j)].sum = (*fi).P0(j);
						TD[(*fi).V1(j)].sum = (*fi).P1(j);
						TD[(*fi).V0(j)].cnt = 1;
						TD[(*fi).V1(j)].cnt = 1;
					}
		
		
		// se l'edge j e' di bordo si deve mediare solo con gli adiacenti
		for (fi = m.face.begin(); fi != m.face.end(); ++fi)
			if (!(*fi).IsD())
				for (int j = 0; j < 3; ++j)
					if ((*fi).IsB(j)) {
						TD[(*fi).V(j)].sum += (*fi).V1(j)->P();
						TD[(*fi).V1(j)].sum += (*fi).V(j)->P();
						++TD[(*fi).V(j)].cnt;
						++TD[(*fi).V1(j)].cnt;
					}
		
		
		// calculate displacement sum from 
		for (fi = m.face.begin(); fi != m.face.end(); ++fi) {
			if (!(*fi).IsD()) {
				for (int j = 0; j < 3; ++j) {
					if (face::IsBorder((*fi), j)) {
						TD[(*fi).V(j)].dsp  = std::numeric_limits<typename MESH_TYPE::ScalarType>::max();
						TD[(*fi).V1(j)].dsp = std::numeric_limits<typename MESH_TYPE::ScalarType>::max();
					}
					else {
						Point3<typename MESH_TYPE::ScalarType> newPoint = TD[(*fi).V(j)].sum / (float) TD[(*fi).V(j)].cnt;
						Point3<typename MESH_TYPE::ScalarType> newNormal = ((newPoint - (*fi).P1(j)) ^ (newPoint - (*fi).P2(j))).Normalize();
						TD[(*fi).V(j)].dsp += Angle((*fi).N(), newNormal);
					}
				}
			}
		}

			/*if(QualityWeight>0)
			 { // quality weighted smoothing
			 // We assume that weights are in the 0..1 range.
			 assert(tri::HasPerVertexQuality(m));
			 for(vi=m.vert.begin();vi!=m.vert.end();++vi)
			 if(!(*vi).IsD() && TD[*vi].cnt>0 )
			 if(!SmoothSelected || (*vi).IsS())
			 {
			 float q=1.0-(*vi).Q();
			 (*vi).P()=(*vi).P()*(1.0-q) + (TD[*vi].sum/TD[*vi].cnt)*q;
			 }
			 }*/
		
		for (vi = m.vert.begin(); vi != m.vert.end(); ++vi)
			if (!(*vi).IsD() && TD[*vi].cnt>0)
				if (!SmoothSelected || (*vi).IsS())
					if (math::ToDeg(TD[*vi].dsp / (float) TD[*vi].cnt) < threshold)
						(*vi).P() = TD[*vi].sum / (float) TD[*vi].cnt;
	}
	
	TD.Stop();
}

};
};

#endif 
