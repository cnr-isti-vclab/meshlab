/****************************************************************************
 * Rgb Triangulations Plugin                                                 *
 *                                                                           *
 * Author: Daniele Panozzo (daniele.panozzo@gmail.com)                       *
 * Copyright(C) 2007                                                         *
 * DISI - Department of Computer Science                                     *
 * University of Genova                                                      *
 *                                                                           *
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
 ****************************************************************************/

/****************************************************************************
 * The code of the class InteractiveEdit is based on the plugin editpaint,   *
 * developed by Gfrei Andreas.                                               *
 ****************************************************************************/

#include "interactiveEdit.h"
#include "selectiveRefinement.h"

namespace rgbt
{

InteractiveEdit::InteractiveEdit()
{
	pixels = 0;
	first = 1;
}

InteractiveEdit::~InteractiveEdit()
{
}

/** same doubt as above */
/*inline void mergePos(double percent,const float c1[3],const float c2[3], float dest[3]) {
 dest[0]=c1[0]*percent+c2[0]*(1.0-percent);
 dest[1]=c1[1]*percent+c2[1]*(1.0-percent);
 dest[2]=c1[2]*percent+c2[2]*(1.0-percent);
 }
 */

/** transforms a point with a matrix */
/*
 inline void calcCoord(float x,float y,float z,double matrix[],double *xr,double *yr,double *zr) {
 *xr=x*matrix[0]+y*matrix[4]+z*matrix[8]+matrix[12];
 *yr=x*matrix[1]+y*matrix[5]+z*matrix[9]+matrix[13];	
 *zr=x*matrix[2]+y*matrix[6]+z*matrix[10]+matrix[14];
 }
 */
/** checks if a point (2d) is in a rect between p0 and p1 and diameter 2*radius 
 or in the circle (p0,radius) or in the circle (p1.radius) */
//    (Cx-Ax)(Bx-Ax) + (Cy-Ay)(By-Ay)
//r = ------------------------------
//                  L²
inline int isIn(const QPointF &p0, const QPointF &p1, float dx, float dy,
		float radius, float *dist)
{
	if (p0!=p1)
	{ /** this must be checked first, because of the color decrease tool */
		float x2=(p1.x()-p0.x());
		float y2=(p1.y()-p0.y());
		//double l=sqrt(x2*x2+y2*y2);
		float l_square=x2*x2+y2*y2;
		float r=(dx-p0.x())*(p1.x()-p0.x())+(dy-p0.y())*(p1.y()-p0.y());
		//r=r/(l*l);
		r=r/l_square;

		float px=p0.x()+r*(p1.x()-p0.x());
		float py=p0.y()+r*(p1.y()-p0.y());

		px=px-dx;
		py=py-dy;

		if (r>=0 && r<=1 && (px*px+py*py<radius*radius))
		{
			*dist=sqrt(px*px+py*py)/radius;
			return 1;
		}
	}

	// there could be some problem when point is nearer p0 or p1 and viceversa
	// so i have to check both. is only needed with smooth_borders
	bool found=0;
	float x1=(dx-p1.x());
	float y1=(dy-p1.y());
	float bla1=x1*x1+y1*y1;
	if (bla1<radius*radius)
	{
		*dist=sqrt(bla1)/radius;
		found=1;/*return 1;*/
	}

	if (p0==p1)
		return found;

	float x0=(dx-p0.x());
	float y0=(dy-p0.y());
	float bla0=x0*x0+y0*y0;
	if (bla0<radius*radius)
	{
		if (found==1)
			*dist=min((*dist), (float)sqrt(bla0)/radius);
		else
			*dist=sqrt(bla0)/radius;
		return 1;
	}

	return found;
}

/** checks if a point is in a triangle (2D) */
inline bool pointInTriangle(const QPointF &p, const QPointF &a,
		const QPointF &b, const QPointF &c)
{
	float fab=(p.y()-a.y())*(b.x()-a.x()) - (p.x()-a.x())*(b.y()-a.y());
	float fbc=(p.y()-c.y())*(a.x()-c.x()) - (p.x()-c.x())*(a.y()-c.y());
	float fca=(p.y()-b.y())*(c.x()-b.x()) - (p.x()-b.x())*(c.y()-b.y());
	if (fab*fbc>0 && fbc*fca>0)
		return true;
	return false;
}

inline bool pointInTriangle(const float p_x, const float p_y, const float a_x,
		const float a_y, const float b_x, const float b_y, const float c_x,
		const float c_y)
{
	float fab=(p_y-a_y)*(b_x-a_x) - (p_x-a_x)*(b_y-a_y);
	float fbc=(p_y-c_y)*(a_x-c_x) - (p_x-c_x)*(a_y-c_y);
	float fca=(p_y-b_y)*(c_x-b_x) - (p_x-b_x)*(c_y-b_y);
	if (fab*fbc>0 && fbc*fca>0)
		return true;
	return false;
}

/** checks if a triangle is front or backfaced */
inline bool isFront(const QPointF &a, const QPointF &b, const QPointF &c)
{
	return (b.x()-a.x())*(c.y()-a.y())-(b.y()-a.y())*(c.x()-a.x())>0;
}

/** checks if a triangle is front or backfaced */
inline bool isFront(const float a_x, const float a_y, const float b_x,
		const float b_y, const float c_x, const float c_y)
{
	return (b_x-a_x)*(c_y-a_y)-(b_y-a_y)*(c_x-a_x)>0;
}

inline bool lineHitsCircle(QPointF& LineStart, QPointF& LineEnd,
		QPointF& CircleCenter, float Radius, QPointF* const pOut = 0)
{
	const float RadiusSq = Radius * Radius;
	QPointF PMinusM =LineStart - CircleCenter;

	float pm_squ=PMinusM.x()*PMinusM.x()+PMinusM.y()*PMinusM.y();
	if (pm_squ <= RadiusSq)
	{ /// startpoint in circle
		if (pOut)
			*pOut = LineStart;
		return true;
	}
	QPointF LineDir=LineEnd - LineStart; /// line direction
	// u * (p - m) 
	const float UDotPMinusM = LineDir.x()*PMinusM.x()+LineDir.y()*PMinusM.y();//Vector2D_Dot(LineDir, PMinusM);
	// u*u
	const float LineDirSq = LineDir.x()*LineDir.x()+LineDir.y()*LineDir.y();
	//   (u * (p - m))^2
	// - (u*u * ((p - m)^2 - r^2)) 
	const float d = UDotPMinusM * UDotPMinusM - LineDirSq * (pm_squ - RadiusSq);

	if (d < 0.0f)
		return false;
	else if (d < 0.0001f)
	{
		const float s = -UDotPMinusM / LineDirSq;
		if (s< 0.0f || s> 1.0f)return false;
		else
		{
			if(pOut) *pOut = LineStart + s * LineDir;
			return true;
		}
	}
	else
	{
		const float s = (-UDotPMinusM - sqrtf(d)) / LineDirSq;
		if(s < 0.0f || s > 1.0f) return false;
		else
		{
			if(pOut) *pOut = LineStart + s * LineDir;
			return true;
		}
	}
}

void InteractiveEdit::DrawXORCircle(MeshModel &m,GLArea * gla, bool doubleDraw)
{
	int PEZ=18;
	/** paint the normal circle in pixel-mode */
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0,gla->curSiz.width(),gla->curSiz.height(),0,-1,1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_LOGIC_OP);
	glLogicOp(GL_XOR);
	glColor3f(1,1,1);

