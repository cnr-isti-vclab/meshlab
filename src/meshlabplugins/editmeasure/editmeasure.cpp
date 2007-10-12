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
#include "EditMeasure.h"
#include <wrap/gl/picking.h>
using namespace vcg;

EditMeasurePlugin::EditMeasurePlugin() {
  	haveToPick=false;

	isDragging=false;
	qFont.setFamily("Helvetica");
	qFont.setPixelSize(10);
	
	actionList << new QAction(QIcon(":/images/icon_measure.png"),"Get Info", this);
  foreach(QAction *editAction, actionList)
    editAction->setCheckable(true);      
}

QList<QAction *> EditMeasurePlugin::actions() const {
	return actionList;
}


const QString EditMeasurePlugin::Info(QAction *action) 
{
  if( action->text() != tr("Get Info") ) assert (0);

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
		Point3f wStart =PixelConvert(startPoint);
		Point3f wEnd =PixelConvert(endPoint);
		glColor3f(1.0,1.0,1.0);
		if(wStart[0]>wEnd[0])
					RenderLabel(wStart[0]+5,wStart[1],QString("%1").arg(Distance(startPoint,endPoint)),gla);
		else 	RenderLabel(wEnd[0]+5,wEnd[1]    ,QString("%1").arg(Distance(startPoint,endPoint)),gla);
}
		glPopAttrib();
  assert(!glGetError());
}

void EditMeasurePlugin::StartEdit(QAction * /*mode*/, MeshModel &/*m*/, GLArea *gla )
{
	gla->setCursor(QCursor(QPixmap(":/images/cur_measure.png"),15,15));	
}

void EditMeasurePlugin::RenderLabel(int x, int y, QString text,GLArea * gla)
{
 QFontMetrics fm(qFont);
 int tw = fm.width(text); //textWidthInPixels
 int th = fm.height();  //textHeightInPixels

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
								glVertex2f(x,y);
								glVertex2f(x+tw,y);
								glVertex2f(x+tw,y-th);
								glVertex2f(x,y-th);
				glEnd();
				int offset=2;
				glColor4f(0,0,0,0.2);
				glBegin(GL_QUADS);
								glVertex2f(x-offset,y+offset);
								glVertex2f(x+tw+offset,y+offset);
								glVertex2f(x+tw+offset,y-th-offset);
								glVertex2f(x-offset,y-th-offset);
				glEnd();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		glPopAttrib();
 
 glColor3f(1,1,1);
 gla->renderText(x,y,text,qFont);

 
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
