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
	@file	CIFXQuadEdge.cpp

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


#include <math.h>

#include "predicates.h"
#include "CIFXQuadEdge.h"
#include "IFXOSUtilities.h"

/*********************** Basic Topological Operators ************************/
int InCircle(const SIFXPoint2d& a, const SIFXPoint2d& b,
			 const SIFXPoint2d& c, const SIFXPoint2d& d);

IFXRESULT CIFXSubdivision::MakeEdge(CIFXEdge **ppEdge)
{
	IFXRESULT iResult = IFX_OK;

	if(ppEdge == NULL)
	{
		iResult=IFX_E_INVALID_POINTER;
	}
	else
	{
		CIFXQuadEdge *ql = new CIFXQuadEdge;

		if(ql!=NULL)
		{
			// IFXList has no error checking
			void ** error = reinterpret_cast<void**>(m_edgeList.Append(ql));
			if(error == NULL)
			{
				iResult=IFX_E_OUT_OF_MEMORY;
			}
			else {
				*ppEdge = ql->e;
				m_uNEdges++;
			}
		}
		else
		{
			*ppEdge = NULL;
			iResult=IFX_E_OUT_OF_MEMORY;
		}
	}

	IFXRETURN(iResult);
}

/**
	This operator affects the two edge rings around the origins of a and b,
	and, independently, the two edge rings around the left faces of a and b.
	In each case, (i) if the two rings are distinct, Splice will combine
	them into one; (ii) if the two are the same ring, Splice will break it
	into two separate pieces.
	Thus, Splice can be used both to attach the two edges together, and
	to break them apart. See Guibas and Stolfi (1985) p.96 for more details
	and illustrations.
*/
void CIFXSubdivision::Splice(CIFXEdge* a, CIFXEdge* b)
{
	CIFXEdge* alpha = a->Onext()->Rot();
	CIFXEdge* beta  = b->Onext()->Rot();

	CIFXEdge* t1 = b->Onext();
	CIFXEdge* t2 = a->Onext();
	CIFXEdge* t3 = beta->Onext();
	CIFXEdge* t4 = alpha->Onext();

	a->next = t1;
	b->next = t2;
	alpha->next = t3;
	beta->next = t4;
}

IFXRESULT CIFXSubdivision::DeleteEdge(CIFXEdge* e)
{
	IFXRESULT iResult = IFX_OK;
	if(e == NULL)
	{
		iResult = IFX_E_INVALID_POINTER;
	}
	else
	{

		if(m_startingEdge->Qedge() == e->Qedge())
		{
			m_startingEdge = e->Oprev();
		}

		Splice(e, e->Oprev());
		Splice(e->Sym(), e->Sym()->Oprev());

		CIFXQuadEdge* l=e->Qedge();

		if(m_edgeList.Remove(l))
		{
			delete l;
			m_uNEdges--;
		}
		else
		{
			iResult = IFX_E_ABORTED;
		}

	}

	IFXRETURN(iResult);
}

/************* Topological Operations for Delaunay Diagrams *****************/
CIFXSubdivision::CIFXSubdivision()
{

	m_uiControlWordFp = IFXOSControlFP(0, 0);
	IFXOSControlFP( 16, 48 );     // set FPU control word for double precision
	SIFXPoint2d zeros(0,0);
	m_uNEdges=0;
	m_uNPoints=0;
	m_domainMax = zeros;
	m_domainMin = zeros;

	m_bInitialized=FALSE;

	m_startingEdge=NULL;
	m_pPointArray = NULL;

	m_pointList.SetAutoDestruct(TRUE);
	m_edgeList.SetAutoDestruct(TRUE);
	m_triList.SetAutoDestruct(TRUE);

}

/**
	Initialize a CIFXSubdivision to the triangle defined by the points a, b, c.
*/
CIFXSubdivision::~CIFXSubdivision()
{
	IFXOSControlFP( m_uiControlWordFp, 48 ); // set FPU control word for double precision
	m_edgeList.DeleteAll();
	m_pointList.DeleteAll();
	m_triList.DeleteAll();

	if(m_pPointArray)
		delete[] m_pPointArray;

	exactcleanup();
}


/// Initialize a CIFXSubdivision to the triangle defined by the points a, b, c.
IFXRESULT CIFXSubdivision::Initialize(const SIFXPoint2d& minp, const SIFXPoint2d& maxp)
{
	IFXRESULT iResult=IFX_OK;

	SIFXPoint2d *da, *db, *dc, *dd;
	void **error = NULL;

	iResult = exactinit();

	if(IFXFAILURE(iResult))
	{
		return iResult;
	}

	da = new SIFXPoint2d(minp.x,minp.y);
	if(da == NULL )
	{
		IFXASSERT(0);
		iResult = IFX_E_OUT_OF_MEMORY;
		IFXRETURN(iResult);
	}

	error = reinterpret_cast<void**>(m_pointList.Append(da));
	if(error == NULL)
	{
		IFXASSERT(0);
		iResult=IFX_E_OUT_OF_MEMORY;
		IFXRETURN(iResult);
	}

	db = new SIFXPoint2d(maxp.x,minp.y);
	if(db == NULL)
	{
		IFXASSERT(0);
		iResult = IFX_E_OUT_OF_MEMORY;
		IFXRETURN(iResult);
	}
	error = reinterpret_cast<void**>(m_pointList.Append(db));
	if(error == NULL)
	{
		IFXASSERT(0);
		iResult=IFX_E_OUT_OF_MEMORY;
		IFXRETURN(iResult);
	}

	dc = new SIFXPoint2d(maxp.x,maxp.y);
	if(dc == NULL)
	{
		IFXASSERT(0);
		iResult = IFX_E_OUT_OF_MEMORY;
		IFXRETURN(iResult);
	}
	error = reinterpret_cast<void**>(m_pointList.Append(dc));
	if(error == NULL)
	{
		IFXASSERT(0);
		iResult=IFX_E_OUT_OF_MEMORY;
		IFXRETURN(iResult);
	}

	dd = new SIFXPoint2d(minp.x,maxp.y);
	if(dd == NULL)
	{
		IFXASSERT(0);
		iResult = IFX_E_OUT_OF_MEMORY;
		IFXRETURN(iResult);
	}
	error = reinterpret_cast<void**>(m_pointList.Append(dd));
	if(error == NULL)
	{
		IFXASSERT(0);
		iResult=IFX_E_OUT_OF_MEMORY;
		IFXRETURN(iResult);
	}

	CIFXEdge* ea = NULL;
	iResult = MakeEdge(&ea);

	if(IFXFAILURE(iResult))
		IFXRETURN(iResult);

	ea->EndPoints(da, db);

	CIFXEdge* eb = NULL;
	iResult = MakeEdge(&eb);

	if(IFXFAILURE(iResult))
		IFXRETURN(iResult);

	Splice(ea->Sym(), eb);
	eb->EndPoints(db, dc);

	CIFXEdge* ec = NULL;
	iResult = MakeEdge(&ec);

	if(IFXFAILURE(iResult))
		IFXRETURN(iResult);

	Splice(eb->Sym(), ec);
	ec->EndPoints(dc, dd);

	CIFXEdge* ed = NULL;
	iResult = MakeEdge(&ed);

	if(IFXFAILURE(iResult))
		IFXRETURN(iResult);

	Splice(ec->Sym(),ed);
	ed->EndPoints(dd, da);

	Splice(ed->Sym(),ea);

	CIFXEdge* ee = NULL;
	iResult = MakeEdge(&ee);

	if(IFXFAILURE(iResult))
		IFXRETURN(iResult);

	Splice(ee,ea);
	Splice(ee->Sym(),ec);
	ee->EndPoints(da,dc);

	m_startingEdge = ea;

	if(InCircle(ee->Org2d(), *db, ee->Dest2d(), *dd))
	{
		Swap(ee);
	}

	m_domainMax = maxp;
	m_domainMin = minp;

	m_uNPoints=0;
	m_bInitialized=TRUE;

	IFXRETURN(iResult);
}


