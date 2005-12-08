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
Revision 1.53  2005/12/08 18:21:56  vannini
Rewritten tiled rendering functions. Now we use grabFrameBuffer() instead of glReadPixels.

Known bug:
when in wireframe mode, there is a 1 pixel space between tiles on the final image...

Revision 1.52  2005/12/06 20:54:53  alemochi
Added more space between property displayed

Revision 1.51  2005/12/06 15:13:23  glvertex
Disabled textures during print info

Revision 1.50  2005/12/06 10:42:03  vannini
Snapshot dialog now works

Revision 1.49  2005/12/05 18:15:27  vannini
Added snapshot save dialog (not used yet)

Revision 1.48  2005/12/05 18:09:08  ggangemi
added:
MeshRenderInterface *iRender;
void setRender(MeshRenderInterface *);
MeshRenderInterface * getRender();

Revision 1.47  2005/12/05 12:16:46  ggangemi
iRendersList -> iDecoratorsList

Revision 1.46  2005/12/05 10:27:38  vannini
Snapshot in png format instead of ppm

Revision 1.45  2005/12/04 22:19:47  alemochi
Added in Info Pane number of triangles and vertices

Revision 1.44  2005/12/04 17:47:18  davide_portelli
Added menu windows->Next and Shortcut "CTRL+PageDown"
Added reset trackbal Shortcut "CTRL+H"
Optimize fullscreen

Revision 1.43  2005/12/04 17:00:56  vannini
Code optimization

Revision 1.42  2005/12/04 11:59:48  vannini
Tiled rendering now works.
By default "save snapshot" saves a (X*Y)*2 pixels PPM (4 tiles) where X is the glarea width and Y the glarea height

Revision 1.41  2005/12/04 11:49:39  glvertex
solved some little bugs
now texture button works (not always correct: TO FIX)

Revision 1.40  2005/12/04 10:43:45  glvertex
Some cleanup and renaming

Revision 1.39  2005/12/03 17:04:34  glvertex
Added backface culling action and slots
Added shortcuts for fancy and double lighting

Revision 1.38  2005/12/03 16:26:45  vannini
New code for snapshot and tiled rendering (disabled by default because not fully functional)

Revision 1.37  2005/12/03 15:41:21  glvertex
showLog initial value set to true

Revision 1.36  2005/12/02 17:51:17  glvertex
removed some old plugin code to testing new interfaces

Revision 1.35  2005/12/02 15:40:18  alemochi
removed unused variable

Revision 1.34  2005/12/02 15:30:36  alemochi
Changed fps, added a control

Revision 1.33  2005/12/02 13:51:43  alemochi
Changed fps (problem with initialization fps)

Revision 1.32  2005/12/02 11:57:59  glvertex
- show log
- show info area
- show trackball
- some renaming

Revision 1.31  2005/12/02 00:52:10  cignoni
Added support for textures

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
	iRender=0; //MeshRender support
	iDecoratorsList=0;
	currentTime=0;
	lastTime=0;
	deltaTime=0;
	cfps=0;
	currentHeight=100;
	currentWidth=200;
	logVisible = true;
	takeSnapTile=false;
	infoAreaVisible = false;
	trackBallVisible = true;
	time.start();
}



void GLArea::DisplayModelInfo()
{

	QString strMessage;
	QString strVertex="Vertex   "+QString("").setNum(mm->cm.vert.size(),10);
	QString strTriangle="Triangle "+QString("").setNum(mm->cm.face.size(),10);
  //strVertex+=strVertex.setNum(mm->cm.vert.size(),10);
	//strTriangle.setNum(mm->cm.face.size(),10);
  renderText(currentWidth-currentWidth*0.15,currentHeight-25,strVertex);
	renderText(currentWidth-currentWidth*0.15,currentHeight-45,strTriangle);


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
}


