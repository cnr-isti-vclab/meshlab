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
/****************************************************************************

//TODO there could be some IsD() missing
//TODO in some cases bits instead of hashtables would be better
//TODO optimize undo to not include when new and old colors are the same
//TODO color smooth has some problems
//TODO bright colors problem
//TODO bug when pen smaller than polys and fast mouse movement .... WORKS BETTER BUT NOT SOLVED
################################
//TODO paint selection selects some wrong piece: SOLVED
//TODO opengl line width after gradient: SOLVED
//TODO PaintToolbox does not close: SOLVED
//TODO remove FF or VF: SOLVED removed FF
//TODO error with undo and color smooth: SOLVED
*/

/*****************************************************************************
first version: 0.1 
autor: Gfrei Andreas 
date:  07/02/2007 
email: gfrei.andreas@gmx.net

The first version contains:
---------PAINTING------------
*) Painting with pixel & percentual pen & smooth borders & opacity
*) Fill
*) Gradient with linear and circular fill
*) Color smooth
---------EDITING-------------
*) Face selection with the pen
*) Face smooth with the pen (smooth borders and quantity selectable)
---------CONFIG--------------
*) fast or slow and accurate face search
*****************************************************************************/
#include <QtGui>

#include <math.h>
#include <stdlib.h>
#include <meshlab/glarea.h>
#include "editpaint.h"
#include <stdio.h>
#include <wrap/gl/pick.h>
#include <limits>

#include<vcg/complex/algorithms/update/topology.h>
#include<vcg/complex/algorithms/update/bounding.h>
//#include <vcg/space/triangle3.h>

using namespace std;
using namespace vcg;

EditPaintPlugin::EditPaintPlugin() {
	isDragging=false;
	first=true;
	paintbox=0;
	pixels=0;
	pressed=0;
}

/** the destructor is never called */
EditPaintPlugin::~EditPaintPlugin() {
	if (paintbox!=0) { delete paintbox; paintbox=0; }
	qDebug() << "~EditPaint" << endl;
}

const QString EditPaintPlugin::Info() {
	return tr("Paint on your mesh, select and smooth faces, all with a pen-like tool.");
}

/** an undo==1 or a redo was called */
void EditPaintPlugin::undo(int value) {
	if (!color_undo.contains(current_gla)) {
		color_undo.insert(current_gla,new ColorUndo());
	}
	if (value==1) color_undo[current_gla]->undo();
	else color_undo[current_gla]->redo();
	current_gla->update();
	paintbox->setUndo(color_undo[current_gla]->hasUndo());
	paintbox->setRedo(color_undo[current_gla]->hasRedo());
}

void EditPaintPlugin::StartEdit(MeshModel &m, GLArea * parent) {
	parent->setCursor(QCursor(QPixmap(":/images/cursor_paint.png"),1,1));	
    first=true;
	pressed=0;
	tri::UpdateBounding<CMeshO>::Box(m.cm);
	if (paintbox==0) { 
		paintbox=new PaintToolbox(parent->window());
		paint_dock=new QDockWidget(parent->window());
		paint_dock->setAllowedAreas(Qt::NoDockWidgetArea);
		paint_dock->setWidget(paintbox);
		QPoint p=parent->window()->mapToGlobal(QPoint(0,0));
		paint_dock->setGeometry(-5+p.x()+parent->window()->width()-paintbox->width(),p.y(),/*width()*/paintbox->width(),paintbox->height());
		paint_dock->setFloating(true);
		//qDebug() << parent->parentWidget()->parentWidget()->window()->windowTitle() << endl;
		QObject::connect(paintbox, SIGNAL(undo_redo(int)),this, SLOT(undo(int)));
	}
	paint_dock->setVisible(true);
	paint_dock->layout()->update();
	//m.updateDataMask(MeshModel::MM_FACEFACETOPO);
	m.updateDataMask(MeshModel::MM_VERTFACETOPO | MeshModel::MM_VERTMARK );

	parent->getCurrentRenderMode().colorMode=vcg::GLW::CMPerVert;
	parent->mm()->ioMask|=MeshModel::IOM_VERTCOLOR;
	parent->mm()->ioMask|=MeshModel::IOM_VERTQUALITY;
	
	curSel.clear();
	parent->update();

	current_gla=parent;
	if (!color_undo.contains(parent)) {
		color_undo.insert(parent,new ColorUndo());
		paintbox->setUndo(false);
		paintbox->setRedo(false);
	} else {
		paintbox->setUndo(color_undo[parent]->hasUndo());
		paintbox->setRedo(color_undo[parent]->hasRedo());
	}
}

// this is called only when we change editor or we want to close it.
void EditPaintPlugin::EndEdit(MeshModel &/*m*/, GLArea * /*parent*/) {
	qDebug() <<"EditPaintPlugin::ENDEDIT"<<endl;
	if (paintbox!=0) { 
			delete paintbox; 
			delete paint_dock; 
			paintbox=0;
			paint_dock=0;
	 }
}

void EditPaintPlugin::mousePressEvent(QMouseEvent * event, MeshModel &m, GLArea * gla) {
	//qDebug() << "pressStart" << endl;
	has_track=gla->isTrackBallVisible();
	gla->showTrackBall(false);
	first=true;
	pressed=1;
	isDragging = true;
	visited_vertexes.clear();
	start=event->pos();
	cur=start;
	prev=start;
	inverse_y=gla->curSiz.height()-cur.y();
	curr_mouse=event->button();
	current_gla=gla;

	pen.painttype=paintbox->paintType();
	pen.paintutensil=paintbox->paintUtensil();
	pen.backface=paintbox->getPaintBackface();
	pen.invisible=paintbox->getPaintInvisible();
	switch (paintbox->paintType()) {
		case 1: { pen.radius=paintbox->getRadius()*0.5; } break;
		case 2: { pen.radius=paintbox->getRadius()*m.cm.bbox.Diag()*0.01*0.5; } break;
		case 3: { pen.radius=paintbox->getRadius()*m.cm.bbox.DimY()*0.01*0.5; } break;
		case 4: { pen.radius=paintbox->getRadius()*0.5; } break;
	}
	curSel.clear();
}

