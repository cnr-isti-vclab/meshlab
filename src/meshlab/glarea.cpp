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
Revision 1.82  2006/01/25 01:06:51  alemochi
irrelevant change

Revision 1.81  2006/01/25 00:56:51  alemochi
Added trackball to change directional lighting

Revision 1.80  2006/01/24 17:29:04  alemochi
spaces removed

Revision 1.79  2006/01/24 17:19:36  alemochi
Added help on screen (key F1)

Revision 1.78  2006/01/23 15:25:43  fmazzant
No significant changes

Revision 1.77  2006/01/23 09:09:30  fmazzant
commented code

Revision 1.76  2006/01/23 08:56:49  fmazzant
added GetMeshInfoString(int mask_meshmodel).
This member shows the information of the Mesh in terms of VC,VQ,FC,FQ,WT
where:
VC = VertColor,VQ = VertQuality,FC = FaceColor,FQ = FaceQuality,WT = WedgTexCoord

Revision 1.75  2006/01/19 23:11:39  glvertex
No significant changes

Revision 1.74  2006/01/17 16:35:27  glvertex
Added Scalable fonts

Revision 1.73  2006/01/14 10:19:06  mariolatronico
GLArea::ButtonPressed is not an aggregate type,
changed ButtonPressed::element to element

Revision 1.72  2006/01/13 10:42:44  alemochi
delete comments in paintGL

Revision 1.71  2006/01/13 10:12:59  alemochi
Added control to fov and changed behavior

Revision 1.70  2006/01/13 09:28:47  cignoni
Added scaling of texture images to a power of two

Revision 1.69  2006/01/12 22:32:00  alemochi
remove unnecessary code

Revision 1.68  2006/01/12 22:14:39  alemochi
added vertigo effect and moveable near clip plane

Revision 1.65  2006/01/10 16:29:29  alemochi
now background and panel info not move with fov

Revision 1.64  2006/01/09 18:42:50  alemochi
Added fov in Info Pane

Revision 1.63  2006/01/09 18:31:18  alemochi
Fov, work in progress.....

Revision 1.62  2006/01/07 12:07:16  glvertex
Set default font

Revision 1.61  2006/01/07 11:04:49  glvertex
Added Apply Last Filter action

Revision 1.60  2006/01/02 18:54:52  glvertex
added multilevel logging support

Revision 1.59  2005/12/22 20:01:23  glvertex
- Added support for more than one shader
- Some methods renamed
- Adjusted some accelerators keys
- Fixed up minor visual issues

Revision 1.58  2005/12/22 10:33:37  vannini
Removed old code
On SaveSnapshotDialog, "Save" button is default

Revision 1.57  2005/12/14 22:25:57  cignoni
minor formatting changes

Revision 1.56  2005/12/14 00:25:50  cignoni
completed multiple texture support

Revision 1.55  2005/12/13 14:31:51  alemochi
Changed names of member functions.

Revision 1.54  2005/12/08 22:53:11  cignoni
added missing glclearcolor

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
	iRenderer=0; //Shader support
	iDecoratorsList=0;
	currentTime=0;
	lastTime=0;
	deltaTime=0;
	cfps=0;
	fov=60;
	currentHeight=100;
	currentWidth=200;
	logVisible = true;
	takeSnapTile=false;
	infoAreaVisible = false;
	trackBallVisible = true;
	currentSharder = NULL;
	lastFilterRef = NULL;
	time.start();
	objDist=3.0;
	currLogLevel = -1;
	currentButton=GLArea::BUTTON_NONE;
	clipRatioFar=1;
	clipRatioNear=1;
	helpVisible=false;
	activeDefaultTrackball=true;
}

/*
	This member returns the information of the Mesh in terms of VC,VQ,FC,FQ,WT
	where:
	VC = VertColor,VQ = VertQuality,FC = FaceColor,FQ = FaceQuality,WT = WedgTexCoord
*/
QString GLArea::GetMeshInfoString(int mask)
{
	QString info;
	if( mask & MeshModel::IOM_VERTQUALITY){info.append("VQ ");}
	if( mask & MeshModel::IOM_VERTCOLOR){info.append("VC ");}
	if( mask & MeshModel::IOM_FACEQUALITY){info.append("FQ ");}
	if( mask & MeshModel::IOM_FACECOLOR){info.append("FC ");}
	if( mask & MeshModel::IOM_WEDGTEXCOORD){info.append("WT ");}
	return info;
}

