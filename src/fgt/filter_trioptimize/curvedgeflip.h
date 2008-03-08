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

#include <vcg/complex/local_optimization/tri_edge_flip.h>
#include <vcg/space/triangle3.h>
#include <vcg/space/point3.h>

namespace vcg
{
namespace tri
{

class CurvData
{
public:
	friend const CurvData operator+(const CurvData& lhs, const CurvData& rhs);
	friend CurvData &operator+=(CurvData& lhs, const CurvData& rhs);
	
	CurvData()
	{
		A = 0;
		H = 0;
		K = 0;
	}
	
	float Value()
	{	
		//float a = A / 8.0;
		float h = H / 8.0;
		//float k = (2 * M_PI) - K;
		
		/*// F1
		return powf((h / 8.0), 2.0f) / (a / 8.0);*/
		// F2
		return h;
		/*// F3
		if(K > 0) return 2.0 * h;
		else return 2.0 * math::Sqrt(powf(h, 2.0f) - a * k);*/
	}
	
	float A;
	float H;
	float K;
};

const CurvData operator+ (const CurvData &lhs, const CurvData &rhs)
{
	CurvData result;
	result.A = lhs.A + rhs.A;
	result.H = lhs.H + rhs.H;
	result.K = lhs.K + rhs.K;
	return result;
}

CurvData &operator+=(CurvData &lhs, const CurvData& rhs)
{
	lhs.A += rhs.A;
	lhs.H += rhs.H;
	lhs.K += rhs.K;
	return lhs;
}

/* This flip happens only if decreases the curvature of the surface */
template <class TRIMESH_TYPE, class MYTYPE>
class CurvEdgeFlip : public TriEdgeFlip<TRIMESH_TYPE, MYTYPE>
{
protected:
	typedef typename TRIMESH_TYPE::FaceType       FaceType;
	typedef typename TRIMESH_TYPE::FacePointer    FacePointer;
	typedef typename TRIMESH_TYPE::FaceIterator   FaceIterator;
	typedef typename TRIMESH_TYPE::VertexIterator VertexIterator;
	typedef typename TRIMESH_TYPE::VertexType     VertexType;
	typedef typename TRIMESH_TYPE::ScalarType     ScalarType;
	typedef typename TRIMESH_TYPE::VertexPointer  VertexPointer;
	typedef typename TRIMESH_TYPE::CoordType      CoordType;
	typedef vcg::face::Pos<FaceType>              PosType;
	typedef vcg::face::VFIterator<FaceType> VFIteratorType;
	typedef typename LocalOptimization<TRIMESH_TYPE>::HeapElem HeapElem;
	typedef typename LocalOptimization<TRIMESH_TYPE>::HeapType HeapType;
	typedef TriEdgeFlip<TRIMESH_TYPE, MYTYPE > Parent;
	typedef typename vcg::Triangle3<ScalarType> TriangleType;
	
	// New curvature precomputed for the vertexes of the faces 
	// adjacent to edge to be flipped
	ScalarType _cv0, _cv1, _cv2, _cv3;
	
	static CurvData FaceCurv(VertexPointer v0,
	                      VertexPointer v1,
	                      VertexPointer v2,
	                      CoordType fNormal)
	{
		CurvData res;
		
		float ang0 = math::Abs(Angle(v1->P() - v0->P(), v2->P() - v0->P() ));
		float ang1 = math::Abs(Angle(v0->P() - v1->P(), v2->P() - v1->P() ));
		float ang2 = M_PI - (ang0 + ang1);
		
		float s01 = SquaredDistance(v1->P(), v0->P());
		float s02 = SquaredDistance(v2->P(), v0->P());
		
		// voronoi cell of vertex i
		if ((ang0 < M_PI) && (ang1 < M_PI) && (ang2 < M_PI)) // non obctuse
			res.A += (s02 * (1.0 / tan(ang1)) + s01 * (1.0 / tan(ang2) ));
		else {
			// obctuse
			TriangleType triangle(v0->P(), v1->P(), v2->P());
			res.A += (0.5f * DoubleArea(triangle) - 
					(s02 * tan(ang1) + s01 * tan(ang2)) );
		}
		
		res.K += ang0;
		
		ang1 = (Angle(fNormal, v1->N()));
		ang2 = (Angle(fNormal, v2->N()));
		res.H += ( Distance(v0->P(), v1->P()) * ang1 + 
				Distance(v0->P(), v2->P()) * ang2 );
		
		return res;
	}
	
