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
Revision 1.91  2006/02/03 11:45:42  cignoni
Corrected bug for ortho trackball

Revision 1.90  2006/02/01 12:43:20  glvertex
Optimized onClose code

Revision 1.89  2006/02/01 11:49:12  glvertex
Closing confirmation for modified files

Revision 1.88  2006/01/31 17:04:44  alemochi
Changed fancy lighting (use two lights instead of double lighting)

Revision 1.87  2006/01/31 11:09:07  alemochi
remove unnecessary comments

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
	logVisible = true;
	helpVisible=false;
	takeSnapTile=false;
	activeDefaultTrackball=true;
	infoAreaVisible = false;
	trackBallVisible = true;
	currentSharder = NULL;
	lastFilterRef = NULL;
	mm = NULL;
	time.start();
	currLogLevel = -1;
	currentButton=GLArea::BUTTON_NONE;
	
	// Projection Matrix starting settings
	objDist = 3.f;
	fov = 60;
	clipRatioFar = 1;
	clipRatioNear = 1;
	nearPlane = .2f;
	farPlane = 5.f;
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

QSize GLArea::minimumSizeHint() const {return QSize(400,300);}
QSize GLArea::sizeHint() const				{return QSize(400,300);}


void GLArea::initializeGL()
{
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	static float diffuseColor[]={1.0,1.0,1.0,1.0};
	glEnable(GL_LIGHT0);
	glDisable(GL_LIGHT1);
	glLightfv(GL_LIGHT1,GL_DIFFUSE,diffuseColor);
	trackball.center=Point3f(0, 0, 0);
	trackball.radius= 1;

	trackball_light.center=Point3f(0, 0, 0);
	trackball_light.radius= 1;
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

	// Set Modelview and Projection matrix
	setView();

	// Enter in 2D screen Mode and
	// draws the background
	if(!takeSnapTile)
	{
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(-1,1,-1,1,-1,1);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		glPushAttrib(GL_ENABLE_BIT);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);

		glBegin(GL_TRIANGLE_STRIP);
			glColor(cs.bColorTop);  	glVertex2f(-1, 1);
			glColor(cs.bColorBottom);	glVertex2f(-1,-1);
			glColor(cs.bColorTop);		glVertex2f( 1, 1);
			glColor(cs.bColorBottom);	glVertex2f( 1,-1);
		glEnd();

		glPopAttrib();
		glPopMatrix(); // restore modelview
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	}

	// glVertex: commented out
	// First draw the trackball from a fixed point of view
	//glLoadIdentity();
	//gluLookAt(0,0,3, 0,0,0 ,0,1,0);
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//gluPerspective(60.f,(float)currentWidth/currentHeight,.1f,5.f);
	//glMatrixMode(GL_MODELVIEW);

	// Apply trackball for the model
	//trackball.GetView();
	//trackball.Apply(trackBallVisible && !takeSnapTile);

	// ============== LIGHT TRACKBALL ==============
	// Apply the trackball for the light direction
	glPushMatrix();
	glColor3f(1,1,0);
  glDisable(GL_LIGHTING);
	trackball_light.GetView();
	trackball_light.Apply(!(isDefaultTrackBall()));

	static float lightPosF[]={0.0,0.0,1.0,0.0};
	glLightfv(GL_LIGHT0,GL_POSITION,lightPosF);
	static float lightPosB[]={0.0,0.0,-1.0,0.0};
	glLightfv(GL_LIGHT1,GL_POSITION,lightPosB);

  if (!(isDefaultTrackBall()))
	{
		glBegin(GL_LINES);
			glVertex3f(0,0,0);
			glVertex3f(0,0,200);
		glEnd();
	}
	glPopMatrix();
	// =============================================
	

	// Finally apply the Trackball for the model
	// (get messy when in orthoMode)
	trackball.GetView();
	trackball.Apply(trackBallVisible && !takeSnapTile);

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
		myGluPerspective(fov, (GLdouble) currentWidth / (GLdouble) currentHeight, nearPlane, farPlane);
	}

	// Set proper colorMode
	glDisable(GL_COLOR_MATERIAL);
	if(rm.colorMode != GLW::CMNone)
	{
		glEnable(GL_COLOR_MATERIAL);
		glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
	}

	if(iRenderer && currentSharder) iRenderer->Render(currentSharder, *mm, rm, this); 

	mm->Render(rm.drawMode,rm.colorMode,rm.textureMode);
	
	if(iRenderer) {
		glUseProgramObjectARB(0);
	}

	if(iDecoratorsList){
		pair<QAction *,MeshDecorateInterface *> p;
		foreach(p,*iDecoratorsList){p.second->Decorate(p.first,*mm,rm,this);}
	}

	// ...and take a snapshot
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

	// Enter in 2D screen Mode again
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(-1,1,-1,1,-1,1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);

	// Draw the log area background
	// on the bottom of the glArea
	if(infoAreaVisible)
	{
		glBlendFunc(GL_ONE,GL_SRC_ALPHA);
		cs.lColor.V(3) = 128;	// set half alpha value
		glColor(cs.lColor);
		float h = -0.80f;//vcg::math::Min(-0.8f,-.08f*qFont.pointSize());//((.03f * currentHeight) - (currentHeight>>1)) / (float)currentHeight;
		glBegin(GL_TRIANGLE_STRIP);
			glVertex2f(-1.f,h);
			glVertex2f(-1.f,-1.f);
			glVertex2f( 1.f,h);
			glVertex2f( 1.f,-1.f);
		glEnd();
		
		// Now print out the infos
		//========================

		// First the LOG
		glColor4f(1,1,1,1);
		if(logVisible)
		{
			renderText(20,currentHeight - 5 * (qFont.pointSizeF()+(currentHeight/225.f)),tr("LOG MESSAGES"),qFont);
			log.glDraw(this,currLogLevel,3,qFont.pointSizeF()+(currentHeight/225.f),qFont);
		}

		// Second the MESH INFO (numVert,NumFaces,....)
		displayMeshInfo();
		
		// Third the ENV INFO (Fps,ClippingPlanes,....)
		currentTime=time.elapsed();
		deltaTime=currentTime-lastTime;
		updateFps();

		displayEnvInfo();
	}
	
	// Finally display HELP if requested
	if (isHelpVisible()) displayHelp();

	// Closing 2D
	glPopAttrib();
	glPopMatrix(); // restore modelview
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void GLArea::displayMeshInfo()
{	
	float fontSpacingV = qFont.pointSizeF()+(currentHeight/225.f);
	float startPos= currentHeight-(fontSpacingV/3);
	
	renderText(currentWidth*.5f,startPos-5*fontSpacingV,tr("MESH INFO"),qFont);

	renderText(currentWidth*.5f,startPos-3*fontSpacingV,tr("Vertices: %1").arg(mm->cm.vert.size()),qFont);
	renderText(currentWidth*.5f,startPos-2*fontSpacingV,tr("Faces: %1").arg(mm->cm.face.size()),qFont);
	renderText(currentWidth*.5f,startPos-  fontSpacingV,GetMeshInfoString(mm->mask),qFont);
}