/** 
	Add a new edge e connecting the destination of a to the
	origin of b, in such a way that all three have the same
	left face after the connection is complete.
	Additionally, the data pointers of the new edge are set.
*/
IFXRESULT CIFXSubdivision::Connect(CIFXEdge* a, CIFXEdge* b, CIFXEdge **ppEdge)
{
	IFXRESULT iResult= IFX_OK;

	CIFXEdge* e = NULL;
	if(IFXSUCCESS(iResult))
	{
		iResult = MakeEdge(&e);
	}

	if(IFXSUCCESS(iResult))
	{
		Splice(e, a->Lnext());
		Splice(e->Sym(), b);
		e->EndPoints(a->Dest(), b->Org());

		if(ppEdge != NULL)
		{
			*ppEdge = e;
		}
	}

	IFXRETURN(iResult);
}

/**
	Essentially turns edge e counterclockwise inside its enclosing
	quadrilateral. The data pointers are modified accordingly.
*/
void CIFXSubdivision::Swap(CIFXEdge* e)
{
	CIFXEdge* a = e->Oprev();
	CIFXEdge* b = e->Sym()->Oprev();
	Splice(e, a);
	Splice(e->Sym(), b);
	Splice(e, a->Lnext());
	Splice(e->Sym(), b->Lnext());
	e->EndPoints(a->Dest(), b->Dest());
}

/*************** Geometric Predicates for Delaunay Diagrams *****************/

/** 
	Returns twice the area of the oriented triangle (a, b, c), i.e., the
	area is positive if the triangle is oriented counterclockwise.
*/
inline REAL TriArea(const SIFXPoint2d& a, const SIFXPoint2d& b, const SIFXPoint2d& c)
{
	return orient2d((REAL *)&a,(REAL *)&b, (REAL *)&c);
}


// Ordinary precision TriArea is replaced by an infinate precision predicate.
//inline Real TriArea(const SIFXPoint2d& a, const SIFXPoint2d& b, const SIFXPoint2d& c)
// Returns twice the area of the oriented triangle (a, b, c), i.e., the
// area is positive if the triangle is oriented counterclockwise.
//{
//  return (b.x - a.x)*(c.y - a.y) - (b.y - a.y)*(c.x - a.x);
//}


/** 
	Returns TRUE if the point d is inside the circle defined by the
	points a, b, c. See Guibas and Stolfi (1985) p.107.
*/
int InCircle(const SIFXPoint2d& a, const SIFXPoint2d& b,
			 const SIFXPoint2d& c, const SIFXPoint2d& d)
{
	return incircle((REAL *)&a,(REAL *)&b, (REAL *)&c, (REAL *)&d) > 0;
}


// Ordinary precision InCircle is replaced by an infinate precision predicate.
//int InCircle(const SIFXPoint2d& a, const SIFXPoint2d& b,
//       const SIFXPoint2d& c, const SIFXPoint2d& d)
// Returns TRUE if the point d is inside the circle defined by the
// points a, b, c. See Guibas and Stolfi (1985) p.107.
//{
//  return (a.x*a.x + a.y*a.y) * TriArea(b, c, d) -
//         (b.x*b.x + b.y*b.y) * TriArea(a, c, d) +
//         (c.x*c.x + c.y*c.y) * TriArea(a, b, d) -
//         (d.x*d.x + d.y*d.y) * TriArea(a, b, c) > 0;
//}


/// Returns TRUE if the points a, b, c are in a counterclockwise order
static
int ccw(const SIFXPoint2d& a, const SIFXPoint2d& b, const SIFXPoint2d& c)
{
	return (TriArea(a, b, c) > 0);
}

static
int RightOf(const SIFXPoint2d& x, CIFXEdge* e)
{
	return ccw(x, e->Dest2d(), e->Org2d());
}

static
int LeftOf(const SIFXPoint2d& x, CIFXEdge* e)
{
	return ccw(x, e->Org2d(), e->Dest2d());
}

static
int Colinear(const SIFXPoint2d& x, CIFXEdge* e)
{
	F64 area2x = TriArea(x, e->Org2d(), e->Dest2d());
	return ( fabs(area2x) == 0 );
}

static
inline int XOR( int x, int y) { return !x ^ !y; }

static
int IntersectProp( CIFXEdge *a, CIFXEdge *b)
{
	if (
		Colinear(b->Dest2d(),a) ||
		Colinear(b->Org2d(),a) ||
		Colinear(a->Org2d(),b) ||
		Colinear(a->Dest2d(),b)
		)
		return FALSE;

	return
		XOR( LeftOf(b->Org2d(),a), LeftOf(b->Dest2d(),a) ) &&
		XOR( LeftOf(a->Org2d(),b), LeftOf(a->Dest2d(),b) );
}

static
int Between(const SIFXPoint2d& c, CIFXEdge *e)
{
	if(!Colinear(c, e))
		return FALSE;

	SIFXPoint2d a,b;
	a=e->Org2d();
	b=e->Dest2d();

	if( b.x != a.x)
	{
		return
			((a.x <= c.x) && (c.x <= b.x)) ||
			((a.x >= c.x) && (c.x >= b.x));
	}
	else
	{
		return
			((a.y <= c.y) && (c.y <= b.y)) ||
			((a.y >= c.y) && (c.y >= b.y));
	}
}

