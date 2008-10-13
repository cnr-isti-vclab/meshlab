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

$Log: glarea.cpp,v $
Revision 1.143  2008/04/22 16:15:45  bernabei
By default, tablet events are treated as mouse events

Revision 1.142  2008/04/22 14:54:38  bernabei
Added support for tablet events

Revision 1.141  2008/04/04 10:07:12  cignoni
Solved namespace ambiguities caused by the removal of a silly 'using namespace' in meshmodel.h

Revision 1.140  2008/02/05 18:06:47  benedetti
added calls to editing plugins' keyReleaseEvent and keyPressEvent

Revision 1.139  2008/01/10 17:15:16  cignoni
unsaved dialog has a better behaviour

Revision 1.138  2008/01/04 18:23:24  cignoni
Corrected a wrong type (glwidget instead of glarea) in the decoration callback.

Revision 1.137  2008/01/04 00:46:28  cignoni
Changed the decoration framework. Now it accept a, global, parameter set. Added static calls for finding important directories in a OS independent way.

Revision 1.136  2007/12/13 00:18:28  cignoni
added meshCreation class of filter, and the corresponding menu new under file

Revision 1.135  2007/12/10 10:26:18  corsini
remove number of passes dependency in the main rendering cycle

Revision 1.134  2007/11/20 12:04:09  cignoni
shortening and refactoring

Revision 1.133  2007/11/19 15:21:46  ponchio
Temporary fix for QT glError bug causin an assert(glError()) to trigger.

Revision 1.132  2007/11/17 15:40:23  cignoni
removed QT2VCG trackball helper

Revision 1.131  2007/11/09 11:23:46  cignoni
attempts to manage 4.3 issues with fonts and opengl

Revision 1.130  2007/11/05 22:38:55  cignoni
Remove static map of cursors that caused the annoying deallocation bug under vs2005

Revision 1.129  2007/10/24 16:38:08  fuscof
paintGL draw the model only during the first rendering pass

Revision 1.128  2007/10/09 13:02:09  fuscof
Initial implementation of multipass rendering.
Please note that MeshRenderInterface has been modified to get the number of rendering passes.

Revision 1.127  2007/10/08 19:48:45  cignoni
corrected behavior for endedit

Revision 1.126  2007/09/15 09:06:20  cignoni
Added notification of ortho projection


****************************************************************************/

#include <GL/glew.h>
#include <QtGui>

// widget capace di fare rendering di un Modello
#include <vcg/space/box3.h>
#include <wrap/gl/space.h>
#include "meshmodel.h"
#include "interfaces.h"
#include "glarea.h"
#include "layerDialog.h"
#include "mainwindow.h"

#include <wrap/gl/picking.h>
#include <wrap/qt/trackball.h>

using namespace std; 
using namespace vcg; 

GLArea::GLArea(QWidget *parent)
: QGLWidget(parent)
{
	animMode=AnimNone;
	iRenderer=0; //Shader support
	iDecoratorsList=0;
	iEdit=0;
	currentEditor=0;
	suspendedEditor=false;
	cfps=0;
	lastTime=0;
	hasToPick=false;
	hasToUpdateTexture=false;
	helpVisible=false;
	takeSnapTile=false;
	activeDefaultTrackball=true;
	infoAreaVisible = true;
	trackBallVisible = true;
	currentShader = NULL;
	lastFilterRef = NULL;
	//lastEditRef = NULL;
	currLogLevel = -1;
	setAttribute(Qt::WA_DeleteOnClose,true);
	// Projection Matrix starting settings
	//objDist = 3.f;
	fov = 60;
	clipRatioFar = 1;
	clipRatioNear = 1;
	nearPlane = .2f;
	farPlane = 5.f;
	pointSize = 2.0f;
	layerDialog = new LayerDialog(this);
	
	connect((const MeshDocument*)&meshDoc, SIGNAL(currentMeshChanged(int)), this, SLOT(setCurrentlyActiveLayer(int)));
	
	/*getting the meshlab MainWindow from parent, which is QWorkspace.
	*note as soon as the GLArea is added as Window to the QWorkspace the parent of GLArea is a QWidget,
	*which takes care about the window frame (its parent is the QWorkspace again).
	*/
	MainWindow* mainwindow = dynamic_cast<MainWindow*>(parent->parentWidget());
	//connecting the MainWindow Slots to GLArea signal (simple passthrough)
	if(mainwindow != NULL){
		connect(this,SIGNAL(updateMainWindowMenus()),mainwindow,SLOT(updateMenus()));
	}else{
		qDebug("The parent of the GLArea parent is not a pointer to the meshlab MainWindow.");
	}


}

