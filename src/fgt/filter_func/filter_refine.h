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

#include <common/meshmodel.h>

using namespace vcg;
using namespace mu;

// MidPoint Function Object used by RefineE() to find new vertex position on splitted edge
// Sets new attributes values on created vertex.
template<class MESH_TYPE>
struct MidPointCustom : public std::unary_function<face::Pos<typename MESH_TYPE::FaceType> , typename MESH_TYPE::CoordType >
{
public :
	MidPointCustom(std::string expr1, std::string expr2, std::string expr3, bool &error, std::string &msg)
	{
		// expr1, expr2, expr3 are parameters for x,y and z coord respectively
		this->p1.SetExpr(expr1);
		this->p2.SetExpr(expr2);
		this->p3.SetExpr(expr3);

		this->setVars(p1);
		this->setVars(p2);
		this->setVars(p3);
		this->varDefined = false;

		// this piece of code is used only to test expressions and 
		//  return a message in case of muparser's error
		try {
			p1.Eval();
			p2.Eval();
			p3.Eval();
		} catch(Parser::exception_type &e) {
			msg = e.GetMsg().c_str();
			error = true;
		}
	}

	void operator()(typename MESH_TYPE::VertexType &nv, face::Pos<typename MESH_TYPE::FaceType> ep)
	{
		// set variables only the first time. parser doesn't work otherwise
		if(!varDefined)
		{
			this->setVars(p1);
			this->setVars(p2);
			this->setVars(p3);
			this->varDefined = true;
		}
		
		// first and second vertex of current edge
		x0 = ep.V()->P()[0];
		y0 = ep.V()->P()[1];
		z0 = ep.V()->P()[2];

		x1 = ep.VFlip()->P()[0];
		y1 = ep.VFlip()->P()[1];
		z1 = ep.VFlip()->P()[2];
	
		// new midpoint
		nv.P() = Point3f(p1.Eval(),p2.Eval(),p3.Eval());


		if( MESH_TYPE::HasPerVertexColor())
		{
			// calculate new R,G,B component for midpoint based on the distance of two vertices
			// and color distance.
			float distance_r = math::Abs((float)ep.V()->C()[0] - (float)ep.VFlip()->C()[0]);
			float distance_g = math::Abs((float)ep.V()->C()[1] - (float)ep.VFlip()->C()[1]);
			float distance_b = math::Abs((float)ep.V()->C()[2] - (float)ep.VFlip()->C()[2]);
			double clength = Distance(ep.V()->P(), ep.VFlip()->P());

			double r = Distance(ep.V()->P(),nv.P()) * (distance_r / clength);
			double g = Distance(ep.V()->P(),nv.P()) * (distance_g / clength);
			double b = Distance(ep.V()->P(),nv.P()) * (distance_b / clength);

			// set R component
			if(ep.V()->C()[0] < ep.VFlip()->C()[0])
				nv.C()[0] = ep.V()->C()[0] +r;
			else nv.C()[0] = ep.V()->C()[0] -r;

			// set G component
			if(ep.V()->C()[1] < ep.VFlip()->C()[1])
				nv.C()[1] = ep.V()->C()[1] +g;
			else nv.C()[1] = ep.V()->C()[1] -g;

			// set B component
			if(ep.V()->C()[2] < ep.VFlip()->C()[2])
				nv.C()[2] = ep.V()->C()[2] +b;
			else nv.C()[2] = ep.V()->C()[2] -b;
		}
		
		if( MESH_TYPE::HasPerVertexQuality()) 
		{
			// calculate new quality for midpoint based on the distance of two vertices
			// and quality distance
			double f = Distance(ep.V()->P(),nv.P()) * 
				(math::Abs(ep.V()->Q() - ep.VFlip()->Q()) / Distance(ep.V()->P(), ep.VFlip()->P()));

			if(ep.V()->Q() < ep.VFlip()->Q())
				nv.Q() = ep.V()->Q() + f;
			else nv.Q() = ep.V()->Q() - f;

		}
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
	Parser p1,p2,p3;
	bool varDefined;
	double x0,y0,z0,x1,y1,z1;

	// parser variables
	void setVars(Parser &p) 
	{
		p.DefineVar("x0", &x0);
		p.DefineVar("y0", &y0);
		p.DefineVar("z0", &z0);
		p.DefineVar("x1", &x1);
		p.DefineVar("y1", &y1);
		p.DefineVar("z1", &z1);
	}
};

// Edge pred function object used by RefineE() to choose which edges are selected to refine.
// expr is a string that represents a boolean function evaluated by muparser
template <class MESH_TYPE>
class CustomEdge
{
public:
	CustomEdge(std::string expr, bool &error, std::string &msg)
	{
		// set expression and variables needed by parser
		this->p.SetExpr(expr);
		this->setVars(p);
		this->varDefined = false;
		
		// check error
		try {
			p.Eval();
		} catch(Parser::exception_type &e) {
			msg = e.GetMsg().c_str();
			error = true;
		}
	}

