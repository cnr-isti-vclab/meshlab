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


#ifndef VORONOI_PROCESSING_H
#define VORONOI_PROCESSING_H

namespace vcg
{

template <class MeshType>
class ClusteringSampler
	{
	public:
		typedef typename MeshType::VertexType			VertexType;
		
		ClusteringSampler()
		{
			sampleVec=0;
		};
		ClusteringSampler(std::vector<VertexType *> *_vec)
		{
			sampleVec = _vec;
		}
		std::vector<VertexType *> *sampleVec;
		
		void AddVert(const VertexType &p) 
		{
			sampleVec->push_back((VertexType *)(&p));
		}
	}; // end class ClusteringSampler



template <class MeshType >
class VoronoiProcessing
{
	  typedef typename MeshType::CoordType				CoordType;
    typedef typename MeshType::ScalarType				ScalarType;
		typedef typename MeshType::VertexType				VertexType;
    typedef typename MeshType::VertexPointer		VertexPointer;
    typedef typename MeshType::VertexIterator		VertexIterator;
    typedef typename MeshType::FacePointer			FacePointer;
    typedef typename MeshType::FaceIterator			FaceIterator;
    typedef typename MeshType::FaceType					FaceType;
    typedef typename MeshType::FaceContainer		FaceContainer;
	public:

// Base vertex voronoi coloring algorithm.
// it assumes VF adjacency. 
static void VertexColoring(MeshType &m, int seedNum, std::vector<VertexType *> &seedVec)
{
			ClusteringSampler<MeshType> vc(&seedVec);
			tri::SurfaceSampling<MeshType, ClusteringSampler<MeshType> >::VertexUniform(m,vc,seedNum);
			
			std::queue<VertexPointer> VQ;
			
			tri::UpdateQuality<MeshType>::VertexConstant(m,0);
			
			for(int i=0;i<seedNum;++i)
			{
				VQ.push(seedVec[i]);
				seedVec[i]->Q()=i+1;
			}
			
			while(!VQ.empty())
			{
				VertexPointer vp = VQ.front();
				VQ.pop();
				
				std::vector<VertexPointer> vertStar;
				
				face::VFIterator<FaceType> vfi(vp);
				while(!vfi.End())
				{
					vertStar.push_back(vfi.F()->V1(vfi.I()));
					vertStar.push_back(vfi.F()->V2(vfi.I()));
					++vfi;
				}
				
				std::sort(vertStar.begin(),vertStar.end());
				typename std::vector<VertexPointer>::iterator new_end = std::unique(vertStar.begin(),vertStar.end());
				for(typename std::vector<VertexPointer>::iterator vv = vertStar.begin();vv!=new_end;++vv)
				{
					if((*vv)->Q()==0) 
					{
						(*vv)->Q()=vp->Q();
						VQ.push(*vv);
					}
				}
				}

	}

	// This function assumes that in the mOld mesh,  for each vertex you have a quality that denotes the index of the cluster
	// mNew is created by collasping onto a single vertex all the vertices that lies in the same cluster. 
	// Non degenerate triangles are preserved.
	
static void VoronoiClustering(MeshType &mOld, MeshType &mNew, std::vector<VertexType *> &seedVec)			
{
	std::set<Point3i> clusteredFace;

	FaceIterator fi;
	for(fi=mOld.face.begin();fi!=mOld.face.end();++fi)
	{
		if( (fi->V(0)->Q() != fi->V(1)->Q() ) && 
				(fi->V(0)->Q() != fi->V(2)->Q() ) &&
				(fi->V(1)->Q() != fi->V(2)->Q() )  )
				clusteredFace.insert( Point3i(int(fi->V(0)->Q()), int(fi->V(1)->Q()), int(fi->V(2)->Q())));																	 
	}								 

	tri::Allocator<MeshType>::AddVertices(mNew,seedVec.size());
	for(uint i=0;i< seedVec.size();++i)
	mNew.vert[i].ImportLocal(*(seedVec[i]));																										

	tri::Allocator<MeshType>::AddFaces(mNew,clusteredFace.size());
	std::set<Point3i>::iterator fsi; ;

	for(fi=mNew.face.begin(),fsi=clusteredFace.begin(); fsi!=clusteredFace.end();++fsi,++fi)
	{
		(*fi).V(0) = & mNew.vert[(int)(fsi->V(0)-1)];
		(*fi).V(1) = & mNew.vert[(int)(fsi->V(1)-1)];
		(*fi).V(2) = & mNew.vert[(int)(fsi->V(2)-1)];
	}
}

};

} // end namespace vcg
#endif
