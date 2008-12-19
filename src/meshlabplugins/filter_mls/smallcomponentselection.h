/****************************************************************************
* VCGLib                                                            o o     *
* Visual and Computer Graphics Library                            o     o   *
*                                                                _   O  _   *
* Copyright(C) 2004                                                \/)\/    *
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

#ifndef SMALL_COMPONENT_SELECTION_H
#define SMALL_COMPONENT_SELECTION_H

namespace vcg {
namespace tri {

template<class _MeshType>
class SmallComponent
{

public:
	typedef _MeshType MeshType;
	typedef typename MeshType::VertexType     VertexType;
	typedef typename MeshType::VertexPointer  VertexPointer;
	typedef typename MeshType::VertexIterator VertexIterator;
	typedef typename MeshType::FaceType       FaceType;
	typedef typename MeshType::FacePointer    FacePointer;
	typedef typename MeshType::FaceIterator   FaceIterator;

	static int Select(MeshType &m, float nbFaceRatio = 0.1, bool nonClosedOnly = false)
	{
		assert(tri::HasFFAdjacency(m) && "The small component selection procedure requires face to face adjacency.");

		// the different components as a list of face pointer
		std::vector< std::vector<FacePointer> > components;

		for(uint faceSeed = 0; faceSeed<m.face.size(); )
		{
			// find the first not selected face
			bool foundSeed = false;
			while (faceSeed<m.face.size())
			{
				CFaceO& f = m.face[faceSeed];
				if (!f.IsS())
				{
					if (nonClosedOnly)
					{
						for (int k=0; k<3; ++k)
							if (f.IsB(k))
							{
								foundSeed = true;
								break;
							}
					}
					else
						foundSeed = true;
					if (foundSeed)
						break;
				}
				++faceSeed;
			}
			if (!foundSeed) // no more seed, stop
				break;

			// expand the region from this face...
			components.resize(components.size()+1);
			std::vector<FacePointer> activefaces;
			activefaces.push_back(&m.face[faceSeed]);
			while (!activefaces.empty())
			{
				FacePointer f = activefaces.back();
				activefaces.pop_back();
				if (f->IsS())
					continue;

				f->SetS();
				components.back().push_back(f);
				for (int k=0; k<3; ++k)
				{
					if (f->IsB(k))
						continue;
					FacePointer of = f->FFp(k);
					if (!of->IsS())
						activefaces.push_back(of);
				}
			}
			++faceSeed;
		}
		UpdateSelection<MeshType>::ClearFace(m);

		// now the segmentation is done, let's compute the absolute face count threshold
		int total_selected = 0;
		int maxComponent = 0;
		for (uint i=0; i<components.size(); ++i)
		{
			//std::cout << "Component " << i << " -> " << components[i].size() << "\n";
			total_selected += components[i].size();
			maxComponent = std::max<int>(maxComponent,components[i].size());
		}
		int remaining = m.face.size() - total_selected;
		uint th = std::max(maxComponent,remaining) * nbFaceRatio;

		int selCount = 0;
		for (uint i=0; i<components.size(); ++i)
		{
			if (components[i].size()<th)
			{
				selCount += components[i].size();
				for (uint j=0; j<components[i].size(); ++j)
					components[i][j]->SetS();
			}
		}
		return selCount;
	}

	static void DeleteFaceVert(MeshType &m)
	{
		typename MeshType::FaceIterator fi;
		typename MeshType::VertexIterator vi;
		UpdateSelection<MeshType>::ClearVertex(m);
		UpdateSelection<MeshType>::VertexFromFaceStrict(m);
		
		for(fi=m.face.begin();fi!=m.face.end();++fi)
			if (!(*fi).IsD() && (*fi).IsS() )
				tri::Allocator<CMeshO>::DeleteFace(m,*fi);
		for(vi=m.vert.begin();vi!=m.vert.end();++vi)
			if (!(*vi).IsD() && (*vi).IsS() )
				tri::Allocator<CMeshO>::DeleteVertex(m,*vi);
	}

}; // end class

}	// End namespace
}	// End namespace


#endif