void GLArea::displayModelInfo()
{	
	float fontSpacingV = (currentHeight*.01f)+3;
	float startPos= currentHeight-(fontSpacingV/3);

	QString strMessage;
	//QString strVertex="Vertices   "+QString("").setNum(mm->cm.vert.size(),10);
	//QString strTriangle="Faces "+QString("").setNum(mm->cm.face.size(),10);
	//strVertex+=strVertex.setNum(mm->cm.vert.size(),10);
	//strTriangle.setNum(mm->cm.face.size(),10);
	QString strNear=QString("  Nplane:%1").arg(nearPlane,2,'f',1);
	QString strFar=QString("  Fplane:%1").arg(farPlane,2,'f',1);
	QString strViewer=QString("Viewer:%1").arg(objDist,2,'f',1);
	
	//old version
	/*renderText(currentWidth-currentWidth*.15f,startPos-4*fontSpacingV,strViewer+strNear+strFar,qFont);
	renderText(currentWidth-currentWidth*.15f,startPos-3*fontSpacingV,QString("Fov ")+QString::number((int)fov,10),qFont);
	renderText(currentWidth-currentWidth*.15f,startPos-2*fontSpacingV,strVertex,qFont);
	renderText(currentWidth-currentWidth*.15f,startPos-fontSpacingV,strTriangle,qFont);*/
	//renderText(currentWidth-currentWidth*0.15,currentHeight-80,strFar);

	//new version
	renderText(currentWidth-currentWidth*.15f,startPos-4.5*fontSpacingV,strViewer+strNear+strFar,qFont);
	renderText(currentWidth-currentWidth*.15f,startPos-3*fontSpacingV,QString("Fov ")+QString::number((int)fov,10),qFont);
	renderText(currentWidth-currentWidth*.15f,startPos-1.5*fontSpacingV,GetMeshInfoString(mm->mask),qFont);
}

void GLArea::renderFps()
{	
	QString strInfo=QString("FPS: %1").arg(cfps,7,'f',1);
	renderText(currentWidth-currentWidth*.15f,currentHeight-2,strInfo,qFont);	
}


QSize GLArea::minimumSizeHint() const {return QSize(400,300);}
QSize GLArea::sizeHint() const				{return QSize(400,300);}


