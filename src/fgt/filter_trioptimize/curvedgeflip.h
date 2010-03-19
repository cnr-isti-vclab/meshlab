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

#ifndef __CURVEDGEFLIP
#define __CURVEDGEFLIP

#include <vcg/container/simple_temporary_data.h>
#include <vcg/complex/local_optimization/tri_edge_flip.h>
#include <vcg/space/triangle3.h>
#include <vcg/space/point3.h>

#include <vcg/complex/trimesh/update/normal.h>

#include "curvdata.h"

namespace vcg
{
namespace tri
{

/* 
 * This flip happens only if decreases the curvature of the surface
 * Edge flip optimization based on the paper
 * "optimizing 3d triangulations using discrete curvature analysis"
 * http://www2.in.tu-clausthal.de/~hormann/papers/Dyn.2001.OTU.pdf
 */
template <class TRIMESH_TYPE, class MYTYPE, class CURVEVAL>
class CurvEdgeFlip : public TopoEdgeFlip<TRIMESH_TYPE, MYTYPE>
{
protected:
	typedef typename TRIMESH_TYPE::FaceType FaceType;
	typedef typename TRIMESH_TYPE::FacePointer FacePointer;
	typedef typename TRIMESH_TYPE::FaceIterator FaceIterator;
	typedef typename TRIMESH_TYPE::VertexIterator VertexIterator;
	typedef typename TRIMESH_TYPE::VertexType VertexType;
	typedef typename TRIMESH_TYPE::ScalarType ScalarType;
	typedef typename TRIMESH_TYPE::VertexPointer VertexPointer;
	typedef typename TRIMESH_TYPE::CoordType CoordType;
	typedef vcg::face::Pos<FaceType> PosType;
	typedef vcg::face::VFIterator<FaceType> VFIteratorType;
	typedef typename LocalOptimization<TRIMESH_TYPE>::HeapElem HeapElem;
	typedef typename LocalOptimization<TRIMESH_TYPE>::HeapType HeapType;
	typedef TriEdgeFlip<TRIMESH_TYPE, MYTYPE> Parent;
	typedef typename vcg::Triangle3<ScalarType> TriangleType;

	typedef typename TRIMESH_TYPE::VertContainer VertContainer;
	
	// New curvature precomputed for the vertexes of the faces 
	// adjacent to edge to be flipped
	ScalarType _cv0, _cv1, _cv2, _cv3;
	
	
	static CurvData FaceCurv(VertexPointer v0,
	                      VertexPointer v1,
	                      VertexPointer v2,
	                      CoordType fNormal)
	{
		CurvData res;
		
		float ang0 = math::Abs(Angle(v1->P() - v0->P(), v2->P() - v0->P()));
		float ang1 = math::Abs(Angle(v0->P() - v1->P(), v2->P() - v1->P()));
		float ang2 = M_PI - ang0 - ang1;
		
		float s01 = SquaredDistance(v1->P(), v0->P());
		float s02 = SquaredDistance(v2->P(), v0->P());
		
		// voronoi cell of vertex
		if (ang0 >= M_PI/2) {
			TriangleType triangle(v0->P(), v1->P(), v2->P());
			res.A += (0.5f * DoubleArea(triangle) -
					(s01 * tan(ang1) + s02 * tan(ang2)) / 8.0 );
		} else if (ang1 >= M_PI/2)
			res.A += (s01 * tan(ang0)) / 8.0;
		else if (ang2 >= M_PI/2)
			res.A += (s02 * tan(ang0) / 8.0);
		else // non obctuse triangle
			res.A += ((s02 / tan(ang1)) + (s01 / tan(ang2))) / 8.0;
		
		res.K += ang0;
		
		ang1 = math::Abs(Angle(fNormal, v1->N()));
		ang2 = math::Abs(Angle(fNormal, v2->N()));
		res.H += ( (math::Sqrt(s01) / 2.0) * ang1 + 
				   (math::Sqrt(s02) / 2.0) * ang2 );
		
		return res;
	}
	
