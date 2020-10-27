#ifndef _OCME_IMPOSTOR_CREATE_H
#define _OCME_IMPOSTOR_CREATE_H

#include "impostor_definition.h"
#include <vcg/space/plane3.h>
#include <vcg/space/fitting3.h>



template <class MeshType>
void  Impostor::AddSamplesFromFace(MeshType & m){
	}

template <class MeshType>
void  Impostor::AddSamplesFromVertex(MeshType & m){
//
//	// stupid first approximation, just take the barycenters of all the faces
//	typename MeshType::VertexIterator vi;
//	for(vi = m.vert.begin(); vi != m.vert.end(); ++vi)if(!(*vi).IsD())
//	{
//		const vcg::Point3f &  p = (*vi).P();
//			if(box.IsIn(p)){
//                                const int ii = std::min<int>( int((p[0]-box.min[0])/cellsize),Gridsize()-1);
//                                const int jj = std::min<int>( int((p[1]-box.min[1])/cellsize),Gridsize()-1);
//                                const int kk = std::min<int>( int((p[2]-box.min[2])/cellsize),Gridsize()-1);
//
//				samples[ii][jj][kk].push_back(p);
//
//#ifdef _RASTER_MASK_
//				/* TMP */
//				int K = MaskType::K();
//				const int im = std::min<int>( int((p[0]-box.min[0]-ii*cellsize)/(cellsize/K)),K-1);
//				const int jm = std::min<int>( int((p[1]-box.min[1]-jj*cellsize)/(cellsize/K)),K-1);
//				const int km = std::min<int>( int((p[2]-box.min[2]-kk*cellsize)/(cellsize/K)),K-1);
//				if(!masks[ii][jj][kk]){
//					masks[ii][jj][kk] = new MaskType();
//				}
//
//				if((*masks[ii][jj][kk])[im][jm][km] < 255 ) (*masks[ii][jj][kk])[im][jm][km]++;
//#endif
//			}
//		}
}


#endif