	QPoint mid= QPoint(cur.x(),/*gla->curSiz.height()-*/cur.y());
	if(doubleDraw)
	{
		glBegin(GL_LINE_LOOP);
		for (int lauf=0; lauf<PEZ; lauf++)
		glVertex2f(mid.x()+sin(M_PI*(float)lauf/9.0)*pen.radius,mid.y()+cos(M_PI*(float)lauf/9.0)*pen.radius);
		glEnd();
	}

	glBegin(GL_LINE_LOOP);
	for (int lauf=0; lauf<PEZ; lauf++)
	glVertex2f(mid.x()+sin(M_PI*(float)lauf/9.0)*pen.radius,mid.y()+cos(M_PI*(float)lauf/9.0)*pen.radius);
	glEnd();

	glDisable(GL_LOGIC_OP);
	// Closing 2D
	glPopAttrib();
	glPopMatrix(); // restore modelview
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

}

//getInternFaces(m,&curSel,&newSel,&faceSel,gla,pen,cur,prev,pixels,mvmatrix,projmatrix,viewport);
/** finds the faces or vertexes in the circle */
void InteractiveEdit::getInternFaces(MeshModel & m,list<int> *actual,vector<Vert_Data> * risult, vector<CMeshO::FacePointer> * face_risult,
		GLArea * gla,Penn &pen,QPoint &cur, QPoint &prev, GLfloat * pixels,
		double mvmatrix[16],double projmatrix[16],GLint viewport[4])
{

	QHash <CFaceO *,CFaceO *> selected;
	QHash <CVertexO *,CVertexO *> sel_vert;
	list<int>::iterator fpi;
	vector<CMeshO::FacePointer> temp_po;

	if (actual->size()==0)
	{
		CMeshO::FaceIterator fi;
		for(fi=m.cm.face.begin();fi!=m.cm.face.end();++fi)
		if(!(*fi).IsD())
		{
			temp_po.push_back((&*fi));
		}
	}
	else
	for(fpi=actual->begin();fpi!=actual->end();++fpi)
	{
		temp_po.push_back(&(m.cm.face[*fpi]));
	}

	actual->clear();

	QPointF mid=QPointF(cur.x(),gla->curSiz.height()- cur.y());
	QPointF mid_prev=QPointF(prev.x(),gla->curSiz.height()- prev.y());

	QPointF p[3],z[3];
	double tx,ty,tz;

	bool backface=pen.backface;
	bool invisible=pen.invisible;
	for (unsigned int lauf2=0; lauf2<temp_po.size(); lauf2++)
	{
		CFaceO * fac=temp_po.at(lauf2);
		//std::cout << fac->IsD() << std::endl;
		int intern=0;
		int checkable=0; /// to avoid problems when the pen is smaller then the polys
		for (int lauf=0; lauf<3; lauf++)
		{
			//if ((fac)->V(lauf) - &(m.cm.vert[0]) < 0 || (fac)->V(lauf) - &(m.cm.vert[0]) >= m.cm.vert.size())
			//	continue;
			if (gluProject((fac)->V(lauf)->P()[0],(fac)->V(lauf)->P()[1],(fac)->V(lauf)->P()[2],
							mvmatrix,projmatrix,viewport,&tx,&ty,&tz)==GL_TRUE)
			{
				checkable++;
			}
			if (tz<0 || tz>1)
			{
				checkable--;
			}
			p[lauf].setX(tx); p[lauf].setY(ty);
			if (tx>=0 && tx<viewport[2] && ty>=0 && ty<viewport[3])
			{
				z[lauf].setX(tz);
				z[lauf].setY((float)pixels[(int)(((int)ty)*viewport[2]+(int)tx)]);
			}
			else
			{
				z[lauf].setX(1); z[lauf].setY(0);
			}
		}
		if (backface || isFront(p[0],p[1],p[2]))
		{
			//checkable++;
			/// could be useful to calc the medium of z in the matrix
			for (int lauf=0; lauf<3; lauf++)
			{
				if (invisible || (z[lauf].x()<=z[lauf].y()+0.003))
				{
					float dist;
					if (isIn(mid,mid_prev,p[lauf].x(),p[lauf].y(),pen.radius,&dist)==1)
					{
						intern=1;
						if (!sel_vert.contains(fac->V(lauf)))
						{
							Vert_Data d;
							d.v=fac->V(lauf);
							d.distance=dist;
							risult->push_back(/*fac->V(lauf)*/d);
							sel_vert.insert(fac->V(lauf),fac->V(lauf));
						}
					}
					QPointF pos_res;
					//if (pen.paintutensil==SELECT && intern==0 && lineHitsCircle(p[lauf],p[(lauf+1)%3],mid,pen.radius,&pos_res))
					if (intern==0 && lineHitsCircle(p[lauf],p[(lauf+1)%3],mid,pen.radius,&pos_res))
					{
						intern=1; continue;
					}
				}
			}
			if (checkable==3 && intern==0 && (pointInTriangle(mid,p[0],p[1],p[2]) || pointInTriangle(mid_prev,p[0],p[1],p[2])))
			{
				intern=-1;
			}
		}
		if (checkable==3 && intern!=0 && !selected.contains(fac))
		{
			selected.insert((fac),(fac));
			actual->push_back(fac->Index());
			vector <CFaceO *> surround;
			for (int lauf=0; lauf<3; lauf++)
				if (!selected.contains(fac->FFp(lauf)))
					temp_po.push_back(fac->FFp(lauf));

			if (intern!=0)
			face_risult->push_back(fac);
		}
	}
}

