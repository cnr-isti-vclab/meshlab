//***************************************************************************
//
//  Copyright (c) 1999 - 2006 Intel Corporation
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//***************************************************************************

/**
	@file	CIFXQuadEdge.h

			This module contains the CIFXQuadEdge, CIFXEdge and 
			CIFXSubdivision classes.

	@note	This code in this module is a modification of the code from the 
			article "Incremental Delaunay Triangulation" by Dani Lischinski 
			(danix@graphics.cornell.edu) in "Graphics Gems IV", Academic 
			Press, 1994, p. 47-59, code: p. 51-58.  The actual source is 
			available from www.acm.org/pubs/tog/GraphicsGems/gemsiv.zip 
			in the "delaunay" folder.  The associated website's root address 
			is www.graphicsgems.org.  It states that the Graphics Gems code 
			can be used without restrictions.

	@note	Another helpful article was "Primitives for the manipulation of 
			general subdivisions and the computation of Voronoi" by Leonidas 
			Guibas and Jorge Stolfi( both from Xerox Palo Alto Research 
			Center and Stanford Univ.) in "ACM Transactions on Graphics", 
			Volume 4, Issue 2, ACM Press, April 1985, p. 74 - 123.  
*/


#ifndef CIFXQuadEdge_H
#define CIFXQuadEdge_H


#include "CIFXGeom2D.h"
#include "IFXList.h"

#define EPS 1e-6

class CIFXQuadEdge;
class CIFXSubdivision;

class CIFXEdge 
{
	friend class CIFXQuadEdge;
	friend class CIFXSubdivision;
private:
	int num;
	CIFXEdge *next;
	SIFXPoint2d *data;
public:
	CIFXEdge()      { data = 0; }
	CIFXEdge* Rot();
	CIFXEdge* invRot();
	CIFXEdge* Sym();
	CIFXEdge* Onext();
	CIFXEdge* Oprev();
	CIFXEdge* Dnext();
	CIFXEdge* Dprev();
	CIFXEdge* Lnext();
	CIFXEdge* Lprev();
	CIFXEdge* Rnext();
	CIFXEdge* Rprev();
	SIFXPoint2d* Org();
	SIFXPoint2d* Dest();
	const SIFXPoint2d& Org2d() const;
	const SIFXPoint2d& Dest2d() const;
	void  EndPoints(SIFXPoint2d*, SIFXPoint2d*);
	CIFXQuadEdge* Qedge() { return (CIFXQuadEdge *)(this - num); }
};

class CIFXQuadEdge 
{
	friend class CIFXSubdivision;
private:
	CIFXEdge e[4];
	BOOL fixed;
	BOOL eflag[4];
public:
	CIFXQuadEdge();
	void MakeFixed() { fixed = TRUE; }
	BOOL IsFixed() { return fixed; }
};



class CIFXSubdivision 
{
	friend CIFXEdge *MakeEdge();
public:
	BOOL m_bInitialized;
	U32 m_uiControlWordFp;
	SIFXPoint2d m_domainMax;
	SIFXPoint2d m_domainMin;
	IFXList <CIFXQuadEdge> m_edgeList;
	IFXList <SIFXPoint2d> m_pointList;
	IFXList <SIFXIndexTriangle> m_triList;
	U32 m_uNEdges;
	U32 m_uNPoints;
	SIFXPoint2d *m_pPointArray;


	CIFXEdge *m_startingEdge;
	IFXRESULT Locate(const SIFXPoint2d&, CIFXEdge **ppEdge);
	IFXRESULT DeleteEdge(CIFXEdge* e);
	void Splice(CIFXEdge* a, CIFXEdge* b);
	IFXRESULT MakeEdge(CIFXEdge **ppEdge);
	void Swap(CIFXEdge* e);
	IFXRESULT Connect(CIFXEdge* a, CIFXEdge* b, CIFXEdge **ppEdge=NULL);

public:
	CIFXSubdivision();
	~CIFXSubdivision();
	IFXRESULT Initialize(const SIFXPoint2d&, const SIFXPoint2d&);
	IFXRESULT TriangluatePseudopolygonEdges(CIFXEdge *spine);
	IFXRESULT InsertSite(const SIFXPoint2d&, CIFXEdge **ppEdge);
	IFXRESULT InsertEdge(const SIFXPoint2d& a, const SIFXPoint2d& b);
	IFXRESULT IntersectInsertEdge(const SIFXPoint2d& a, const SIFXPoint2d& b);
	IFXRESULT GenerateTriangleList(IFXList<SIFXEdge> &EdgeSet);
};

inline CIFXQuadEdge::CIFXQuadEdge()
{
	e[0].num = 0, e[1].num = 1, e[2].num = 2, e[3].num = 3;
	e[0].next = &(e[0]); e[1].next = &(e[3]);
	e[2].next = &(e[2]); e[3].next = &(e[1]);

	fixed = FALSE;

	eflag[0]=FALSE;
	eflag[1]=FALSE;
	eflag[2]=FALSE;
	eflag[3]=FALSE;
}

/************************* Edge Algebra *************************************/

inline CIFXEdge* CIFXEdge::Rot()
// Return the dual of the current edge, directed from its right to its left.
{
  return (num < 3) ? this + 1 : this - 3;
}

inline CIFXEdge* CIFXEdge::invRot()
// Return the dual of the current edge, directed from its left to its right.
{
  return (num > 0) ? this - 1 : this + 3;
}

inline CIFXEdge* CIFXEdge::Sym()
// Return the edge from the destination to the origin of the current edge.
{
  return (num < 2) ? this + 2 : this - 2;
}

inline CIFXEdge* CIFXEdge::Onext()
// Return the next ccw edge around (from) the origin of the current edge.
{
  return next;
}

inline CIFXEdge* CIFXEdge::Oprev()
// Return the next cw edge around (from) the origin of the current edge.
{
  return Rot()->Onext()->Rot();
}

inline CIFXEdge* CIFXEdge::Dnext()
// Return the next ccw edge around (into) the destination of the current edge.
{
  return Sym()->Onext()->Sym();
}

inline CIFXEdge* CIFXEdge::Dprev()
// Return the next cw edge around (into) the destination of the current edge.
{
  return invRot()->Onext()->invRot();
}

inline CIFXEdge* CIFXEdge::Lnext()
// Return the ccw edge around the left face following the current edge.
{
  return invRot()->Onext()->Rot();
}

inline CIFXEdge* CIFXEdge::Lprev()
// Return the ccw edge around the left face before the current edge.
{
  return Onext()->Sym();
}

inline CIFXEdge* CIFXEdge::Rnext()
// Return the edge around the right face ccw following the current edge.
{
  return Rot()->Onext()->invRot();
}

inline CIFXEdge* CIFXEdge::Rprev()
// Return the edge around the right face ccw before the current edge.
{
  return Sym()->Onext();
}

/************** Access to data pointers *************************************/

inline SIFXPoint2d* CIFXEdge::Org()
{
  return data;
}

inline SIFXPoint2d* CIFXEdge::Dest()
{
  return Sym()->data;
}

inline const SIFXPoint2d& CIFXEdge::Org2d() const
{
  return *data;
}

inline const SIFXPoint2d& CIFXEdge::Dest2d() const
{
  return (num < 2) ? *((this + 2)->data) : *((this - 2)->data);
}

inline void CIFXEdge::EndPoints(SIFXPoint2d* org, SIFXPoint2d* de)
{
  data = org;
  Sym()->data = de;
}

#endif