static
int Intersect( CIFXEdge *a, CIFXEdge *b )
{
	if( IntersectProp(a,b) )
		return TRUE;
	else if (
		Between(b->Org2d(), a)  ||
		Between(b->Dest2d(), a) ||
		Between(a->Org2d(), b)  ||
		Between(a->Dest2d(), b)
		)
		return TRUE;
	else
		return FALSE;
}

inline int OnEdge(const SIFXPoint2d x, CIFXEdge *e)
{
	return Between(x,e);
}


// Ordinary precision OnEdge is replaced by an infinate precision version.
//int OnEdge(const SIFXPoint2d& x, CIFXEdge* e)
// A predicate that determines if the point x is on the edge e.
// The point is considered on if it is in the EPS-neighborhood
// of the edge.
//{
//  Real t1, t2, t3;
//  t1 = (x - e->Org2d()).norm();
//  t2 = (x - e->Dest2d()).norm();
//  if (t1 < EPS || t2 < EPS)
//      return TRUE;
//  t3 = (e->Org2d() - e->Dest2d()).norm();
//  if (t1 > t3 || t2 > t3)
//      return FALSE;
//  Line line(e->Org2d(), e->Dest2d());
//  return (fabs(line.eval(x)) < EPS);
//}


/************* An Incremental Algorithm for the Construction of *************/
/************************ Delaunay Diagrams *********************************/

/** 
	Returns an edge e, s.t. either x is on e, or e is an edge of
	a triangle containing x. The search starts from startingEdge
	and proceeds in the general direction of x. Based on the
	pseudocode in Guibas and Stolfi (1985) p.121.
*/
IFXRESULT CIFXSubdivision::Locate(const SIFXPoint2d& x, CIFXEdge **ppEdge)
{
	IFXRESULT iResult = IFX_OK;
	*ppEdge = NULL;

	if(!m_bInitialized)
	{
		IFXASSERT(0);
		IFXRETURN(IFX_E_NOT_INITIALIZED);
	}

	if(
		x.x >= m_domainMax.x ||
		x.x <= m_domainMin.x ||
		x.y >= m_domainMax.y ||
		x.y <= m_domainMin.y
		)
	{
		IFXASSERT(0);
		IFXRETURN(IFX_E_INVALID_RANGE);
	}

	CIFXEdge* e = m_startingEdge;
	U32 itterations = 0;
	U32 max_itterations = m_uNEdges;

	while (TRUE) 
	{
		if (x == e->Org2d())
		{
			*ppEdge = e;
			break;
		}
		else if(x == e->Dest2d())
		{
			*ppEdge = e->Sym();
			break;
		}
		else if (RightOf(x, e))
		{
			e = e->Sym();
		}
		else if (!RightOf(x, e->Onext()))
		{
			e = e->Onext();
		}
		else if (!RightOf(x, e->Dprev()))
		{
			e = e->Dprev();
		}
		else
		{
			*ppEdge = e;
			break;
		}

		itterations ++;
		if(itterations > max_itterations)
		{
			iResult = IFX_E_ABORTED;
			break;
		}
	}

	IFXRETURN(iResult);
}

static
IFXRESULT SolveIntersection(CIFXEdge *a, CIFXEdge *b, SIFXPoint2d *site)
{
	IFXRESULT iResult = IFX_OK;

	if(site == NULL)
	{
		iResult = IFX_E_INVALID_POINTER;
	}
	else if(!Colinear( a->Org2d(), b) && ! Colinear(a->Dest2d() , b) )
	{
		F64 a1,b1,c1;
		F64 a2,b2,c2;

		F64 dx,dy;
		dx = a->Dest2d().x - a->Org2d().x;
		dy = a->Dest2d().y - a->Org2d().y;


		a1 = -dy;
		b1 = dx;
		c1 = a->Org2d().y * dx - a->Org2d().x * dy;


		dx = b->Dest2d().x - b->Org2d().x;
		dy = b->Dest2d().y - b->Org2d().y;

		a2 = -dy;
		b2 = dx;
		c2 = b->Org2d().y * dx - b->Org2d().x * dy;

		F64 det=b1*a2 - b2*a1;

		if(fabs(det)>0.0)
		{
			site->x = static_cast<REAL>((b1*c2 - b2*c1) / det);
			site->y = static_cast<REAL>((a2*c1 - a1*c2) / det);
		}
		else
		{
			// lines do not intersect.
			IFXASSERT(0);
			iResult = IFX_E_ABORTED;
		}
	}
	else
	{
		// edges intersect along a line segment.
		IFXASSERT(0);
		iResult = IFX_E_ABORTED;
	}

	return iResult;
}