GLArea::~GLArea()
{
	// warn any iRender plugin that we're deleting glarea
	if (iRenderer)
		iRenderer->Finalize(currentShader, *mm(), this);
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
  glPixelStorei(GL_PACK_ROW_LENGTH, 0);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
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
  GLenum err = glewInit();
	if (err != GLEW_OK ) {
    assert(0);
  }
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
			QString outfile=QString("%1/%2%3.png")
        .arg(ss.outdir)
        .arg(ss.basename)
        .arg(ss.counter++,2,10,QChar('0'));
			bool ret = snapBuffer.save(outfile,"PNG");		
			if (ret) log.Logf(GLLogStream::Info,"Snapshot saved to %s",outfile.toLocal8Bit().constData());
					else log.Logf(GLLogStream::Error,"Error saving %s",outfile.toLocal8Bit().constData());
			
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
	xDim = fabs(fLeft * 2);
	yDim = fabs(fTop * 2);
	
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
  QTime time;
  time.start();
	//int lastTime=time.elapsed();
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


	// ============== LIGHT TRACKBALL ==============
	// Apply the trackball for the light direction
	glPushMatrix();
	trackball_light.GetView();
	trackball_light.Apply(!(isDefaultTrackBall()));

	static float lightPosF[]={0.0,0.0,1.0,0.0};
	glLightfv(GL_LIGHT0,GL_POSITION,lightPosF);
	static float lightPosB[]={0.0,0.0,-1.0,0.0};
	glLightfv(GL_LIGHT1,GL_POSITION,lightPosB);

  if (!(isDefaultTrackBall()))
	{
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
		glColor3f(1,1,0);
    glDisable(GL_LIGHTING);
	  const unsigned int lineNum=3;
		glBegin(GL_LINES);
    for(unsigned int i=0;i<=lineNum;++i)
      for(unsigned int j=0;j<=lineNum;++j) {
        glVertex3f(-1.0f+i*2.0/lineNum,-1.0f+j*2.0/lineNum,-2);
        glVertex3f(-1.0f+i*2.0/lineNum,-1.0f+j*2.0/lineNum, 2);
      }
		glEnd();
    glPopAttrib();
	}
	glPopMatrix();
  // =============================================
	/// Compute BBox 
	Box3f FullBBox=meshDoc.bbox();
	foreach(MeshModel * mp, meshDoc.meshList) 
	 FullBBox.Add(mp->cm.Tr,mp->cm.bbox);
		
	// Finally apply the Trackball for the model
	trackball.GetView();
  glPushMatrix(); 
	trackball.Apply(trackBallVisible && !takeSnapTile && !(iEdit && !suspendedEditor));
	float d=2.0f/FullBBox.Diag();
	glScale(d);
	
	glTranslate(-FullBBox.Center());
  setLightModel();

	// Modify frustum... 
	if (takeSnapTile)
	{
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		myGluPerspective(fov, (GLdouble) curSiz.width() / (GLdouble) curSiz.height(), nearPlane, farPlane);
		glMatrixMode(GL_MODELVIEW);
	}

	// Set proper colorMode
	if(rm.colorMode != GLW::CMNone)
	{
		glEnable(GL_COLOR_MATERIAL);
		glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
	}
	else glColor(Color4b::LightGray);
	
	if(rm.backFaceCull) 
		glEnable(GL_CULL_FACE);
	else 
		glDisable(GL_CULL_FACE);

	if(!meshDoc.busy)
	{
		int totPasses = 1;
		if (iRenderer) {
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			totPasses = iRenderer->passNum();
		}

		do {
			// render the current meshes
			if (iRenderer && currentShader) {
				iRenderer->Render(currentShader, *mm(), rm, this);
			}

			// handle the other meshes
			foreach(MeshModel * mp, meshDoc.meshList) 
			{
				if(mp->visible)
					mp->Render(rm.drawMode,rm.colorMode,rm.textureMode);
			}

		} while (--totPasses);
	
			if(iRenderer) 
			{
				glPopAttrib();
				glUseProgramObjectARB(0);
			}

			if(iEdit)
				iEdit->Decorate(*mm(),this);
	
			// Draw the selection
			if(rm.selectedFaces)  mm()->RenderSelectedFaces();
	
			if(iDecoratorsList)
			{
				pair<QAction *,FilterParameterSet *> p;
				//assert(decorInterface);
				foreach(p,*iDecoratorsList)
				{
					MeshDecorateInterface * decorInterface = qobject_cast<MeshDecorateInterface *>(p.first->parent());
					assert(decorInterface);
					decorInterface->Decorate(p.first,*mm(),p.second,this,qFont);
				}
			}
	} ///end if busy 
	
	// ...and take a snapshot
	if (takeSnapTile)
	{
		glPushAttrib(GL_ENABLE_BIT);
		tileBuffer=grabFrameBuffer(true);
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
		pasteTile();
		update();
		glPopAttrib();
	}
 glPopMatrix(); // now we are back in pre-trackball space
  if(hasToPick) 
  { // Double click move picked point to center
    Point3f pp;
    hasToPick=false;
    if(Pick<Point3f>(pointToPick[0],pointToPick[1],pp)) {
          trackball.Translate(-pp);
          trackball.Scale(1.25f);
          QCursor::setPos(mapToGlobal(QPoint(width()/2+2,height()/2+2)));
        }  
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
		displayInfo();    
		updateFps(time.elapsed());
	}
	
	// Finally display HELP if requested
	if (isHelpVisible()) displayHelp();

	// Closing 2D
	glPopAttrib();
	glPopMatrix(); // restore modelview
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
  int error = glGetError();
	if(error) {
		log.Logf(GLLogStream::Info,"There are gl errors");
	}
}