RgbInteractiveEdit::RgbInteractiveEdit(CMeshO& m, RgbInfo& i, TopologicalOpC& to)
{
	this->m = &m;
	this->info = &i;
	this->to = &to;
}
RgbInteractiveEdit::~RgbInteractiveEdit()
{

}

bool RgbInteractiveEdit::vertexToRemove(RgbVertexC& v, int* level, double* lenght)
{
	bool blenght;
	bool blevel;
	
	if (lenght)
	{
		double edgelenght = SelectiveRefinement::maxEdge(v);
		blenght = edgelenght < *lenght;
	}
	else
		blenght = false;
	
	if (level)
	{
		//int edgelevel = maxEdgeLevel(v);
		int edgelevel = v.getLevel();
		blevel = edgelevel > *level;
	}
	else
		blevel = false;
	//std::cout << blenght << " " << blevel << std::endl;
	return (blenght || blevel);

}
bool RgbInteractiveEdit::edgeToSplit(RgbTriangleC& t,int index, int* level, double* lenght)
{
	bool blenght;
	bool blevel;
	
	if (lenght)
	{
		double edgelenght = SelectiveRefinement::edgeLenght(t,index);
		blenght = edgelenght > *lenght;
	}
	else
		blenght = false;
	
	if (level)
	{
		int edgelevel = t.getEdgeLevel(index);
		blevel = edgelevel < *level;
	}
	else
		blevel = false;
	
	return (blenght || blevel);
}
void RgbInteractiveEdit::processVertex(int v, int* level, double* lenght)
{
	RgbTriangleC t;
	int index;
	if (SelectiveRefinement::IsValidVertex(v,m,info,&t,&index,true))
	{
		if (vertexToRemove(t.V(index),level,lenght))
		{
			if (RgbPrimitives::vertexRemoval_Possible(t,index))
			{
				//vector<RgbTriangleC> vt;
				//vt.reserve(4);
				RgbPrimitives::vertexRemoval(t,index,*to);
			}
		}
	}
}