	bool operator()(face::Pos<typename MESH_TYPE::FaceType> ep)
	{
		// set variables only the first time. parser doesn't work otherwise
		if(!varDefined)
		{
			this->setVars(p);
			this->varDefined = true;
		}
		
    setVarVal(ep);
		
		bool ret = p.Eval();
		ep.FlipV();
		setVarVal(ep);
		ret = ret | bool(p.Eval());
		
		qDebug("Eval returned %i (%f %f",ret?1:0,x0,x1);
		return ret;
}
		
void setVarVal(face::Pos<typename MESH_TYPE::FaceType> &ep)
{
		// parser variables are related to vertex attributes
		// set coords, normals, quality for the two edge's vertices
		x0 = ep.V()->P()[0];
		y0 = ep.V()->P()[1];
		z0 = ep.V()->P()[2];

		x1 = ep.VFlip()->P()[0];
		y1 = ep.VFlip()->P()[1];
		z1 = ep.VFlip()->P()[2];

		nx0 = ep.V()->N()[0];
		ny0 = ep.V()->N()[1];
		nz0 = ep.V()->N()[2];

		nx1 = ep.VFlip()->N()[0];
		ny1 = ep.VFlip()->N()[1];
		nz1 = ep.VFlip()->N()[2];

		r0 = ep.V()->C()[0];
		g0 = ep.V()->C()[1];
		b0 = ep.V()->C()[2];

		r1 = ep.VFlip()->C()[0];
		g1 = ep.VFlip()->C()[1];
		b1 = ep.VFlip()->C()[2];

		q0 = ep.V()->Q();
		q1 = ep.VFlip()->Q();
	}
protected:
	Parser p;
	bool varDefined;
	double x0,y0,z0,x1,y1,z1,nx0,ny0,nz0,nx1,ny1,nz1,r0,g0,b0,r1,g1,b1,q0,q1;

	// parser variables
	void setVars(Parser &p)
	{
		p.DefineVar("x0", &x0);
		p.DefineVar("y0", &y0);
		p.DefineVar("z0", &z0);
		p.DefineVar("x1", &x1);
		p.DefineVar("y1", &y1);
		p.DefineVar("z1", &z1);

		p.DefineVar("nx0", &nx0);
		p.DefineVar("ny0", &ny0);
		p.DefineVar("nz0", &nz0);
			
		p.DefineVar("nx1", &nx1);
		p.DefineVar("ny1", &ny1);
		p.DefineVar("nz1", &nz1);

		p.DefineVar("r0", &r0);
		p.DefineVar("g0", &g0);
		p.DefineVar("b0", &b0);

		p.DefineVar("r1", &r1);
		p.DefineVar("g1", &g1);
		p.DefineVar("b1", &b1);

		p.DefineVar("q0", &q0);
		p.DefineVar("q1", &q1);
	}
};