void GLArea::displayEnvInfo()
{	
	float fontSpacingV = qFont.pointSizeF()+(currentHeight/225.f);
	float startPos = currentHeight-(fontSpacingV/3);

	QString strNear=QString("Nplane: %1  ").arg(nearPlane,2,'f',1);
	QString strFar=QString("Fplane: %1").arg(farPlane,2,'f',1);
	QString strViewer=QString("Viewer: %1  ").arg(objDist,2,'f',1);

	renderText(currentWidth-currentWidth*.25f,startPos-5*fontSpacingV,tr("ENV INFO"),qFont);
	
	renderText(currentWidth-currentWidth*.25f,startPos-3*fontSpacingV,strViewer+strNear+strFar,qFont);
	renderText(currentWidth-currentWidth*.25f,startPos-2*fontSpacingV,QString("FOV: ")+QString::number((int)fov,10),qFont);


	if ((cfps>0) && (cfps<200)) 
	{
		QString strInfo=QString("FPS: %1").arg(cfps,7,'f',1);
		renderText(currentWidth-currentWidth*.25f,startPos-fontSpacingV,strInfo,qFont);	
	}
}


void GLArea::resizeGL(int _width, int _height)
{
	// glVertex: No longer needed. Every frame we set a new projection matrix
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//gluPerspective(fov, float(_width)/float(_height), nearPlane, farPlane);
	//glMatrixMode(GL_MODELVIEW);
	glViewport(0,0, _width, _height);
	currentWidth=_width;
	currentHeight=_height;

		// Set font size depending on window size (min = 1, max = 9)
	qFont.setPointSizeF(vcg::math::Clamp<float>(-3 + sqrtf(_width*_width + _height*_height) * .01f,1,9));
}