//void GLArea::pasteTile()
//{
//	int snapBufferOffset, q; 
//	int vpLineSize=vpWidth * SSHOT_BYTES_PER_PIXEL;
//
//	snapBufferOffset=SSHOT_BYTES_PER_PIXEL * ((vpWidth * vpHeight * (totalCols * tileRow)) + (vpWidth * tileCol)); 
//	q=vpLineSize * (vpHeight - 1);
//	
//	for (int y=0; y < vpHeight; ++y)
//	{
//		memcpy((void*) &snapBuffer[snapBufferOffset], (void*) &tileBuffer[q], vpLineSize);
//		q-=vpLineSize;
//		snapBufferOffset+=vpLineSize * totalCols;
//	}
//
//	tileCol++;
//
//	if (tileCol >= totalCols)
//	{
//		tileCol=0;
//		tileRow++;
//
//		if (tileRow >= totalRows)
//		{
//						
//			QImage img = QImage((uchar*) &snapBuffer[0], vpWidth * totalCols, vpHeight * totalRows, QImage::Format_ARGB32);
//			
//			QString outfile=ss.outdir;
//			outfile.append("/");
//			outfile.append(ss.basename);
//			
//			QString cnt;
//			cnt.setNum(ss.counter);
//
//			if (ss.counter<10)
//				cnt.prepend("0");
//			if (ss.counter<100)
//				cnt.prepend("0");
//
//			outfile.append(cnt);
//			outfile.append(".png");			
//				
//			bool ret = img.save(outfile,"PNG");		
//
//			if (ret)
//			{
//				ss.counter++;
//				if (ss.counter>999)
//					ss.counter=0;
//                log.Log(GLLogStream::Info,"Snapshot saved to %s",outfile.toLocal8Bit().constData());
//			}
//			else
//			{
//			    log.Log(GLLogStream::Error,"Error saving %s",outfile.toLocal8Bit().constData());
//			}
//
//			takeSnapTile=false;
//			delete(tileBuffer);
//			delete(snapBuffer);
//
//		}
//	}
//
//}

void GLArea::pasteTile()
{
	if (snapBuffer.isNull())
		snapBuffer = QImage(tileBuffer.width() * ss.resolution, tileBuffer.height() * ss.resolution, tileBuffer.format());

	uchar *snapPtr = snapBuffer.bits() + (tileBuffer.bytesPerLine() * tileCol) + ((totalCols * tileRow) * tileBuffer.numBytes());
	uchar *tilePtr = tileBuffer.bits();
    
	for (int y=0; y < tileBuffer.height(); y++)
	{
		memcpy((void*) snapPtr, (void*) tilePtr, tileBuffer.bytesPerLine());		
		snapPtr+=tileBuffer.bytesPerLine() * totalCols;
		tilePtr+=tileBuffer.bytesPerLine();
	}

	tileBuffer=QImage();

	tileCol++;

	if (tileCol >= totalCols)
	{
		tileCol=0;
		tileRow++;

		if (tileRow >= totalRows)
		{				
			QString outfile=ss.outdir;
			outfile.append("/");
			outfile.append(ss.basename);
			
			QString cnt;
			cnt.setNum(ss.counter);

			if (ss.counter<10)
				cnt.prepend("0");
			if (ss.counter<100)
				cnt.prepend("0");

			outfile.append(cnt);
			outfile.append(".png");			
				
			bool ret = snapBuffer.save(outfile,"PNG");		

			if (ret)
			{
				ss.counter++;
				if (ss.counter>999)
					ss.counter=0;
                log.Log(GLLogStream::Info,"Snapshot saved to %s",outfile.toLocal8Bit().constData());
			}
			else
			{
			    log.Log(GLLogStream::Error,"Error saving %s",outfile.toLocal8Bit().constData());
			}

			takeSnapTile=false;
			snapBuffer=QImage();
		}
	}
}


void GLArea::myGluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
	GLdouble fLeft, fRight, fBottom, fTop, left, right, bottom, top, xDim, yDim, xOff, yOff, tDimX, tDimY;
	
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
	yOff = tDimY * tileRow;
	xOff = tDimX * tileCol;

	// Nuovo frustum
	left = fLeft + xOff;
	right = fLeft + xOff + tDimX;
	bottom = fTop - yOff - tDimY;
	top = fTop - yOff;

	glFrustum(left, right, bottom, top, zNear, zFar);

}