void GLArea::initializeGL()
{
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

	GLfloat pfront[] = {0,0,1,0};
	
	glLightfv(GL_LIGHT0,GL_POSITION,pfront);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
}



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
	glClearColor(1.0,1.0,1.0,0.0);	//vannini: alpha was 1.0
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0,0,3, 0,0,0 ,0,1,0);
	glPushMatrix();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60,(float)vpWidth/vpHeight,0.2,5);
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	
	if (!takeSnapTile)
	{
		glPushMatrix();
		glLoadIdentity();
		glPushAttrib(GL_ENABLE_BIT);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glBegin(GL_TRIANGLE_STRIP);
		glColor(cs.bColorTop);  	glVertex3f(-4.f, 4.f,-1.f);
		glColor(cs.bColorBottom);	glVertex3f(-4.f,-4.f,-1.f);
		glColor(cs.bColorTop);		glVertex3f( 4.f, 4.f,-1.f);
		glColor(cs.bColorBottom);	glVertex3f( 4.f,-4.f,-1.f);
		glEnd();		
		glPopAttrib();
		glPopMatrix();
	}

	float lightPos[]={0.0,0.0,1.0,0.0};
  glPushMatrix();
	glPushAttrib(GL_ENABLE_BIT);
	glColor3f(1,1,0);
	glDisable(GL_LIGHTING);
	trackball_light.center=Point3f(0, 0, 0);
	trackball_light.radius= 1;
	trackball_light.GetView();
	trackball_light.Apply(!(isDefaultTrackBall()));
	if (!(isDefaultTrackBall()))
	{
		glBegin(GL_LINES);
			glVertex3f(0,0,0);
			glVertex3f(0,0,200);
		glEnd();
	}
	glLightfv(GL_LIGHT0,GL_POSITION,lightPos);
	glPopAttrib();
	glPopMatrix();


	trackball.center=Point3f(0, 0, 0);
	trackball.radius= 1;
	trackball.GetView();
	trackball.Apply(trackBallVisible && !takeSnapTile);
	
	// Setting camera e projection
	setVertigoCamera();
	trackball.Apply(false);
	float d=2.0f/mm->cm.bbox.Diag();
	glScale(d);
	glTranslate(-mm->cm.bbox.Center());
  setLightModel();

	// Modify frustum... 
	if (takeSnapTile)
	{
		glGetIntegerv(GL_MATRIX_MODE, &old_matrixMode);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		myGluPerspective(fov, (GLdouble) vpWidth / (GLdouble) vpHeight, 0.2, 5);
	}

	// Set proper colorMode
	glDisable(GL_COLOR_MATERIAL);
	if(rm.colorMode != GLW::CMNone)
	{
		glEnable(GL_COLOR_MATERIAL);
		glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
	}

	if(iRenderer && currentSharder) {
		//iRender->Render(new QAction("Toon Shader", this), *mm, rm, this); 
		iRenderer->Render(currentSharder, *mm, rm, this); 

	}

	mm->Render(rm.drawMode,rm.colorMode,rm.textureMode);
	
	if(iRenderer) {
		glUseProgramObjectARB(0);
	}

	if(iDecoratorsList){
		pair<QAction *,MeshDecorateInterface *> p;
		foreach(p,*iDecoratorsList){p.second->Decorate(p.first,*mm,rm,this);}
	}

	// ...and take a snapshot
	
	glMatrixMode(GL_PROJECTION); // restore fix projection matrix
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);  // restore fix viewpoint
	glPopMatrix();

	if (takeSnapTile)
	{
		glPushAttrib(GL_ENABLE_BIT);
		glPixelStorei(GL_PACK_ROW_LENGTH, 0);
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		tileBuffer=grabFrameBuffer(true);
		glPopMatrix();
		glMatrixMode(old_matrixMode);
		pasteTile();
		update();
		glPopAttrib();
	}

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
			log.glDraw(this,currLogLevel,3,qFont);

		displayModelInfo();
		currentTime=time.elapsed();
		deltaTime=currentTime-lastTime;
		updateFps();
		if ((cfps>0) && (cfps<200)) renderFps();

		glPopAttrib();
		glPopMatrix();

	}
	if (isHelpVisible()) renderHelpOnScreen();

	
	// ==============================	

	// ==============================
}
void GLArea::resizeGL(int _width, int _height)
{
	//int side = qMin(width, height);
	currentWidth=_width;
	currentHeight=_height;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov, float(_width)/float(_height), 0.2, 5);
	glMatrixMode(GL_MODELVIEW);
	vpWidth=_width;
	vpHeight=_height;
	glViewport(0,0, _width, _height);

		// Set font size depending on window size (min = 1, max = 9)
	qFont.setPointSizeF(vcg::math::Clamp<float>(-3 + sqrtf(currentWidth*currentWidth + currentHeight*currentHeight) * .01f,1,9));
}


void GLArea::renderHelpOnScreen()
{
	glPushAttrib(GL_ENABLE_BIT);
	float maxh=-0.1154;	 // 0.1154=height/2
	float minh=0.1154;
	float minw=-0.1154*currentWidth/currentHeight; // -width/2
	float maxw=minw+abs(minw)*2*170/currentWidth;
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	//glBlendFunc(GL_ONE_MINUS_DST_COLOR,GL_ZERO); // Invert color background
	Color4b hColor(100,100,0,128);
	glColor(hColor);
	glBlendFunc(GL_ONE,GL_SRC_ALPHA);
	glBegin(GL_TRIANGLE_STRIP);
		glVertex3f(minw ,maxh, 2.8);
		glVertex3f(minw, minh, 2.8);
		glVertex3f(maxw, maxh, 2.8);
		glVertex3f(maxw, minh, 2.8);
	glEnd();
	glColor(Color4b(255,255,255,255));
	glDisable(GL_DEPTH_TEST);
  float fontSpacingV = (currentHeight*.01f)+3;
	renderText(15,1.5*fontSpacingV,QString("   HELP ON SCREEN"));
  renderText(2,3*fontSpacingV,QString("Drag: "));
	renderText(100,3*fontSpacingV,QString("Rotate"));
	renderText(2,4.5*fontSpacingV,QString("Ctrl-Drag: "));
	renderText(100,4.5*fontSpacingV,QString("Pan"));
	renderText(2,6*fontSpacingV,QString("Wheel: "));
	renderText(100,6*fontSpacingV,QString("Zoom"));
	renderText(2,7.5*fontSpacingV,QString("Shift-Drag: "));
  renderText(100,7.5*fontSpacingV,QString("Zoom"));
	renderText(2,9*fontSpacingV,QString("Shift-Wheel: "));
	renderText(100,9*fontSpacingV,QString("Vertigo Effect"));
	renderText(2,10.5*fontSpacingV,QString("Ctrl-Shift-Wheel: "));
	renderText(100,10.5*fontSpacingV,QString("Far"));
	renderText(2,12*fontSpacingV,QString("Ctrl-Wheel: "));
	renderText(100,12*fontSpacingV,QString("Near"));
	glDisable(GL_BLEND);
	glPopAttrib();
}