IFXRESULT CIFXSubdivision::IntersectInsertEdge(const SIFXPoint2d& A, const SIFXPoint2d& B)
{
	IFXRESULT iResult = IFX_OK;

	SIFXEdge *newEdge = new SIFXEdge;
	if(newEdge == NULL)
	{
		IFXASSERT(0);
		iResult = IFX_E_OUT_OF_MEMORY;
		IFXRETURN(iResult);
	}

	newEdge->a = A;
	newEdge->b = B;

	//The MAC cannot handle deep recursion.  Use IFXList as a stack.
	void **error = NULL;
	IFXList<SIFXEdge> stack;
	stack.SetAutoDestruct(TRUE);
	IFXListContext context;
	error = reinterpret_cast<void**>(stack.Prepend(newEdge));
	if(error == NULL)
	{
		IFXASSERT(0);
		iResult = IFX_E_OUT_OF_MEMORY;
		IFXRETURN(iResult);
	}
	stack.ToHead(context);

	SIFXPoint2d a,b;

	U32 maincount = 0;
	U32 mainlimit = m_uNEdges;

	while((newEdge=stack.GetHead()) != NULL)
	{

		if((maincount ++) > mainlimit)
			break;

		a = newEdge ->a;
		b = newEdge ->b;

		error = reinterpret_cast<void**>(stack.Remove(newEdge,context));
		if(error == NULL)
		{
			IFXASSERT(0);
			iResult = IFX_E_ABORTED;
			IFXRETURN(iResult);
		}
		else
		{
			delete newEdge;
		}

		CIFXEdge* eb;
		iResult = InsertSite(b, &eb);
		if(IFXFAILURE(iResult))
		{
			IFXRETURN(iResult);
		}

		// if (a) and (b) are identical, there is no constained edge to insert.
		// simply insert the point (a) and bail.
		if(a == b)
		{
			continue;
		}

		CIFXEdge* ea = NULL;
		iResult = InsertSite(a,&ea);
		if(IFXFAILURE(iResult))
		{
			IFXRETURN(iResult);
		}


		// check to see if edge [a,b] already exists.
		// it could have been created by inserting (a) or (b).

		U32 itterations = 0;
		U32 max_itterations = m_uNEdges;

		BOOL edgeExists = FALSE;
		CIFXEdge* e = ea;
		do 
		{
			if(e->Dest2d() == b && e->Org2d() == a)
			{
				e->Qedge()->MakeFixed();
				edgeExists = TRUE;
				// an edge already exists: done.
			}

			if (e->Onext() == ea )
			{
				// (b) is not in the obrit of (a): continue inserting new edge.
				break;
			}
			else
			{
				e = e->Onext();
			}

			if( ++itterations > max_itterations)
			{
				iResult = IFX_E_ABORTED;
				IFXRETURN(iResult);
			}

		} while (IFXSUCCESS(iResult));

		if(edgeExists)
		{
			continue;
		}

		// create the constrained edge.  It is not yet part of the triangulation
		// and is only used to test for intersections with other edges.
		CIFXEdge* proposedEdge;
		iResult = MakeEdge(&proposedEdge);
		if(IFXFAILURE(iResult))
		{
			IFXRETURN(iResult);
		}
		proposedEdge->EndPoints(const_cast<SIFXPoint2d*>(&a), const_cast<SIFXPoint2d*>(&b));

		// check for and edge [a,c] that goes part way from (a) to (b), meaning (c) is on [a,b].
		BOOL partialEdgeExists = FALSE;
		itterations = 0;
		max_itterations = m_uNEdges;
		e = ea;
		do 
		{
			if(Between(e->Dest2d(), proposedEdge) && e->Org2d()==a)
			{
				e->Qedge()->MakeFixed();  // mark the edge [a,c]
				//iResult = IntersectInsertEdge(e->Dest2d(),b);  // insert the edge [c,b]
				//IFXRETURN(iResult);

				SIFXEdge * nextEdge = new SIFXEdge;
				if(nextEdge == NULL)
				{
					IFXASSERT(0);
					iResult = IFX_E_OUT_OF_MEMORY;
					IFXRETURN(iResult);
				}

				nextEdge->a = e->Dest2d();
				nextEdge->b = b;
				error = reinterpret_cast<void **>(stack.Prepend(nextEdge));
				if(error == NULL)
				{
					IFXASSERT(0);
					iResult = IFX_E_OUT_OF_MEMORY;
					IFXRETURN(iResult);
				}

				iResult = DeleteEdge(proposedEdge);
				if(IFXFAILURE(iResult))
				{
					IFXRETURN(iResult);
				}
				partialEdgeExists = TRUE;
				break;
			}

			if (e->Onext() == ea )
			{
				// (b) is not in the orbit of (a): continue inserting new edge.
				break;
			}
			else
			{
				e = e->Onext();
			}

			if( ++itterations > max_itterations)
			{
				iResult = IFX_E_ABORTED;
				IFXRETURN(iResult);
			}

		} while (IFXSUCCESS(iResult));

		if(partialEdgeExists)
		{
			continue;
		}

		// find fist edge in the orbit of (a) that intersects the new edge.
		// orient the edge using the left hand rule with repsect to the point (a).
		itterations = 0;
		max_itterations = m_uNEdges;
		CIFXEdge* cutEdge=NULL;
		e = ea;
		do 
		{
			if
				(
				!(e->Lnext()->Dest2d() == b) &&
				!(e->Lnext()->Org2d() == b) &&
				!(e->Lnext()->Dest2d() == a) &&
				!(e->Lnext()->Org2d() == a) &&
				Intersect( e->Lnext() , proposedEdge)
				)
			{
				cutEdge=e->Lnext();
				if(LeftOf(a,cutEdge))
				{
					// orietent the edge to be counter-clockwise around (a).
					cutEdge = cutEdge->Sym();
				}
				break;
			}

			if (e->Onext() == ea )
			{
				// no edges found to remove - done.
				// sites (a) and (b) exist but are not connected.
				break;
			}
			else
			{
				e = e->Onext();
			}

			if( ++itterations > max_itterations)
			{
				iResult = IFX_E_ABORTED;
				IFXRETURN(iResult);
			}

		} while (IFXSUCCESS(iResult));

		CIFXEdge* fixedEdge = NULL;
		CIFXEdge* leftEdge = cutEdge;

		if(cutEdge == NULL)
		{
			// no cut edge found.
			fixedEdge = NULL;
		}
		else if(cutEdge->Qedge()->IsFixed())
		{
			fixedEdge = cutEdge;
		}
		else
		{
			// the first cut edge found is not fixed.
			// step through the lattice and find an interseced fixed edge
			// until point (b) is reached.
			itterations = 0;
			max_itterations = m_uNEdges;
			while(IFXSUCCESS(iResult))
			{
				if( leftEdge->Dest2d() == b )
					break;
				else if
					(
					!(leftEdge->Dest2d() == b) &&
					!(leftEdge->Org2d() == b) &&
					!(leftEdge->Dest2d() == a) &&
					!(leftEdge->Org2d() == a) &&
					Intersect( leftEdge , proposedEdge)
					)
				{
					if(leftEdge->Qedge()->IsFixed())
					{
						fixedEdge = leftEdge;
						break;
					}
					leftEdge = leftEdge->Onext();
				}
				else
				{
					leftEdge=leftEdge->Rprev();
				}

				if( ++itterations > max_itterations)
				{
					iResult = IFX_E_ABORTED;
					IFXRETURN(iResult);
				}
			}
		}

		if(fixedEdge != NULL)
		{
			SIFXPoint2d x;
			SIFXPoint2d fixedOrg2d;
			SIFXPoint2d fixedDest2d;

			if(IntersectProp(proposedEdge,fixedEdge))
			{
				iResult=SolveIntersection(proposedEdge,fixedEdge,&x);
				if(IFXFAILURE(iResult))
				{
					IFXRETURN(iResult);
				}

				fixedOrg2d = fixedEdge->Org2d();
				fixedDest2d = fixedEdge->Dest2d();

				//CIFXEdge *fixedDestLeft=fixedEdge->Lnext();
				//CIFXEdge *fixedOrgLeft=fixedEdge->Oprev();

				fixedEdge->Qedge()->fixed=FALSE;

				iResult = InsertEdge(fixedOrg2d,x);
				if(IFXFAILURE(iResult))
				{
					IFXRETURN(iResult);
				}

				iResult = InsertEdge(fixedDest2d,x);
				if(IFXFAILURE(iResult))
				{
					IFXRETURN(iResult);
				}

				//Resursive step - add edge to stack.
				//iResult = IntersectInsertEdge(a,x);
				if(IFXFAILURE(iResult))
				{
					IFXRETURN(iResult);
				}
				SIFXEdge * nextEdge = new SIFXEdge;
				if(nextEdge == NULL)
				{
					IFXASSERT(0);
					iResult = IFX_E_OUT_OF_MEMORY;
					IFXRETURN(iResult);
				}
				nextEdge->a = a;
				nextEdge->b = x;
				error = reinterpret_cast<void **>(stack.Prepend(nextEdge));
				if(error == NULL)
				{
					IFXASSERT(0);
					iResult = IFX_E_OUT_OF_MEMORY;
					IFXRETURN(iResult);
				}

				//Resursive step - add edge to stack.
				//iResult = IntersectInsertEdge(x,b);
				/*SIFXEdge * */ nextEdge = new SIFXEdge;
				if(nextEdge == NULL)
				{
					IFXASSERT(0);
					iResult = IFX_E_OUT_OF_MEMORY;
					IFXRETURN(iResult);
				}
				nextEdge->a = x;
				nextEdge->b = b;
				error = reinterpret_cast<void **>(stack.Prepend(nextEdge));
				if(error == NULL)
				{
					IFXASSERT(0);
					iResult = IFX_E_OUT_OF_MEMORY;
					IFXRETURN(iResult);
				}


			}
			else
			{
				// the proposed edge [a,b] overlaps some portion of the fixed edge [c,d].
				// meaning [a,b] and [c,d] are colinear and at least one of the points (a) or (b) is
				// between (c) and (d).

				SIFXPoint2d x1, x2;
				BOOL bOrg = FALSE;
				BOOL bDest = FALSE;

				if(OnEdge(fixedEdge->Org2d(),proposedEdge))
				{
					bOrg=TRUE;
					x1=fixedEdge->Org2d();
				}

				if(OnEdge(fixedEdge->Dest2d(),proposedEdge))
				{
					bDest=TRUE;
					x2=fixedEdge->Dest2d();
				}


				if(bDest==TRUE && bOrg==TRUE)
				{
					F64 d1,d2;

					d1=((x1.x-a.x)*(x1.x-a.x)+(x1.y-a.y)*(x1.y-a.y));
					d2=((x2.x-a.x)*(x2.x-a.x)+(x2.y-a.y)*(x2.y-a.y));

					if(d1<d2)
					{
						x=x1;
					}
					else
					{
						x=x2;
					}

				}
				else if(bDest==TRUE)
				{
					x=x2;
				}
				else if(bOrg==TRUE)
				{
					x=x1;
				}
				else
				{
					// failed
					iResult = IFX_E_ABORTED;
					IFXRETURN(iResult);
				}

				if(bDest==TRUE || bOrg==TRUE)
				{

					iResult = InsertEdge(a,x);
					if(IFXFAILURE(iResult))
					{
						IFXRETURN(iResult);
					}

					SIFXEdge * nextEdge = new SIFXEdge;
					if(nextEdge == NULL)
					{
						IFXASSERT(0);
						iResult = IFX_E_OUT_OF_MEMORY;
						IFXRETURN(iResult);
					}
					nextEdge->a = x;
					nextEdge->b = b;
					error = reinterpret_cast<void **>(stack.Prepend(nextEdge));
					if(error == NULL)
					{
						IFXASSERT(0);
						iResult = IFX_E_OUT_OF_MEMORY;
						IFXRETURN(iResult);
					}
				}
				else
				{
					iResult = IFX_E_ABORTED;
					IFXRETURN(iResult);
				}

			}

		}
		else
		{
			iResult = InsertEdge(a,b);
			if(IFXFAILURE(iResult))
			{
				IFXRETURN(iResult);
			}

		}

		//    cleanup:
		{
			iResult = DeleteEdge(proposedEdge);
			if(IFXFAILURE(iResult))
			{
				IFXRETURN(iResult);
			}
		}

	}

	IFXRETURN(iResult);

}