void EditPaintPlugin::mouseMoveEvent(QMouseEvent * event, MeshModel &/*m*/, GLArea * gla) {
	if (!isDragging) prev=cur; /** to prevent losses when two mouseEvents occur befor one decorate */
	cur=event->pos();
	isDragging = true;
	gla->update();
}

void EditPaintPlugin::pushUndo(GLArea * gla) {
	if (!color_undo.contains(gla)) {
		color_undo.insert(gla,new ColorUndo());
	} else {}
	color_undo[gla]->pushUndo();
	paintbox->setUndo(color_undo[gla]->hasUndo());
	paintbox->setRedo(color_undo[gla]->hasRedo());
}
  
void EditPaintPlugin::mouseReleaseEvent(QMouseEvent * event, MeshModel &m, GLArea * gla) {
	gla->showTrackBall(has_track);
	pushUndo(gla);
	visited_vertexes.clear();
	gla->update();
	prev=cur;
	cur=event->pos();
	pressed=2;
	isDragging=false;
	switch (paintbox->paintUtensil()) {
		case FILL: { } break;
		case PICK: { } break;
		case PEN: { } break;
		case GRADIENT: { isDragging=true; gla->update(); } break;
	}

}

/** colorizes a vertex with the new color.
  i'm not shure if the function is correct */
inline void colorize(CVertexO * vertice,const Color4b& newcol,int opac) {
	Color4b orig=vertice->C();
	orig[0]=min(255,((newcol[0]-orig[0])*opac+orig[0]*(100))/100);
	orig[1]=min(255,((newcol[1]-orig[1])*opac+orig[1]*(100))/100);
	orig[2]=min(255,((newcol[2]-orig[2])*opac+orig[2]*(100))/100);
	orig[3]=min(255,((newcol[3]-orig[3])*opac+orig[3]*(100))/100);
	vertice->C()=orig;
}

/** same doubt as above */
inline void mergeColor(double percent,const Color4b& c1,const Color4b& c2,Color4b* dest) {
	(*dest)[0]=min(255.0,((c1[0]-c2[0])*percent+c2[0]));//(*dest)[0]=min(255.0,(c1[0]*percent+c2[0]*(1.0-percent)));
	(*dest)[1]=min(255.0,((c1[1]-c2[1])*percent+c2[1]));//(*dest)[1]=min(255.0,(c1[1]*percent+c2[1]*(1.0-percent)));
	(*dest)[2]=min(255.0,((c1[2]-c2[2])*percent+c2[2]));//(*dest)[2]=min(255.0,(c1[2]*percent+c2[2]*(1.0-percent)));
	(*dest)[3]=min(255.0,((c1[3]-c2[3])*percent+c2[3]));//(*dest)[3]=min(255.0,(c1[3]*percent+c2[3]*(1.0-percent)));
}

/** same doubt as above */
inline void mergePos(double percent,const float c1[3],const float c2[3], float dest[3]) {
	dest[0]=c1[0]*percent+c2[0]*(1.0-percent);
	dest[1]=c1[1]*percent+c2[1]*(1.0-percent);
	dest[2]=c1[2]*percent+c2[2]*(1.0-percent);
}

/** transforms a point with a matrix */
inline void calcCoord(float x,float y,float z,double matrix[],double *xr,double *yr,double *zr) {
	*xr=x*matrix[0]+y*matrix[4]+z*matrix[8]+matrix[12];
	*yr=x*matrix[1]+y*matrix[5]+z*matrix[9]+matrix[13];	
	*zr=x*matrix[2]+y*matrix[6]+z*matrix[10]+matrix[14];
}

/** checks if a point (2d) is in a rect between p0 and p1 and diameter 2*radius 
    or in the circle (p0,radius) or in the circle (p1.radius) */
//    (Cx-Ax)(Bx-Ax) + (Cy-Ay)(By-Ay)
//r = ------------------------------
//                  L²
inline int isIn(const QPointF &p0,const QPointF &p1,float dx,float dy,float radius,float *dist) {
	if (p0!=p1) { /** this must be checked first, because of the color decrease tool */
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
	
		if (r>=0 && r<=1 && (px*px+py*py<radius*radius)) { *dist=sqrt(px*px+py*py)/radius; return 1; }
	}

	// there could be some problem when point is nearer p0 or p1 and viceversa
	// so i have to check both. is only needed with smooth_borders
	bool found=0;
	float x1=(dx-p1.x());
	float y1=(dy-p1.y());
	float bla1=x1*x1+y1*y1;
	if (bla1<radius*radius) { *dist=sqrt(bla1)/radius; found=1;/*return 1;*/}

	if (p0==p1) return found;

	float x0=(dx-p0.x());
	float y0=(dy-p0.y());
	float bla0=x0*x0+y0*y0;
	if (bla0<radius*radius) { 
		if (found==1) *dist=min((*dist),(float)sqrt(bla0)/radius);
		else *dist=sqrt(bla0)/radius;
		return 1;
	}

	return found;
}

/** checks if a point is in a triangle (2D) */
inline bool pointInTriangle(const QPointF &p,const QPointF &a,const QPointF &b,const QPointF &c) {
	float fab=(p.y()-a.y())*(b.x()-a.x()) - (p.x()-a.x())*(b.y()-a.y());
	float fbc=(p.y()-c.y())*(a.x()-c.x()) - (p.x()-c.x())*(a.y()-c.y());
	float fca=(p.y()-b.y())*(c.x()-b.x()) - (p.x()-b.x())*(c.y()-b.y());
	if (fab*fbc>0 && fbc*fca>0) return true;
	return false;
}

