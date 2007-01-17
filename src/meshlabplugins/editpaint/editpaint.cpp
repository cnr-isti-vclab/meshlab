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


****************************************************************************/
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
using namespace vcg;

EditPaintPlugin::EditPaintPlugin() {
	isDragging=false;
	paintbox=0;
	actionList << new QAction(QIcon(":/images/pinsel.png"),"Vertex painting", this);
	QAction *editAction;
	foreach(editAction, actionList)
	editAction->setCheckable(true);
	qDebug("CONSTRUCTOR");
	//paintbox=new PaintToolbox("Vertex painting");
	//paintbox->setVisible(false);
}

EditPaintPlugin::~EditPaintPlugin() {
	if (paintbox!=0) { delete paintbox; paintbox=0; }
	qDebug() << "~EditPaint" << endl;
}


QList<QAction *> EditPaintPlugin::actions() const {
	return actionList;
}

const QString EditPaintPlugin::Info(QAction *action) {
	if( action->text() != tr("Vertex painting") ) assert (0);
	return tr("Testo da inserire ............");
}

const PluginInfo &EditPaintPlugin::Info() {
	static PluginInfo ai; 
	ai.Date=tr(__DATE__);
	ai.Version = tr("0.00001");
	ai.Author = ("Andreas Gfrei");
	return ai;
} 

void EditPaintPlugin::mousePressEvent(QAction * ac, QMouseEvent * event, MeshModel &m, GLArea * gla) {
	LastSel.clear();
	temporaneo.clear();
	start=event->pos();
	cur=start;
	pen.pos=cur;
	QPoint mid= QPoint(pen.pos.x(),gla->curSiz.height()-pen.pos.y());
	pen.width=QPoint(paintbox->getRadius(),paintbox->getRadius());
	curSel.clear();
	curr_mouse=event->button();
	/*GLPickTri<CMeshO>::PickFace(mid.x(), mid.y(), m.cm, curSel, pen.width.x(), pen.width.y());
	qDebug() <<mid<<" trovati: "<< curSel.size() <<endl;
	isDragging = true;
	Decorate(ac,m,gla);
	gla->update();*/
}
  
void EditPaintPlugin::mouseMoveEvent(QAction *,QMouseEvent * event, MeshModel &/*m*/, GLArea * gla) {
	prev=cur;
	cur=event->pos();
	pen.pos=cur;
	isDragging = true;
	
	// now the management of the update 
	//static int lastMouse=0;
	static int lastRendering=clock();
	int curT = clock();
	//qDebug("mouseMoveEvent: curt %i last %i",curT,lastRendering);
	if(gla->lastRenderingTime() < 50 || (curT - lastRendering) > 1000 )
	{
		lastRendering=curT;
		gla->update();
	//qDebug("mouseMoveEvent: ----");
	}
	else{
		gla->makeCurrent();
		glDrawBuffer(GL_FRONT);
		DrawXORRect(gla,true);
		glDrawBuffer(GL_BACK);
		glFlush();
	}
}
  
void EditPaintPlugin::mouseReleaseEvent  (QAction *,QMouseEvent * event, MeshModel &/*m*/, GLArea * gla) {
	temporaneo.clear();
	gla->update();
	prev=cur;
	cur=event->pos();
}

void berechneKoord(float x,float y,float z,double matrix[],double *xr,double *yr,double *zr) {
	//Dreif num;
	//float x=orig.x;
	//float y=orig.y;
	//float z=orig.z;
	*xr=x*matrix[0]+y*matrix[4]+z*matrix[8]+matrix[12];
	*yr=x*matrix[1]+y*matrix[5]+z*matrix[9]+matrix[13];	
	*zr=x*matrix[2]+y*matrix[6]+z*matrix[10]+matrix[14];
	//cout << num.x<<"  "<< num.y<<"  "<< num.z<<endl;
	/*num.x=x*matrix[0]+x*matrix[1]+x*matrix[2]+matrix[12];
	num.y=y*matrix[4]+y*matrix[5]+y*matrix[6]+matrix[13];	
	num.z=z*matrix[8]+z*matrix[9]+z*matrix[10]+matrix[14];*/
	//return num;
}