// Adapted incremental DT algorithm to an incremental CTD algorithm as suggested in
//    Shewchuck, Triangle, Applied Computational Geometry FCRC'96 Workshop Selected Papers (1996) pp. 203-22
// and
//    Anglada, An Improved Incremental Algorithem for Constructing Restricted Delaunay Trianglulations,
//      Compters & Graphics. vol. 21, no.2, March-April 1997, pp 215-23
IFXRESULT CIFXSubdivision::InsertEdge(const SIFXPoint2d& a, const SIFXPoint2d& b)
{

	IFXRESULT iResult = IFX_OK;

	CIFXEdge* ea;
	iResult = InsertSite(a,&ea);
	if(IFXFAILURE(iResult))
	{
		IFXRETURN(iResult);
	}

	// if (a) and (b) are identical, there is no constained edge to insert.
	// simply insert the point (a) and bail.
	if(a == b)
		IFXRETURN(iResult);

	CIFXEdge* eb;
	iResult = InsertSite(b,&eb);
	if(IFXFAILURE(iResult))
	{
		IFXRETURN(iResult);
	}

	U32 itterations = 0;
	U32 max_itterations = m_uNEdges;
	// check if inserting (b) had the side effect of creating edge [a,b].
	CIFXEdge* e = eb;
	do {
		if(e->Org2d() == b && e->Dest2d() == a)
		{
			e->Qedge()->MakeFixed();
			IFXRETURN(iResult);  // an edge already exists: done.
		}

		if (e->Onext() == eb )
		{
			// (b) is not in the obrit of (a): continue inserting new edge.
			break;
		}
		else
		{
			e = e->Onext();
		}

		if( ++itterations > max_itterations)
		{
			iResult = IFX_E_ABORTED;
			IFXRETURN(iResult);
		}

	} while(IFXSUCCESS(iResult));

	// create the constrained edge.  It is not yet part of the triangulation
	// and is only used to test for intersections with other edges.
	CIFXEdge* base;
	iResult = MakeEdge(&base);
	if(IFXFAILURE(iResult))
	{
		IFXRETURN(iResult);
	}
	base->EndPoints(const_cast<SIFXPoint2d*>(&a), const_cast<SIFXPoint2d*>(&b));

	// find fist edge in the orbit of (a) that intersects the new edge.
	// orient the edge using the left hand rule with repsect to the point (a).
	itterations = 0;
	max_itterations = m_uNEdges;
	CIFXEdge* cutEdge=NULL;
	e = ea;
	do {

		if
			(
			!(e->Lnext()->Dest2d() == b) &&
			!(e->Lnext()->Org2d() == b) &&
			!(e->Lnext()->Dest2d() == a) &&
			!(e->Lnext()->Org2d() == a) &&
			Intersect( e->Lnext() , base)
			)
		{
			cutEdge=e->Lnext();
			if(LeftOf(a,cutEdge))
			{
				// orient the edge to be counter-clockwise around (a).
				cutEdge = cutEdge->Sym();
			}
			break;
		}

		if (e->Onext() == ea )
		{
			// no edges found to remove - done.
			break;
		}
		else
		{
			e = e->Onext();
		}

		if( ++itterations > max_itterations)
		{
			iResult = IFX_E_ABORTED;
			IFXRETURN(iResult);
		}


	} while (IFXSUCCESS(iResult));


	IFXList <CIFXEdge> cutEdges;
	CIFXEdge* leftEdge = cutEdge;
	if(cutEdge != NULL)
	{
		IFXListContext context;
		CIFXEdge* edgeNode;

		// step through the lattice and find all other intersected
		// delaunay edges until point (b) is reached.
		itterations = 0;
		max_itterations = m_uNEdges;
		while(IFXSUCCESS(iResult))
		{
			if( leftEdge->Dest2d() == b )
				break;
			else if
				(
				!(leftEdge->Dest2d() == b) &&
				!(leftEdge->Org2d() == b) &&
				!(leftEdge->Dest2d() == a) &&
				!(leftEdge->Org2d() == a) &&
				Intersect( leftEdge , base)
				)
			{
				void **error = NULL;
				error = reinterpret_cast<void **>(cutEdges.Append(leftEdge));
				if(error == NULL)
				{
					IFXASSERT(0);
					iResult = IFX_E_OUT_OF_MEMORY;
					IFXRETURN(iResult);
				}
				leftEdge = leftEdge->Onext();
			}
			else
				leftEdge=leftEdge->Rprev();

			if( ++itterations > max_itterations)
			{
				iResult = IFX_E_ABORTED;
				IFXRETURN(iResult);
			}

		}

		// delete all intersected edges excpet for the fist one.
		cutEdges.ToHead(context);
		cutEdges.PostIncrement(context);
		while((edgeNode = cutEdges.PostIncrement(context)) != NULL)
		{
			iResult = DeleteEdge(edgeNode);
			if(IFXFAILURE(iResult))
			{
				IFXRETURN(iResult);
			}
		}

		CIFXEdge *start;
		CIFXEdge *next;

		// use the fist cut edge to find the face of the
		// pseudopolygon created by deleting the cut edges
		edgeNode=cutEdges.ToHead(context);
		start=edgeNode->Lnext();

		// delete the fist edge now.
		iResult = DeleteEdge(edgeNode);
		if(IFXFAILURE(iResult))
		{
			IFXRETURN(iResult);
		}

		// walk a long the pseudopolygon to find the starting point (a)
		// of the constrained edge [a,b].  We cannot use the edge ea
		// found above because it may have been deleted.
		itterations = 0;
		max_itterations = m_uNEdges;
		next=start->Lnext();
		while(IFXSUCCESS(iResult))
		{
			if(next->Dest2d() == a)
			{
				break;
			}
			if(start == next)
			{
				// should never happen
				//        IFXASSERT(0);
				iResult = IFX_E_ABORTED;
				IFXRETURN(iResult);
			}
			else
			{
				next=next->Lnext();
			}

			if( ++itterations > max_itterations)
			{
				iResult = IFX_E_ABORTED;
				IFXRETURN(iResult);
			}
		}

		ea = next;

		// find starting point (b)
		itterations = 0;
		max_itterations = m_uNEdges;
		next=start->Lnext();
		while(IFXSUCCESS(iResult))
		{
			if(next->Org2d() == b)
			{
				break;
			}
			if(start == next)
			{
				// should never happen
				IFXASSERT(0);
				iResult = IFX_E_ABORTED;
				IFXRETURN(iResult);
			}
			else
			{
				next=next->Lnext();
			}

			if( ++itterations > max_itterations)
			{
				iResult = IFX_E_ABORTED;
				IFXRETURN(iResult);
			}
		}
		eb = next;


		// insert the constrained edge
		CIFXEdge *spine;
		iResult = Connect(ea,eb,&spine);
		if(IFXFAILURE(iResult))
		{
			IFXRETURN(iResult);
		}
		spine->Qedge()->MakeFixed();

		// get rid of temporary edge used to test for intersections
		DeleteEdge(base);

		// triagulate both halves of the pseudopolygon which is now
		// cut in two by the new constrained edge.
		iResult = TriangluatePseudopolygonEdges(spine);
		if(IFXFAILURE(iResult))
		{
			IFXRETURN(iResult);
		}

		iResult = TriangluatePseudopolygonEdges(spine->Sym());
		if(IFXFAILURE(iResult))
		{
			IFXRETURN(iResult);
		}

	}
	else
	{

		// since no edges are cut by [a,b], some edge in the
		// triangluation already goes from (a) to (b).
		// this case was already checked when (b) was inserted.
		// clean up and leave.
		DeleteEdge(base);
	}


	IFXRETURN(iResult);
}