void GLArea::displayInfo()
{	
	qFont.setStyleStrategy(QFont::NoAntialias);
	qFont.setFamily("Helvetica");
	qFont.setPixelSize(12);
//	qFont.setWeight(1);
	glBlendFunc(GL_ONE,GL_SRC_ALPHA);
	cs.lColor.V(3) = 128;	// set half alpha value
	glColor(cs.lColor);
	int lineNum =4;
	float lineSpacing = qFont.pixelSize()*1.5f;
	float barHeight = -1 + 2.0*(lineSpacing*(lineNum+.25))/float(curSiz.height());

	glBegin(GL_QUADS);
		glVertex2f(-1.f,barHeight); glVertex2f( 1.f,barHeight);
		glVertex2f( 1.f,-1.f);      glVertex2f(-1.f,-1.f);
	glEnd();
		
	// First the LOG
	glColor4f(1,1,1,1);

  float middleCol=curSiz.width()*0.40;
  float rightCol=curSiz.width()*0.85;
 	float startPos = curSiz.height()-(5+lineSpacing*(lineNum));

	renderText(20,startPos+ 1*lineSpacing,tr("LOG MESSAGES"),qFont);
	log.glDraw(this,currLogLevel,3,lineSpacing,qFont);

	if(meshDoc.size()==1) 
	{ 
		renderText(middleCol,startPos+ 1*lineSpacing,tr("Vertices: %1").arg(mm()->cm.vn),qFont);
		renderText(middleCol,startPos+ 2*lineSpacing,tr("Faces: %1").arg(mm()->cm.fn),qFont);
	}
	else
	{
		renderText(middleCol,startPos+ 1*lineSpacing,tr("<%1>").arg(QFileInfo(mm()->fileName.c_str()).fileName()),qFont);
		renderText(middleCol,startPos+ 2*lineSpacing,tr("Vertices: %1 (%2)").arg(mm()->cm.vn).arg(meshDoc.vn()),qFont);
		renderText(middleCol,startPos+ 3*lineSpacing,tr("Faces: %1 (%2)").arg(mm()->cm.fn).arg(meshDoc.fn()),qFont);
	}
	if(rm.selectedFaces)  
		 renderText(middleCol,startPos+ 4*lineSpacing,tr("Selected: %1").arg(mm()->cm.sfn),qFont);
	renderText(rightCol,startPos+ 4*lineSpacing,GetMeshInfoString(mm()->ioMask),qFont);

  if(fov>5) renderText(rightCol,startPos+1*lineSpacing,QString("FOV: ")+QString::number((int)fov,10),qFont);
			 else renderText(rightCol,startPos+1*lineSpacing,QString("FOV: Ortho"),qFont);
	if ((cfps>0) && (cfps<500)) 
			renderText(rightCol,startPos+2*lineSpacing,QString("FPS: %1").arg(cfps,7,'f',1),qFont);
	if ((clipRatioNear!=1) || (clipRatioFar!=1)) 
			renderText(rightCol,startPos+3*lineSpacing,QString("Clipping: N:%1 F:%2").arg(clipRatioNear,7,'f',1).arg(clipRatioFar,7,'f',1),qFont);

}