void RgbInteractiveEdit::processEdge(int v1,int v2, int* level, double* lenght)
{
	RgbTriangleC t;
	int index;

	if (SelectiveRefinement::IsValidEdge(v1,v2,m,info,&t,&index))
	{
		if (edgeToSplit(t,index,level,lenght))
		{
			vector<RgbTriangleC> vt;
			RgbPrimitives::recursiveEdgeSplit(t,index,*to,&vt);
		}
	}
}

int RgbInteractiveEdit::minEdgeLevel(RgbVertexC& v)
{
	vector<RgbEdgeC> ve;
	vector<RgbEdgeC>::iterator it;
	v.VF(ve);
	int tmp = ve[0].t.getEdgeLevel(ve[0].index);
	for (it = ve.begin();it != ve.end(); it++)
	{
		int l = it->t.getEdgeLevel(it->index);
		if (tmp > l)
			tmp = l;
	}
	return tmp;
}

bool RgbInteractiveEdit::maxEdgeLevel(RgbVertexC& v)
{
	vector<RgbEdgeC> ve;
	vector<RgbEdgeC>::iterator it;
	v.VF(ve);
	std::cout << ve.size() << std::endl;
	int tmp = ve[0].t.getEdgeLevel(ve[0].index);
	for (it = ve.begin();it != ve.end(); it++)
	{
		int l = it->t.getEdgeLevel(it->index);
		if (tmp > l)
			tmp = l;
	}
	return tmp;
}

}