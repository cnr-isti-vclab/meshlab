/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2009                                          \/)\/    *
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

#include <GL/glew.h>
#include <QtGui>
#include <QGLWidget>
#include <wrap/gl/pick.h>


//#include "alignDialog.h"
//#include <meshlab/glarea.h>
#include "editalign.h"
#include "AlignPairWidget.h"
#include "AlignPairDialog.h"

#include <wrap/qt/trackball.h>
#include <wrap/gl/picking.h>
#include <wrap/gl/space.h>

AlignPairWidget::AlignPairWidget (QWidget * parent) :QGLWidget (parent)
{
  currentTrack=0;
	freeMesh=0;
	gluedTree=0;
	tt[0]=&trackLeft;
	tt[1]=&trackRight;	
	
	freePickedPointVec.clear();
	gluedPickedPointVec.clear();

	//resize(800,400);
	QRect rr= QApplication::desktop()->screenGeometry ( this );
	setMinimumSize(rr.width()*0.8,rr.width()*0.5);
	hasToPick=false;
	pointToPick=vcg::Point2i(-1,-1);
}


void AlignPairWidget::initMesh(MeshNode *_freeMesh, MeshTree *_gluedTree)
{
	freeMesh=_freeMesh;
	gluedTree=_gluedTree;
	assert(freeMesh->glued==false);
	assert(gluedTree->gluedNum()>0);
	updateGL();
}

void AlignPairWidget::initializeGL ()
{
  glClearColor(0, 0, 0, 0); 
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_NORMALIZE);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
}

void AlignPairWidget::paintGL ()
{
	//MeshModel * mm[2]={meshLeft,meshRight};
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	if(freeMesh==0 || gluedTree==0) return;
	
	for(int i=0;i<2;++i)
		{
			if(i==0)  glViewport (0, 0, (GLsizei) width()/2, (GLsizei) height()); 
       else			glViewport (width()/2, 0, (GLsizei) width()/2, (GLsizei) height()); 
			 
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
				gluPerspective(30, (AlignPairWidget::width()/2)/(float)AlignPairWidget::height(), 0.1, 100);
				glMatrixMode(GL_MODELVIEW);
				glLoadIdentity();
				gluLookAt(0,0,6,   0,0,0,   0,1,0);
				tt[i]->center=vcg::Point3f(0, 0, 0);
				tt[i]->radius= 1;
				tt[i]->GetView();
				tt[i]->Apply(false);
				vcg::Box3f bb;
				if(i==0) bb=freeMesh->bbox();
				else	   bb=gluedTree->gluedBBox();
				
				glPushMatrix();
					bool allowScaling = qobject_cast<AlignPairDialog *>(parent())->allowScalingCB->isChecked();
					if(allowScaling)  vcg::glScale(3.0f/bb.Diag());
					else vcg::glScale(3.0f/gluedTree->gluedBBox().Diag());
					vcg::glTranslate(-bb.Center());
					if(i==0)
						{
							freeMesh->m->Render(vcg::GLW::DMFlat,vcg::GLW::CMPerMesh,vcg::GLW::TMNone);
							drawPickedPoints(freePickedPointVec,vcg::Color4b(vcg::Color4b::Red));	
						} else				{
							foreach(MeshNode *mn, gluedTree->nodeList) 
								if(mn->glued && mn != freeMesh && mn->m->visible) mn->m->Render(vcg::GLW::DMFlat,vcg::GLW::CMPerMesh,vcg::GLW::TMNone);
							drawPickedPoints(gluedPickedPointVec,vcg::Color4b(vcg::Color4b::Blue));	
						}
								
						int pickSide= ( pointToPick[0] < (width()/2) )? 0 : 1;
						if(hasToPick && pickSide==i)
							{	
							vcg::Point3f pp;
								hasToPick=false;
								if(vcg::Pick<vcg::Point3f>(pointToPick[0],pointToPick[1],pp)) 
								{
									qDebug("Picked point %i %i -> %f %f %f",pointToPick[0],pointToPick[1],pp[0],pp[1],pp[2]);
									if(pickSide == 0) freePickedPointVec.push_back(pp);
									else gluedPickedPointVec.push_back(pp);
									hasToPick=false;
									update();
								}
							}
				glPopMatrix();
				tt[i]->DrawPostApply();
		} 
}