void GLArea::resizeGL(int _width, int _height)
{
	glViewport(0,0, _width, _height);
	curSiz.setWidth(_width);
	curSiz.setHeight(_height);
}


void GLArea::displayHelp()
{
	glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT );
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glBegin(GL_TRIANGLE_STRIP);
		glColor4f(.5f,.8f,1.f,.6f); glVertex2f(-1, 1);
		glColor4f(.0f,.0f,.0f,.0f); glVertex2f(-1,-1);
		glColor4f(.5f,.8f,1.f,.6f); glVertex2f(-.5f,1);
		glColor4f(.0f,.0f,.0f,.0f); glVertex2f(-.5f,-1);
	glEnd();


  float fontSpacingV = (curSiz.height()*.01f)+3;
	float hPosition = curSiz.width()*.1f;
	glColor(Color4b::White);
	//qFont.setBold(true);
	renderText(2+hPosition-(qFont.pointSize()*9),1.5*fontSpacingV,QString("MeshLab Quick Help"),qFont);qFont.setBold(false);
  renderText(2,3*fontSpacingV,QString("Drag:"),qFont);								renderText(hPosition,3*fontSpacingV,QString("Rotate"),qFont);
	renderText(2,4.5*fontSpacingV,QString("Ctrl-Drag:"),qFont);					renderText(hPosition,4.5*fontSpacingV,QString("Pan"),qFont);
	renderText(2,6*fontSpacingV,QString("Shift-Drag:"),qFont);					renderText(hPosition,6*fontSpacingV,QString("Zoom"),qFont);
	renderText(2,7.5*fontSpacingV,QString("Alt-Drag:"),qFont);					renderText(hPosition,7.5*fontSpacingV,QString("Z-Panning"),qFont);
	renderText(2,9*fontSpacingV,QString("Ctrl-Shift-Drag:"),qFont);			renderText(hPosition,9*fontSpacingV,QString("Rotate light"),qFont);
	renderText(2,10.5*fontSpacingV,QString("Wheel:"),qFont);						renderText(hPosition,10.5*fontSpacingV,QString("Zoom"),qFont);
	renderText(2,12*fontSpacingV,QString("Shift-Wheel:"),qFont);				renderText(hPosition,12*fontSpacingV,QString("Change perspective"),qFont);
	renderText(2,13.5*fontSpacingV,QString("Ctrl-Wheel:"),qFont);				renderText(hPosition,13.5*fontSpacingV,QString("Move far clipping plane"),qFont);
	renderText(2,15*fontSpacingV,QString("Ctrl-Shift-Wheel:"),qFont);		renderText(hPosition,15*fontSpacingV,QString("Move near clipping plane"),qFont);
	renderText(2,16.5*fontSpacingV,QString("Double Click:"),qFont);			renderText(hPosition,16.5*fontSpacingV,QString("Center on mouse"),qFont);
	renderText(2,18*fontSpacingV,QString("F1:"),qFont);									renderText(hPosition,18*fontSpacingV,QString("Toggle this help"),qFont);
	renderText(2,19.5*fontSpacingV,QString("Alt+enter:"),qFont);						renderText(hPosition,19.5*fontSpacingV,QString("Enter/Exit fullscreen mode"),qFont);
	glPopAttrib();
}

void GLArea::saveSnapshot()
{ 
	totalCols=totalRows=ss.resolution;
	tileRow=tileCol=0;

	takeSnapTile=true;
	update();
}

