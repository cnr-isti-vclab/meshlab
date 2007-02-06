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

//TODO CLEAN ME
//TODO bright colors problem
//TODO add IsD() check
//TODO bits instead of hashtables
//TODO optimize undo to not include when new and old colors are the same
//TODO bug when pen smaller than polys
//TODO error with undo
//TODO color smooth has some problem
################################
//TODO paint selection selects some wrong piece: SOLVED
//TODO opengl line width after gradient: SOLVED
//TODO PaintToolbox does not close: SOLVED
//TODO remove FF or VF: SOLVED removed FF
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

#include<vcg/complex/trimesh/update/topology.h>
#include<vcg/complex/trimesh/update/bounding.h>
//#include <vcg/space/triangle3.h>

using namespace vcg;

EditPaintPlugin::EditPaintPlugin() {
	isDragging=false;
	paintbox=0;
	pixels=0;
	first=true;
	pressed=0;
	//color_undo= new ColorUndo();
	actionList << new QAction(QIcon(":/images/pinsel.png"),"Vertex painting, Face selection and smoothing", this);
	QAction *editAction;
	//QObject::connect(editAction, SIGNAL(redo()),this, SLOT(redo()));
	foreach(editAction, actionList)
	editAction->setCheckable(true);
	//qDebug("CONSTRUCTOR");
	//paintbox=new PaintToolbox("Vertex painting");
	//paintbox->setVisible(false);
	//worker= new PaintWorker();
	//worker->start(QThread::HighestPriority);
}

EditPaintPlugin::~EditPaintPlugin() {
	if (paintbox!=0) { delete paintbox; paintbox=0; }
	qDebug() << "~EditPaint" << endl;
}


QList<QAction *> EditPaintPlugin::actions() const {
	return actionList;
}

const QString EditPaintPlugin::Info(QAction *action) {
	if( action->text() != tr("Vertex painting, Face selection and smoothing") ) assert (0);
	return tr("Paint on your mesh, select and smooth faces, all with a pen-like tool.");
}

const PluginInfo &EditPaintPlugin::Info() {
	static PluginInfo ai; 
	ai.Date=tr(__DATE__);
	ai.Version = tr("0.1");
	ai.Author = ("Andreas Gfrei");
	return ai;
} 

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