inline bool pointInTriangle(const float p_x,const float p_y,const float a_x,const float a_y,const float b_x,const float b_y,const float c_x,const float c_y) {
	float fab=(p_y-a_y)*(b_x-a_x) - (p_x-a_x)*(b_y-a_y);
	float fbc=(p_y-c_y)*(a_x-c_x) - (p_x-c_x)*(a_y-c_y);
	float fca=(p_y-b_y)*(c_x-b_x) - (p_x-b_x)*(c_y-b_y);
	if (fab*fbc>0 && fbc*fca>0) return true;
	return false;
}

/** checks if a triangle is front or backfaced */
inline bool isFront(const QPointF &a,const QPointF &b,const QPointF &c) {
	return (b.x()-a.x())*(c.y()-a.y())-(b.y()-a.y())*(c.x()-a.x())>0;
}

/** checks if a triangle is front or backfaced */
inline bool isFront(const float a_x, const float a_y, const float b_x, const float b_y, const float c_x, const float c_y) {
	return (b_x-a_x)*(c_y-a_y)-(b_y-a_y)*(c_x-a_x)>0;
}

inline bool lineHitsCircle(QPointF& LineStart,QPointF& LineEnd,QPointF& CircleCenter, float Radius, QPointF* const pOut = 0) {
	const float RadiusSq = Radius * Radius;
	QPointF PMinusM =LineStart - CircleCenter;
	
	float pm_squ=PMinusM.x()*PMinusM.x()+PMinusM.y()*PMinusM.y();
	if(pm_squ <= RadiusSq) { /// startpoint in circle
		if(pOut) *pOut = LineStart;
		return true;
	}
	QPointF LineDir=LineEnd - LineStart; /// line direction
	// u * (p - m) 
	const float UDotPMinusM = LineDir.x()*PMinusM.x()+LineDir.y()*PMinusM.y();//Vector2D_Dot(LineDir, PMinusM);
	// u*u
	const float LineDirSq = LineDir.x()*LineDir.x()+LineDir.y()*LineDir.y();
	//   (u * (p - m))^2
	// - (u*u * ((p - m)^2 - r^2)) 
	const float d =   UDotPMinusM * UDotPMinusM - LineDirSq * (pm_squ - RadiusSq);
	
	if(d < 0.0f) return false;
	else if(d < 0.0001f) {
	const float s = -UDotPMinusM / LineDirSq;
	if(s < 0.0f || s > 1.0f) return false;
		else {
			if(pOut) *pOut = LineStart + s * LineDir;
			return true;
		}
	}
	else {
		const float s = (-UDotPMinusM - sqrtf(d)) / LineDirSq;
		if(s < 0.0f || s > 1.0f) return false;
		else {
			if(pOut) *pOut = LineStart + s * LineDir;
			return true;
		}
	}
}