void GLArea::setCurrentlyActiveLayer(int meshId)
{
	qDebug() << "setCurrent: " << meshId;
	
	//get the mesh that was current before this change
	MeshModel *outgoingMeshModel = meshDoc.mm();
		
	//if we have an edit tool open, notify it that the current layer has changed
	if(iEdit) iEdit->LayerChanged(meshDoc, *outgoingMeshModel, this);	
}

void GLArea::setCurrentEditAction(QAction *editAction)
{
	assert(editAction);
	currentEditor = editAction;
	
	iEdit = actionToMeshEditMap.value(currentEditor); 
	assert(iEdit); 
	iEdit->StartEdit(meshDoc, this);

	log.Logf(GLLogStream::Info,"Started Mode %s", qPrintable(currentEditor->text()));
}


void GLArea::closeEvent(QCloseEvent *event)
{
	bool close = true;
	if(isWindowModified())
	{
		 QMessageBox::StandardButton ret=QMessageBox::question(
                this,  tr("MeshLab"), tr("File '%1' modified.\n\nClose without saving?").arg(getFileName()),
								QMessageBox::Yes|QMessageBox::No,
								QMessageBox::No);
		if(ret==QMessageBox::No)
		{
			close = false;	// don't close please!
			event->ignore();
			return;
		}
	}
  if(getCurrentEditAction()) endEdit();	
	event->accept();
}

void GLArea::keyReleaseEvent ( QKeyEvent * e )
{
	e->ignore();
	if(iEdit && !suspendedEditor)  iEdit->keyReleaseEvent(e,*mm(),this);
	else{
      if(e->key()==Qt::Key_Control) trackball.ButtonUp(QT2VCG(Qt::NoButton, Qt::ControlModifier ) );
      if(e->key()==Qt::Key_Shift) trackball.ButtonUp(QT2VCG(Qt::NoButton, Qt::ShiftModifier ) );
      if(e->key()==Qt::Key_Alt) trackball.ButtonUp(QT2VCG(Qt::NoButton, Qt::AltModifier ) );
  }
}	

void GLArea::keyPressEvent ( QKeyEvent * e )
{
	e->ignore();
	if(iEdit && !suspendedEditor)  iEdit->keyPressEvent(e,*mm(),this);
	else{
      if(e->key()==Qt::Key_Control) trackball.ButtonDown(QT2VCG(Qt::NoButton, Qt::ControlModifier ) );
      if(e->key()==Qt::Key_Shift) trackball.ButtonDown(QT2VCG(Qt::NoButton, Qt::ShiftModifier ) );
      if(e->key()==Qt::Key_Alt) trackball.ButtonDown(QT2VCG(Qt::NoButton, Qt::AltModifier ) );
  }
}

void GLArea::mousePressEvent(QMouseEvent*e)
{
  e->accept();
	setFocus();
	
  if( (iEdit && !suspendedEditor) && !(e->buttons() & Qt::MidButton) )
		  iEdit->mousePressEvent(e,*mm(),this);
  else {          
	    if ((e->modifiers() & Qt::ShiftModifier) && (e->modifiers() & Qt::ControlModifier) && 
          (e->button()==Qt::LeftButton) )
            activeDefaultTrackball=false;
	      else activeDefaultTrackball=true;
      
	    if (isDefaultTrackBall())
          trackball.MouseDown(e->x(),height()-e->y(), QT2VCG(e->button(), e->modifiers() ) );
	    else trackball_light.MouseDown(e->x(),height()-e->y(), QT2VCG(e->button(), Qt::NoModifier ) );
  }
	update();
}