void GLArea::displayHelp()
{
	glPushAttrib(GL_ENABLE_BIT);
	glPushMatrix();
	glLoadIdentity();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glColor4f(.3f,.7f,.8f,.5f);
	glBlendFunc(GL_ONE,GL_SRC_ALPHA);
  float fontSpacingV = (currentHeight*.01f)+3;
	renderText(15,1.5*fontSpacingV,QString("HELP ON SCREEN"),qFont);
  renderText(2,3*fontSpacingV,QString("Drag: "),qFont);
	renderText(100,3*fontSpacingV,QString("Rotate"),qFont);
	renderText(2,4.5*fontSpacingV,QString("Ctrl-Drag: "),qFont);
	renderText(100,4.5*fontSpacingV,QString("Pan"),qFont);
	renderText(2,6*fontSpacingV,QString("Wheel: "),qFont);
	renderText(100,6*fontSpacingV,QString("Zoom"),qFont);
	renderText(2,7.5*fontSpacingV,QString("Shift-Drag: "),qFont);
  renderText(100,7.5*fontSpacingV,QString("Zoom"),qFont);
	renderText(2,9*fontSpacingV,QString("Shift-Wheel: "),qFont);
	renderText(100,9*fontSpacingV,QString("Vertigo Effect"),qFont);
	renderText(2,10.5*fontSpacingV,QString("Ctrl-Shift-Wheel: "),qFont);
	renderText(100,10.5*fontSpacingV,QString("Far"),qFont);
	renderText(2,12*fontSpacingV,QString("Ctrl-Wheel: "),qFont);
	renderText(100,12*fontSpacingV,QString("Near"),qFont);
	renderText(2,13.5*fontSpacingV,QString("Ctrl-Shift-Drag: "),qFont);
	renderText(100,13.5*fontSpacingV,QString("Move light"),qFont);
	glPopMatrix();
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


void GLArea::closeEvent(QCloseEvent *event)
{
	bool close = true;
	if(isWindowModified())
	{
		if(QMessageBox::question(
                this,
                tr("MeshLab"),
                tr("File %1 modified.\n\n"
                   "Continue without saving?")
                .arg(fileName),
								QMessageBox::Yes|QMessageBox::Default,
								QMessageBox::No|QMessageBox::Escape,
								QMessageBox::NoButton) == QMessageBox::No)
		{
			close = false;	// don't close please!
		}
	}

	event->ignore();
	if(close)
	{
		if(mm){	delete mm;mm = NULL;}	// quit without saving
		event->accept();
	}
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
	}
	else if (currentButton & KEY_CTRL) 
	{
		if (notch<0) clipRatioNear*=1.2f;
		else clipRatioNear/=1.2f; 
	}
	else {trackball.MouseWheel( e->delta()/ float(WHEEL_DELTA)); update(); }
	
	updateGL();
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
  static GLfloat standard_light[]={1.f,1.f,1.f,1.f};
  static GLfloat l_diffuseFancyBack[]={.81f,.61f,.61f,1.f};
  static GLfloat l_diffuseFancyFront[]={.71f,.71f,.95f,1.f};
	
	glDisable(GL_LIGHTING);
	if (rm.lighting) 
	{
		glEnable(GL_LIGHTING);
		if (rm.doubleSideLighting) glEnable(GL_LIGHT1);
		else glDisable(GL_LIGHT1);
		if(rm.fancyLighting)
		{
			glLightfv(GL_LIGHT0,GL_DIFFUSE,l_diffuseFancyFront);
			glLightfv(GL_LIGHT1,GL_DIFFUSE,l_diffuseFancyBack);
		}
		else
		{
			glLightfv(GL_LIGHT0,GL_DIFFUSE,standard_light);
			glLightfv(GL_LIGHT1,GL_DIFFUSE,standard_light);
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

void GLArea::setView()
{
	GLfloat fAspect = (GLfloat)currentWidth/ currentHeight;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// Si deve mettere la camera ad una distanza che inquadri la sfera unitaria bene.
	
	float ratio = 1.75f;
	objDist = ratio / tanf(vcg::math::ToRad(fov*.5f));

	nearPlane = objDist - 2.f*clipRatioNear;
	farPlane =  objDist + 2.f*clipRatioFar;
	if(nearPlane<=objDist*.1f) nearPlane=objDist*.1f;

	if(fov==5)		glOrtho(-ratio*fAspect,ratio*fAspect,-ratio,ratio,objDist - 2.f*clipRatioNear, objDist+2.f*clipRatioFar);
	   else    		gluPerspective(fov, fAspect, nearPlane, farPlane);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, objDist,0, 0, 0, 0, 1, 0);
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