void getInv(double orig[],double inv[]) {
	inv[0]=orig[0]; inv[1]=orig[4]; inv[2]=orig[8];  inv[3]=orig[12];
	inv[4]=orig[1]; inv[5]=orig[5]; inv[6]=orig[9];  inv[7]=orig[13];
	inv[8]=orig[2]; inv[9]=orig[6]; inv[10]=orig[10];inv[11]=orig[14];
	inv[12]=orig[3];inv[13]=orig[7];inv[14]=orig[11]; inv[15]=orig[15];
}


void EditPaintPlugin::DrawXORRect(GLArea * gla, bool doubleDraw) {
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
				glVertex2f(mid.x()+sin(M_PI*(float)lauf/9.0)*pen.width.x()/2.0,mid.y()+cos(M_PI*(float)lauf/9.0)*pen.width.y()/2.0);
				//qDebug() << (mid.x()+sin(M_PI*(lauf/9.0)*10.0)) << " " << mid.y()+cos(M_PI*(lauf/9.0)*10.0) << endl;
			}
			glEnd();
		}
	
		glBegin(GL_LINE_LOOP);
		for (int lauf=0; lauf<PEZ; lauf++) {
			glVertex2f(mid.x()+sin(M_PI*(float)lauf/9.0)*pen.width.x()/2.0,mid.y()+cos(M_PI*(float)lauf/9.0)*pen.width.y()/2.0);
			//qDebug() << (mid.x()+sin(M_PI*(lauf/9.0)*10.0)) << " " << mid.y()+cos(M_PI*(lauf/9.0)*10.0) << endl;
		}
		glEnd();
	
		glDisable(GL_LOGIC_OP);
	
	// Closing 2D
		glPopAttrib();
		glPopMatrix(); // restore modelview
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	} else {
		double dX, dY, dZ;
		double dX2, dY2, dZ2;
		
		double mvmatrix[16];
		double projmatrix[16];
		int viewport[4];
		
		glGetIntegerv(GL_VIEWPORT, viewport);
		glGetDoublev (GL_MODELVIEW_MATRIX, mvmatrix);
		glGetDoublev (GL_PROJECTION_MATRIX, projmatrix);
		
		QPoint mid= QPoint(cur.x(),gla->curSiz.height()-cur.y());
	
		gluUnProject ((double) mid.x(), mid.y(), 0.0, mvmatrix, projmatrix, viewport, &dX, &dY, &dZ);
		//pos_maus = Dreif ( (float) dX, (float) dY, (float) dZ );
		
		gluUnProject ((double) mid.x(), mid.y(), 1.0, mvmatrix, projmatrix, viewport, &dX2, &dY2, &dZ2);
		//pos_maus2 = Dreif ( (float) dX, (float) dY, (float) dZ );
	
		glPushAttrib(GL_ENABLE_BIT);
	//	glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_COLOR_LOGIC_OP);
		glLogicOp(GL_XOR);
		glColor3f(1,1,1);
	
		/*for (int lauf=0; lauf<PEZ; lauf++) {
			glBegin(GL_LINES);
				glVertex3f(dX+sin(M_PI*(float)lauf/9.0)*pen.width.x()*0.01,dY+cos(M_PI*(float)lauf/9.0)*pen.width.x()*0.01,dZ);
				glVertex3f(dX2+sin(M_PI*(float)lauf/9.0)*pen.width.x()*0.01,dY2+cos(M_PI*(float)lauf/9.0)*pen.width.x()*0.01,dZ2);
			glEnd();
		}*/
	
		glPushMatrix();
		glLoadIdentity();
		gluLookAt(dX,dY,dZ,dX2,dY2,dZ2,1,0,0);
		double mvmatrix2[16];
		glGetDoublev (GL_MODELVIEW_MATRIX, mvmatrix2);
		glPopMatrix();
	
		double tx,ty,tz;
	
		glPushMatrix();
		//mvmatrix[12]=mvmatrix[13]=mvmatrix[14]=0;
		//mvmatrix[0]=mvmatrix[5]=mvmatrix[10]=1;
	
		double inv_mvmatrix[16];
		//getInv(mvmatrix2,inv_mvmatrix);
	
		Matrix44d temp(mvmatrix2);
		Invert(temp);
		for (int lauf=0; lauf<16; lauf++) inv_mvmatrix[lauf]=temp[lauf/4][lauf%4];
		
		float radius=pen.width.x()*paintbox->diag*0.01*0.1;
	
		glBegin(GL_LINES);
		for (int lauf=0; lauf<PEZ; lauf++) {
			berechneKoord(sin(M_PI*(float)lauf/9.0)*radius,
					cos(M_PI*(float)lauf/9.0)*radius,
				-2000,inv_mvmatrix,&tx,&ty,&tz);
	
			glVertex3f(tx,ty,tz);
	
			berechneKoord(sin(M_PI*(float)lauf/9.0)*radius,
					cos(M_PI*(float)lauf/9.0)*radius,
				2000,inv_mvmatrix,&tx,&ty,&tz);
			glVertex3f(tx,ty,tz);
			//tx/=inv_mvmatrix[10];
			//ty/=inv_mvmatrix[10];
			//tz/=inv_mvmatrix[10];
			//gluProject(sin(M_PI*(float)lauf/9.0)*10,cos(M_PI*(float)lauf/9.0)*10,0,mvmatrix,projmatrix,viewport,&tx,&ty,&tz);
			//glVertex3f(dX,dY,dZ);
			//glVertex3f(dX2+tx,dY2+ty,dZ2+tz);
		}
		glEnd();
	
		glPopMatrix();
	
		glPopAttrib();
	}

}