void GLArea::mouseMoveEvent(QMouseEvent*e)
{ 
      if( (iEdit && !suspendedEditor) && !(e->buttons() & Qt::MidButton) )
    		  	iEdit->mouseMoveEvent(e,*mm(),this);
      else {
		    if (isDefaultTrackBall()) 
			{
			  trackball.MouseMove(e->x(),height()-e->y());
			  setCursorTrack(trackball.current_mode);
			}
		    else trackball_light.MouseMove(e->x(),height()-e->y());
        update();
      }
}
// When mouse is released we set the correct mouse cursor
void GLArea::mouseReleaseEvent(QMouseEvent*e)
{
  //clearFocus();
	activeDefaultTrackball=true;
	if( (iEdit && !suspendedEditor) && (e->button() != Qt::MidButton) )
			iEdit->mouseReleaseEvent(e,*mm(),this);
    else {
          if (isDefaultTrackBall()) trackball.MouseUp(e->x(),height()-e->y(), QT2VCG(e->button(), e->modifiers() ) );
	        else trackball_light.MouseUp(e->x(),height()-e->y(), QT2VCG(e->button(),e->modifiers()) );
		      setCursorTrack(trackball.current_mode);
        }
	
	update();
}

//Processing of tablet events, interesting only for painting plugins
void GLArea::tabletEvent(QTabletEvent*e)
{
	if(iEdit && !suspendedEditor) iEdit->tabletEvent(e,*mm(),this);
	else e->ignore();
}

void GLArea::wheelEvent(QWheelEvent*e)
{
	const int WHEEL_STEP = 120;
	float notch = e->delta()/ float(WHEEL_STEP);
  switch(e->modifiers())
  {
    case Qt::ShiftModifier + Qt::ControlModifier	: clipRatioFar  *= powf(1.2f, notch); break;
    case Qt::ControlModifier											: clipRatioNear *= powf(1.2f, notch); break;
    case Qt::AltModifier													: pointSize = math::Clamp(pointSize*powf(1.2f, notch),0.01f,150.0f);
			foreach(MeshModel * mp, meshDoc.meshList) 
				mp->glw.SetHintParamf(GLW::HNPPointSize,pointSize);
			break;
    case Qt::ShiftModifier												: fov = math::Clamp(fov*powf(1.2f,notch),5.0f,90.0f); break;
    default:
      trackball.MouseWheel( e->delta()/ float(WHEEL_STEP)); 
      break;
	}
	updateGL();
}		


void GLArea::mouseDoubleClickEvent ( QMouseEvent * e ) 
{
  hasToPick=true;
  pointToPick=Point2i(e->x(),height()-e->y());
	updateGL();
}

