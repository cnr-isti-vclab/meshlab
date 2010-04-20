/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2009                                          \/)\/    *
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

#ifndef SLICE_FUNCTORS
#define SLICE_FUNCTORS
#include <vcg/space/intersection3.h>
#include <vcg/math/matrix44.h>


using namespace vcg;

enum { VERTEX_LEFT, VERTEX_RIGHT, VERTEX_SLICE };

template <class MESH_TYPE>
class SlicedEdge
{
public:
  SlicedEdge(const Plane3f &_p)
	{
	  p=_p;
  }
	bool operator()(face::Pos<typename MESH_TYPE::FaceType> ep)
	{
	  if (Distance(ep.V()->P(),p)<0) 
			ep.V()->Q()=VERTEX_LEFT;
		else if (Distance(ep.V()->P(),p)>0)
			ep.V()->Q()=VERTEX_RIGHT;
		else
			ep.V()->Q()=VERTEX_SLICE;

    if (Distance(ep.VFlip()->P(),p)<0)  
			ep.VFlip()->Q()=VERTEX_LEFT;
		else if (Distance(ep.VFlip()->P(),p)>0)  
			ep.VFlip()->Q()=VERTEX_RIGHT;
		else
			ep.VFlip()->Q()=VERTEX_SLICE;

    return (ep.V()->Q() != ep.VFlip()->Q()) && (ep.V()->Q()!=VERTEX_SLICE) && (ep.VFlip()->Q()!=VERTEX_SLICE);
  }

protected:
  Plane3f p;
};


template<class MESH_TYPE>
struct SlicingFunction : public std::unary_function<face::Pos<typename MESH_TYPE::FaceType> , typename MESH_TYPE::CoordType >
{
public :
  SlicingFunction(const Plane3f& _p)
	{
	  p=_p;
	}

	void operator()(typename MESH_TYPE::VertexType &nv, face::Pos<typename MESH_TYPE::FaceType> ep)
	{
		Segment3f seg(ep.V()->P(),ep.VFlip()->P());
	  Point3f pp;
    IntersectionPlaneSegment<typename MESH_TYPE::ScalarType>(p,seg,pp);
    nv.P()=pp;
    nv.Q()=VERTEX_SLICE;
	}

	// raw calculation for wedgeinterp
	Color4<typename MESH_TYPE::ScalarType> WedgeInterp(Color4<typename MESH_TYPE::ScalarType> &c0, Color4<typename MESH_TYPE::ScalarType> &c1)
	{
		Color4<CMeshO::ScalarType> cc;
		return cc.lerp(c0,c1,0.5f);
	}

	template<class FL_TYPE>
	TexCoord2<FL_TYPE,1> WedgeInterp(TexCoord2<FL_TYPE,1> &t0, TexCoord2<FL_TYPE,1> &t1)
	{
		TexCoord2<FL_TYPE,1> tmp;
		assert(t0.n()== t1.n());
		tmp.n()=t0.n();
		tmp.t()=(t0.t()+t1.t())/2.0;
		return tmp;
	}
protected:
  Plane3f p;
};


#endif