IFXRESULT CIFXSubdivision::TriangluatePseudopolygonEdges(CIFXEdge *pEdge)
{

	IFXRESULT iResult = IFX_OK;

	if(pEdge == NULL)
	{
		iResult = IFX_E_INVALID_POINTER;
		IFXRETURN(iResult);
	}

	IFXList<CIFXEdge> edgeStack;
	IFXListContext context;
	edgeStack.SetAutoDestruct(TRUE);

	// This is a beautiful recursive algorithm.  But, the MAC has
	// such limited stack space, an IFXList is used to create an
	// equivalent iterative algorithm.

	void **error = NULL;
	error = reinterpret_cast<void **>(edgeStack.Prepend(pEdge));
	if(error == NULL)
	{
		IFXASSERT(0);
		iResult = IFX_E_OUT_OF_MEMORY;
		IFXRETURN(iResult);
	}
	edgeStack.ToHead(context);


	CIFXEdge *spine;
	CIFXEdge *c;
	CIFXEdge *v;

	//U32 itterations = 0;
	//U32 max_itterations = (m_uNPoints + m_uNEdges);  // impossible to reach this limit.
	while( (spine = edgeStack.GetHead()) != NULL)
	{

		error = reinterpret_cast<void**>(edgeStack.Remove(spine,context));
		if(error == NULL)
		{
			IFXASSERT(0);
			iResult = IFX_E_ABORTED;
			IFXRETURN(iResult);
		}

		c=spine->Lnext()->Lnext();
		v=c->Lnext();

		if(v == spine)
		{ // left face of (spine) is already a trinagle
			continue;
		}

		if(c == spine)
		{ // degenerate case, (spine) is part of the original pseudopolygon
			// and triagle was added by the parent node in the recursion tree.
			continue;
		}

		// pick the division point
		U32 itterations = 0;
		U32 max_itterations = m_uNEdges;
		while(v != spine)
		{
			if(InCircle(spine->Org2d(),spine->Dest2d(),c->Org2d(),v->Org2d()) )
				c = v;

			v=v->Lnext();

			if(itterations > max_itterations)
			{
				iResult = IFX_E_ABORTED;
				IFXRETURN(iResult);
			}
		}


		// create the new triangle
		if(c == spine->Lnext())
		{
			//already an edge
		}
		else
		{
			iResult = Connect(spine,c);
			if(IFXFAILURE(iResult))
			{
				IFXRETURN(iResult);
			}
		}

		if(c == spine->Lprev())
		{
			//already an edge
		}
		else
		{
			iResult = Connect(spine->Lprev(),c);
			if(IFXFAILURE(iResult))
			{
				IFXRETURN(iResult);
			}
		}


		// look at the remaining parts of the polygon
		// iResult = TriangluatePseudopolygonEdges(spine->Lnext()->Sym());
		// iResult = TriangluatePseudopolygonEdges(spine->Lprev()->Sym());


		error = reinterpret_cast<void**>(edgeStack.Append(spine->Lprev()->Sym()));
		if(error == NULL)
		{
			IFXASSERT(0);
			iResult = IFX_E_ABORTED;
			IFXRETURN(iResult);
		}
		error = reinterpret_cast<void**>(edgeStack.Prepend(spine->Lnext()->Sym()));
		if(error == NULL)
		{
			IFXASSERT(0);
			iResult = IFX_E_ABORTED;
			IFXRETURN(iResult);
		}

		if(++itterations > max_itterations)
		{
			iResult = IFX_E_ABORTED;
			IFXRETURN(iResult);
		}

	}

	IFXRETURN(iResult);

}