void GLArea::setCursorTrack(vcg::TrackMode *tm)
{
 //static QMap<QString,QCursor> curMap;
 if(curMap.isEmpty())
 {
  curMap[QString("")]=QCursor(Qt::ArrowCursor);	
  curMap["SphereMode"]=QCursor(QPixmap(":/images/cursors/plain_trackball.png"),1,1);	
  curMap["PanMode"]=QCursor(QPixmap(":/images/cursors/plain_pan.png"),1,1);	
  curMap["ScaleMode"]=QCursor(QPixmap(":/images/cursors/plain_zoom.png"),1,1);	
 }
 if(tm) setCursor(curMap[tm->Name()]);
 else setCursor(curMap[""]);

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
void GLArea::updateTexture()
{
	hasToUpdateTexture = true;
}

// Texture loading done during the first paint.
void GLArea::initTexture()
{  
	if(hasToUpdateTexture)
	{
		mm()->glw.TMId.clear();
		qDebug("Beware: deleting the texutres could lead to problems for shared textures.");
		hasToUpdateTexture = false;	
	}

	if(!mm()->cm.textures.empty() && mm()->glw.TMId.empty()){
		glEnable(GL_TEXTURE_2D);
		for(unsigned int i =0; i< mm()->cm.textures.size();++i){
			QImage img, imgScaled, imgGL;
			img.load(mm()->cm.textures[i].c_str());
			// image has to be scaled to a 2^n size. We choose the first 2^N <= picture size.
			int bestW=pow(2.0,floor(::log(double(img.width() ))/::log(2.0)));
			int bestH=pow(2.0,floor(::log(double(img.height()))/::log(2.0)));
			
			qDebug("texture[ %i ] =  %s ( %i x %i )",	i,mm()->cm.textures[i].c_str(), imgGL.width(), imgGL.height());
			imgScaled=img.scaled(bestW,bestH,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
			imgGL=convertToGLFormat(imgScaled);
			mm()->glw.TMId.push_back(0);
			glGenTextures( 1, (GLuint*)&(mm()->glw.TMId.back()) );
			glBindTexture( GL_TEXTURE_2D, mm()->glw.TMId.back() );
			glTexImage2D( GL_TEXTURE_2D, 0, 3, imgGL.width(), imgGL.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, imgGL.bits() );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
			gluBuild2DMipmaps(GL_TEXTURE_2D, 3, imgGL.width(), imgGL.height(), GL_RGBA, GL_UNSIGNED_BYTE, imgGL.bits() );
			
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ); 
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			qDebug("      	will be loaded as GL texture id %i  ( %i x %i )",mm()->glw.TMId.back() ,imgGL.width(), imgGL.height());
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
	  case LDOUBLE:		rm.doubleSideLighting = state;	break;
	  case LFANCY:		rm.fancyLighting = state; break;
	}
	updateGL();
}

void GLArea::setBackFaceCulling(bool enabled)
{
	rm.backFaceCull = enabled;
	updateGL();
}

void GLArea::setSelectionRendering(bool enabled)
{
	rm.selectedFaces = enabled;
	updateGL();
}

void GLArea::setLightModel()
{	
	glDisable(GL_LIGHTING);
	if (rm.lighting) 
	{
		glEnable(GL_LIGHTING);

		if (rm.doubleSideLighting) 
			glEnable(GL_LIGHT1);
		else 
			glDisable(GL_LIGHT1);

		if(rm.fancyLighting)
		{
			glLightfv(GL_LIGHT0, GL_AMBIENT, ls.ambientFancyFront);
			glLightfv(GL_LIGHT0, GL_DIFFUSE, ls.diffuseFancyFront);
			glLightfv(GL_LIGHT0, GL_SPECULAR, ls.specularFancyFront);

			glLightfv(GL_LIGHT1, GL_AMBIENT, ls.ambientFancyBack);
			glLightfv(GL_LIGHT1, GL_DIFFUSE, ls.diffuseFancyBack);
			glLightfv(GL_LIGHT1, GL_SPECULAR, ls.specularFancyBack);
		}
		else
		{
			glLightfv(GL_LIGHT0, GL_AMBIENT, ls.ambient);
			glLightfv(GL_LIGHT0, GL_DIFFUSE, ls.diffuse);
			glLightfv(GL_LIGHT0, GL_SPECULAR, ls.specular);

			glLightfv(GL_LIGHT1, GL_AMBIENT, ls.ambient);
			glLightfv(GL_LIGHT1, GL_DIFFUSE, ls.diffuse);
			glLightfv(GL_LIGHT1, GL_SPECULAR, ls.specular);
		}
	}
}


void GLArea::setCustomSetting(const ColorSetting & s)
{
	cs.bColorBottom = s.bColorBottom;
	cs.bColorTop = s.bColorTop;
	cs.lColor = s.lColor;
}

void GLArea::setSnapshotSetting(const SnapshotSetting & s)
{
	ss=s;
}

void GLArea::setView()
{
	GLfloat fAspect = (GLfloat)curSiz.width()/ curSiz.height();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// Si deve mettere la camera ad una distanza che inquadri la sfera unitaria bene.
	
	ratio = 1.75f;
	float objDist = ratio / tanf(vcg::math::ToRad(fov*.5f));

	nearPlane = objDist - 2.f*clipRatioNear;
	farPlane =  objDist + 10.f*clipRatioFar;
	if(nearPlane<=objDist*.1f) nearPlane=objDist*.1f;

	if(fov==5)		glOrtho(-ratio*fAspect,ratio*fAspect,-ratio,ratio,objDist - 2.f*clipRatioNear, objDist+2.f*clipRatioFar);
	   else    		gluPerspective(fov, fAspect, nearPlane, farPlane);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, objDist,0, 0, 0, 0, 1, 0);
}

void GLArea::updateFps(float deltaTime)
{
 	static float fpsVector[10];
	static int j=0;
	float averageFps=0;
  if (deltaTime>0) {
    fpsVector[j]=deltaTime;
	  j=(j+1) % 10;
  }
	for (int i=0;i<10;i++) averageFps+=fpsVector[i];
	cfps=1000.0f/(averageFps/10);
  lastTime=deltaTime;
}

void GLArea::resetTrackBall(){trackball.Reset();updateGL();}

void GLArea::hideEvent(QHideEvent * /*event*/)
{
	trackball.current_button=0;
}

