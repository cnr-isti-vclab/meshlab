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
Revision 1.30  2005/12/01 17:20:48  vannini
Added basic tiled rendering functions
saveSnapshot saves a 4x resolution snapshot

Revision 1.29  2005/12/01 04:06:13  glvertex
Now the log area is a percentage of the window height

Revision 1.28  2005/12/01 03:27:33  glvertex
- Cleaned lighting code
- Minimum size now is (400,300)
- Added rendering log area on the bottom of the screen
- Solved zBuff conflicts during rendering texts

Revision 1.27  2005/11/30 16:26:56  cignoni
All the modification, restructuring seen during the 30/12 lesson...

Revision 1.26  2005/11/30 00:43:19  alemochi
FPS modified (not work correctly)

Revision 1.25  2005/11/30 00:21:07  alemochi
Added function to display fps

Revision 1.24  2005/11/29 18:32:55  alemochi
Added customize menu to change colors of environment

Revision 1.23  2005/11/29 11:22:23  vannini
Added experimental snapshot saving function

Revision 1.22  2005/11/28 21:05:37  alemochi
Added menu preferences and configurable background

Revision 1.21  2005/11/28 01:06:04  davide_portelli
Now GLA contains a list of RenderMode, instead of a single RenderMode.
Thus it is possible to have more active RenderMode (MeshRenderInterface)
at the same time, and when there are many opened windows, the menù of rendering
is consisting.

Revision 1.20  2005/11/27 12:18:58  alemochi
deleted unused variables

Revision 1.19  2005/11/27 04:04:38  glvertex
Changed literal double values to literal float to avoid conversion warnings

Revision 1.18  2005/11/27 03:50:58  glvertex
- Added method setColorMode
- Now getCurrentRenderMode is inline method

Revision 1.17  2005/11/26 21:47:37  alemochi
Solved problems about fancy lighting and double side lighting

Revision 1.16  2005/11/26 16:53:54  glvertex
getRenderState --> getCurrentRenderMode

Revision 1.15  2005/11/26 14:09:15  alemochi
Added double side lighting and fancy lighting (working only double side+fancy)

Revision 1.14  2005/11/25 11:55:59  alemochi
Added function to Enable/Disable lighting (work in progress)

Revision 1.13  2005/11/24 01:38:36  cignoni
Added new plugins intefaces, tested with shownormal render mode

Revision 1.12  2005/11/22 21:51:53  alemochi
Changed frustum values.

Revision 1.11  2005/11/20 23:48:01  alemochi
*** empty log message ***

Revision 1.10  2005/11/20 23:40:23  alemochi
Added updating normalperface

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
#include "meshmodel.h"
#include "interfaces.h"
#include "glarea.h"

using namespace vcg; 
GLArea::GLArea(QWidget *parent)
: QGLWidget(parent)
{
	iRendersList=0;
	timerFPS=new QTimer(this);
	connect(timerFPS, SIGNAL(timeout()), this, SLOT(updateFps()));
	timerFPS->start(1000);
	timerIdle=new QTimer(this);
	timerIdle->start(10);
	connect(timerIdle,SIGNAL(timeout()),this,SLOT(redraw()));
	cfps=0;
	lfps=0;
	currentHeight=100;
	currentWidth=200;
}

QSize GLArea::minimumSizeHint() const {
	return QSize(400,300);
}

QSize GLArea::sizeHint() const {
	return QSize(400,300);
}

void GLArea::initializeGL()
{
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	glEnable(GL_NORMALIZE);

	GLfloat pfront[] = {0,0,1,0};
	
	glLightfv(GL_LIGHT0,GL_POSITION,pfront);
	glEnable(GL_LIGHT0);
	
	/*glLightfv(GL_LIGHT1,GL_POSITION,pback);
	glLightfv(GL_LIGHT1,GL_DIFFUSE,l_diffuseFancy);*/
	glEnable(GL_LIGHTING);

 
	rm.drawMode	= GLW::DMSmooth;
	rm.drawColor = GLW::CMNone;
	
}

void GLArea::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

// == Backround
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glBegin(GL_TRIANGLE_STRIP);
	glColor(cs.bColorTop);  	glVertex3f(-1.f, 1.f,-1.f);
	glColor(cs.bColorBottom);	glVertex3f(-1.f,-1.f,-1.f);
	glColor(cs.bColorTop);		glVertex3f( 1.f, 1.f,-1.f);
	glColor(cs.bColorBottom);	glVertex3f( 1.f,-1.f,-1.f);

	glEnd();
	glPopAttrib();
