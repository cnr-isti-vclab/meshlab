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
//TODO better to vertex painting switch
//TODO bits instead of hashtables
//TODO PaintToolbox does not close
//TODO lines problem with percentual painting SOLVED BUT BLAH
----------- SOLVED: ------------
//TODO first paint hang problem -- PROBALLY SOLVED, not shure
//TODO trackball zbuffer problem OK
version 0.1 gfrei

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
	pixels=0;
	first=true;
	pressed=false;
	actionList << new QAction(QIcon(":/images/pinsel.png"),"Vertex painting", this);
	QAction *editAction;
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
	if( action->text() != tr("Vertex painting") ) assert (0);
	return tr("Colorize the polygons of your mesh");
}

const PluginInfo &EditPaintPlugin::Info() {
	static PluginInfo ai; 
	ai.Date=tr(__DATE__);
	ai.Version = tr("0.001");
	ai.Author = ("Andreas Gfrei");
	return ai;
} 

void EditPaintPlugin::mousePressEvent(QAction * ac, QMouseEvent * event, MeshModel &m, GLArea * gla) {
	has_track=gla->isTrackBallVisible();
	//qDebug() << "pressStart" << endl;
	gla->showTrackBall(false);
	LastSel.clear();
	first=true;
	temporaneo.clear();
	start=event->pos();
	cur=start;
	prev=start;
	QPoint mid= QPoint(cur.x(),gla->curSiz.height()-cur.y());

	if (paintType()==1) pen.radius=paintbox->getRadius()*0.5;
	else pen.radius=paintbox->getRadius()*m.cm.bbox.Diag()*0.01*0.5;

	//pen.width=QPoint(paintbox->getRadius(),paintbox->getRadius());
	curSel.clear();
	curr_mouse=event->button();
	inverse_y=gla->curSiz.height()-cur.y();
	/*GLPickTri<CMeshO>::PickFace(mid.x(), mid.y(), m.cm, curSel, pen.width.x(), pen.width.y());
	qDebug() <<mid<<" trovati: "<< curSel.size() <<endl;
	
	Decorate(ac,m,gla);
	gla->update();*/
	isDragging = true;
	pressed=true;
	switch (paintbox->paintUtensil()) {
		case FILL: { return; }
		case PICK: { 

			return; 
		}
		case PEN: {}
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

	prev=cur;
	cur=event->pos();
	//pen.pos=cur;
	isDragging = true;
	// now the management of the update 
	//static int lastMouse=0;
	static int lastRendering=0;//clock();
	int curT = clock();
	//qDebug("mouseMoveEvent: curt %i last %i",curT,lastRendering);
	if(gla->lastRenderingTime() < 50 || (curT - lastRendering) > 1000 )
	{
		lastRendering=curT;
		gla->update();
	//qDebug("mouseMoveEvent: ----");
	}
	else {
		gla->makeCurrent();
		glDrawBuffer(GL_FRONT);
		DrawXORRect(gla,true);
		glDrawBuffer(GL_BACK);
		glFlush();
	}
	//qDebug() << "moveEnd" << endl;
}
  
void EditPaintPlugin::mouseReleaseEvent  (QAction *,QMouseEvent * event, MeshModel &m, GLArea * gla) {
	gla->showTrackBall(has_track);
	temporaneo.clear();
	gla->update();
	prev=cur;
	cur=event->pos();
	switch (paintbox->paintUtensil()) {
		case FILL: { return; }
		case PICK: { return; }
		case PEN: {}
	}
	isDragging=false;
}

void calcCoord(float x,float y,float z,double matrix[],double *xr,double *yr,double *zr) {
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
		
		float radius=pen.radius;
	
		glBegin(GL_LINES);
		for (int lauf=0; lauf<PEZ; lauf++) {
			calcCoord(sin(M_PI*(double)lauf/9.0)*radius,
					cos(M_PI*(double)lauf/9.0)*radius,
				-1000,inv_mvmatrix,&tx,&ty,&tz);
	
			glVertex3f(tx,ty,tz);
	
			calcCoord(sin(M_PI*(double)lauf/9.0)*radius,
					cos(M_PI*(double)lauf/9.0)*radius,
				1000,inv_mvmatrix,&tx,&ty,&tz);
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

		//TTTTTTTTTTTTEREEEEEEEEEEEEEEEEESSSSSSSSSSSSSSSSSTTTTTTTTTTTTTT
/*
		glPushAttrib(GL_ENABLE_BIT);
		//glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		//glDisable(GL_DEPTH_TEST);
		glEnable(GL_COLOR_LOGIC_OP);
		glLogicOp(GL_XOR);
		glColor3f(1,1,1);

		double tempx,tempy,tempz;
		calcCoord(dX,dY,dZ,mvmatrix,&tempx,&tempy,&tempz);
		qDebug()<< "ber1: "<<tempx<<" "<<tempy<<" "<<tempz<<endl;
		calcCoord(dX2,dY2,dZ2,mvmatrix,&tempx,&tempy,&tempz);
		qDebug()<< "ber2: "<<tempx<<" "<<tempy<<" "<<tempz<<endl;

		Matrix44d temp2(mvmatrix);
		Invert(temp2);
		for (int lauf=0; lauf<16; lauf++) inv_mvmatrix[lauf]=temp2[lauf/4][lauf%4];
		glBegin(GL_LINES);
		for (int lauf=0; lauf<PEZ; lauf++) {
			double ta,tb,tc;
			calcCoord(//sin(M_PI*(float)lauf/9.0)*radius*0.01
					-2+lauf*0.1,0,
					//cos(M_PI*(float)lauf/9.0)*radius*0.01,
				-5,inv_mvmatrix,&tx,&ty,&tz);
			glVertex3f(tx,ty,tz);
			gluProject(tx,ty,tz,mvmatrix,projmatrix,viewport,&ta,&tb,&tc);
			qDebug() <<"screen: "<< ta << " "<<  tb << " "<< tc << endl;
			calcCoord(//sin(M_PI*(float)lauf/9.0)*radius*0.01
					-2+lauf*0.1,
					0,//cos(M_PI*(float)lauf/9.0)*radius*0.01
				5,inv_mvmatrix,&tx,&ty,&tz);
			glVertex3f(tx,ty,tz);
		}
		glEnd();
		glPopAttrib();*/
		//TTTTTTEEEEEEEEESSSSSSSSSSSTTTTTTTTTTTTTT EEEEEEEENNNNNNNDDDDDDDDDEEEEEEEEE
	}

}


//                 (y2 - y1)
//    0 = y - y1 - --------- (x - x1)
//                 (x2 - x1)

//    (Cx-Ax)(Bx-Ax) + (Cy-Ay)(By-Ay)
//r = ------------------------------
//                  L²

int isIn(QPointF p0,QPointF p1,float dx,float dy,float radius) {
	//qDebug() << p0 << "  " << p1 << endl;
	float x0=(dx-p0.x());
	float y0=(dy-p0.y());
	float bla0=x0*x0+y0*y0;
	if (bla0<radius*radius) return 1;
	if (p0==p1) return 0;
	/*float x1=(dx-p1.x());
	float y1=(dy-p1.y());
	float bla1=x1*x1+y1*y1;
	if (bla1<radius*radius) return 1;*/

	/*float val=dy-p0.y() -(p1.y()-p0.y())/(p1.x()-p0.x())*(dx-p0.x());
	if (fabsf(val)<radius) {
		return 1;
	}*/

	float x2=(p1.x()-p0.x());
	float y2=(p1.y()-p0.y());
	float l=sqrt(x2*x2+y2*y2); //TODO TOGGLE FROM HERE
	float r=(dx-p0.x())*(p1.x()-p0.x())+(dy-p0.y())*(p1.y()-p0.y());
	r=r/(l*l);

	//float s=(p0.y()-dy)*(p1.x()-p0.x())-(p0.x()-dx)*(p1.y()-p0.y());
	//s=s/(l*l);
	
	float px=p0.x()+r*(p1.x()-p0.x());
	float py=p0.y()+r*(p1.y()-p0.y());

	px=px-dx;
	py=py-dy;

	if (r>0 && r<1 && (px*px+py*py<radius*radius)) return 1;

	//QPointF inver(y2,x2);
	/*
	float bla2=sqrt(sq*sq+radius*radius*4);
	if (sqrt(bla0)+sqrt(bla1)<bla2) return 1;*/
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

void getInternFaces(MeshModel & m,vector<CMeshO::FacePointer> *actual,vector<CMeshO::VertexPointer> * risult, 
	GLArea * gla,Penn &pen,QPoint &cur, QPoint &prev, QPoint&old_size, GLfloat * pixels,
	double mvmatrix[16],double projmatrix[16],int viewport[4]) {


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

	if (pen.painttype==1) { // PIXEL 
		for (int lauf2=0; lauf2<temp_po.size(); lauf2++) {
			CFaceO * fac=temp_po.at(lauf2);
			bool intern=false;
	
			for (int lauf=0; lauf<3; lauf++) {
				gluProject((fac)->V(lauf)->P()[0],(fac)->V(lauf)->P()[1],(fac)->V(lauf)->P()[2],mvmatrix,projmatrix,viewport,&tx,&ty,&tz);
				p[lauf]=QPointF(tx,ty);
				//qDebug() << "zzz: "<<(int)(((int)ty)*old_size.x()+(int)tx)<<" t: "<<tx<<" "<<ty<<" "<<tz<<endl;
				if (tx>=0 && tx<old_size.x() && ty>=0 && ty<=old_size.y())
					z[lauf]=QPointF(tz,(GLfloat)pixels[(int)(((int)ty)*old_size.x()+(int)tx)]);
				else z[lauf]=QPoint(1,0);
				//qDebug() << "zzz_ende"<<endl;
				//float zz;
   				//glReadPixels( (int)tx, /*viewport[3]-*/(int)ty, 1, 1,GL_DEPTH_COMPONENT, GL_FLOAT, &zz );
				//qDebug () << "?? "<< /*viewport[3]-*/(int)ty<<"  "<<old_size.y()-(int)ty<< endl;
				//qDebug() << tx <<" "<<ty<<" z:"<<(float)tz<<" buf:"<<(GLfloat)pixels[(int)((/*old_size.y()-*/(int)ty)*old_size.x()+(int)tx)]
				//	<<" buf2: "<<zz<<endl;
			}
	
			if (backface || isFront(p[0],p[1],p[2])) {
				for (int lauf=0; lauf<3; lauf++) if (invisible || (z[lauf].x()<=z[lauf].y()+0.005)){
					tx=p[lauf].x();
					ty=p[lauf].y();
					if (isIn(mid,mid_prev,tx,ty,pen.radius)==1) {
						intern=true;
						if (!sel_vert.contains(fac->V(lauf))) {
							risult->push_back(fac->V(lauf));
							sel_vert.insert(fac->V(lauf),fac->V(lauf));
							//qDebug() << tx << " " << ty << " " << tz <<"   "<< mid<<endl;
						}
					}
				}
				if (!intern && (pointInTriangle(mid,p[0],p[1],p[2]) || pointInTriangle(mid_prev,p[0],p[1],p[2]))) {
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

	} else { // PERCENTUAL
		double dX, dY, dZ;
		double dX2, dY2, dZ2;
		/*gluUnProject ((double) mid.x(), mid.y(), 0.0, mvmatrix, projmatrix, viewport, &dX, &dY, &dZ);
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
		for (int lauf=0; lauf<16; lauf++) inv_mvmatrix[lauf]=temp[lauf/4][lauf%4];*/

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
		for (int lauf2=0; lauf2<temp_po.size(); lauf2++) {
			CFaceO * fac=temp_po.at(lauf2);
			bool intern=false;

			double distance[3];

			for (int lauf=0; lauf<3; lauf++) { 
				//TODO problema: se la proiezione cambia c'e' un errore => melgio ricalcolare mid e mid_prev con mvmatrix ???
				double dx,dy,dz; // distance
				calcCoord((fac)->V(lauf)->P()[0],(fac)->V(lauf)->P()[1],(fac)->V(lauf)->P()[2],mvmatrix,&dx,&dy,&dz);
				gluProject((fac)->V(lauf)->P()[0],(fac)->V(lauf)->P()[1],(fac)->V(lauf)->P()[2],mvmatrix,projmatrix,viewport,&tx,&ty,&tz);
				//p[lauf]=QPointF(dx,dy);
				p[lauf]=QPointF(tx,ty);
				if (tx>=0 && tx<old_size.x() && ty>=0 && ty<=old_size.y())
					z[lauf]=QPointF(tz,(GLfloat)pixels[(int)(((int)ty)*old_size.x()+(int)tx)]);
				else z[lauf]=QPoint(1,0);
				distance[lauf]=dz;
				//qDebug() << dx << " " << dy << " " << dz<<" "<<mid<<endl;
				/*double a,b,c;
				calcCoord((fac)->V(lauf)->P()[0],(fac)->V(lauf)->P()[1],(fac)->V(lauf)->P()[2],mvmatrix2,&a,&b,&c);
				gluProject((fac)->V(lauf)->P()[0],(fac)->V(lauf)->P()[1],(fac)->V(lauf)->P()[2],mvmatrix,projmatrix,viewport,&tx,&ty,&tz);
				z[lauf]=QPointF(tz,(GLfloat)pixels[(int)(((int)ty)*old_size.x()+(int)tx)]);
				p[lauf]=QPointF(a,b);*/
				//gluProject((fac)->V(lauf)->P()[0],(fac)->V(lauf)->P()[1],(fac)->V(lauf)->P()[2],mvmatrix,projmatrix,viewport,&tx,&ty,&tz);
				//p[lauf]=QPointF(tx,ty);
			}
	
			if (backface || isFront(p[0],p[1],p[2])) {
				for (int lauf=0; lauf<3; lauf++)if (invisible || (z[lauf].x()<=z[lauf].y()+0.005)) {
					tx=p[lauf].x();
					ty=p[lauf].y();
					//QPointF poo(mid.x()*(distance[lauf])*-1.0,mid.y()*(distance[lauf])*-1.0);
					//if (isIn(poo,poo,tx,ty,/*0.08*gla->trackball.track.sca*/radius)==1) {
					//THE 2*190 should not be a constant !!!
					if (isIn(mid,mid_prev,tx,ty,pen.radius*1.0*scale_fac*190.0*4.0*1.0/distance[lauf])==1) {
					//if (isIn(QPoint(0,0),QPoint(0,0),tx,ty,radius)==1) {  
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
	for (int lauf2=0; lauf2<temp_po.size(); lauf2++) {
		CFaceO * fac=temp_po.at(lauf2);
		if (who==fac->IsS()) {
			for (int lauf=0; lauf<3; lauf++) {
				if (!temporaneo.contains(fac->V(lauf))) {
					temporaneo.insert(fac->V(lauf),fac->V(lauf));
					colorize(fac->V(lauf),newcol,opac);
				}
			}
			for (int lauf=0; lauf<3; lauf++) { 
				if (!visited.contains(fac->FFp(lauf))) {
					temp_po.push_back(fac->FFp(lauf));
					visited.insert(fac->FFp(lauf),fac->FFp(lauf));
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
	//vector<CMeshO::FacePointer> knut;
	GLPickTri<CMeshO>::PickFace(mid.x(), mid.y(), m.cm, val,2,2);
	//qDebug() << m.cm.SimplexNumber() << endl;
	//qDebug() << val.size() << " knut"<< endl;
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


void EditPaintPlugin::Decorate(QAction * ac, MeshModel &m, GLArea * gla) {
	//qDebug()<<"startdecor"<<endl;
	//if (cur.x()==0 && cur.y()==0) return;
	updateMatrixes();
	QPoint mid=QPoint(cur.x(),gla->curSiz.height()-  cur.y());
	if (first) {
		//worker->waitTillPause();
		first=false;
		if (pixels!=0) { free(pixels); }
		old_size=QPoint(gla->curSiz.width(),gla->curSiz.height());
		pixels=(GLfloat *)malloc(sizeof(GLfloat)*old_size.x()*old_size.y());
		glReadPixels(0,0,old_size.x(),old_size.y(),GL_DEPTH_COMPONENT,GL_FLOAT,pixels);
		//worker->clear(pixels);
		//worker->setModelArea(&m,gla);
	}
	if(isDragging)
	{
	//qDebug() << "decorate" << endl;
	switch (paintbox->paintUtensil()) {
		case FILL: { 
			if (!pressed) return;
			CFaceO * temp_face;
			if(getFaceAtMouse(m,temp_face)) {
				fillFrom(m,temp_face);
			}
			pressed=false;
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
			pressed=false;
			return; 
		}
		case PEN: {}
	}

	DrawXORRect(gla,false);

	pen.painttype=paintType();
	pen.backface=paintbox->getPaintBackface();
	pen.invisible=paintbox->getPaintInvisible();

	/*PaintData pa;pa.start=cur;pa.end=prev;pa.pen=pen;
	pa.color=paintbox->getColor(curr_mouse);
	pa.opac=paintbox->getOpacity();
	worker->addData(pa);*/

	vector<CMeshO::VertexPointer>::iterator vpo;
	vector<CMeshO::VertexPointer> newSel;
	
	//curSel.clear();
	//GLPickTri<CMeshO>::PickFace(mid.x(), mid.y(), m.cm, curSel, pen.width.x(), pen.width.y());
	//CFaceO * tmp=0;
	//if (GLPickTri<CMeshO>::PickNearestFace(mid.x(), mid.y(), m.cm, tmp, pen.width.x(), pen.width.y()))

	getInternFaces(m,&curSel,&newSel,gla,pen,cur,prev,old_size,pixels,mvmatrix,projmatrix,viewport);

	int opac=paintbox->getOpacity();
	Color4b newcol=paintbox->getColor(curr_mouse);
	for(vpo=newSel.begin();vpo!=newSel.end();++vpo) {
		if (!temporaneo.contains(*vpo)) {
			temporaneo.insert(*vpo,(*vpo)->C());
			colorize(*vpo,newcol,opac);
		}
	}
	isDragging=false;
	pressed=false;
	}
	//qDebug()<<"enddecor"<<endl;
}

void EditPaintPlugin::StartEdit(QAction * /*mode*/, MeshModel &m, GLArea * parent) {
	first=true;
	pressed=false;
	//qDebug() <<"startedit"<<  endl;
	//pen.radius=;
	tri::UpdateBounding<CMeshO>::Box(m.cm);
	if (paintbox==0) paintbox=new PaintToolbox(parent);
	//paintbox->setGeometry();
	paintbox->setVisible(true);
	//paintbox->diag=m.cm.bbox.Diag();
	//m.updateDataMask(MeshModel::MM_FACECOLOR);
	m.updateDataMask(MeshModel::MM_FACETOPO);
	//m.Enable(MeshModel::MM_FACECOLOR);
	//parent->setColorMode(vcg::GLW::CMPerVert);
	parent->getCurrentRenderMode().colorMode=vcg::GLW::CMPerVert;
	parent->mm->ioMask|=MeshModel::IOM_VERTCOLOR;
	parent->mm->ioMask|=MeshModel::IOM_VERTQUALITY;
	
	LastSel.clear();
	curSel.clear();
	parent->update();
}

void EditPaintPlugin::EndEdit(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/) {
	qDebug() <<"ENDEDIT"<<endl;
	if (paintbox!=0) { paintbox->setVisible(false); delete paintbox; paintbox=0; }
}

void EditPaintPlugin::updateMe() {

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
		QPoint old_size=QPoint(gla->curSiz.width(),gla->curSiz.height());
		getInternFaces(*mesh,&curSel,&newSel,gla,data.pen,data.start,data.end,old_size,pixels,mvmatrix,projmatrix,viewport);
	
		for(vpo=newSel.begin();vpo!=newSel.end();++vpo) {
			if (!temporaneo.contains(*vpo)) {
				temporaneo.insert(*vpo,(*vpo)->C());
				colorize(*vpo,data.color,data.opac);
			}
		}
		if (gla!=0) gla->update();
	}
}

Q_EXPORT_PLUGIN(EditPaintPlugin)