int EditPaintPlugin::isIn(QPointF p0,QPointF p1,float dx,float dy,float radius) {
	float x=(dx-p0.x());
	float y=(dy-p0.y());
	float bla=x*x+y*y;
	if (bla<radius*radius) return 1;
	/*x=(dx-p1.x());
	y=(dy-p1.y());
	bla=x*x+y*y;
	if (bla<15*15) return 1;*/
	return 0;
}

bool pointInTriangle(QPointF p,QPointF a, QPointF b,QPointF c) {
	float fab=(p.y()-a.y())*(b.x()-a.x()) - (p.x()-a.x())*(b.y()-a.y());
	float fbc=(p.y()-c.y())*(a.x()-c.x()) - (p.x()-c.x())*(a.y()-c.y());
	float fca=(p.y()-b.y())*(c.x()-b.x()) - (p.x()-b.x())*(c.y()-b.y());
	if (fab*fbc>0 && fbc*fca>0) return true;
	return false;
}

bool isFront(QPointF a, QPointF b, QPointF c) {
	//return (a.x()*b.x()+a.y()*b.y())>0;
	return (b.x()-a.x())*(c.y()-a.y())-(b.y()-a.y())*(c.x()-a.x())>0;
}

int EditPaintPlugin::paintType() { return paintbox->paintType(); }