void GLArea::paintGL()
{
	GLint old_matrixMode;
	lastTime=time.elapsed();
	initTexture();
	glDisable(GL_TEXTURE_2D); // FIX FIX FIX to move in trimesh.h ?
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	
	if (!takeSnapTile)
	{
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
	}

	gluLookAt(0,0,3,   0,0,0,   0,1,0);

	trackball.center=Point3f(0, 0, 0);
	trackball.radius= 1;
	trackball.GetView();
	trackball.Apply(trackBallVisible && !takeSnapTile);
	
	glColor3f(1.f,1.f,1.f);
	//Box3f bb(Point3f(-.5,-.5,-.5),Point3f(.5,.5,.5));
	//glBoxWire(bb);
	float d=1.0f/mm->cm.bbox.Diag();
	glScale(d);
	glTranslate(-mm->cm.bbox.Center());

	SetLightModel();

	// Modify frustum... 
	if (takeSnapTile)
	{
		glGetIntegerv(GL_MATRIX_MODE, &old_matrixMode);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		myGluPerspective(60, (GLdouble) vpWidth / (GLdouble) vpHeight, 0.2, 5);
		
	}

	// Set proper colorMode
	glDisable(GL_COLOR_MATERIAL);
	if(rm.colorMode != GLW::CMNone)
	{
		glEnable(GL_COLOR_MATERIAL);
		glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
	}
	
	if(iRender) {
		iRender->Render(new QAction("Toon Shader", this), *mm, rm, this); 
	}

	mm->Render(rm.drawMode,rm.colorMode,rm.textureMode);

	if(iRender) {
		glUseProgramObjectARB(0);
	}

	if(iDecoratorsList){
		pair<QAction *,MeshDecorateInterface *> p;
		foreach(p,*iDecoratorsList){p.second->Decorate(p.first,*mm,rm,this);}
	}

	// ...and take a snapshot
	if (takeSnapTile)
	{
		glPixelStorei(GL_PACK_ROW_LENGTH, 0);
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		tileBuffer=grabFrameBuffer(true);
		glPopMatrix();
		glMatrixMode(old_matrixMode);
		pasteTile();
		update();
	}
	
// ==============================	
// Draw the log area background
// on the bottom of the glArea
	if(infoAreaVisible)
	{
		glPushAttrib(GL_ENABLE_BIT);
		glPushMatrix();
		glLoadIdentity();

		glDisable(GL_TEXTURE_2D);
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
		if(logVisible)
			log.glDraw(this,0,3);
			// More info to add.....

		currentTime=time.elapsed();
		deltaTime=currentTime-lastTime;
		updateFps();
		if ((cfps>0) && (cfps<200)) renderFps();
		DisplayModelInfo();


		glPopAttrib();
		glPopMatrix();

	}

// ==============================

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
	vpWidth=_width;
	vpHeight=_height;
	glViewport(0,0, _width, _height);
}

void GLArea::saveSnapshot()
{ 
	int vp[4];
	
	totalCols=totalRows=ss.resolution;
	tileRow=tileCol=0;

	takeSnapTile=true;
	update();

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
	rm.colorMode = mode;
	updateGL();
}

// Texture loading done during the first paint.
void GLArea::initTexture()
{
  if(!mm->cm.textures.empty() && mm->glw.TMId.empty()){
        for(unsigned int i =0; i< mm->cm.textures.size();++i){
          QImage img, imgGL;
          bool ret=img.load(mm->cm.textures[i].c_str());
          imgGL=convertToGLFormat(img);
          qDebug("loaded texture %s. in %i w %i  h %i",mm->cm.textures[i].c_str(),i, imgGL.width(), imgGL.height());
          mm->glw.TMId.push_back(0);
          glEnable(GL_TEXTURE_2D);
          glGenTextures( 1, &(mm->glw.TMId.back()) );
          glBindTexture( GL_TEXTURE_2D, mm->glw.TMId.back() );
          glTexImage2D( GL_TEXTURE_2D, 0, 3, imgGL.width(), imgGL.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, imgGL.bits() );
          glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
          glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ); 
          glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	
          qDebug("loaded texture  %s. in %i",mm->cm.textures[i].c_str(),i);
        }
        setTextureMode(GLW::TMPerWedge);
      }
}

void GLArea::setTextureMode(vcg::GLW::TextureMode mode)
{
	rm.textureMode = mode;
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

void GLArea::setBackFaceCulling(bool enabled)
{
	glDisable(GL_CULL_FACE);
	if(enabled)
		glEnable(GL_CULL_FACE);

	rm.BackFaceCull = enabled;

	updateGL();
}

void GLArea::SetLightModel()
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
	QString delta;
	fps.setNum((int)cfps,10);
	strInfo+=fps;
	renderText(currentWidth-currentWidth*0.15,currentHeight-5,strInfo,q);
}


void GLArea::setCustomSetting(const ColorSetting & s)
{
	cs.bColorBottom=s.bColorBottom;
	cs.bColorTop=s.bColorTop;
	cs.lColor=s.lColor;
}
void GLArea::setSnapshotSetting(const SnapshotSetting & s)
{
	ss=s;
}

void GLArea::updateFps()
{
	static int j=0;
	float averageFps=0;
	if (deltaTime>0) fpsVector[j]=deltaTime;
	j=(j+1) % 10;
	for (int i=0;i<10;i++) averageFps+=fpsVector[i];
	cfps=1000.0f/(averageFps/10);
}
void GLArea::resetTrackBall(){trackball.Reset();updateGL();}

void GLArea::setRender(MeshRenderInterface * rend) {	iRender = rend; }
MeshRenderInterface * GLArea::getRender() { return iRender; }