void AlignPairWidget::drawPickedPoints(std::vector<vcg::Point3f> &pointVec, vcg::Color4b color)
{
	glPushAttrib(GL_ENABLE_BIT | GL_POINT_BIT | GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE);
	glDepthFunc(GL_ALWAYS);
	//glDisable(GL_DEPTH_TEST);
	for(uint i=0; i<pointVec.size();++i)
		{
			vcg::Point3f &pt =pointVec[i];
			glPointSize(5.0);	glColor(vcg::Color4b::Black);
			glBegin(GL_POINTS);
				glVertex(pt);
			glEnd();
			glPointSize(3.0);	glColor(color);
			glBegin(GL_POINTS);
				glVertex(pt);
			glEnd();
    	renderText( pt[0],pt[1],pt[2], QString("%1").arg(i) );
		}
	glPopAttrib();	
}

void AlignPairWidget::keyReleaseEvent (QKeyEvent * e)
{
  e->ignore ();
	
  for(int i=0;i<2;++i)
		{
			if (e->key () == Qt::Key_Control) tt[i]->ButtonUp (QT2VCG (Qt::NoButton, Qt::ControlModifier));
			if (e->key () == Qt::Key_Shift)   tt[i]->ButtonUp (QT2VCG (Qt::NoButton, Qt::ShiftModifier));
			if (e->key () == Qt::Key_Alt)     tt[i]->ButtonUp (QT2VCG (Qt::NoButton, Qt::AltModifier));
		}
	updateGL ();
}

void AlignPairWidget::keyPressEvent (QKeyEvent * e)
{
  e->ignore ();
  for(int i=0;i<2;++i)
		{
			if (e->key () == Qt::Key_Control)  tt[i]->ButtonDown (QT2VCG (Qt::NoButton, Qt::ControlModifier));
			if (e->key () == Qt::Key_Shift)    tt[i]->ButtonDown (QT2VCG (Qt::NoButton, Qt::ShiftModifier));
			if (e->key () == Qt::Key_Alt)      tt[i]->ButtonDown (QT2VCG (Qt::NoButton, Qt::AltModifier));
		}
  updateGL ();
}
void AlignPairWidget::mouseDoubleClickEvent(QMouseEvent * e)
{
	hasToPick=true;
	pointToPick=vcg::Point2i(e->x(), height() -e->y());
	updateGL ();

}
void AlignPairWidget::mousePressEvent (QMouseEvent * e)
{
  e->accept ();
  setFocus ();
	int index = e->x () < ( width() /2) ? 0 : 1 ;
	currentTrack = tt[index];
  currentTrack->MouseDown (e->x (), height () - e->y (), QT2VCG (e->button (), e->modifiers ()));
}

void AlignPairWidget::mouseMoveEvent (QMouseEvent * e)
{
	if(!currentTrack) {
			qDebug("Warning useless mousemove");
			return;
		}
	if (e->buttons ()) {
    currentTrack->MouseMove (e->x (), height () - e->y ());
    updateGL ();
  }
}

void AlignPairWidget::mouseReleaseEvent (QMouseEvent * e)
{
		if(!currentTrack) {
			qDebug("Warning useless mouse release");
			return;
		}
  currentTrack->MouseUp (e->x (), height () - e->y (), QT2VCG (e->button (), e->modifiers ()));
	currentTrack=0;
}

void AlignPairWidget::wheelEvent (QWheelEvent * e)
{
  const int WHEEL_STEP = 120;
  AlignPairDialog * dd= qobject_cast<AlignPairDialog *>(parent());
	if(dd->allowScalingCB->isChecked())
		{
			int index = e->x () < ( width() /2) ? 0 : 1 ;
			tt[index]->MouseWheel (e->delta () / float (WHEEL_STEP), QTWheel2VCG (e->modifiers ()));
		}
	else 
	{
		tt[0]->MouseWheel (e->delta () / float (WHEEL_STEP), QTWheel2VCG (e->modifiers ()));
		tt[1]->MouseWheel (e->delta () / float (WHEEL_STEP), QTWheel2VCG (e->modifiers ()));
	}
  updateGL ();
}