void EditPaintPlugin::StartEdit(QAction * /*mode*/, MeshModel &m, GLArea * parent) {
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
	//m.updateDataMask(MeshModel::MM_FACETOPO);
	m.updateDataMask(MeshModel::MM_VERTFACETOPO);

	parent->getCurrentRenderMode().colorMode=vcg::GLW::CMPerVert;
	parent->mm->ioMask|=MeshModel::IOM_VERTCOLOR;
	parent->mm->ioMask|=MeshModel::IOM_VERTQUALITY;
	
	LastSel.clear();
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

void EditPaintPlugin::EndEdit(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/) {
	qDebug() <<"ENDEDIT"<<endl;
	if (paintbox!=0) { paintbox->setVisible(false); delete paintbox; paintbox=0; }
}

void EditPaintPlugin::mousePressEvent(QAction * ac, QMouseEvent * event, MeshModel &m, GLArea * gla) {
	//qDebug() << "pressStart" << endl;
	has_track=gla->isTrackBallVisible();
	gla->showTrackBall(false);
	LastSel.clear();
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

	pen.painttype=paintType();
	pen.backface=paintbox->getPaintBackface();
	pen.invisible=paintbox->getPaintInvisible();
	switch (paintType()) {
		case 1: { pen.radius=paintbox->getRadius()*0.5; } break;
		case 2: { pen.radius=paintbox->getRadius()*m.cm.bbox.Diag()*0.01*0.5; } break;
		case 3: { pen.radius=paintbox->getRadius()*m.cm.bbox.DimY()*0.01*0.5; } break;
		case 4: { pen.radius=paintbox->getRadius()*0.5; } break;
	}
	curSel.clear();

	switch (paintbox->paintUtensil()) {
		case FILL: {} break;
		case PICK: {} break;
		case PEN: {} break;
	}
	//qDebug() << "pressEnd" << endl;
}
  
void EditPaintPlugin::mouseMoveEvent(QAction *,QMouseEvent * event, MeshModel &/*m*/, GLArea * gla) {
	//qDebug() << "moveStart" << endl;
	switch (paintbox->paintUtensil()) {
		case FILL: { return; }
		case PICK: { return; }
		case PEN: {}
	}
	
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
  
void EditPaintPlugin::mouseReleaseEvent  (QAction *,QMouseEvent * event, MeshModel &m, GLArea * gla) {
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

inline void colorize(CVertexO * vertice,const Color4b& newcol,int opac) {
	Color4b orig=vertice->C();
	orig[0]=min(255,((newcol[0]-orig[0])*opac+orig[0]*(100))/100);
	orig[1]=min(255,((newcol[1]-orig[1])*opac+orig[1]*(100))/100);
	orig[2]=min(255,((newcol[2]-orig[2])*opac+orig[2]*(100))/100);
	orig[3]=min(255,((newcol[3]-orig[3])*opac+orig[3]*(100))/100);
	vertice->C()=orig;
}

inline void calcCoord(float x,float y,float z,double matrix[],double *xr,double *yr,double *zr) {
	*xr=x*matrix[0]+y*matrix[4]+z*matrix[8]+matrix[12];
	*yr=x*matrix[1]+y*matrix[5]+z*matrix[9]+matrix[13];	
	*zr=x*matrix[2]+y*matrix[6]+z*matrix[10]+matrix[14];
}

void getTranspose(double orig[],double inv[]) {
	inv[0]=orig[0]; inv[1]=orig[4]; inv[2]=orig[8];  inv[3]=orig[12];
	inv[4]=orig[1]; inv[5]=orig[5]; inv[6]=orig[9];  inv[7]=orig[13];
	inv[8]=orig[2]; inv[9]=orig[6]; inv[10]=orig[10];inv[11]=orig[14];
	inv[12]=orig[3];inv[13]=orig[7];inv[14]=orig[11]; inv[15]=orig[15];
}

inline int isIn(QPointF p0,QPointF p1,float dx,float dy,float radius,float *dist) {
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
	
		if (r>0 && r<1 && (px*px+py*py<radius*radius)) { *dist=sqrt(px*px+py*py)/sqrt(radius*radius); return 1; }
	}

	float x0=(dx-p0.x());
	float y0=(dy-p0.y());
	float bla0=x0*x0+y0*y0;
	if (bla0<radius*radius) { *dist=sqrt(bla0)/sqrt(radius*radius); return 1;}
	if (p0==p1) return 0;

	float x1=(dx-p1.x());
	float y1=(dy-p1.y());
	float bla1=x1*x1+y1*y1;
	if (bla1<radius*radius) { *dist=sqrt(bla1)/sqrt(radius*radius); return 1;}

	return 0;
}

inline bool pointInTriangle(QPointF p,QPointF a, QPointF b,QPointF c) {
	float fab=(p.y()-a.y())*(b.x()-a.x()) - (p.x()-a.x())*(b.y()-a.y());
	float fbc=(p.y()-c.y())*(a.x()-c.x()) - (p.x()-c.x())*(a.y()-c.y());
	float fca=(p.y()-b.y())*(c.x()-b.x()) - (p.x()-b.x())*(c.y()-b.y());
	if (fab*fbc>0 && fbc*fca>0) return true;
	return false;
}

inline bool isFront(QPointF a, QPointF b, QPointF c) {
	return (b.x()-a.x())*(c.y()-a.y())-(b.y()-a.y())*(c.x()-a.x())>0;
}


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

void EditPaintPlugin::DrawXORRect(MeshModel &m,GLArea * gla, bool doubleDraw) {
	int PEZ=18;
	if (paintType()==1) {
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
			for (int lauf=0; lauf<PEZ; lauf++) {
				glVertex2f(mid.x()+sin(M_PI*(float)lauf/9.0)*pen.radius,mid.y()+cos(M_PI*(float)lauf/9.0)*pen.radius);
				//qDebug() << (mid.x()+sin(M_PI*(lauf/9.0)*10.0)) << " " << mid.y()+cos(M_PI*(lauf/9.0)*10.0) << endl;
			}
			glEnd();
		}
	
		glBegin(GL_LINE_LOOP);
		for (int lauf=0; lauf<PEZ; lauf++) {
			glVertex2f(mid.x()+sin(M_PI*(float)lauf/9.0)*pen.radius,mid.y()+cos(M_PI*(float)lauf/9.0)*pen.radius);
			//qDebug() << (mid.x()+sin(M_PI*(lauf/9.0)*10.0)) << " " << mid.y()+cos(M_PI*(lauf/9.0)*10.0) << endl;
		}
		glEnd();
	
		glDisable(GL_LOGIC_OP);

		/*glDisable(GL_DEPTH_TEST);
		glBegin(GL_TRIANGLES);
		Color4b co=paintbox->getColor(curr_mouse);
		glColor3f(co[0],co[1],co[2]);
		for (int lauf=0; lauf<PEZ; lauf++) {
			glVertex2f(mid.x(),mid.y());
			glVertex2f(mid.x()+sin(M_PI*(float)lauf/9.0)*pen.width.x()/2.0,mid.y()+cos(M_PI*(float)lauf/9.0)*pen.width.y()/2.0);
			glVertex2f(mid.x()+sin(M_PI*(float)(lauf+1)/9.0)*pen.width.x()/2.0,mid.y()+cos(M_PI*(float)(lauf+1)/9.0)*pen.width.y()/2.0);
		}
		glEnd();*/
	
	// Closing 2D
		glPopAttrib();
		glPopMatrix(); // restore modelview
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	} else {
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
			calcCoord(sin(M_PI*(double)lauf/(double)(PEZ/2))*radius,cos(M_PI*(double)lauf/(double)(PEZ/2))*radius,diag,inv_mvmatrix,&tx,&ty,&tz);
			//glVertex3f(tx,ty,tz);
			gluProject(tx,ty,tz,mvmatrix,projmatrix,viewport,&a,&b,&c);

			calcCoord(sin(M_PI*(double)lauf/(double)(PEZ/2))*radius,cos(M_PI*(double)lauf/(double)(PEZ/2))*radius,0,inv_mvmatrix,&tx2,&ty2,&tz2);
			//glVertex3f(tx2,ty2,tz2);
			gluProject(tx2,ty2,tz2,mvmatrix,projmatrix,viewport,&a2,&b2,&c2);

			double da=(a-a2);// /(double)STEPS;
			double db=(b-b2);// /(double)STEPS;
			double dc=(c-c2);// /(double)STEPS;
			double pix_x=a2;
			double pix_y=b2;
			double pix_z=c2;
			for (int lauf2=0; lauf2<STEPS; lauf2++) {
				double inv_yy=gla->curSiz.height()-pix_y;
				//circle_points[lauf]=QPoint(pix_x,inv_yy);
				//qDebug() << pix_x << " "<<pix_y << endl;
				//if (pix_z<=0 || pix_z>=1)qDebug() <<"OK: "<< pix_x << " "<<pix_y <<" pix_z: "<< pix_z<<" zz: "<<endl;
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
					/*if (zz<0.99999 && zz<pix_z && pix_z>0 ) {
						circle_points[lauf]=QPoint(pix_x,inv_yy);
						//lauf2=1000;
						break;
					}*/
				
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
		
		/*glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		//Color4b co=paintbox->getColor(curr_mouse);
		//qDebug() << co[0] << " " << co[1] << " " << co[2] << endl;
		glBegin(GL_TRIANGLES);
		glColor4d(1,1,1,0.0);
		for (int lauf=0; lauf<PEZ; lauf++) {
			glVertex2f(mid.x(),gla->curSiz.height()-mid.y());
			glVertex2f(circle_points[lauf].x(),circle_points[lauf].y());
			glVertex2f(circle_points[(lauf+1)%PEZ].x(),circle_points[(lauf+1)%PEZ].y());
		} 
		glEnd();
		glDisable(GL_BLEND);*/
		glEnable(GL_COLOR_LOGIC_OP);
		glLogicOp(GL_XOR);
		glColor3f(1,1,1);
		glBegin(GL_LINE_STRIP);
		for (int lauf=0; lauf<PEZ; lauf++) {
			glVertex2f(circle_points[lauf].x(),circle_points[lauf].y());
		} glVertex2f(circle_points[0].x(),circle_points[0].y());
		/*if(doubleDraw) {
			for (int lauf=0; lauf<PEZ; lauf++) {
				glVertex2f(circle_points[lauf].x(),circle_points[lauf].y());
			} glVertex2f(circle_points[0].x(),circle_points[0].y());
		}*/
		glEnd();
		glDisable(GL_COLOR_LOGIC_OP);
		glPopAttrib();
		glPopMatrix(); // restore modelview
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	}

}

//    (Cx-Ax)(Bx-Ax) + (Cy-Ay)(By-Ay)
//r = ------------------------------
//                  L²


inline void getSurroundingFacesFF(CFaceO * fac,int vert_pos,vector<CFaceO *> *surround) {
	CVertexO * vert=fac->V(vert_pos);
	CFaceO * curr_f;
	face::Pos<CFaceO> ip(fac,vert_pos);
	do {
		curr_f=ip.f;
		ip.FlipE();
		ip.FlipF();
		if (curr_f!=ip.f) surround->push_back(ip.f);
	} while(curr_f!=fac);
}

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
		//qDebug() << "d" << endl;
		}
		curr_f=temp;
	} while (curr_f!=first_fac && curr_f!=0);
}