void EditPaintPlugin::getInternFaces(vector<CMeshO::FacePointer> *actual,vector<CMeshO::VertexPointer> * risult, GLArea * gla) {
	int painttype=paintType();
	QHash <CFaceO *,CFaceO *> selezionati;
	QHash <CVertexO *,CVertexO *> sel_vert;
	vector<CMeshO::FacePointer>::iterator fpi;

	vector<CMeshO::FacePointer> temp_po;
	for(fpi=actual->begin();fpi!=actual->end();++fpi) {
		temp_po.push_back(*fpi);
	}
	actual->clear();

	QPointF mid=QPointF(cur.x(),gla->curSiz.height()-  cur.y());
	QPointF mid_prev=QPointF(prev.x(),gla->curSiz.height()-  prev.y());
	QPointF p[3];
	float radius=pen.width.x()*0.5;
	double tx,ty,tz;

	double mvmatrix[16];
	double projmatrix[16];
	int viewport[4];
	
	glGetIntegerv(GL_VIEWPORT, viewport);
	glGetDoublev (GL_MODELVIEW_MATRIX, mvmatrix);
	glGetDoublev (GL_PROJECTION_MATRIX, projmatrix);

	if (painttype==1) {
		for (int lauf2=0; lauf2<temp_po.size(); lauf2++) {
			CFaceO * fac=temp_po.at(lauf2);
			bool intern=false;
	
			for (int lauf=0; lauf<3; lauf++) {
				gluProject((fac)->V(lauf)->P()[0],(fac)->V(lauf)->P()[1],(fac)->V(lauf)->P()[2],mvmatrix,projmatrix,viewport,&tx,&ty,&tz);
				p[lauf]=QPointF(tx,ty);
			}
	
			if (isFront(p[0],p[1],p[2])) {
				for (int lauf=0; lauf<3; lauf++) {
					tx=p[lauf].x();
					ty=p[lauf].y();
					if (isIn(mid,mid_prev,tx,ty,radius)==1) {
						intern=true;
						if (!sel_vert.contains(fac->V(lauf))) {
							risult->push_back(fac->V(lauf));
							sel_vert.insert(fac->V(lauf),fac->V(lauf));
							//qDebug() << tx << " " << ty << " " << tz <<"   "<< mid<<endl;
						}
					}
				}
				if ((pointInTriangle(mid,p[0],p[1],p[2]) || pointInTriangle(mid_prev,p[0],p[1],p[2]))) {
					intern=true;
				}
			}
			if (intern && ! selezionati.contains(fac)) {
				selezionati.insert((fac),(fac));
				actual->push_back(fac);
				for (int lauf=0; lauf<3; lauf++) {
					CFaceO * nf=(fac)->FFp(lauf);
					//qDebug () << "nf" << nf->V(0)->P()[0] << endl;
					if (!selezionati.contains(nf)) {
						//actual->push_back(nf);
						temp_po.push_back(nf);
					}
				}
			}
		}

	} else {
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
	
		radius=pen.width.x()*paintbox->diag*0.01*0.1;
	
		//for(fpi=temp.begin();fpi!=temp.end();++fpi) {
		for (int lauf2=0; lauf2<temp_po.size(); lauf2++) {
			CFaceO * fac=temp_po.at(lauf2);
			bool intern=false;
	
			for (int lauf=0; lauf<3; lauf++) {
				double a,b,c;
				berechneKoord((fac)->V(lauf)->P()[0],(fac)->V(lauf)->P()[1],(fac)->V(lauf)->P()[2],mvmatrix2,&a,&b,&c);
				p[lauf]=QPointF(a,b);
				//gluProject((fac)->V(lauf)->P()[0],(fac)->V(lauf)->P()[1],(fac)->V(lauf)->P()[2],mvmatrix,projmatrix,viewport,&tx,&ty,&tz);
				//p[lauf]=QPointF(tx,ty);
			}
	
			if (isFront(p[0],p[1],p[2])) {
				for (int lauf=0; lauf<3; lauf++) {
					tx=p[lauf].x();
					ty=p[lauf].y();
					//if (isIn(mid,mid_prev,tx,ty)==1) {
					if (isIn(QPoint(0,0),QPoint(0,0),tx,ty,radius)==1) {  
						intern=true;
						if (!sel_vert.contains(fac->V(lauf))) {
							risult->push_back(fac->V(lauf));
							sel_vert.insert(fac->V(lauf),fac->V(lauf));
							//qDebug() << tx << " " << ty << " " << tz <<"   "<< mid<<endl;
						}
					}
				}
				if ((pointInTriangle(mid,p[0],p[1],p[2]) || pointInTriangle(mid_prev,p[0],p[1],p[2]))) {
					intern=true;
				}
			}
			if (intern && ! selezionati.contains(fac)) {
				selezionati.insert((fac),(fac));
				actual->push_back(fac);
				for (int lauf=0; lauf<3; lauf++) {
					CFaceO * nf=(fac)->FFp(lauf);
					//qDebug () << "nf" << nf->V(0)->P()[0] << endl;
					if (!selezionati.contains(nf)) {
						//actual->push_back(nf);
						temp_po.push_back(nf);
					}
				}
			}
		}
	}


	//qDebug() << "----------"<< endl;

}

