/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2006                                          \/)\/    *
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

$Log$
Revision 1.2  2005/11/17 14:54:27  glvertex
Some little changes to allow differents rendering modes (not working yet)

Revision 1.1  2005/10/18 10:38:02  cignoni
First rough version. It simply load a mesh.

****************************************************************************/

#include <QtGui>

// widget capace di fare rendering di un Modello

#include <vcg/space/box3.h>
#include <GL/glew.h>
#include <wrap/gl/space.h>

#include "interfaces.h"
#include "glarea.h"
#include "meshmodel.h"

using namespace vcg; 
GLArea::GLArea(QWidget *parent)
    : QGLWidget(parent)
{
    
}

QSize GLArea::minimumSizeHint() const {
  return QSize(100,100);
}

QSize GLArea::sizeHint() const {
  return QSize(100,100);
}

void GLArea::initializeGL()
{
    glShadeModel(GL_FLAT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

		renderMode	= GLW::DMPoints;
		renderColor = GLW::CMNone;
}

void GLArea::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	  glLoadIdentity();

		// Draws a smooth background
		// Why drops so slow??

		//glDisable(GL_DEPTH_TEST);
		//glShadeModel(GL_SMOOTH);
		//glBegin(GL_TRIANGLE_STRIP);
		//	glColor3f(0.f,0.f,0.f);	glVertex3f(-1.f, 1.f,-1.f);
		//	glColor3f(.2f,.2f,.4f);	glVertex3f(-1.f,-1.f,-1.f);
		//	glColor3f(0.f,0.f,0.f);	glVertex3f( 1.f, 1.f,-1.f);
		//	glColor3f(.2f,.2f,.4f);	glVertex3f( 1.f,-1.f,-1.f);
		//glEnd();
		//glShadeModel(GL_FLAT);
		//glEnable(GL_DEPTH_TEST);

		glColor3f(1.f,1.f,1.f);
		gluLookAt(0,0,3,   0,0,0,   0,1,0);        
    
    trackball.center=Point3f(0, 0, 0);
    trackball.radius= 1;
    trackball.GetView();
    trackball.Apply();
        
    Box3f bb(Point3f(-.5,-.5,-.5),Point3f(.5,.5,.5));
    glBoxWire(bb);
    float d=1.0f/mm->cm.bbox.Diag();
    glScale(d);
    glTranslate(-mm->cm.bbox.Center());


		mm->Render(renderMode,renderColor);
}

void GLArea::resizeGL(int _width, int _height)
{
    //int side = qMin(width, height);
	  glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, float(_width)/float(_height), 1, 100);
	  glMatrixMode(GL_MODELVIEW);

    glViewport(0,0, _width, _height);
}

Trackball::Button QT2VCG(Qt::MouseButton qtbt,  Qt::KeyboardModifiers modifiers)
{
	int vcgbt=Trackball::BUTTON_NONE;
	if(qtbt == Qt::LeftButton		) vcgbt |= Trackball::BUTTON_LEFT;
	if(qtbt == Qt::RightButton		) vcgbt |= Trackball::BUTTON_RIGHT;
	if(qtbt == Qt::MidButton			) vcgbt |= Trackball::BUTTON_MIDDLE;
	if(modifiers == Qt::ShiftModifier		)	vcgbt |= Trackball::KEY_SHIFT;
	if(modifiers == Qt::ControlModifier ) vcgbt |= Trackball::KEY_CTRL;
	if(modifiers == Qt::AltModifier     ) vcgbt |= Trackball::KEY_ALT;

	return Trackball::Button(vcgbt);
}

void GLArea::mousePressEvent(QMouseEvent*e)
{
	trackball.MouseDown(e->x(),height()-e->y(), QT2VCG(e->button(), e->modifiers() ) );
	update();
}

void GLArea::mouseMoveEvent(QMouseEvent*e)
{ 
	if(e->buttons() | Qt::LeftButton) 
	{
		trackball.MouseMove(e->x(),height()-e->y());
		update();
	}
}

void GLArea::mouseReleaseEvent(QMouseEvent*e)
{
	trackball.MouseUp(e->x(),height()-e->y(), QT2VCG(e->button(), e->modifiers() ) );
	update();
}

void GLArea::wheelEvent(QWheelEvent*e)
{
	const int WHEEL_DELTA =120;
	trackball.MouseWheel( e->delta()/ float(WHEEL_DELTA) );
	update();
}