	// f1, f2 --> this faces are to be ignored
	static CurvData Curvature(VertexPointer v, FacePointer f1 = NULL, FacePointer f2 = NULL)
	{		
		CurvData curv;
		VFIteratorType vfi(v);
		
		while(!vfi.End()) {
			if(vfi.F() != f1 && vfi.F() != f2 && !vfi.F()->IsD()) {
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
	void Execute(TRIMESH_TYPE &m)
	{
		int i = this->_pos.I();
		
		PosType app = this->_pos;
		app.FlipF(); app.FlipE(); app.FlipV();
		
		// save precomputed curvature in vertex quality
		this->_pos.F()->V0(i)->Q() = _cv0;
		this->_pos.F()->V1(i)->Q() = _cv1;
		this->_pos.F()->V2(i)->Q() = _cv2;
		app.V()->Q() = _cv3;
		
		// do the flip
		vcg::face::FlipEdge(*this->_pos.f, this->_pos.z);	
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
		int i = this->_pos.I();
		
		FacePointer f1 = this->_pos.F();
		
		v0 = this->_pos.F()->V0(i);
		v1 = this->_pos.F()->V1(i);
		v2 = this->_pos.F()->V2(i);
		
		PosType app = this->_pos;
		app.FlipF(); app.FlipE(); app.FlipV();
		v3 = app.V();
		
		FacePointer f2 = app.F();
		
		// save sum of curvatures of ve
		float cbefore = v0->Q() + v1->Q() + v2->Q() + v3->Q();
		
		CurvData cd0, cd1, cd2, cd3;
		CoordType n1 = NormalizedNormal(v0->P(), v3->P(), v2->P());
		CoordType n2 = NormalizedNormal(v1->P(), v2->P(), v3->P());
		cd0 = FaceCurv(v0, v3, v2, n1) + Curvature(v0, f1, f2);
		cd1 = FaceCurv(v1, v2, v3, n2) + Curvature(v1, f1, f2);
		cd2 = FaceCurv(v2, v0, v3, n1) + FaceCurv(v2, v3, v1, n2) + Curvature(v2, f1, f2);
		cd3 = FaceCurv(v3, v2, v0, n1) + FaceCurv(v3, v1, v2, n2) + Curvature(v3, f1, f2);
		
		_cv0 = cd0.Value();
		_cv1 = cd1.Value();
		_cv2 = cd2.Value();
		_cv3 = cd3.Value();
		float cafter = _cv0 + _cv1 + _cv2 + _cv3;
		
		this->_priority = (cafter - cbefore);
		return this->_priority;
	}
	
	static void Init(TRIMESH_TYPE &mesh, HeapType &heap)
	{
		heap.clear();
		
		// initialize vertex quality with vertex curvature
		VertexIterator vi;
		for(vi = mesh.vert.begin(); vi != mesh.vert.end(); ++vi)
			(*vi).Q() = Curvature(&*vi).Value();
		
		FaceIterator fi;
		for(fi = mesh.face.begin(); fi != mesh.face.end(); ++fi)
			if(!(*fi).IsD()) {
				for(unsigned int i = 0; i < 3; i++) {
					VertexPointer v0 = (*fi).V0(i);
					VertexPointer v1 = (*fi).V1(i);
					if (v1 - v0 > 0) {
						MYTYPE* newflip = new MYTYPE(PosType(&*fi, i), mesh.IMark());
						if(newflip->Priority() < 0 && newflip->IsFeasible())
							heap.push_back(HeapElem(newflip));
						else delete newflip;
					}
				}
			}
	}
	
	void UpdateHeap(HeapType &heap)
	{
		this->GlobalMark()++;
		
		// The flip creates a diagonal edge on index _pos.I() + 1
		// We must push on heap every edge of the mesh with a vertex in a
		// face involved in the flip operation, except the edge just flipped
		
		// index of the edge just flipped
		int flipped = (this->_pos.I() + 1)  % 3;
		PosType pos(this->_pos.f, flipped);
		pos.FlipF(); pos.FlipE(); pos.FlipV();
		
		FacePointer face1 = this->_pos.F();
		
		face1->V(0)->IMark() = this->GlobalMark();
		face1->V(1)->IMark() = this->GlobalMark();
		face1->V(2)->IMark() = this->GlobalMark();
		pos.V()->IMark() = this->GlobalMark();
		
		FacePointer face2 = pos.F();
		
		// edges of the first face, except the flipped edge
		for(int i = 0; i < 3; i++) if(i != flipped)
			heap.push_back(new MYTYPE(PosType(face1, i), this->GlobalMark()));
		
		// edges of the second face, except the flipped edge
		for(int i = 0; i < 3; i++) if(i != face1->FFi(flipped))
			heap.push_back(new MYTYPE(PosType(face2, i), this->GlobalMark()));
		
		// every edge with v0, v1 v3 of face1
		for(int i = 0; i < 3; i++) {
			PosType startpos(face1, i);
			PosType epos = startpos;
			do {
				if(epos.F() != face1 && epos.FFlip() != face1 && 
				   epos.F() != face2 && epos.FFlip() != face2)
					heap.push_back(HeapElem(new MYTYPE(epos, this->GlobalMark())));
				epos.NextE();
			} while(epos != startpos && !epos.IsBorder());
		}
		
		//PosType startpos = pos;
		PosType epos = pos;
		do {
			if(epos.F() != face2 && epos.FFlip() != face2)
				heap.push_back(HeapElem(new MYTYPE(epos, this->GlobalMark())));
			epos.NextE();
		} while(epos != pos && !epos.IsBorder());
		
		std::push_heap(heap.begin(),heap.end());
		//*/
	}
}; // end CurvEdgeFlip class


}; // namespace tri
}; // namespace vcg

#endif // __CURVEDGEFLIP
