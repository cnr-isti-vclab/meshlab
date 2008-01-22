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
  History
$Log: meshedit.cpp,v $
****************************************************************************/
#include <QtGui>

#include <math.h>
#include <stdlib.h>
#include <meshlab/glarea.h>
#include "editmeasure.h"
#include <wrap/gl/picking.h>
using namespace vcg;
 #include <QFontMetricsF>

EditMeasurePlugin::EditMeasurePlugin() {
  	haveToPick=false;

	isDragging=false;
	qFont.setFamily("Helvetica");
	qFont.setPixelSize(10);
	
	actionList << new QAction(QIcon(":/images/icon_measure.png"),"Measuring Tool", this);
  foreach(QAction *editAction, actionList)
    editAction->setCheckable(true);      
}

QList<QAction *> EditMeasurePlugin::actions() const {
	return actionList;
}


const QString EditMeasurePlugin::Info(QAction *action) 
{
  if( action->text() != tr("Measuring Tool") ) assert (0);

	return tr("Allow to measure distances between points of a model");
}

const PluginInfo &EditMeasurePlugin::Info() 
{
   static PluginInfo ai; 
   ai.Date=tr(__DATE__);
	 ai.Version = tr("1.0");
	 ai.Author = ("Paolo Cignoni");
   return ai;
} 
 
void EditMeasurePlugin::mouseMoveEvent  (QAction *,QMouseEvent * event, MeshModel &/*m*/, GLArea * gla)
{
	if(isDragging) 
		{
			qDebug("pressed");
			gla->update();
			cur=event->pos();
		}
}
void EditMeasurePlugin::mouseReleaseEvent  (QAction *,QMouseEvent * event, MeshModel &/*m*/, GLArea * gla)
{
	gla->update();
	cur=event->pos();
	haveToPick=true;
}

void EditMeasurePlugin::Decorate(QAction * /*ac*/, MeshModel &m, GLArea * gla)
{
	// Picking and management of internal state
	if(haveToPick)  
	{
		Point3f pt;
		bool ret=Pick(cur.x(), gla->height() - cur.y(), pt);
		if(ret) // we have not picked the background
		{
				if(isDragging) 
				{
						endPoint = pt;
						isDragging = false;		
						suspendEditToggle();
				}
				else
				{	
					qStartPoint = cur;
					 startPoint = pt;
					 isDragging = true;
				}
		}
		gla->setMouseTracking(isDragging);
		haveToPick=false;
	}   
	
	// Drawing of the current line
	glPushAttrib(GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT | GL_LINE_BIT );
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glDepthMask(false);
	glLineWidth(2.5);
	glColor3f(1.0,0.6,0.3);
	
 if(isDragging)
 {  
		Point3f wStartPoint = PixelConvert(startPoint);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0,gla->width(),gla->height(),0);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		glDisable(GL_DEPTH_TEST);
				glBegin(GL_LINES);
								glVertex2f(wStartPoint[0],wStartPoint[1]);
								glVertex2f(cur.x(),cur.y());
				glEnd();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
 }
else if(startPoint != endPoint)
{	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);
    glBegin(GL_LINES);
			glVertex(startPoint);
			glVertex(endPoint);
		glEnd();
		glDisable(GL_DEPTH_TEST);
		glLineWidth(0.7);
		glBegin(GL_LINES);
			glVertex(startPoint);
			glVertex(endPoint);
		glEnd();	
		Point3f wStart = PixelConvert(startPoint);
		Point3f wEnd   = PixelConvert(endPoint);
		glColor3f(1.0,1.0,1.0);
		if(wStart[0]>wEnd[0])
					RenderLabel(wStart[0]+5,wStart[1],QString(" %1 ").arg(Distance(startPoint,endPoint)),gla);
		else 	RenderLabel(wEnd[0]+5,wEnd[1]    ,QString(" %1 ").arg(Distance(startPoint,endPoint)),gla);
}
		glPopAttrib();
  assert(!glGetError());
}

void EditMeasurePlugin::StartEdit(QAction * /*mode*/, MeshModel &/*m*/, GLArea *gla )
{
	gla->setCursor(QCursor(QPixmap(":/images/cur_measure.png"),15,15));	
  connect(this, SIGNAL(suspendEditToggle()),gla,SLOT(suspendEditToggle()) );
}

void EditMeasurePlugin::RenderLabel(int x, int y, QString text,GLArea * gla)
{
 QFontMetrics fm(qFont);
 QRect brec=fm.boundingRect(text);
//	qDebug("brec = %i %i %i %i",brec.left(),brec.right(),brec.top(),brec.bottom());
 	glPushAttrib(GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT | GL_LINE_BIT );
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0,gla->width(),gla->height(),0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
				glColor4f(0,0,0,0.5);
				glBegin(GL_QUADS);
								glVertex2f(x+brec.left(),y+brec.bottom());
								glVertex2f(x+brec.right(),y+brec.bottom());
								glVertex2f(x+brec.right(),y+brec.top());
								glVertex2f(x+brec.left(),y+brec.top());
				glEnd();
				int offset=2;
				glColor4f(0,0,0,0.2);
				glBegin(GL_QUADS);
								glVertex2f(x+brec.left()-offset,y+brec.bottom()+offset);
								glVertex2f(x+brec.right()+offset,y+brec.bottom()+offset);
								glVertex2f(x+brec.right()+offset,y+brec.top()-offset);
								glVertex2f(x+brec.left()-offset,y+brec.top()-offset);
				glEnd();
 glColor3f(1,1,1);
 gla->renderText(x,y,0.99f,text,qFont);
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		glPopAttrib();  
}


Point3f EditMeasurePlugin::PixelConvert(vcg::Point3f pointA)
{
	GLint			viewportV[4];
	GLdouble		modelM[16];
	GLdouble		projectionM[16];
	glGetIntegerv(GL_VIEWPORT, viewportV);
	glGetDoublev(GL_MODELVIEW_MATRIX, modelM);
	glGetDoublev(GL_PROJECTION_MATRIX, projectionM);
	GLdouble wx,wy,wz;
	gluProject(pointA[0], pointA[1], pointA[2], modelM, projectionM, viewportV, &wx, &wy, &wz);
	return Point3f(wx,viewportV[3]-wy,wz);
}


Q_EXPORT_PLUGIN(EditMeasurePlugin)