IFXRESULT CIFXSubdivision::InsertSite(const SIFXPoint2d& x, CIFXEdge **ppEdge)
// Inserts a new point into a subdivision representing a Delaunay
// triangulation, and fixes the affected edges so that the result
// is still a Delaunay triangulation. This is based on the
// pseudocode from Guibas and Stolfi (1985) p.120, with slight
// modifications and a bug fix.
{

	IFXRESULT iResult = IFX_OK;

	BOOL bEdgeIsFixed = FALSE;
	SIFXPoint2d dest;

	CIFXEdge* e;
	iResult = Locate(x, &e);

	if(IFXFAILURE(iResult))
		IFXRETURN(iResult);

	if( e == NULL) {
		// input point x outside valid domian.
		*ppEdge = NULL;
		IFXRETURN(iResult);
	}
	else if ((x == e->Org2d()))
	{
		// point already exists
		*ppEdge = e;
		IFXRETURN(iResult);
	}
	else if(x == e->Dest2d())
	{
		// point already exists
		*ppEdge = e->Sym();
		IFXRETURN(iResult);
	}
	else if (OnEdge(x, e)) {

		e = e->Oprev();
		if(e->Onext()->Qedge()->IsFixed())
		{
			bEdgeIsFixed = TRUE;
			dest = e->Onext()->Dest2d();
		}

		DeleteEdge(e->Onext());
	}

	// Connect the new point to the vertices of the containing
	// triangle (or quadrilateral, if the new point fell on an
	// existing edge.)
	CIFXEdge* base;
	iResult = MakeEdge(&base);
	if(IFXFAILURE(iResult))
	{
		IFXRETURN(iResult);
	}

	SIFXPoint2d* site = new SIFXPoint2d(x);
	if(site == NULL)
	{
		IFXASSERT(0);
		iResult =IFX_E_OUT_OF_MEMORY;
		IFXRETURN(iResult);
	}

	site->index = m_uNPoints ++;
	void **error = NULL;
	error = reinterpret_cast<void **>(m_pointList.Append(site));
	if(error == NULL)
	{
		IFXASSERT(0);
		iResult=IFX_E_OUT_OF_MEMORY;
		IFXRETURN(iResult);
	}

	base->EndPoints(e->Org(), site);
	if(bEdgeIsFixed == TRUE)
	{
		base->Qedge()->MakeFixed();
	}


	Splice(base, e);
	m_startingEdge = base;
	U32 itterations = 0;
	U32 max_itterations = m_uNEdges;
	do {
		iResult = Connect(e, base->Sym(),&base);
		if(IFXFAILURE(iResult))
		{
			IFXRETURN(iResult);
		}

		if(bEdgeIsFixed == TRUE &&  base->Org2d() == dest)
		{
			base->Qedge()->MakeFixed();
		}
		e = base->Oprev();

		if( itterations > max_itterations)
		{
			iResult = IFX_E_ABORTED;
			IFXRETURN(iResult);
		}
	} while (e->Lnext() != m_startingEdge);


	// Examine suspect edges to ensure that the Delaunay condition
	// is satisfied.
	itterations = 0;
	max_itterations = m_uNEdges;
	do {
		CIFXEdge* t = e->Oprev();
		if (RightOf(t->Dest2d(), e) &&
			InCircle(e->Org2d(), t->Dest2d(), e->Dest2d(), x)
			&& ! e->Qedge()->IsFixed()
			)
		{
			Swap(e);
			e = e->Oprev();

		}
		else if (e->Onext() == m_startingEdge)
		{ // no more suspect edges
			break;
		}
		else
		{ // pop a suspect edge
			e = e->Onext()->Lprev();
		}

		if( itterations > max_itterations)
		{
			iResult = IFX_E_ABORTED;
			IFXRETURN(iResult);
		}
	} while (TRUE);

	*ppEdge = base->Sym();
	IFXRETURN(iResult);
}

/*****************************************************************************/