	/* Compute vertex curvature walking around a verte with VF adjacency
	 * f1, f2 --> this faces are to be ignored
	 * */
	static CurvData Curvature(VertexPointer v, FacePointer f1 = NULL, FacePointer f2 = NULL)
	{		
		CurvData curv;
		VFIteratorType vfi(v);
		
		while (!vfi.End()) {
			if (vfi.F() != f1 && vfi.F() != f2 && !vfi.F()->IsD()) {
				int i = vfi.I();
				curv += FaceCurv(vfi.F()->V0(i),
				                 vfi.F()->V1(i),
				                 vfi.F()->V2(i),
				                 vfi.F()->N());
			}
			++vfi;
		}
		
		return curv;
	}
	
public:
	CurvEdgeFlip() {}
	
	CurvEdgeFlip(PosType pos, int mark)
	{
		this->_pos = pos;
		this->_localMark = mark;
		this->_priority = ComputePriority();
	}
	
	
	CurvEdgeFlip(CurvEdgeFlip &par)
	{
		this->_pos = par.GetPos();
		this->_localMark = par.GetMark();
		this->_priority = par.Priority();
	}
	
	// temporary empty (flip is already done in constructor)
	void Execute(TRIMESH_TYPE& m)
	{
		int i = this->_pos.E();
		FacePointer f1 = this->_pos.F();
		int j = f1->FFi(i);
		FacePointer f2 = f1->FFp(i);
		VertexPointer v0 = f1->V0(i), v1 = f1->V1(i),
		              v2 = f1->V2(i), v3 = f2->V2(j);
		
		// save precomputed curvature in vertex quality
		v0->Q() = _cv0;
		v1->Q() = _cv1;
		v2->Q() = _cv2;
		v3->Q() = _cv3;
		
		CoordType n1 = Normal(v0->P(), v3->P(), v2->P());
		CoordType n2 = Normal(v1->P(), v2->P(), v3->P());
		
		// compute and update normals on vertices after flip
		v0->N() = v0->N() - f1->N() - f2->N() + n1;
		v1->N() = v1->N() - f1->N() - f2->N() + n2;
		v2->N() = v2->N() - f1->N() + n1 + n2;
		v3->N() = v3->N() - f2->N() + n1 + n2;
		
		// fix VF adjacency - detach
		assert(f1->V1(i) == v1);
		vcg::face::VFDetach(*f1, (i + 1) % 3); // detach v1 from f1
		assert(f2->V1(j) == v0);
		vcg::face::VFDetach(*f2, (j + 1) % 3); // detach v0 from f2

		// do the flip
		vcg::face::FlipEdge(*this->_pos.F(), this->_pos.E());

		// fix VF adjacency - append
		assert(f2->V1(j) == v2);
		vcg::face::VFAppend(f2, (j + 1) % 3); // attach v2 with f2
		assert(f1->V1(i) == v3);
		vcg::face::VFAppend(f1, (i + 1) % 3); // attach v3 with f1
		
		// update face normals after the flip
		f1->N() = n1;
		f2->N() = n2;

		// avoid texture coordinates swap after flip 
		if (tri::HasPerWedgeTexCoord(m)) {
			f2->WT((j + 1) % 3) = f1->WT((i + 2) % 3);
			f1->WT((i + 1) % 3) = f2->WT((j + 2) % 3);
		}
	}
	
	
	virtual bool IsFeasible()
	{
		// First the flip must be topologically correct.
		if(!vcg::face::CheckFlipEdge(*this->_pos.F(), this->_pos.E()))
			return false;
		
		// then the angle between the involved normals must be greater???
		if (math::ToDeg(Angle(this->_pos.FFlip()->cN(), this->_pos.F()->cN()) ) <= this->CoplanarAngleThresholdDeg() )
			return false;
		
		CoordType v0, v1, v2, v3;
		int i = this->_pos.E();
		FacePointer f = this->_pos.F();
		v0 = f->P0(i);
		v1 = f->P1(i);
		v2 = f->P2(i);
		v3 = f->FFp(i)->P2(f->FFi(i));
		
		// Take the parallelogram formed by the adjacent faces of edge
		// If a corner of the parallelogram on extreme of edge to flip is >= 180
		// the flip will produce two identical faces - avoid this
		if( (Angle(v2 - v0, v1 - v0) + Angle(v3 - v0, v1 - v0) >= M_PI) ||
		    (Angle(v2 - v1, v0 - v1) + Angle(v3 - v1, v0 - v1) >= M_PI))
			return false;
		
		// if any of two faces adj to edge in non writable, the flip is unfeasible
		if (!this->_pos.F()->IsW() || !this->_pos.F()->FFp(i)->IsW())
			return false;
		
		return true;
	}

	
	ScalarType ComputePriority()
	{
		/*
		     1  
		    /|\
		   / | \
		  2  |  3 
		   \ | /
		    \|/
		     0
		*/
		
		if(!this->IsFeasible())
			return std::numeric_limits<ScalarType>::infinity();
		
		VertexPointer v0, v1, v2, v3;
		int i = this->_pos.E();
		
		FacePointer f1 = this->_pos.F();
		v0 = f1->V0(i);
		v1 = f1->V1(i);
		v2 = f1->V2(i);
		
		FacePointer f2 = this->_pos.F()->FFp(i);
		v3 = f2->V2(f1->FFi(i));
		
		// save sum of curvatures of vertexes
		float cbefore = v0->Q() + v1->Q() + v2->Q() + v3->Q();

		// saving current vertex normals
		CoordType nv0orig = v0->N();
		CoordType nv1orig = v1->N();
		CoordType nv2orig = v2->N();
		CoordType nv3orig = v3->N();
		
		CurvData cd0, cd1, cd2, cd3;
		CoordType n1 = Normal(v0->P(), v3->P(), v2->P());
		CoordType n2 = Normal(v1->P(), v2->P(), v3->P());

		// new vertex normal after the flip
		// set this now to evaluate new curvatures
		v0->N() = nv0orig - f1->N() - f2->N() + n1;
		v1->N() = nv1orig - f1->N() - f2->N() + n2;
		v2->N() = nv2orig - f1->N() + n1 + n2;
		v3->N() = nv3orig - f2->N() + n1 + n2;
		
		cd0 = FaceCurv(v0, v3, v2, n1) + Curvature(v0, f1, f2);
		cd1 = FaceCurv(v1, v2, v3, n2) + Curvature(v1, f1, f2);
		cd2 = FaceCurv(v2, v0, v3, n1) + FaceCurv(v2, v3, v1, n2) + Curvature(v2, f1, f2);
		cd3 = FaceCurv(v3, v2, v0, n1) + FaceCurv(v3, v1, v2, n2) + Curvature(v3, f1, f2);
		
		// restore original pervertex normals after computation
		v0->N() = nv0orig;
		v1->N() = nv1orig;
		v2->N() = nv2orig;
		v3->N() = nv3orig;

		CURVEVAL curveval;
		
		_cv0 = curveval(cd0);
		_cv1 = curveval(cd1);
		_cv2 = curveval(cd2);
		_cv3 = curveval(cd3);
		float cafter = _cv0 + _cv1 + _cv2 + _cv3;
		
		// The priority of an edge flip is **how much we lower the overall curvature**. 
		// If after the flip the sum of the curvature is decreased it is a good move;
		// good flips have:    cafter < cbefore
		// Since the local optimization is designed to make the miniumum cost move we put inside 
		// negative values (the more negative the better).
		this->_priority = (cafter - cbefore);
		//qDebug("computed curvature change, %f->%f (priority = %f)", cbefore,cafter,this->_priority);
		return this->_priority;
	}
	
	
	static void Init(TRIMESH_TYPE &m, HeapType &heap)
	{
		CURVEVAL curveval;
		heap.clear();

		// comuputing edge flip priority require non normalized vertex normals AND non normalized face normals.
			vcg::tri::UpdateNormals<TRIMESH_TYPE>::PerVertexPerFace(m);

		VertexIterator vi;
		for (vi = m.vert.begin(); vi != m.vert.end(); ++vi)
			if (!(*vi).IsD() && (*vi).IsW())
				(*vi).Q() = curveval(Curvature(&(*vi)));

		FaceIterator fi;
		for (fi = m.face.begin(); fi != m.face.end(); ++fi)
			if (!(*fi).IsD())
				for (unsigned int i = 0; i < 3; i++)
					if ((*fi).V1(i) - (*fi).V0(i) > 0) {
						PosType newpos(&*fi, i);
            Insert(heap, newpos, tri::IMark(m));
					}
	}
}; // end CurvEdgeFlip class


}; // namespace tri
}; // namespace vcg

#endif // __CURVEDGEFLIP