void GLArea::saveSnapshot()
{ 
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


void GLArea::keyPressEvent ( QKeyEvent * e )  
{
	e->ignore();
	//currentButton=GLArea::BUTTON_NONE;
	if (e->key ()==Qt::Key_F1)      {helpVisible=!helpVisible;e->accept();updateGL();}
	if (e->key ()==Qt::Key_Shift)		{currentButton|=GLArea::KEY_SHIFT;e->accept();}
	if (e->key ()==Qt::Key_Control) {currentButton|=GLArea::KEY_CTRL; e->accept();}
	if (e->key ()==Qt::Key_Alt)			{currentButton|=GLArea::KEY_ALT; e->accept();}
}


void GLArea::keyReleaseEvent ( QKeyEvent * e )
{
	e->ignore();
	if (e->key()==Qt::Key_Shift)		{currentButton-=GLArea::KEY_SHIFT;e->accept();}
	if (e->key()==Qt::Key_Control)	{currentButton-=GLArea::KEY_CTRL;e->accept();}
	if (e->key()==Qt::Key_Alt)			{currentButton-=GLArea::KEY_ALT;e->accept();}
	if (!isDefaultTrackBall())
		if (!((currentButton & KEY_SHIFT) && (currentButton & KEY_CTRL))) activeDefaultTrackball=true;
}
void GLArea::mousePressEvent(QMouseEvent*e)
{
  e->accept();
	if ((currentButton & KEY_SHIFT) && (currentButton & KEY_CTRL))
    if (e->button()==Qt::LeftButton) activeDefaultTrackball=false;
		else activeDefaultTrackball=true;

	if (isDefaultTrackBall())	trackball.MouseDown(e->x(),height()-e->y(), QT2VCG(e->button(), e->modifiers() ) );
	else trackball_light.MouseDown(e->x(),height()-e->y(), QT2VCG(e->button(), e->modifiers() ) );
	update();
}




void GLArea::mouseMoveEvent(QMouseEvent*e)
{ 
	if(e->buttons() | Qt::LeftButton) 
	{
		if (isDefaultTrackBall()) trackball.MouseMove(e->x(),height()-e->y());
		else trackball_light.MouseMove(e->x(),height()-e->y());
		update();
	}
}

void GLArea::mouseReleaseEvent(QMouseEvent*e)
{
	activeDefaultTrackball=true;
	if (isDefaultTrackBall()) trackball.MouseUp(e->x(),height()-e->y(), QT2VCG(e->button(), e->modifiers() ) );
	else trackball_light.MouseUp(e->x(),height()-e->y(), QT2VCG(e->button(), e->modifiers() ) );
	update();
}

void GLArea::wheelEvent(QWheelEvent*e)
{
	const int WHEEL_DELTA =120;
	float notch=e->delta()/ float(WHEEL_DELTA);
	
	if (currentButton & KEY_SHIFT) 
	{
		if (currentButton & KEY_CTRL)
		{
			if (notch<0) clipRatioFar*=1.2f;
			else clipRatioFar/=1.2f; 
		}
		else 
		{
			if (notch<0) fov/=1.2f;
			else fov*=1.2f;
			if (fov>90) fov=90;
			if (fov<5)  fov=5;
		}
		updateGL();
	}
	else if (currentButton & KEY_CTRL) 
	{
		if (notch<0) clipRatioNear*=1.2f;
		else clipRatioNear/=1.2f; 
		updateGL();
	}
	else { trackball.MouseWheel( e->delta()/ float(WHEEL_DELTA)); update(); }
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
		glEnable(GL_TEXTURE_2D);
		for(unsigned int i =0; i< mm->cm.textures.size();++i){
			QImage img, imgScaled, imgGL;
			img.load(mm->cm.textures[i].c_str());
      // image has to be scaled to a 2^n size. We choose the first 2^N <= picture size.
      int bestW=pow(2.0,floor(::log(double(img.width() ))/::log(2.0)));
      int bestH=pow(2.0,floor(::log(double(img.height()))/::log(2.0)));
      imgScaled=img.scaled(bestW,bestH,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
			imgGL=convertToGLFormat(imgScaled);
			qDebug("loaded texture %s. with id %i w %i  h %i",mm->cm.textures[i].c_str(),i, imgGL.width(), imgGL.height());
			mm->glw.TMId.push_back(0);

			glGenTextures( 1, &(mm->glw.TMId.back()) );
			glBindTexture( GL_TEXTURE_2D, mm->glw.TMId.back() );
			glTexImage2D( GL_TEXTURE_2D, 0, 3, imgGL.width(), imgGL.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, imgGL.bits() );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ); 
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

			qDebug("loaded texture  %s. in %i",mm->cm.textures[i].c_str(),mm->glw.TMId[i]);
		}
	}
	glDisable(GL_TEXTURE_2D);
}