int EditPaintPlugin::paintType() { return paintbox->paintType(); }

void getInternFaces(MeshModel & m,vector<CMeshO::FacePointer> *actual,vector<Vert_Data> * risult, vector<CMeshO::FacePointer> * face_risult,
	GLArea * gla,Penn &pen,QPoint &cur, QPoint &prev, GLfloat * pixels,
	double mvmatrix[16],double projmatrix[16],int viewport[4], int what) {


	QHash <CFaceO *,CFaceO *> selezionati;
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
	QPointF p[3];
	QPointF z[3];
	double tx,ty,tz;

	bool backface=pen.backface;
	bool invisible=pen.invisible;

	//qDebug() << "bf: "<<backface << " inv: "<<invisible << endl;

	if (pen.painttype==1) { /// PIXEL 
		for (int lauf2=0; lauf2<temp_po.size(); lauf2++) {
			CFaceO * fac=temp_po.at(lauf2);
			int intern=0;
	
			for (int lauf=0; lauf<3; lauf++) {
				gluProject((fac)->V(lauf)->P()[0],(fac)->V(lauf)->P()[1],(fac)->V(lauf)->P()[2],mvmatrix,projmatrix,viewport,&tx,&ty,&tz);
				p[lauf]=QPointF(tx,ty);
				//qDebug() << "zzz: "<<(int)(((int)ty)*old_size.x()+(int)tx)<<" t: "<<tx<<" "<<ty<<" "<<tz<<endl;
				if (tx>=0 && tx<viewport[2] && ty>=0 && ty<viewport[3])
					z[lauf]=QPointF(tz,(GLfloat)pixels[(int)(((int)ty)*viewport[2]+(int)tx)]);
				else z[lauf]=QPoint(1,0);
				//qDebug() << "zzz_ende"<<endl;
				//float zz;
   				//glReadPixels( (int)tx, /*viewport[3]-*/(int)ty, 1, 1,GL_DEPTH_COMPONENT, GL_FLOAT, &zz );
				//qDebug () << "?? "<< /*viewport[3]-*/(int)ty<<"  "<<old_size.y()-(int)ty<< endl;
				//qDebug() << tx <<" "<<ty<<" z:"<<(float)tz<<" buf:"<<(GLfloat)pixels[(int)((/*old_size.y()-*/(int)ty)*old_size.x()+(int)tx)]
				//	<<" buf2: "<<zz<<endl;
			}
	
			if (backface || isFront(p[0],p[1],p[2])) {
				for (int lauf=0; lauf<3; lauf++) if (invisible || (z[lauf].x()<=z[lauf].y()+0.003)){
					tx=p[lauf].x();
					ty=p[lauf].y();
					float dist;
					if (isIn(mid,mid_prev,tx,ty,pen.radius,&dist)==1) {
						intern=1;
						if (what!=SELECT && !sel_vert.contains(fac->V(lauf))) {
							Vert_Data d;
							d.v=fac->V(lauf);
							d.distance=dist;
							risult->push_back(/*fac->V(lauf)*/d);
							sel_vert.insert(fac->V(lauf),fac->V(lauf));
							//qDebug() << tx << " " << ty << " " << tz <<"   "<< mid<<endl;
						}
					}
				}
				if (!intern && (pointInTriangle(mid,p[0],p[1],p[2]) || pointInTriangle(mid_prev,p[0],p[1],p[2]))) {
					intern=-1;
				}
			}
			if (intern!=0 && !selezionati.contains(fac)) {
				selezionati.insert((fac),(fac));
				actual->push_back(fac);

				vector <CFaceO *> surround;
				for (int lauf=0; lauf<3; lauf++) getSurroundingFacesVF(fac,lauf,&surround);

				for (int lauf3=0; lauf3<surround.size(); lauf3++) {
					if (!selezionati.contains(surround[lauf3])) {
						//actual->push_back(surround[lauf3]);
						temp_po.push_back(surround[lauf3]);
					} 
				}

				if (what==SELECT && intern>0) face_risult->push_back(fac);
			}
		}

	} else { /// PERCENTUAL
		double dX, dY, dZ;
		double dX2, dY2, dZ2;
		gluUnProject ((double) mid.x(), mid.y(), 0.0, mvmatrix, projmatrix, viewport, &dX, &dY, &dZ);
		gluUnProject ((double) mid.x(), mid.y(), 1.0, mvmatrix, projmatrix, viewport, &dX2, &dY2, &dZ2);
		glPushMatrix();
		glLoadIdentity();
		gluLookAt(dX,dY,dZ,dX2,dY2,dZ2,1,0,0);
		double mvmatrix2[16];
		glGetDoublev (GL_MODELVIEW_MATRIX, mvmatrix2);
		glPopMatrix();
		double inv_mvmatrix[16];
		Matrix44d temp(mvmatrix2);
		Invert(temp);
		for (int lauf=0; lauf<16; lauf++) inv_mvmatrix[lauf]=temp[lauf/4][lauf%4];

		double ttx,tty,ttz;
		calcCoord(dX,dY,0,mvmatrix,&ttx,&tty,&ttz);
		//mid=QPointF((double)mid.x()/(double)gla->curSiz.width()*2.0-1.0,(double)mid.y()/(double)gla->curSiz.height()*2.0-1.0);
		//mid=QPointF(mid.x()*2,mid.y()*2);
		//mid=QPointF(ttx,tty);
		//qDebug() <<mid<<" scale: "<<gla->trackball.track.sca<<"  "<<endl;
		/*double inv2_mvmatrix[16];
		Matrix44d temp2(projmatrix);
		Invert(temp2);
		for (int lauf=0; lauf<16; lauf++) inv2_mvmatrix[lauf]=temp2[lauf/4][lauf%4];
		calcCoord(mid.x(), mid.y(),0,inv2_mvmatrix,&ttx,&tty,&ttz);
		qDebug() << mid_temp << " "<< ttx<<" "<< tty << endl; */
	
		//calcCoord(0,pen.width.x()*paintbox->diag*0.01*0.5,0,mvmatrix,&tx,&ty,&tz);
		calcCoord(0,0,0,mvmatrix,&dX,&dY,&dZ);
		//radius=pen.width.x()*m.cm.bbox.Diag()*0.01*0.5;
		//radius=sqrt((ty-dY)*(ty-dY)+(tx-dX)*(tx-dX)+(tz-dZ)*(tz-dZ));
		calcCoord(0,1,0,mvmatrix,&tx,&ty,&tz);
		double scale_fac=sqrt((ty-dY)*(ty-dY)+(tx-dX)*(tx-dX)+(tz-dZ)*(tz-dZ));
		//qDebug() <<"scale_fac: "<< scale_fac << " radius: "<<radius<<endl;
		//for(fpi=temp.begin();fpi!=temp.end();++fpi) {
		//qDebug() << pen.radius << "  "<<scale_fac<<"  "<<old_size<<" diag: "<<m.cm.bbox.Diag()<<endl;
		float fo=gla->getFov()*0.5;
		float fov = 1.0/tan(fo*M_PI/180.0)*0.5;
		//qDebug() << fo << " " << fov << endl;
		for (int lauf2=0; lauf2<temp_po.size(); lauf2++) {
			CFaceO * fac=temp_po.at(lauf2);
			int intern=0;

			double distance[3];
			//Debug() << fov << endl;
			for (int lauf=0; lauf<3; lauf++) { 
				//TODO problema: se la proiezione cambia c'e' un errore => melgio ricalcolare mid e mid_prev con mvmatrix ???
				double dx,dy,dz; // distance
				calcCoord((fac)->V(lauf)->P()[0],(fac)->V(lauf)->P()[1],(fac)->V(lauf)->P()[2],mvmatrix,&dx,&dy,&dz);
				gluProject((fac)->V(lauf)->P()[0],(fac)->V(lauf)->P()[1],(fac)->V(lauf)->P()[2],mvmatrix,projmatrix,viewport,&tx,&ty,&tz);
				//p[lauf]=QPointF(dx,dy);
				p[lauf]=QPointF(tx,ty);
				if (tx>=0 && tx<viewport[2] && ty>=0 && ty<viewport[3])
					z[lauf]=QPointF(tz,(GLfloat)pixels[(int)(((int)ty)*viewport[2]+(int)tx)]);
				else z[lauf]=QPoint(1,0);
				distance[lauf]=dz;
				//qDebug() << dx << " " << dy << " " << dz<<" "<<mid<<endl;
				/*double a,b,c;
				calcCoord((fac)->V(lauf)->P()[0],(fac)->V(lauf)->P()[1],(fac)->V(lauf)->P()[2],mvmatrix2,&a,&b,&c);
				gluProject((fac)->V(lauf)->P()[0],(fac)->V(lauf)->P()[1],(fac)->V(lauf)->P()[2],mvmatrix,projmatrix,viewport,&tx,&ty,&tz);
				if (tx>=0 && tx<old_size.x() && ty>=0 && ty<old_size.y())
					z[lauf]=QPointF(tz,(GLfloat)pixels[(int)(((int)ty)*old_size.x()+(int)tx)]);
				else z[lauf]=QPoint(1,0);
				p[lauf]=QPointF(a,b);*/
				//gluProject((fac)->V(lauf)->P()[0],(fac)->V(lauf)->P()[1],(fac)->V(lauf)->P()[2],mvmatrix,projmatrix,viewport,&tx,&ty,&tz);
				//p[lauf]=QPointF(tx,ty);
			}
	
			if (backface || isFront(p[0],p[1],p[2])) {
				for (int lauf=0; lauf<3; lauf++)if (invisible || (z[lauf].x()<=z[lauf].y()+0.003)) {
					tx=p[lauf].x();
					ty=p[lauf].y();
					//QPointF poo(mid.x()*(distance[lauf])*-1.0,mid.y()*(distance[lauf])*-1.0);
					//if (isIn(poo,poo,tx,ty,/*0.08*gla->trackball.track.sca*/radius)==1) {
					/** i have NO idea why it works with viewport[3]*0.88 */
					float dist;
					if (isIn(mid,mid_prev,tx,ty,pen.radius*scale_fac *viewport[3]*fov/distance[lauf],&dist)==1) {
					//if (isIn(QPoint(0,0),QPoint(0,0),tx,ty,pen.radius)==1) {  
						intern=1;
						if (what!=SELECT && !sel_vert.contains(fac->V(lauf))) {
							Vert_Data d;
							d.v=fac->V(lauf);
							d.distance=dist;
							risult->push_back(/*fac->V(lauf)*/d);
							sel_vert.insert(fac->V(lauf),fac->V(lauf));
							//qDebug() << tx << " " << ty << " " << tz <<"   "<< mid<<endl;
						}
					}
				}
				if ((pointInTriangle(mid,p[0],p[1],p[2]) || pointInTriangle(mid_prev,p[0],p[1],p[2]))) {
					intern=-1;
				}
			}
			if (intern!=0 && ! selezionati.contains(fac)) {
				selezionati.insert((fac),(fac));
				actual->push_back(fac);
				/*for (int lauf=0; lauf<3; lauf++) {
					//face::Pos<CFaceO> ip(fac,lauf);
					//ip.FlipF();
					CFaceO * nf=fac->FFp(lauf);//ip.f;
					if (!selezionati.contains(nf) && !nf->IsD()) {
						//actual->push_back(nf);
						temp_po.push_back(nf);
					}
				}*/
				vector <CFaceO *> surround;
				for (int lauf=0; lauf<3; lauf++) getSurroundingFacesVF(fac,lauf,&surround);

				for (int lauf3=0; lauf3<surround.size(); lauf3++) {
					if (!selezionati.contains(surround[lauf3])) {
						//actual->push_back(surround[lauf3]);
						temp_po.push_back(surround[lauf3]);
					} 
				}
				if (what==SELECT && intern>0) face_risult->push_back(fac);
			}
		}
	}
	//qDebug() << "----------"<< endl;
}