inline F64 WindingContribution(SIFXEdge &edge, SIFXPoint2d &point)
{

	SIFXPoint2d va(edge.a.x - point.x, edge.a.y - point.y);
	SIFXPoint2d vb(edge.b.x - point.x, edge.b.y - point.y);

	F64 norma=sqrt(va.x*va.x + va.y*va.y);
	F64 normb=sqrt(vb.x*vb.x + vb.y*vb.y);

	F64 theta;

	if(norma*normb == 0.0)
	{
		theta = 0.0;
	}
	else
	{
		F64 area2x = TriArea(point, edge.a, edge.b);

		F64 sinx = area2x/(norma*normb);
		if(sinx>1.0)
			sinx = 1.0;
		else if(sinx<-1.0)
			sinx = -1.0;

		F64 cosx;
		//cosx = dot(va,vb)/(norma*normb);
		cosx = (va.x*vb.x + va.y*vb.y)/(norma*normb);
		if(cosx>1.0)
			cosx=1.0;
		else if(cosx<-1.0)
			cosx=-1.0;

		theta = atan2(sinx,cosx);
	}

	return theta;
}


IFXRESULT CIFXSubdivision::GenerateTriangleList(IFXList<SIFXEdge> &EdgeSet)
{

	IFXRESULT iResult = IFX_OK;

	IFXListContext context;
	m_edgeList.ToHead(context);
	CIFXQuadEdge *node;

	IFXList<SIFXTriangle> localTriList;
	localTriList.SetAutoDestruct(TRUE);


	void **error = NULL;

	while(IFXSUCCESS(iResult) && (node=m_edgeList.PostIncrement(context)) != NULL)
	{
		if(!(node->eflag[0]))
		{

			SIFXTriangle *t = new SIFXTriangle;
			if(t==NULL)
			{
				IFXASSERT(0);
				iResult = IFX_E_OUT_OF_MEMORY;
			}

			node->eflag[0]=TRUE;
			CIFXEdge* e = node->e;
			t->a=e->Org2d();
			e=e->Lnext();
			e->Qedge()->eflag[e->num] = TRUE;
			t->b=e->Org2d();
			e=e->Lnext();
			t->c=e->Org2d();
			e->Qedge()->eflag[e->num] = TRUE;

			error = reinterpret_cast<void **>(localTriList.Append(t));
			if(error == NULL)
			{
				IFXASSERT(0);
				iResult=IFX_E_OUT_OF_MEMORY;
			}

		}

		if(!(node->eflag[2]))
		{
			SIFXTriangle *t = new SIFXTriangle;
			if(t==NULL)
			{
				IFXASSERT(0);
				iResult = IFX_E_OUT_OF_MEMORY;
			}

			node->eflag[2]=TRUE;
			CIFXEdge* e = node->e+2;
			t->a=e->Org2d();
			e=e->Lnext();
			e->Qedge()->eflag[e->num] = TRUE;
			t->b=e->Org2d();
			e=e->Lnext();
			t->c=e->Org2d();
			e->Qedge()->eflag[e->num] = TRUE;

			error = reinterpret_cast<void **>(localTriList.Append(t));
			if(error == NULL)
			{
				IFXASSERT(0);
				iResult=IFX_E_OUT_OF_MEMORY;
			}
		}
	}

	SIFXEdge *EdgeArray = NULL;
	U32 nEdges  = 0;
	if(IFXSUCCESS(iResult) )
	{
		nEdges = EdgeSet.GetNumberElements();
		EdgeArray = new SIFXEdge[nEdges];

		// MM's memory manager returns null for allocations of size 0!
		if(EdgeArray==NULL && nEdges != 0)
		{
			IFXASSERT(0);
			iResult = IFX_E_OUT_OF_MEMORY;
		}

	}

	if(IFXSUCCESS(iResult) )
	{
		SIFXEdge *eNode;
		U32 i=0;
		EdgeSet.ToHead(context);
		while((eNode=EdgeSet.PostIncrement(context)) != NULL)
		{
			EdgeArray[i++] = *eNode;
		}
	}

	if(m_pPointArray)
	{
		delete[] m_pPointArray;
	}

	if(IFXSUCCESS(iResult) )
	{
		//m_uNPoints=m_pointList.GetNumberElements();
		m_pPointArray = new SIFXPoint2d[m_uNPoints];
		// MM's memory manager returns null for allocations of size 0!
		if(m_pPointArray==NULL && m_uNPoints!=0)
		{
			IFXASSERT(0);
			iResult = IFX_E_OUT_OF_MEMORY;
		}
	}

	if(IFXSUCCESS(iResult) )
	{

		SIFXPoint2d *pNode;
		m_pointList.ToHead(context);

		// skip over points which create the domain at initilization.
		m_pointList.PostIncrement(context);
		m_pointList.PostIncrement(context);
		m_pointList.PostIncrement(context);
		m_pointList.PostIncrement(context);

		while((pNode=m_pointList.PostIncrement(context)) != NULL)
		{
			if(pNode->index>=0)
				m_pPointArray[pNode->index] = *pNode;
		}
	}

	F64 theta=0;
	F64 thetaI;
	F64 windingNumber;
	F64 pi = atan(1.0)*4;

	SIFXTriangle *tNode;
	if(IFXSUCCESS(iResult) )
	{
		localTriList.ToHead(context);
	}
	while(IFXSUCCESS(iResult) && (tNode=localTriList.PostIncrement(context)) != NULL)
	{
		SIFXPoint2d cg;

		cg = tNode->a;
		cg.x += tNode->b.x;
		cg.y += tNode->b.y;

		cg.x += tNode->c.x;
		cg.y += tNode->c.y;

		cg.x/=3.0;
		cg.y/=3.0;

		theta=0;
		U32 i;
		for( i=0; i <nEdges; i++)
		{
			thetaI = WindingContribution(EdgeArray[i],cg);
			theta += thetaI;
		}

		windingNumber = theta/(pi*2.0);
		const F64 eps = 1.0e-4;

		if(fabs(windingNumber) <= eps)
		{
		}
		else if(fabs(floor(windingNumber+.5)-windingNumber) < eps)
		{
			SIFXIndexTriangle *tp = new SIFXIndexTriangle;
			if(tp==NULL)
			{
				IFXASSERT(0);
				iResult = IFX_E_OUT_OF_MEMORY;
			}

			tp->ia = tNode->a.index;
			tp->ib = tNode->b.index;
			tp->ic = tNode->c.index;
			error = reinterpret_cast<void **>(m_triList.Append(tp));
			if(error == NULL)
			{
				IFXASSERT(0);
				iResult = IFX_E_OUT_OF_MEMORY;
			}
		}
		else
		{
			//should not go here with good data.
			//leave triangle out of mesh.
			//IFXASSERT(0);
		}

	}

	if(EdgeArray!=NULL)
	{
		delete[] EdgeArray;
	}

	IFXRETURN(iResult);
}