void EditPaintPlugin::Decorate(QAction * ac, MeshModel &m, GLArea * gla) {
	if(isDragging)
	{
	DrawXORRect(gla,false);
	vector<CMeshO::FacePointer>::iterator fpi;
	vector<CMeshO::VertexPointer>::iterator vpo;
	// Starting Sel 
	vector<CMeshO::VertexPointer> newSel;  
	QPoint mid=QPoint(cur.x(),gla->curSiz.height()-  cur.y());
	QPoint wid=(start-cur);
	if(wid.x()<0)  wid.setX(-wid.x());
	if(wid.y()<0)  wid.setY(-wid.y());
	
	CMeshO::FaceIterator fi;
	//for(fi=m.cm.face.begin();fi!=m.cm.face.end();++fi)
	//if(!(*fi).IsD()) (*fi).ClearS();
	if (curSel.size()==0) {
		GLPickTri<CMeshO>::PickFace(mid.x(), mid.y(), m.cm, curSel, pen.width.x(), pen.width.y());
		CFaceO * tmp=0;
		//if (GLPickTri<CMeshO>::PickNearestFace(mid.x(), mid.y(), m.cm, tmp, pen.width.x(), pen.width.y()))
		//	curSel.push_back(tmp);
		//qDebug() << "CERCO: "<<curSel.size()<<endl;
		//qDebug("Pickface: rect %i %i - %i %i",mid.x(),mid.y(),pen.width.x(),pen.width.y());
		//qDebug("Pickface: Got  %i on %i",curSel.size(),m.cm.face.size());
		//qDebug() << (int)&m<< endl;
	} else {}
	getInternFaces(&curSel,&newSel,gla);
	/*for(fpi=curSel.begin();fpi!=curSel.end();++fpi)
		for (int i=0; i<3; i++) {
				//qDebug("before: ");
				(*fpi)->V(i)->C()=paintbox->getColor(curr_mouse);
				//.SetRGB(0,0,0);
				//(*fpi)->V(i)->C()[3]=100;
				//(*fi).V(i)->C().SetRGB(255,255,0);
				//qDebug("after: "+(*fi).V(i)->C()[0]);
		}*/
	int opac=paintbox->getOpacity();
	for(vpo=newSel.begin();vpo!=newSel.end();++vpo) {
		if (!temporaneo.contains(*vpo)) {
			temporaneo.insert(*vpo,(*vpo)->C());
			Color4b temp=paintbox->getColor(curr_mouse);
			Color4b orig=(*vpo)->C();
			temp[0]=min(255,(temp[0]*opac+orig[0]*(100-opac))/100);
			temp[1]=min(255,(temp[1]*opac+orig[1]*(100-opac))/100);
			temp[2]=min(255,(temp[2]*opac+orig[2]*(100-opac))/100);
			temp[3]=min(255,(temp[3]*opac+orig[3]*(100-opac))/100);
			(*vpo)->C()=temp;
		}
	}

	/*switch(selMode)
	{ 
	case SMSub :
	for(fpi=LastSel.begin();fpi!=LastSel.end();++fpi)
		(*fpi)->SetS();
	for(fpi=NewSel.begin();fpi!=NewSel.end();++fpi)
		(*fpi)->ClearS();
	break;
	case SMAdd :
		for(fpi=LastSel.begin();fpi!=LastSel.end();++fpi)
		(*fpi)->SetS(); 
	case SMClear :
		for(fpi=NewSel.begin();fpi!=NewSel.end();++fpi)
		(*fpi)->SetS();
		break;
	}*/
	isDragging=false;
	}
	
	
}

void EditPaintPlugin::StartEdit(QAction * /*mode*/, MeshModel &m, GLArea * parent) {
	qDebug() <<"startedit"<<  endl;
	pen.width=QPoint(15,15);
	tri::UpdateBounding<CMeshO>::Box(m.cm);
	//parent->setColorMode(vcg::GLW::CMPerVert);
	//parent->mm->ioMask|=MeshModel::IOM_VERTCOLOR;
	//parent->mm->ioMask|=MeshModel::IOM_VERTQUALITY;
	if (paintbox==0) paintbox=new PaintToolbox();
	paintbox->setVisible(true);
	paintbox->diag=m.cm.bbox.Diag();
	//m.Enable(MeshModel::MM_FACECOLOR);
	m.updateDataMask(MeshModel::MM_FACECOLOR);
	m.updateDataMask(MeshModel::MM_FACETOPO);
	LastSel.clear();
	curSel.clear();
	parent->update();
}

void EditPaintPlugin::EndEdit(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/) {
	qDebug() <<"ENDEDIT"<<endl;
	if (paintbox!=0) { paintbox->setVisible(false); delete paintbox; paintbox=0; }
}

Q_EXPORT_PLUGIN(EditPaintPlugin)