void EditPaintPlugin::fillFrom(MeshModel & m,CFaceO * face) {
	//qDebug() << "fillFrom" << endl;
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
	//qDebug() << "getVert" << mid << endl;
	//if (getFacesAtMouse(m,tempSel)) {
	double tx,ty,tz;
	if (getFaceAtMouse(m,temp)) {
		
		QPointF point[3];
		for (int lauf=0; lauf<3; lauf++) {
			gluProject(temp->V(lauf)->P()[0],temp->V(lauf)->P()[1],temp->V(lauf)->P()[2],mvmatrix,projmatrix,viewport,&tx,&ty,&tz);
			point[lauf]=QPointF(tx,ty);
		}
		value=temp->V(getNearest(mid,point,3));
	//qDebug() << "getVert2 " <<(int)value <<  endl;
		return true;
	}
	return false;
}

bool EditPaintPlugin::getVertexesAtMouse() {
	return false;
}

GLfloat getWinkel(GLfloat px,GLfloat pz) {
	GLfloat wink;
	if ((px)!=0) {
		wink=-atan((pz)/(px))*180.0/M_PI;
		if ((px)<0) {wink=180.0+wink;}
	}else {
		if ((pz)>0) wink=-90.0;
		else wink=90.0;
	}
	return wink;
}