/** draws the xor-line */
void EditPaintPlugin::drawLine(GLArea * gla) {
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
	//glEnable(GL_LINE_SMOOTH);
	float wi;
	glGetFloatv(GL_LINE_WIDTH,&wi);
	glLineWidth(4);
	glLogicOp(GL_XOR);
	glColor3f(1,1,1);
	glBegin(GL_LINES);
	glVertex2f(start.x(),start.y());
	glVertex2f(cur.x(),cur.y());
	glEnd();
	glPopAttrib();
	glPopMatrix(); // restore modelview
	glLineWidth(wi);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void EditPaintPlugin::DrawXORCircle(MeshModel &m,GLArea * gla, bool doubleDraw) {
	int PEZ=18;
	/** paint the normal circle in pixel-mode */
	if (paintbox->paintType()==1) { 
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
		if(doubleDraw) {
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
	} else { /** paint the complicated circle in percentual-mode */
		double dX, dY, dZ;
		double dX2, dY2, dZ2;
		
		updateMatrixes();
		
		QPoint mid= QPoint(cur.x(),gla->curSiz.height()-cur.y());
		gluUnProject ((double) mid.x(), mid.y(), 0.0, mvmatrix, projmatrix, viewport, &dX, &dY, &dZ);
		gluUnProject ((double) mid.x(), mid.y(), 1.0, mvmatrix, projmatrix, viewport, &dX2, &dY2, &dZ2);

		glPushMatrix();
		glLoadIdentity();
		gluLookAt(dX,dY,dZ,dX2,dY2,dZ2,1,0,0);
		double mvmatrix2[16];
		glGetDoublev (GL_MODELVIEW_MATRIX, mvmatrix2);
		glPopMatrix();
	
		double tx,ty,tz;
		double tx2,ty2,tz2;
	
		double inv_mvmatrix[16];
	
		Matrix44d temp(mvmatrix2);
		Invert(temp);
		for (int lauf=0; lauf<16; lauf++) inv_mvmatrix[lauf]=temp[lauf/4][lauf%4];
		
		float radius=pen.radius;
		double a,b,c;
		double a2,b2,c2;

		PEZ=64;
		int STEPS=30;
		float diag=m.cm.bbox.Diag()*(-7);
		QPoint circle_points[64]; //because of the .NET 2003 problem

		glPushAttrib(GL_ENABLE_BIT);
		//glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_COLOR_LOGIC_OP);
		glLogicOp(GL_XOR);
		glColor3f(1,1,1);

		glBegin(GL_LINES);
		for (int lauf=0; lauf<PEZ; lauf++) {
			/** calcs the far point of the line */
			calcCoord(sin(M_PI*(double)lauf/(double)(PEZ/2))*radius,cos(M_PI*(double)lauf/(double)(PEZ/2))*radius,diag,inv_mvmatrix,&tx,&ty,&tz);
			//glVertex3f(tx,ty,tz);
			gluProject(tx,ty,tz,mvmatrix,projmatrix,viewport,&a,&b,&c);
			/** calcs the near point of the line */
			calcCoord(sin(M_PI*(double)lauf/(double)(PEZ/2))*radius,cos(M_PI*(double)lauf/(double)(PEZ/2))*radius,0,inv_mvmatrix,&tx2,&ty2,&tz2);
			//glVertex3f(tx2,ty2,tz2);
			gluProject(tx2,ty2,tz2,mvmatrix,projmatrix,viewport,&a2,&b2,&c2);

			double da=(a-a2);// /(double)STEPS;
			double db=(b-b2);// /(double)STEPS;
			double dc=(c-c2);// /(double)STEPS;
			double pix_x=a2;
			double pix_y=b2;
			double pix_z=c2;
			/** to search with quicksearch the nearset zbuffer value in the line */
			for (int lauf2=0; lauf2<STEPS; lauf2++) {
				double inv_yy=gla->curSiz.height()-pix_y;
				double zz=999;
				if ((int)pix_x>=0 && (int)pix_x<gla->curSiz.width() && (int)pix_y>=0 && (int)pix_y<gla->curSiz.height()) 
					zz=(GLfloat)pixels[(int)(((int)pix_y)*gla->curSiz.width()+(int)pix_x)];
					da=da/2.0;
					db=db/2.0;
					dc=dc/2.0;
					if (fabsf(zz-pix_z)<0.001) {
						circle_points[lauf]=QPoint(pix_x,inv_yy);
						break;
					} else if (zz>pix_z) {
						pix_x+=da;
						pix_y+=db;
						pix_z+=dc;
					} else {
						pix_x-=da;
						pix_y-=db;
						pix_z-=dc;
					}
				if (lauf2==STEPS-1) { circle_points[lauf]=QPoint(pix_x,inv_yy); break; }
			}
		}

		glEnd();
		glDisable(GL_COLOR_LOGIC_OP);
		glPopAttrib();
	
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
		/** draws the circle */
		glBegin(GL_LINE_STRIP);
		for (int lauf=0; lauf<PEZ; lauf++) {
			glVertex2f(circle_points[lauf].x(),circle_points[lauf].y());
		} glVertex2f(circle_points[0].x(),circle_points[0].y());

		glEnd();
		glDisable(GL_COLOR_LOGIC_OP);
		glPopAttrib();
		glPopMatrix(); // restore modelview
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	}

}

/** calcs the surrounding faces of a vertex with VF topology */
inline void getSurroundingFacesVF(CFaceO * fac,int vert_pos,vector<CFaceO *> *surround) {
	CVertexO * vert=fac->V(vert_pos);
	int pos=vert->VFi();
	CFaceO * first_fac=vert->VFp();
	CFaceO * curr_f=first_fac;
	do {
		CFaceO * temp=curr_f->VFp(pos);
		if (curr_f!=0 && !curr_f->IsD()) {
			surround->push_back(curr_f);
			pos=curr_f->VFi(pos);
		}
		curr_f=temp;
	} while (curr_f!=first_fac && curr_f!=0);
}

//getInternFaces(m,&curSel,&newSel,&faceSel,gla,pen,cur,prev,pixels,mvmatrix,projmatrix,viewport);
/** finds the faces or vertexes in the circle */
void getInternFaces(MeshModel & m,vector<CMeshO::FacePointer> *actual,vector<Vert_Data> * risult, vector<CMeshO::FacePointer> * face_risult,
	GLArea * gla,Penn &pen,QPoint &cur, QPoint &prev, GLfloat * pixels,
	double mvmatrix[16],double projmatrix[16],GLint viewport[4]) {

	QHash <CFaceO *,CFaceO *> selected;
	QHash <CVertexO *,CVertexO *> sel_vert;
	vector<CMeshO::FacePointer>::iterator fpi;
	vector<CMeshO::FacePointer> temp_po;

	if (actual->size()==0) {
		CMeshO::FaceIterator fi;
        	for(fi=m.cm.face.begin();fi!=m.cm.face.end();++fi) 
            	if(!(*fi).IsD()) {
			temp_po.push_back((&*fi));
		}
	} else
	for(fpi=actual->begin();fpi!=actual->end();++fpi) {
		temp_po.push_back(*fpi);
	}

	actual->clear();

	QPointF mid=QPointF(cur.x(),gla->curSiz.height()-  cur.y());
	QPointF mid_prev=QPointF(prev.x(),gla->curSiz.height()-  prev.y());
	//float p_x[3];float p_y[3];
	//float z_x[3];float z_y[3];
	QPointF p[3],z[3];
	double tx,ty,tz;

	bool backface=pen.backface;
	bool invisible=pen.invisible;
	//qDebug() << "check: "<<temp_po.size()<< endl; 
	int bla=0;
	if (pen.painttype==1) { /// PIXEL 
		for (int lauf2=0; lauf2<temp_po.size(); lauf2++) {
			CFaceO * fac=temp_po.at(lauf2);

			int intern=0;
			int checkable=0; /// to avoid problems when the pen is smaller then the polys
			for (int lauf=0; lauf<3; lauf++) {
				if (gluProject((fac)->V(lauf)->P()[0],(fac)->V(lauf)->P()[1],(fac)->V(lauf)->P()[2],
						mvmatrix,projmatrix,viewport,&tx,&ty,&tz)==GL_TRUE) checkable++;
				if (tz<0 || tz>1) checkable--;
				p[lauf].setX(tx); p[lauf].setY(ty);
				//qDebug() << tx << " "<< ty << " "<<tz << endl;
				//qDebug() << "zzz: "<<(int)(((int)ty)*old_size.x()+(int)tx)<<" t: "<<tx<<" "<<ty<<" "<<tz<<endl;
				if (tx>=0 && tx<viewport[2] && ty>=0 && ty<viewport[3]) {
					z[lauf].setX(tz);
					z[lauf].setY((float)pixels[(int)(((int)ty)*viewport[2]+(int)tx)]);
				}
				else { 
					z[lauf].setX(1); z[lauf].setY(0);
				}
			}
			if (backface || isFront(p[0],p[1],p[2])) {
				//checkable++;
				/// colud be useful to calc the medium of z in the matrix
				for (int lauf=0; lauf<3; lauf++) if (invisible || (z[lauf].x()<=z[lauf].y()+0.003)){
					float dist;
					if (isIn(mid,mid_prev,p[lauf].x(),p[lauf].y(),pen.radius,&dist)==1) {
						intern=1;
						if (pen.paintutensil==SELECT) continue;
						else if (!sel_vert.contains(fac->V(lauf))) {
							Vert_Data d;
							d.v=fac->V(lauf);
							d.distance=dist;
							risult->push_back(/*fac->V(lauf)*/d);
							sel_vert.insert(fac->V(lauf),fac->V(lauf));
							//qDebug() << tx << " " << ty << " " << tz <<"   "<< mid<<endl;
						}
					} 
					QPointF pos_res;
					if (pen.paintutensil==SELECT && intern==0 && lineHitsCircle(p[lauf],p[(lauf+1)%3],mid,pen.radius,&pos_res)) {
						intern=1; continue;
					}
				}
				if (checkable==3 && intern==0 && (pointInTriangle(mid,p[0],p[1],p[2]) || pointInTriangle(mid_prev,p[0],p[1],p[2]))) {
					intern=-1;
				}
			}
			//qDebug() << "ch: "<<checkable << "in: "<<intern <<" bla:"<<bla++<< "  "<< temp_po.size()<< endl;
			if (checkable==3 && intern!=0 && !selected.contains(fac)) {
				selected.insert((fac),(fac));
				actual->push_back(fac);
				vector <CFaceO *> surround;
				for (int lauf=0; lauf<3; lauf++) getSurroundingFacesVF(fac,lauf,&surround);

				for (int lauf3=0; lauf3<surround.size(); lauf3++) {
					if (!selected.contains(surround[lauf3])) {
						temp_po.push_back(surround[lauf3]);
					} 
				}

				if (pen.paintutensil==SELECT && intern!=0) face_risult->push_back(fac);
			}
		}

	} else { /// PERCENTUAL
		double dX, dY, dZ;

		calcCoord(0,0,0,mvmatrix,&dX,&dY,&dZ);
		calcCoord(0,1,0,mvmatrix,&tx,&ty,&tz);
		double scale_fac=sqrt((ty-dY)*(ty-dY)+(tx-dX)*(tx-dX)+(tz-dZ)*(tz-dZ));

		/** to get the correct radius depending on distance but on fov too */
		float fo=gla->getFov()*0.5;
		float fov = 1.0/tan(fo*M_PI/180.0)*0.5;

		for (int lauf2=0; lauf2<temp_po.size(); lauf2++) {
			CFaceO * fac=temp_po.at(lauf2);
			int intern=0;
			int checkable=0;
			double distance[3];
			for (int lauf=0; lauf<3; lauf++) { 
				double dx,dy,dz; // distance
				calcCoord((fac)->V(lauf)->P()[0],(fac)->V(lauf)->P()[1],(fac)->V(lauf)->P()[2],mvmatrix,&dx,&dy,&dz);
				if (gluProject((fac)->V(lauf)->P()[0],(fac)->V(lauf)->P()[1],(fac)->V(lauf)->P()[2],
					mvmatrix,projmatrix,viewport,&tx,&ty,&tz)==GL_TRUE) checkable++;
				if (tz<0 || tz>1) checkable--;
				//p[lauf]=QPointF(dx,dy);
				p[lauf]=QPointF(tx,ty);
				if (tx>=0 && tx<viewport[2] && ty>=0 && ty<viewport[3]) {
					z[lauf].setX(tz);
					z[lauf].setY((float)pixels[(int)(((int)ty)*viewport[2]+(int)tx)]);
				}
				else { z[lauf].setX(1); z[lauf].setY(0); }
				distance[lauf]=dz;
			}
	
			if (backface || isFront(p[0],p[1],p[2])) {
				for (int lauf=0; lauf<3; lauf++)if (invisible || (z[lauf].x()<=z[lauf].y()+0.003)) {
					//QPointF poo(mid.x()*(distance[lauf])*-1.0,mid.y()*(distance[lauf])*-1.0);
					//if (isIn(poo,poo,tx,ty,/*0.08*gla->trackball.track.sca*/radius)==1) {
					float dist;
					if (isIn(mid,mid_prev,p[lauf].x(),p[lauf].y(),pen.radius*scale_fac *viewport[3]*fov/distance[lauf],&dist)==1) {
					//if (isIn(QPoint(0,0),QPoint(0,0),tx,ty,pen.radius)==1) {  
						intern=1;
						if (pen.paintutensil==SELECT) continue;
						else if(!sel_vert.contains(fac->V(lauf))) {
							Vert_Data d;
							d.v=fac->V(lauf);
							d.distance=dist;
							risult->push_back(d);
							sel_vert.insert(fac->V(lauf),fac->V(lauf));
						}
					}
					QPointF pos_res;
					if (pen.paintutensil==SELECT && intern==0 && lineHitsCircle(p[lauf],p[(lauf+1)%3],mid,
						pen.radius*scale_fac *viewport[3]*fov/distance[lauf],&pos_res)) {
						intern=1; continue;
					}
				}
				if (checkable==3 && intern!=0 && (pointInTriangle(mid,p[0],p[1],p[2]) || pointInTriangle(mid_prev,p[0],p[1],p[2]))) {
					intern=-1;
				}
			}
			if (intern!=0 && ! selected.contains(fac)) {
				selected.insert((fac),(fac));
				actual->push_back(fac);
				/// WOULD BE FASTER WITH FF TOPOLOGY
				vector <CFaceO *> surround;
				for (int lauf=0; lauf<3; lauf++) getSurroundingFacesVF(fac,lauf,&surround);

				for (int lauf3=0; lauf3<surround.size(); lauf3++) {
					if (!selected.contains(surround[lauf3])) {
						//actual->push_back(surround[lauf3]);
						temp_po.push_back(surround[lauf3]);
					} 
				}
				if (pen.paintutensil==SELECT && intern!=0) face_risult->push_back(fac);
			}
		}
	}
}

/** fills the mesh starting from face.
If face is selected, it will fill only the selected area, otherwise only the non selected area */
void EditPaintPlugin::fillFrom(MeshModel & m,CFaceO * face) {
	QHash <CFaceO *,CFaceO *> visited;
	QHash <CVertexO *,CVertexO *> temporaneo;
	vector <CFaceO *>temp_po;
	bool who=face->IsS();
	temp_po.push_back(face);
	visited.insert(face,face);
	int opac=paintbox->getOpacity();
	Color4b newcol=paintbox->getColor(curr_mouse);
	UndoItem u;
	for (int lauf2=0; lauf2<temp_po.size(); lauf2++) {
		CFaceO * fac=temp_po.at(lauf2);
		if (who==fac->IsS()) {
			for (int lauf=0; lauf<3; lauf++) {
				if (!temporaneo.contains(fac->V(lauf))) {
					temporaneo.insert(fac->V(lauf),fac->V(lauf));
					u.vertex=fac->V(lauf);
					u.original=fac->V(lauf)->C();
					colorize(fac->V(lauf),newcol,opac);
					color_undo[current_gla]->addItem(u);
				}
			}

			vector <CFaceO *> surround;
			for (int lauf=0; lauf<3; lauf++) getSurroundingFacesVF(fac,lauf,&surround);

			for (int lauf3=0; lauf3<surround.size(); lauf3++) {
				if (!visited.contains(surround[lauf3])) {
					//actual->push_back(surround[lauf3]);
					temp_po.push_back(surround[lauf3]);
					visited.insert(surround[lauf3],surround[lauf3]);
				} 
			}
		}
	}
}

bool EditPaintPlugin::getFaceAtMouse(MeshModel &m,CMeshO::FacePointer& val) {
	QPoint mid=QPoint(cur.x(),inverse_y);
	return (GLPickTri<CMeshO>::PickNearestFace(mid.x(), mid.y(), m.cm, val,2,2));
}

bool EditPaintPlugin::getFacesAtMouse(MeshModel &m,vector<CMeshO::FacePointer> & val) {
	val.clear();
	QPoint mid=QPoint(cur.x(),inverse_y);
	GLPickTri<CMeshO>::PickFace(mid.x(), mid.y(), m.cm, val,2,2);
	return (val.size()>0);
}

bool EditPaintPlugin::getVertexAtMouse(MeshModel &m,CMeshO::VertexPointer& value) {
	CFaceO * temp=0;
	QPoint mid=QPoint(cur.x(),inverse_y);
	double tx,ty,tz;
	if (getFaceAtMouse(m,temp)) {
		QPointF point[3];
		for (int lauf=0; lauf<3; lauf++) {
			gluProject(temp->V(lauf)->P()[0],temp->V(lauf)->P()[1],temp->V(lauf)->P()[2],mvmatrix,projmatrix,viewport,&tx,&ty,&tz);
			point[lauf]=QPointF(tx,ty);
		}
		value=temp->V(getNearest(mid,point,3));
		return true;
	}
	return false;
}

bool EditPaintPlugin::getVertexesAtMouse() {
	return false;
}



bool EditPaintPlugin::hasSelected(MeshModel &m) {
	CMeshO::FaceIterator fi;
	for(fi=m.cm.face.begin();fi!=m.cm.face.end();++fi) {
		if (!(*fi).IsD() && (*fi).IsS()) return true;
	}
	return false;
}

void EditPaintPlugin::fillGradient(MeshModel & m,GLArea * gla) {
	QPoint p=start-cur;
	QHash <CFaceO *,CFaceO *> selezionati;
	QHash <CVertexO *,CVertexO *> temporaneo;

	int opac=paintbox->getOpacity();
	Color4b c1=paintbox->getColor(Qt::LeftButton);
	Color4b c2=paintbox->getColor(Qt::RightButton);

	QPointF p1(start.x(),gla->curSiz.height()-start.y());
	QPointF p0(cur.x(),gla->curSiz.height()-cur.y());
	float x2=(p1.x()-p0.x());
	float y2=(p1.y()-p0.y());
	//double l=sqrt(x2*x2+y2*y2);
	float l_square=x2*x2+y2*y2;

	CVertexO * vertex;
	CMeshO::FaceIterator fi;
	double dx,dy,dz;
	Color4b merger;
	bool tutti=!hasSelected(m);
	float radius=sqrt((float)(p.x()*p.x()+p.y()*p.y()));
	UndoItem u;
	int gradient_type=paintbox->getGradientType();
	int gradient_form=paintbox->getGradientForm();
	for(fi=m.cm.face.begin();fi!=m.cm.face.end();++fi) 
		if (!(*fi).IsD() && (tutti || (*fi).IsS()))
		for (int lauf=0; lauf<3; lauf++) {
			if (!temporaneo.contains((*fi).V(lauf))) {
				vertex=(*fi).V(lauf);
				temporaneo.insert(vertex,vertex);
				gluProject(vertex->P()[0],vertex->P()[1],vertex->P()[2],mvmatrix,projmatrix,viewport,&dx,&dy,&dz);
				//qDebug () << yt <<""<< ye << endl;
				u.vertex=vertex;
				u.original=vertex->C();
				if (gradient_form==0) {
					double r=(dx-p0.x())*(p1.x()-p0.x())+(dy-p0.y())*(p1.y()-p0.y());
					//r=r/(l*l);
					r=r/l_square;
					
					float px=p0.x()+r*(p1.x()-p0.x());
					float py=p0.y()+r*(p1.y()-p0.y());
				
					px=px-dx;
					py=py-dy;
	
					if (gradient_type==0) {
						if (r>=0 && r<=1 /*&& (px*px+py*py<radius*radius)*/) { 
							mergeColor(r,c1,c2,&merger);
							colorize(vertex,merger,opac);
						} else if (r>1) {
							colorize(vertex,c1,opac);
						} else if (r<0) {
							colorize(vertex,c2,opac);
						}
					} else {
						if (r>=0 && r<=1 /*&& (px*px+py*py<radius*radius)*/) { 
							//mergeColor(r,c1,c2,&merger);
							colorize(vertex,c1,((double)opac*0.01)*r*100.0);
						} else if (r>1) {
							colorize(vertex,c1,opac);
						} else if (r<0) {
							//colorize((*fi).V(lauf),c2,opac);
						}
					}
				} else {
					float x0=(dx-p1.x());
					float y0=(dy-p1.y());
					float bla0=x0*x0+y0*y0;
					if (bla0<radius*radius && radius>0) {
						float r=1.0-sqrt(bla0)/sqrt(radius*radius);
						if (gradient_type==0) {
							mergeColor(r,c1,c2,&merger);
							colorize(vertex,merger,opac);
						} else {
							colorize(vertex,c1,((double)opac*0.01)*r*100.0);
						}
					}
				}
				color_undo[current_gla]->addItem(u);
			}
		}
	pushUndo(gla);
	gla->update();
}

/** only in decorare it is possible to obtain the correct zbuffer values and the other opengl stuff */
void EditPaintPlugin::Decorate(MeshModel &m, GLArea * gla) {
	updateMatrixes();
	QPoint mid=QPoint(cur.x(),gla->curSiz.height()-  cur.y());
	int utensil=paintbox->paintUtensil();
	if (first) {
		first=false;
		if (pixels!=0) { free(pixels); }
		pixels=(GLfloat *)malloc(sizeof(GLfloat)*gla->curSiz.width()*gla->curSiz.height());
		glReadPixels(0,0,gla->curSiz.width(),gla->curSiz.height(),GL_DEPTH_COMPONENT,GL_FLOAT,pixels);
	}
	if(isDragging)
	{
	isDragging=false;
	//qDebug() << "decorate" << endl;
	switch (utensil) {
		case FILL: { 
			if (!pressed) return;
			CFaceO * temp_face;
			if(getFaceAtMouse(m,temp_face)) {
				fillFrom(m,temp_face);
			}
			pressed=0;
			return; 
		}
		case PICK: { 
			if (!pressed) return;
			CVertexO * temp_vert=0;
			if (paintbox->getPickMode()==0) {
				if (getVertexAtMouse(m,temp_vert)) {	
					//qDebug() << temp_vert->C()[0] << " " << temp_vert->C()[1] << endl;
					paintbox->setColor(temp_vert->C(),curr_mouse);
				} 
			} else {
				GLubyte pixel[3];
				glReadPixels( mid.x(),mid.y(), 1, 1,GL_RGB,GL_UNSIGNED_BYTE,(void *)pixel);
				paintbox->setColor(pixel[0],pixel[1],pixel[2],curr_mouse);
			}
			pressed=0;
			return; 
		}
		case GRADIENT: {
			if (pressed==1) drawLine(gla);
			else if (pressed==2) { fillGradient(m,gla); pressed=0; }
			return; 
		}
		case PEN: {}
	}

	DrawXORCircle(m,gla,false);

	vector<Vert_Data>::iterator vpo;
	vector<Vert_Data> newSel;
	
	vector<CMeshO::FacePointer> faceSel;
	
	if (paintbox->searchMode()==2) curSel.clear();
	//if (GLPickTri<CMeshO>::PickNearestFace(mid.x(), mid.y(), m.cm, tmp, pen.width.x(), pen.width.y()))
	getInternFaces(m,&curSel,&newSel,&faceSel,gla,pen,cur,prev,pixels,mvmatrix,projmatrix,viewport);

	//UndoItem u;
	QHash <CVertexO *,Vert_Data_3> originals;
	if (utensil==SMOOTH || utensil==POLY_SMOOTH) { /// color smooth and geometry smooth with the pen
		//visited_vertexes.clear();
		Color4b newcol,destCol;
		int opac=paintbox->getSmoothPercentual();
		int c_r,c_g,c_b;
		float p_x,p_y,p_z;
		float newpos[3],destpos[3];
		int decrease_pos=100-paintbox->getDecreasePercentual();
		for(vpo=newSel.begin();vpo!=newSel.end();++vpo) {
			CVertexO * v=vpo->v;
			//float distence=vpo->distance;
			//if (!visited_vertexes.contains(v)) {
				Vert_Data_2 ved; ved.distance=0;
				//CVertexO * vert=v;

				Vert_Data_3 d; d.color=Color4b(v->C()[0],v->C()[1],v->C()[2],v->C()[3]); //d.distance=vpo->distance;
				for (int lauf=0; lauf<3; lauf++) d.pos[lauf]=v->P()[lauf];

				if (!visited_vertexes.contains(v)) {
					if (utensil==SMOOTH) {
						UndoItem u;
						u.vertex=v;
						u.original=Color4b(v->C()[0],v->C()[1],v->C()[2],v->C()[3]);
						color_undo[current_gla]->addItem(u);
					}
					visited_vertexes.insert(v,/*v->C()*/ved);
				}
				if (!originals.contains(v)) originals.insert(v,d);
				
				//qDebug() << "a" << endl;
				CFaceO * f= v->VFp();
				CFaceO * one_face=f;
				int pos=v->VFi();
				c_r=c_g=c_b=0;
				p_x=p_y=p_z=0;
				int count_me=0;
				do { /// calc the sum of the surrounding vertexes pos or and vertexes color
					CFaceO * temp=one_face->VFp(pos);
					//qDebug() << "c:" <<(int)one_face <<endl;
					if (one_face!=0 && !one_face->IsD()) {
						for (int lauf=0; lauf<3; lauf++) {
							if (pos!=lauf) { 
								CVertexO * v=one_face->V(lauf);
								Color4b tco=v->C();
								if (originals.contains(v)) {
									Vert_Data_3 dd=originals[v];
									tco=dd.color;
									p_x+=dd.pos[0]; p_y+=dd.pos[1]; p_z+=dd.pos[2];
								}
								else if (POLY_SMOOTH){
									p_x+=v->P()[0]; p_y+=v->P()[1]; p_z+=v->P()[2];
									
								}
								c_r+=tco[0]; c_g+=tco[1]; c_b+=tco[2]; 
								
							}
						}
						pos=one_face->VFi(pos);
						count_me+=2;
					//qDebug() << "d" << endl;
					}
					one_face=temp;
				} while (one_face!=f && one_face!=0);
				if (count_me>0) { /// calc the new color or pos
					float op=0.0;
					//qDebug() << vpo->distance << endl;
					if (vpo->distance*100.0>(float)decrease_pos) {
						op=(vpo->distance*100.0-(float)decrease_pos)/(float)(100-decrease_pos);
						//op=op*op;
					}
					//qDebug() << "coloro" <<endl;
					if (utensil==SMOOTH) { ///
						newcol[0]=c_r/count_me;
						newcol[1]=c_g/count_me;
						newcol[2]=c_b/count_me;
						//qDebug() << "opac"<< (float)opac/100.0<< endl;
						mergeColor((float)(opac-op*opac)/100.0,newcol,v->C(),&destCol);
						v->C()[0]=destCol[0];
						v->C()[1]=destCol[1];
						v->C()[2]=destCol[2];
					} else {
						
						newpos[0]=p_x/(float)count_me;
						newpos[1]=p_y/(float)count_me;
						newpos[2]=p_z/(float)count_me;
						float po[3]; for (int lauf=0; lauf<3; lauf++) po[lauf]=v->P()[lauf];
						mergePos((float)(opac-op*opac)/100.0,newpos,po,newpos);
						for (int lauf=0; lauf<3; lauf++) v->P()[lauf]=newpos[lauf];
					}
				}
				if (utensil==POLY_SMOOTH) { /// recalc face and vertex normale
					one_face=f;
					pos=v->VFi();
					v->N()[0]=0;v->N()[1]=0;v->N()[2]=0;
					do {
						CFaceO * temp=one_face->VFp(pos);
						//qDebug() << "c:" <<(int)one_face <<endl;
						if (one_face!=0 && !one_face->IsD()) {
							for (int lauf=0; lauf<3; lauf++) 
								if (pos!=lauf) { 
									v->N()+=one_face->V(lauf)->cN();
								}
							face::ComputeNormalizedNormal(*one_face);
							pos=one_face->VFi(pos);
						//qDebug() << "d" << endl;
						}
						one_face=temp;
					} while (one_face!=f && one_face!=0);
					v->N().Normalize();
				}
				//colorize(*vpo,newcol,opac);
			//}
		}
	} else { /// painting or selecting with the pen
		if (utensil==SELECT) {
			vector<CMeshO::FacePointer>::iterator fpo;
			bool sel_or_not=(curr_mouse==Qt::LeftButton);
			for(fpo=faceSel.begin();fpo!=faceSel.end();++fpo) {
				if (sel_or_not) (*fpo)->SetS();
				else (*fpo)->ClearS();
			}
		} else { /// painting
			int opac=paintbox->getOpacity();
			int decrease_pos=100-paintbox->getDecreasePercentual(); // 100 .. all opac ; 0 .. nothing opac
			Color4b newcol=paintbox->getColor(curr_mouse);
			for(vpo=newSel.begin();vpo!=newSel.end();++vpo) {
				CVertexO * v=vpo->v;
				float op=0.0;
				if (vpo->distance*100.0>(float)decrease_pos) {
					op=(vpo->distance*100.0-(float)decrease_pos)/(float)(100-decrease_pos);
					//op=op*op;
				}
				if (!visited_vertexes.contains(v)) {
					Vert_Data_2 ved; 
					ved.distance=opac-op*opac;
					ved.color=Color4b(v->C()[0],v->C()[1],v->C()[2],v->C()[3]);
					visited_vertexes.insert(v,/*v->C()*/ved);
					UndoItem u;
					u.vertex=v;
					u.original=v->C();
					color_undo[current_gla]->addItem(u);
					colorize(v,newcol,opac-op*opac);
				} else { 
					/** if there has already been a painting on this vertex i have to check if 
					the new opacity is greater and eventually repaint */
					if (visited_vertexes[v].distance<opac-op*opac) {
						visited_vertexes[v].distance=opac-op*opac;
						Color4b temp=visited_vertexes[v].color;
						v->C()[0]=temp[0];v->C()[1]=temp[1];v->C()[2]=temp[2];
						colorize(v,newcol,opac-op*opac);
					}
				}
			}
			
		}
	}
	pressed=0;
	}
}

void ColorUndo::undo() {
	if (undos.size()==0) return;
	vector<UndoItem> * temp=undos[undos.size()-1];
	vector<UndoItem> * temp_redo=new vector<UndoItem>();
	UndoItem u;
	Color4b oldcol;
	for (int lauf=0; lauf<temp->size(); lauf++) {
		u=(*temp)[lauf];
		oldcol=u.vertex->C();
		u.vertex->C()=u.original;
		u.original=oldcol;
		temp_redo->push_back(u);
	}
	undos.pop_back();
	temp->clear();
	delete temp;
	redos.push_back(temp_redo);
}

void ColorUndo::redo() {
	if (redos.size()==0) return;
	vector<UndoItem> * temp=redos[redos.size()-1];
	vector<UndoItem> * temp_undo=new vector<UndoItem>();
	UndoItem u;
	Color4b oldcol;
	for (int lauf=0; lauf<temp->size(); lauf++) {
		u=(*temp)[lauf];
		oldcol=u.vertex->C();
		u.vertex->C()=u.original;
		u.original=oldcol;
		temp_undo->push_back(u);
	}
	redos.pop_back();
	temp->clear();
	delete temp;
	undos.push_back(temp_undo);
}