// ==

	gluLookAt(0,0,3,   0,0,0,   0,1,0);


	trackball.center=Point3f(0, 0, 0);
	trackball.radius= 1;
	trackball.GetView();
	trackball.Apply();
	
	glColor3f(1.f,1.f,1.f);
	//Box3f bb(Point3f(-.5,-.5,-.5),Point3f(.5,.5,.5));
	//glBoxWire(bb);
	float d=1.0f/mm->cm.bbox.Diag();
	//float d=1;
	glScale(d);
	glTranslate(-mm->cm.bbox.Center());

	SetLightModel();

	// Set proper colorMode
	glDisable(GL_COLOR_MATERIAL);
	if(rm.drawColor != GLW::CMNone)
	{
		glEnable(GL_COLOR_MATERIAL);
		glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
	}

	mm->Render(rm.drawMode,rm.drawColor);


	if(iRendersList){
		pair<QAction *,MeshRenderInterface *> p;
		foreach(p,*iRendersList){p.second->Render(p.first->text(),*mm,rm,this);}
	}
	
// ==============================	
// Draw the log area background
// on the bottom of the glArea
	glPushAttrib(GL_ENABLE_BIT);
	glPushMatrix();
	glLoadIdentity();

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE,GL_SRC_ALPHA);

	cs.lColor.V(3) = 128;	// set half alpha value
	glColor(cs.lColor);
	float h = ((.03f * currentHeight) - (currentHeight>>1)) / (float)currentHeight;
	glBegin(GL_TRIANGLE_STRIP);
		glVertex3f(-1.f,h,-1.f);
		glVertex3f(-1.f,-1.f, -1.f);
		glVertex3f( 1.f,h,-1.f);
		glVertex3f( 1.f,-1.f, -1.f);
	glEnd();

	// Now print out the infos
	glColor4f(1,1,1,1);
	renderFps();
	log.glDraw(this,0,3);
	// More info to add.....

	glPopAttrib();
	glPopMatrix();

// ==============================


	cfps++;
}

void GLArea::resizeGL(int _width, int _height)
{
	//int side = qMin(width, height);
	currentWidth=_width;
	currentHeight=_height;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, float(_width)/float(_height), 0.2, 5);
	glMatrixMode(GL_MODELVIEW);

	glViewport(0,0, _width, _height);
}

void GLArea::renderSnapTile(std::vector<Color4b> &snap, bool tbVisible, bool bgVisible, GLdouble fovy, GLdouble zNear, GLdouble zFar, int totalRows, int totalCols, int currentRow, int currentCol)
{
	int vp[4];
	
	GLint old_matrixMode;
	GLdouble aspect, vpx, vpy;
	GLdouble fLeft, fRight, fBottom, fTop;			// frustum
	GLdouble tLeft, tRight, tBottom, tTop;			// tile
	GLdouble xDim, yDim, xOff, yOff, tDimX, tDimY;
    
	assert( (totalRows>0) && (currentRow>=0 ) && (currentRow<totalRows) );
	assert( (totalCols>0) && (currentCol>=0 ) && (currentCol<totalCols) );

	glGetIntegerv(GL_VIEWPORT, vp);
	vpx=vp[2];
	vpy=vp[3];
	aspect=vpx / vpy;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glLoadIdentity();

	gluLookAt(0,0,3,   0,0,0,   0,1,0);
	
	trackball.center=Point3f(0, 0, 0);
	trackball.radius= 1;
	trackball.GetView();
	trackball.Apply(false);
	
	glColor3f(1.f,1.f,1.f);

	float d=1.0f/mm->cm.bbox.Diag();
	glScale(d);
	glTranslate(-mm->cm.bbox.Center());
	
	////////
	
 	glGetIntegerv(GL_MATRIX_MODE, &old_matrixMode);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	glLoadIdentity();

	// Calcola frustum
	fTop = zNear * tan(fovy * M_PI / 360.0);
	fLeft = -fTop * aspect;
	fBottom = -fTop;
	fRight = -fLeft;

	// Dimensione totale
	xDim = abs(fLeft * 2);
	yDim = abs(fTop * 2);
	
	// Dimensione di un tile
	tDimX = xDim / totalCols;
	tDimY = yDim / totalRows; 
	
	// Offset del tile
	yOff = tDimY * currentRow;
    xOff = tDimX * currentCol;

	// Nuovo frustum
	tLeft = fLeft + xOff;
	tRight = fLeft + xOff + tDimX;
	tBottom = fTop - yOff - tDimY;
	tTop = fTop - yOff;
	glFrustum(tLeft, tRight, tBottom, tTop, zNear, zFar);
	  
	////////

	SetLightModel();

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
	mm->Render(rm.drawMode,rm.drawColor);
	
	if(iRendersList){
		pair<QAction *,MeshRenderInterface *> p;
		foreach(p,*iRendersList){p.second->Render(p.first->text(),*mm,rm,this);}
	}
	
	glPixelStorei(GL_PACK_ROW_LENGTH, 0);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	
	snap.resize(vpx * vpy);
	glReadPixels(0, 0, vpx, vpy, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid *) &snap[0]);

	// Restore state
	glPopMatrix();
	glMatrixMode(old_matrixMode);

}