void GLArea::setTextureMode(vcg::GLW::TextureMode mode)
{
	rm.textureMode = mode;
	updateGL();
}

void GLArea::setLight(bool state)
{
	rm.lighting = state;
	updateGL();
}

void GLArea::setLightMode(bool state,LightingModel lmode)
{
	switch(lmode)
	{
	case LDOUBLE:		rm.doubleSideLighting = state;
									break;
	
	case LFANCY:		rm.fancyLighting = state;
									break;
	}
	updateGL();

}

void GLArea::setBackFaceCulling(bool enabled)
{
	glDisable(GL_CULL_FACE);
	if(enabled)
		glEnable(GL_CULL_FACE);

	rm.backFaceCull = enabled;

	updateGL();
}

void GLArea::setLightModel()
{
  static GLfloat standard_front[]={1.f,1.f,1.f,1.f};
  static GLfloat standard_back[]={1.f,1.f,1.f,1.f};
  static GLfloat m_diffuseFancyBack[]={.81f,.61f,.61f,1.f};
  static GLfloat m_diffuseFancyFront[]={.71f,.71f,.95f,1.f};
	
	glDisable(GL_LIGHTING);
	if (rm.lighting) 
	{
		glEnable(GL_LIGHTING);
		glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, rm.doubleSideLighting);
		if(rm.fancyLighting)
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

void GLArea::setVertigoCamera()
{
//	GLfloat ClipRatio=1;
	GLfloat fAspect = (GLfloat)vpWidth/ vpHeight;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// Si deve mettere la camera ad una distanza che inquadri la sfera unitaria bene.
	float y=sin(vcg::math::ToRad(fov/2.0));
	float x=cos(vcg::math::ToRad(fov/2.0));
	objDist= 1.5*(x*1.0/y);
	nearPlane = objDist - 2.0*clipRatioNear;
	farPlane =  objDist + 2.0*clipRatioFar;
	if(nearPlane<=objDist/10.0) nearPlane=objDist/10.0;
	if(fov==5)
	{
		glOrtho(-1.5*fAspect,1.5*fAspect,-1.5,1.5,- 2.0*clipRatioNear, 2.0*clipRatioFar);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}
	else
	{
		gluPerspective(fov, fAspect, nearPlane, farPlane);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(0, 0, objDist,0, 0, 0, 0, 1, 0);
	}
}

void GLArea::updateFps()
{
	static int j=0;
	float averageFps=0;
  if (deltaTime>0) {
    fpsVector[j]=deltaTime;
	  j=(j+1) % 10;
  }
	for (int i=0;i<10;i++) averageFps+=fpsVector[i];
	cfps=1000.0f/(averageFps/10);
}
void GLArea::resetTrackBall(){trackball.Reset();updateGL();}