inline void mergeColor(double percent,const Color4b& c1,const Color4b& c2,Color4b* dest) {
	(*dest)[0]=min(255.0,(c1[0]*percent+c2[0]*(1.0-percent)));
	(*dest)[1]=min(255.0,(c1[1]*percent+c2[1]*(1.0-percent)));
	(*dest)[2]=min(255.0,(c1[2]*percent+c2[2]*(1.0-percent)));
	(*dest)[3]=min(255.0,(c1[3]*percent+c2[3]*(1.0-percent)));
	//qDebug() << (*dest)[0] << " "<< (*dest)[1] << " "<< (*dest)[2]<< "percent: "<<percent<<endl;
}

inline void mergePos(double percent,const float c1[3],const float c2[3], float dest[3]) {
	dest[0]=c1[0]*percent+c2[0]*(1.0-percent);
	dest[1]=c1[1]*percent+c2[1]*(1.0-percent);
	dest[2]=c1[2]*percent+c2[2]*(1.0-percent);
	//qDebug() << (*dest)[0] << " "<< (*dest)[1] << " "<< (*dest)[2]<< "percent: "<<percent<<endl;
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
	//vector<CMeshO::FacePointer>::iterator fpi;
	//vector<CMeshO::FacePointer> temp_po;

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
void EditPaintPlugin::Decorate(QAction * ac, MeshModel &m, GLArea * gla) {
	//qDebug()<<"startdecor"<<endl;
	//if (cur.x()==0 && cur.y()==0) return;
	updateMatrixes();
	QPoint mid=QPoint(cur.x(),gla->curSiz.height()-  cur.y());
	int utensil=paintbox->paintUtensil();
	if (first) {
		//worker->waitTillPause();
		first=false;
		if (pixels!=0) { free(pixels); }
		pixels=(GLfloat *)malloc(sizeof(GLfloat)*gla->curSiz.width()*gla->curSiz.height());
		glReadPixels(0,0,gla->curSiz.width(),gla->curSiz.height(),GL_DEPTH_COMPONENT,GL_FLOAT,pixels);
		//worker->clear(pixels);
		//worker->setModelArea(&m,gla);
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

	DrawXORRect(m,gla,false);
	/*PaintData pa;pa.start=cur;pa.end=prev;pa.pen=pen;
	pa.color=paintbox->getColor(curr_mouse);
	pa.opac=paintbox->getOpacity();
	worker->addData(pa);*/

	vector<Vert_Data>::iterator vpo;
	//vector<CMeshO::VertexPointer> newSel;
	vector<Vert_Data> newSel;
	
	vector<CMeshO::FacePointer> faceSel;
	
	if (paintbox->searchMode()==2) curSel.clear();
	//GLPickTri<CMeshO>::PickFace(mid.x(), mid.y(), m.cm, curSel, pen.width.x(), pen.width.y());
	//CFaceO * tmp=0;
	//if (GLPickTri<CMeshO>::PickNearestFace(mid.x(), mid.y(), m.cm, tmp, pen.width.x(), pen.width.y()))

	getInternFaces(m,&curSel,&newSel,&faceSel,gla,pen,cur,prev,pixels,mvmatrix,projmatrix,viewport,utensil);

	UndoItem u;
	QHash <CVertexO *,Vert_Data_3> originals;
	if (utensil==SMOOTH || utensil==POLY_SMOOTH) {
		visited_vertexes.clear();
		Color4b newcol,destCol;
		int opac=paintbox->getSmoothPercentual();
		int c_r,c_g,c_b;
		float p_x,p_y,p_z;
		float newpos[3],destpos[3];
		int decrease_pos=100-paintbox->getDecreasePercentual();
		for(vpo=newSel.begin();vpo!=newSel.end();++vpo) {
			CVertexO * v=vpo->v;
			//float distence=vpo->distance;
			if (!visited_vertexes.contains(v)) {
				Vert_Data_2 ved; ved.distance=0;
				//CVertexO * vert=v;
				u.vertex=v;
				u.original=v->C();
				Vert_Data_3 d; d.color=Color4b(v->C()[0],v->C()[1],v->C()[2],v->C()[3]); //d.distance=vpo->distance;
				for (int lauf=0; lauf<3; lauf++) d.pos[lauf]=v->P()[lauf];
				originals.insert(v,d);
				color_undo[current_gla]->addItem(u);
				visited_vertexes.insert(v,/*v->C()*/ved);
				//qDebug() << "a" << endl;
				CFaceO * f= v->VFp();
				CFaceO * one_face=f;
				int pos=v->VFi();
				c_r=c_g=c_b=0;
				p_x=p_y=p_z=0;
				int count_me=0;
				//qDebug() << "b: " <<pos<< " "<<(int)one_face<<endl;
				//vector <CFaceO *> surround;
				//getSurroundingFaces();
				do {
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
				if (count_me>0) {
					float op=0.0;
					//qDebug() << vpo->distance << endl;
					if (vpo->distance*100.0>(float)decrease_pos) {
						op=(vpo->distance*100.0-(float)decrease_pos)/(float)(100-decrease_pos);
						//op=op*op;
					}
					//qDebug() << "coloro" <<endl;
					if (utensil==SMOOTH) {
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
				//colorize(*vpo,newcol,opac);
			}
		}
		/*for(vpo=newSel.begin();vpo!=newSel.end();++vpo) {
			CVertexO * v=vpo->v;
		}*/
	} else {
		if (utensil==SELECT) {
			vector<CMeshO::FacePointer>::iterator fpo;
			bool sel_or_not=(curr_mouse==Qt::LeftButton);
			for(fpo=faceSel.begin();fpo!=faceSel.end();++fpo) {
				if (sel_or_not) (*fpo)->SetS();
				else (*fpo)->ClearS();
			}
		} else {
			int opac=paintbox->getOpacity();
			int decrease_pos=100-paintbox->getDecreasePercentual(); // 100 .. all opac ; 0 .. nothing opac
			Color4b newcol=paintbox->getColor(curr_mouse);
			for(vpo=newSel.begin();vpo!=newSel.end();++vpo) {
				CVertexO * v=vpo->v;
				float op=0.0;
				//qDebug() << vpo->distance << endl;
				if (vpo->distance*100.0>(float)decrease_pos) {
					op=(vpo->distance*100.0-(float)decrease_pos)/(float)(100-decrease_pos);
					//op=op*op;
				}
				if (!visited_vertexes.contains(v)) {
					//qDebug() << "new" << endl;
					Vert_Data_2 ved; 
					ved.distance=opac-op*opac;
					ved.color=Color4b(v->C()[0],v->C()[1],v->C()[2],v->C()[3]);
					visited_vertexes.insert(v,/*v->C()*/ved);
					u.vertex=v;
					u.original=v->C();
					color_undo[current_gla]->addItem(u);
					colorize(v,newcol,opac-op*opac);
				} else {
					if (visited_vertexes[v].distance<opac-op*opac) {
					visited_vertexes[v].distance=opac-op*opac;
					Color4b temp=visited_vertexes[v].color;
					v->C()[0]=temp[0];v->C()[1]=temp[1];v->C()[2]=temp[2];
					//qDebug() <<temporaneo.size() << temporaneo[v].distance << "  " << opac-op*opac << " "<<temp[0] <<endl;
					colorize(v,newcol,opac-op*opac);//TODO ERROR BECAUSE NEW COLOR
					}
				}
			}
			
		}
	}
	pressed=0;
	} 
	//qDebug()<<"enddecor"<<endl;
}

//void EditPaintPlugin::updateMe() {}


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

PaintWorker::PaintWorker() {
	nothingTodo=true;
	gla=0;
	mesh=0;
}

void PaintWorker::run() {
	int lauf=0;
	//exec();
	while(true) {
		PaintData data;
		mutex.lock();//------------------
		if (dati.size()==0) {
			nothingTodo=true;
			if (gla!=0) gla->update();
			//qDebug() << "worker-wait"<<endl;
			condition.wait(&mutex);
			//qDebug() << "worker-wait-end"<<endl;
			nothingTodo=false;
		} 
		data=dati[0];
		dati.pop_front();
		//qDebug() << "worker-wait-end2"<<endl;
		mutex.unlock();//-----------------
		//qDebug() << data.start<< " "<<data.end <<lauf << endl;

		vector<CMeshO::VertexPointer>::iterator vpo;
		vector<CMeshO::VertexPointer> newSel;
		//qDebug() << "worker-wait-end3"<<endl;
		//if (curSel.size()==0) {
			//getFacesAtMouse(m,curSel);
			//GLPickTri<CMeshO>::PickFace(data.start.x(),gla->curSiz.height()-  data.start.y(), mesh->cm, curSel, data.pen.radius, data.pen.radius);
			//CFaceO * tmp=0;
			/*CMeshO::FaceIterator fi;
        		for(fi=mesh->cm.face.begin();fi!=mesh->cm.face.end();++fi) 
            		if(!(*fi).IsD()) {
				curSel.push_back((&*fi));
			}*/
			curSel.clear();
			
		//}
		//qDebug() << "worker-wait-end4"<<endl;
		/*QPoint old_size=QPoint(gla->curSiz.width(),gla->curSiz.height());
		getInternFaces(*mesh,&curSel,&newSel,gla,data.pen,data.start,data.end,pixels,mvmatrix,projmatrix,viewport);
	
		for(vpo=newSel.begin();vpo!=newSel.end();++vpo) {
			if (!temporaneo.contains(*vpo)) {
				temporaneo.insert(*vpo,(*vpo)->C());
				colorize(*vpo,data.color,data.opac);
			}
		}
		if (gla!=0) gla->update();*/
	}
}


Q_EXPORT_PLUGIN(EditPaintPlugin)