bool GLArea::saveSnapshot(QString path)
{ 
	int tc,tr;
	int vp[4];
	int q;
	int bufferOffset;

	std::vector<Color4b> tile;
	std::vector<Color4b> buffer;
	
	glGetIntegerv(GL_VIEWPORT, vp);
	int vpx=vp[2];
	int vpy=vp[3];
	
	tc=tr=6;
	
	buffer.resize(vpx * tc * vpy * tr);
		
	for (int r=0; r < tr; ++r)
		for (int c=0; c < tc; ++c)
		{
			renderSnapTile(tile, false, false, 60, 0.2, 5, tr ,tc ,r ,c);
		
			bufferOffset=(vpx * vpy * (tc * r)) + (vpx * c); //offset della tile nel buffer
			q=(vpy-1) * vpx;
			for (int y=0; y < vpy; ++y)
			{
				for (int x=0; x < vpx; ++x)
				{
					buffer[bufferOffset]=tile[q];
					q++;
					bufferOffset++;
				}
				q-=(vpx*2);
				bufferOffset+=(vpx * (tc-1));
			}
		}
    
	FILE * fp = fopen(path.toLocal8Bit(),"wb");
	if (fp==0) return false;

	fprintf(fp,"P6\n%d %d\n255\n",vpx * tc, vpy * tr);

	for(int t=0; t < (vpx * tc * vpy * tr); ++t)
	{
		fwrite(&(buffer[t]),3,1,fp);
	}
	
	fclose(fp);

	return true;
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
	rm.drawMode = mode;
	updateGL();
}


void GLArea::setColorMode(vcg::GLW::ColorMode mode)
{
	rm.drawColor = mode;
	updateGL();
}


void GLArea::setLight(bool state)
{
	rm.Lighting = state;
	updateGL();
}

void GLArea::setLightMode(bool state,LightingModel lmode)
{
	switch(lmode)
	{
	case LDOUBLE:		rm.DoubleSideLighting = state;
									break;
	
	case LFANCY:		rm.FancyLighting = state;
									break;
	}
	updateGL();
}



inline void GLArea::SetLightModel()
{
  static GLfloat standard_front[]={1.f,1.f,1.f,1.f};
  static GLfloat standard_back[]={1.f,1.f,1.f,1.f};
  static GLfloat m_diffuseFancyBack[]={.81f,.61f,.61f,1.f};
  static GLfloat m_diffuseFancyFront[]={.71f,.71f,.95f,1.f};
	
	glDisable(GL_LIGHTING);
	if (rm.Lighting) 
	{
		glEnable(GL_LIGHTING);
		glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, rm.DoubleSideLighting);
		if(rm.FancyLighting)
		{
			glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diffuseFancyFront);
			glMaterialfv(GL_BACK, GL_DIFFUSE, m_diffuseFancyBack);
		}
		else
		{
			glMaterialfv(GL_FRONT, GL_DIFFUSE, standard_front);
			glMaterialfv(GL_BACK, GL_DIFFUSE, standard_back);
		}
	}
}



void GLArea::renderFps()
{
	static QFont q("Times",12);
	
	QString strInfo("FPS: ");
	QString fps;
	fps.setNum((int)lfps,10);
	strInfo+=fps;
	renderText(currentWidth-currentWidth*0.15,currentHeight-5,strInfo,q);
}


void GLArea::setCustomSetting(const ColorSetting & s)
{
	cs.bColorBottom=s.bColorBottom;
	cs.bColorTop=s.bColorTop;
	cs.lColor=s.lColor;
}

void GLArea::updateFps()
{
	lfps=cfps;
	cfps=0;
}