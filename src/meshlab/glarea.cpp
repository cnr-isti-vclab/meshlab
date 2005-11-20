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
Revision 1.9  2005/11/20 19:30:50  glvertex
- Added lighting parameters (still working on...)
- Added logging events

Revision 1.8  2005/11/20 14:28:26  glvertex
GLW::DrawMode::DMxxx -> GLW::DMxxx in order to compile under gcc 3.3

Revision 1.7  2005/11/20 04:34:34  davide_portelli
Adding in the file menù, the list of the last open file (Recent File).

Revision 1.6  2005/11/19 18:15:20  glvertex
- Some bug removed.
- Interface more friendly.
- Background.

Revision 1.5  2005/11/19 12:14:20  glvertex
Some cleanup and renaming

Revision 1.4  2005/11/18 18:25:35  alemochi
Rename function in glArea.h

Revision 1.3  2005/11/18 18:10:28  alemochi
Aggiunto slot cambiare la modalita' di rendering

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
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_NORMALIZE);

	GLfloat p[] = {0,0,1,0};

	glLightfv(GL_LIGHT0,GL_POSITION,p);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);

	drawMode	= GLW::DMSmooth;
	drawColor = GLW::CMNone;
}

void GLArea::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glBegin(GL_TRIANGLE_STRIP);
		glColor3f(0.f,0.f,0.f);	glVertex3f(-1.f, 1.f,-1.f);
		glColor3f(.2f,.2f,.4f);	glVertex3f(-1.f,-1.f,-1.f);
		glColor3f(0.f,0.f,0.f);	glVertex3f( 1.f, 1.f,-1.f);
		glColor3f(.2f,.2f,.4f);	glVertex3f( 1.f,-1.f,-1.f);
	glEnd();
	glPopAttrib();

	gluLookAt(0,0,3,   0,0,0,   0,1,0);

	trackball.center=Point3f(0, 0, 0);
	trackball.radius= 1;
	trackball.GetView();
	trackball.Apply();

	glColor3f(1.f,1.f,1.f);
	Box3f bb(Point3f(-.5,-.5,-.5),Point3f(.5,.5,.5));
	glBoxWire(bb);
	float d=1.0f/mm->cm.bbox.Diag();
	//float d=1;
	glScale(d);
	glTranslate(-mm->cm.bbox.Center());

	mm->Render(drawMode,drawColor);

	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_LIGHTING);
		log.glDraw(this,0,3);
	glPopAttrib();
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


void GLArea::setDrawMode(vcg::GLW::DrawMode mode)
{
	drawMode = mode;
	updateGL();
}