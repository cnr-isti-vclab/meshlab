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

#include "../common/interfaces.h"
#include "glarea.h"
#include "mainwindow.h"

#include <wrap/gl/picking.h>
#include <wrap/qt/trackball.h>
#include <wrap/qt/col_qt_convert.h>
#include <wrap/qt/shot_qt.h>
#include <wrap/qt/checkGLError.h>
#include <wrap/qt/gl_label.h>
#include <wrap/io_trimesh/export_ply.h>
#include <wrap/io_trimesh/import_ply.h>

using namespace std;
using namespace vcg;

GLArea::GLArea(MultiViewer_Container *mvcont, RichParameterSet *current)
: QGLWidget(),interrbutshow(false)
{
	this->setParent(mvcont);

	this->updateCustomSettingValues(*current);
	log=mvcont->LogPtr();
	animMode=AnimNone;
	iRenderer=0; //Shader support
	iEdit=0;
	currentEditor=0;
	suspendedEditor=false;
	lastModelEdited = 0;
	cfps=0;
	lastTime=0;
	hasToPick=false;
	hasToGetPickPos=false;
	hasToUpdateTexture=false;
	helpVisible=false;
	takeSnapTile=false;
	activeDefaultTrackball=true;
	infoAreaVisible = true;
	trackBallVisible = true;
	currentShader = NULL;
	lastFilterRef = NULL;
	//lastEditRef = NULL;
	setAttribute(Qt::WA_DeleteOnClose,true);
	fov = 60;
	clipRatioFar = 1;
	clipRatioNear = 1;
	nearPlane = .2f;
	farPlane = 5.f;

	id = mvcont->getNextViewerId();

	updateMeshSetVisibilities();
	updateRasterSetVisibilities();
	setAutoFillBackground(false);

	//Ratser support
	_isRaster =false;
	opacity = 0.5;
	zoom = false;
	targetTex = 0;

	connect(this->md(), SIGNAL(currentMeshChanged(int)), this, SLOT(updateLayer()),Qt::QueuedConnection);
	connect(this->md(), SIGNAL(meshModified()), this, SLOT(updateDecoration()),Qt::QueuedConnection);
	connect(this->md(), SIGNAL(meshSetChanged()), this, SLOT(updateMeshSetVisibilities()));
	connect(this->md(), SIGNAL(rasterSetChanged()), this, SLOT(updateRasterSetVisibilities()));
	connect(this->md(),SIGNAL(documentUpdated()),this,SLOT(completeUpdateRequested()));
	/*getting the meshlab MainWindow from parent, which is QWorkspace.
	*note as soon as the GLArea is added as Window to the QWorkspace the parent of GLArea is a QWidget,
	*which takes care about the window frame (its parent is the QWorkspace again).
	*/
	MainWindow* mainwindow = this->mw();
	//connecting the MainWindow Slots to GLArea signal (simple passthrough)
	if(mainwindow != NULL){
		connect(this,SIGNAL(updateMainWindowMenus()),mainwindow,SLOT(updateMenus()));
		connect(mainwindow,SIGNAL(dispatchCustomSettings(RichParameterSet&)),this,SLOT(updateCustomSettingValues(RichParameterSet&)));
	}else{
		qDebug("The parent of the GLArea parent is not a pointer to the meshlab MainWindow.");
	}

}

GLArea::~GLArea()
{
}

/*
This member returns the information of the Mesh in terms of VC,VQ,FC,FQ,WT
where:
VC = VertColor,VQ = VertQuality,FC = FaceColor,FQ = FaceQuality,WT = WedgTexCoord
*/
QString GLArea::GetMeshInfoString()
{
	QString info;
	if(mm()->hasDataMask(MeshModel::MM_VERTQUALITY) ) {info.append("VQ ");}
	if(mm()->hasDataMask(MeshModel::MM_VERTCOLOR) )   {info.append("VC ");}
	if(mm()->hasDataMask(MeshModel::MM_VERTRADIUS) )  {info.append("VR ");}
	if(mm()->hasDataMask(MeshModel::MM_VERTTEXCOORD)) {info.append("VT ");}
	if(mm()->hasDataMask(MeshModel::MM_VERTCURV) )    {info.append("VK ");}
	if(mm()->hasDataMask(MeshModel::MM_VERTCURVDIR) ) {info.append("VD ");}
	if(mm()->hasDataMask(MeshModel::MM_FACECOLOR) )   {info.append("FC ");}
	if(mm()->hasDataMask(MeshModel::MM_FACEQUALITY) ) {info.append("FQ ");}
	if(mm()->hasDataMask(MeshModel::MM_WEDGTEXCOORD)) {info.append("WT ");}
	if(mm()->hasDataMask(MeshModel::MM_CAMERA) )      {info.append("MC ");}
	if(mm()->hasDataMask(MeshModel::MM_POLYGONAL) )   {info.append("MP ");}

	return info;
}


void GLArea::Logf(int Level, const char * f, ... )
{
  if(this->log==0) return;

	char buf[4096];
	va_list marker;
	va_start( marker, f );

	vsprintf(buf,f,marker);
	va_end( marker );
	this->log->Log(Level,buf);
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
	QString outfile;

	glPushAttrib(GL_ENABLE_BIT);
	QImage tileBuffer=grabFrameBuffer(true).mirrored(false,true);
	if(ss.tiledSave)
	{
		outfile=QString("%1/%2_%3-%4.png")
			.arg(ss.outdir)
			.arg(ss.basename)
			.arg(tileCol,2,10,QChar('0'))
			.arg(tileRow,2,10,QChar('0'));
		tileBuffer.mirrored(false,true).save(outfile,"PNG");
	}
	else
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
	}
	tileCol++;

	if (tileCol >= totalCols)
	{
		tileCol=0;
		tileRow++;

		if (tileRow >= totalRows)
		{
			if(ss.snapAllLayers)
			{
				outfile=QString("%1/%2%3_L%4.png")
					.arg(ss.outdir).arg(ss.basename)
					.arg(ss.counter,2,10,QChar('0'))
					.arg(currSnapLayer,2,10,QChar('0'));
			} else {
				outfile=QString("%1/%2%3.png")
					.arg(ss.outdir).arg(ss.basename)
					.arg(ss.counter++,2,10,QChar('0'));
			}

			if(!ss.tiledSave)
			{
				bool ret = (snapBuffer.mirrored(false,true)).save(outfile,"PNG");
				if (ret) 
        {
          log->Logf(GLLogStream::SYSTEM, "Snapshot saved to %s",outfile.toLocal8Bit().constData());
          if(ss.addToRasters)
          {
            mw()->importRaster(outfile);
            
            RasterModel *rastm = md()->rm();
            rastm->shot = shotFromTrackball().first;
       			float ratio=(float)rastm->currentPlane->image.height()/(float)rastm->shot.Intrinsics.ViewportPx[1];
      			rastm->shot.Intrinsics.ViewportPx[0]=rastm->currentPlane->image.width();
			      rastm->shot.Intrinsics.ViewportPx[1]=rastm->currentPlane->image.height();
			      rastm->shot.Intrinsics.PixelSizeMm[1]/=ratio;
			      rastm->shot.Intrinsics.PixelSizeMm[0]/=ratio;
			      rastm->shot.Intrinsics.CenterPx[0]=(int)((float)rastm->shot.Intrinsics.ViewportPx[0]/2.0);
			      rastm->shot.Intrinsics.CenterPx[1]=(int)((float)rastm->shot.Intrinsics.ViewportPx[1]/2.0);
          }
        }
				else
        {
          log->Logf(GLLogStream::WARNING,"Error saving %s",outfile.toLocal8Bit().constData());
        }
			}
			takeSnapTile=false;
			snapBuffer=QImage();
		}
	}
	update();
	glPopAttrib();
}



void GLArea::drawGradient()
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
	glColor(glas.backgroundTopColor);  	glVertex2f(-1, 1);
	glColor(glas.backgroundBotColor);	glVertex2f(-1,-1);
	glColor(glas.backgroundTopColor);		glVertex2f( 1, 1);
	glColor(glas.backgroundBotColor);	glVertex2f( 1,-1);
	glEnd();

	glPopAttrib();
	glPopMatrix(); // restore modelview
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void GLArea::drawLight()
{
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

}

void GLArea::paintEvent(QPaintEvent */*event*/)
{
	int static count = 0;
	++count;
	if (mvc() == NULL)
		return;
	QPainter painter(this);
	painter.beginNativePainting();

	makeCurrent();


	if(!isValid() )return;
	QTime time;
	time.start();

	initTexture();
	glClearColor(1.0,1.0,1.0,0.0);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	setView();  // Set Modelview and Projection matrix
	if((!takeSnapTile) || (takeSnapTile && !ss.transparentBackground) )
		drawGradient();  // draws the background

	drawLight();


	glPushMatrix();

	// Finally apply the Trackball for the model
	trackball.GetView();
	trackball.Apply(false);
	glPushMatrix();

	//glScale(d);
	//	glTranslate(-FullBBox.Center());
	setLightModel();

	// Set proper colorMode
	if(rm.colorMode != GLW::CMNone)
	{
		glEnable(GL_COLOR_MATERIAL);
		glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
	}
	else glColor(Color4b::LightGray);

	if(rm.backFaceCull) glEnable(GL_CULL_FACE);
	else glDisable(GL_CULL_FACE);

	if(!this->md()->isBusy())
	{
		glPushAttrib(GL_ALL_ATTRIB_BITS);

		if (iRenderer) 
			iRenderer->Render(currentShader, *this->md(), rm, this);
		else
		{
			foreach(MeshModel * mp, this->md()->meshList)
			{
				//Mesh visibility is read from the viewer visibility map, not from the mesh
				mp->glw.SetHintParamf(GLW::HNPPointSize,glas.pointSize);
				mp->glw.SetHintParami(GLW::HNPPointDistanceAttenuation,glas.pointDistanceAttenuation?1:0);
				mp->glw.SetHintParami(GLW::HNPPointSmooth,glas.pointSmooth?1:0);
				if(meshVisibilityMap[mp->id()])
				{
					if (!md()->renderState().isEntityInRenderingState(id,MeshLabRenderState::MESH))
						mp->render(rm.drawMode,rm.colorMode,rm.textureMode);
				}
			}
			md()->renderState().render(rm.drawMode,rm.colorMode,rm.textureMode);

		}
		if(iEdit) {
		  iEdit->setLog(this->log);
		  iEdit->Decorate(*mm(),this,&painter);
		}

		// Draw the selection
		if(rm.selectedFace && (mm() != NULL))  mm()->renderSelectedFace();
		if(rm.selectedVert && (mm() != NULL))  mm()->renderSelectedVert();
		foreach(QAction * p , iDecoratorsList)
		{
			MeshDecorateInterface * decorInterface = qobject_cast<MeshDecorateInterface *>(p->parent());
			decorInterface->setLog(this->log);
			decorInterface->decorate(p,*this->md(),this->glas.currentGlobalParamSet, this,&painter);
		}
		glPopAttrib();
	} ///end if busy

	glPopMatrix(); // We restore the state to immediately after the trackball (and before the bbox scaling/translating)
	
	if(trackBallVisible && !takeSnapTile && !(iEdit && !suspendedEditor))
		trackball.DrawPostApply();

	// The picking of the surface position has to be done in object space,
	// so after trackball transformation (and before the matrix associated to each mesh);
	if(hasToPick && hasToGetPickPos)
	{
		Point3f pp;
		hasToPick=false;
		if(Pick<Point3f>(pointToPick[0],pointToPick[1],pp))
		{
			emit transmitSurfacePos(nameToGetPickPos, pp);
			hasToGetPickPos=false;
		}
	}
	glPopMatrix(); // We restore the state to immediately before the trackball

	//If it is a raster viewer draw the image as a texture
	if(isRaster()) 
		drawTarget();

	// Double click move picked point to center
	// It has to be done in the before trackball space (we MOVE the trackball itself...)
	if(hasToPick && !hasToGetPickPos)
	{
		Point3f pp;
		hasToPick=false;
		if(Pick<Point3f>(pointToPick[0],pointToPick[1],pp))
		{
			trackball.Translate(-pp);
			trackball.Scale(1.25f);
			QCursor::setPos(mapToGlobal(QPoint(width()/2+2,height()/2+2)));
		}
	}

	// ...and take a snapshot
	if (takeSnapTile) pasteTile();

	// Finally display HELP if requested
	if (isHelpVisible())
	  displayHelp(&painter);

	// Draw the log area background
	// on the bottom of the glArea
	if(infoAreaVisible)
	{
		glPushAttrib(GL_ENABLE_BIT);
		glDisable(GL_DEPTH_TEST);
		displayInfo(&painter);
		displayRealTimeLog(&painter);
		updateFps(time.elapsed());
		glPopAttrib();
	}
	
	//Draw highlight if it is the current viewer
	if(mvc()->currentId==id)
		displayViewerHighlight();


	QString error = checkGLError::makeString("There are gl errors:");
	if(!error.isEmpty()) {
		log->Logf(GLLogStream::WARNING,qPrintable(error));
	}

	//check if viewers are linked
	MainWindow *window = qobject_cast<MainWindow *>(QApplication::activeWindow());
	if(window && window->linkViewersAct->isChecked() && mvc()->currentId==id)
		mvc()->updateTrackballInViewers();
	painter.endNativePainting();
}

void GLArea::displayMatrix(QPainter *painter, QRect areaRect)
{
	painter->save();
	qFont.setFamily("Helvetica");
	qFont.setPixelSize(10);
    qFont.setStyleStrategy(QFont::PreferAntialias);
	painter->setFont(qFont);

	QString tableText;
	for(int i=0;i<4;i++)
		tableText+=QString("\t%1\t%2\t%3\t%4\n")
		.arg(mm()->cm.Tr[i][0],5,'f',2).arg(mm()->cm.Tr[i][1],5,'f',2)
		.arg(mm()->cm.Tr[i][2],5,'f',2).arg(mm()->cm.Tr[i][3],5,'f',2);

	QTextOption TO;
	QTextOption::Tab ttt;
	ttt.type=QTextOption::DelimiterTab;
	ttt.delimiter = '.';
	const int columnSpacing = 40;
	ttt.position=columnSpacing;
	QList<QTextOption::Tab> TabList;
	for(int i=0;i<4;++i){
		TabList.push_back(ttt);
		ttt.position+=columnSpacing;
	}
	TO.setTabs(TabList);
	painter->drawText(areaRect, tableText, TO);
	painter->restore();
}
void GLArea::displayRealTimeLog(QPainter *painter)
{
  painter->endNativePainting();
  painter->save();
  painter->setPen(Qt::white);
  Color4b logAreaColor = glas.logAreaColor;
  glas.logAreaColor[3]=128;
  if(mvc()->currentId!=id) logAreaColor /=2.0;

  qFont.setStyleStrategy(QFont::PreferAntialias);
  qFont.setFamily("Helvetica");
  qFont.setPixelSize(11);
  painter->setFont(qFont);
  float margin = qFont.pixelSize();
  QFontMetrics metrics = QFontMetrics(font());
  int border = qMax(4, metrics.leading());
  qreal roundness = 10.0f;
  QTextDocument doc;
  doc.setDefaultFont(qFont);
  int startingpoint = border;
  for(QMap<QString,QString>::const_iterator it = md()->Log.RealTimeLogText.constBegin();it != md()->Log.RealTimeLogText.constEnd();++it)
  {
	  doc.clear();
	  doc.setDocumentMargin(margin*0.75);
	  QColor textColor = Qt::white;
	  QColor headColor(200,200,200);
	  doc.setHtml("<font color=\"" + headColor.name() + "\" size=\"+1\" ><p><i><b>" + it.key() + "</b></i></p></font>"
				  "<font color=\"" + textColor.name() + "\"             >" + it.value() + "</font>");
	  QRect outrect(border,startingpoint,doc.size().width(),doc.size().height());
	  QPainterPath path;
	  painter->setBrush(QBrush(ColorConverter::ToQColor(logAreaColor),Qt::SolidPattern));
	  painter->setPen(ColorConverter::ToQColor(logAreaColor));
	  path.addRoundedRect(outrect,roundness,roundness);
	  painter->drawPath(path);
	  painter->save();
	  painter->translate(border,startingpoint);
	  doc.drawContents(painter);
	  painter->restore();
	  startingpoint = startingpoint + doc.size().height() + margin*.75;
  }	
  md()->Log.RealTimeLogText.clear();
  painter->restore();
  painter->beginNativePainting();
}

void GLArea::displayInfo(QPainter *painter)
{
	if (mvc() == NULL)
		return;
	painter->endNativePainting();
	painter->save();
	painter->setRenderHint(QPainter::HighQualityAntialiasing);
	QPen textPen(QColor(255,255,255,200));
	textPen.setWidthF(0.2f);
	painter->setPen(textPen);

    qFont.setStyleStrategy(QFont::PreferAntialias);
	qFont.setFamily("Helvetica");
	qFont.setPixelSize(12);
	painter->setFont(qFont);
	float barHeight = qFont.pixelSize()*5;
	QFontMetrics metrics = QFontMetrics(font());
	int border = qMax(4, metrics.leading());

	QRect Column_0(width()/10, this->height()-barHeight+border, width()/2, this->height()-border);
	QRect Column_1(width()/2 , this->height()-barHeight+border, width()*3/4,   this->height()-border);
	QRect Column_2(width()*3/4 , this->height()-barHeight+border, width(),   this->height()-border);

	Color4b logAreaColor = glas.logAreaColor;
	glas.logAreaColor[3]=128;
	if(mvc()->currentId!=id) logAreaColor /=2.0;

	painter->fillRect(QRect(0, this->height()-barHeight, width(), this->height()), ColorConverter::ToQColor(logAreaColor));

	QString col1Text,col0Text;

	if(this->md()->size()>0)
	{
		if(this->md()->size()==1)
		{
			col1Text += QString("Mesh: %1\n").arg(mm()->label());
			col1Text += QString("Vertices: %1\n").arg(mm()->cm.vn);
			col1Text += QString("Faces: %1\n").arg(mm()->cm.fn);
		}
		else
		{
			col1Text += QString("Current Mesh: %1\n").arg(mm()->label());
			col1Text += QString("Vertices: %1 (%2)\n").arg(mm()->cm.vn).arg(this->md()->vn());
			col1Text += QString("Faces: %1 (%2)\n").arg(mm()->cm.fn).arg(this->md()->fn());
		}

		if(rm.selectedFace || rm.selectedVert || mm()->cm.sfn>0 || mm()->cm.svn>0 )
			col1Text += QString("Selection: v:%1 f:%2\n").arg(mm()->cm.svn).arg(mm()->cm.sfn);

		col1Text += GetMeshInfoString();

		if(fov>5) col0Text += QString("FOV: %1\n").arg(fov);
		else col0Text += QString("FOV: Ortho\n");
		if ((cfps>0) && (cfps<999))
			col0Text += QString("FPS: %1\n").arg(cfps,7,'f',1);
		if ((clipRatioNear!=1) || (clipRatioFar!=1))
			col0Text += QString("Clipping: N:%1 F:%2\n").arg(clipRatioNear,7,'f',1).arg(clipRatioFar,7,'f',1);
		painter->drawText(Column_1, Qt::AlignLeft | Qt::TextWordWrap, col1Text);
		painter->drawText(Column_0, Qt::AlignLeft | Qt::TextWordWrap, col0Text);
        if(mm()->cm.Tr != Matrix44f::Identity() ) displayMatrix(painter, Column_2);
	}
	painter->restore();
	painter->beginNativePainting();
	//glPopAttrib();
}


void GLArea::displayViewerHighlight()
{
	// Enter in 2D screen Mode again
	glPushAttrib(GL_LINE_BIT);

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
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1.0f,1.0f,1.0f,0.3f);
	for(int width =5; width>0 ; width -= 2)
	{
		glLineWidth(width);
		glBegin(GL_LINE_LOOP);
		glVertex2f(-1.f,1.f);     glVertex2f( 1.f,1.f); glVertex2f( 1.f,-1.f);    glVertex2f(-1.f,-1.f);
		glEnd();
	}
	// Closing 2D
	glPopAttrib();
	glPopMatrix(); // restore modelview
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopAttrib();

}


void GLArea::displayHelp(QPainter *painter)
{
  static QString tableText;
  if(tableText.isEmpty())
  {
      QFile helpFile(":/images/onscreenHelp.txt");
      if(helpFile.open(QFile::ReadOnly))
          tableText=helpFile.readAll();
      else assert(0);
  }
  this->log->RealTimeLog("Quick Help",tableText);
}


void GLArea::saveSnapshot()
{
	// snap all layers
	currSnapLayer=0;

	// number of subparts
	totalCols=totalRows=ss.resolution;
	tileRow=tileCol=0;

	if(ss.snapAllLayers)
	{
		while(currSnapLayer<this->md()->meshList.size())
		{
			tileRow=tileCol=0;
			qDebug("Snapping layer %i",currSnapLayer);
			this->md()->setCurrentMesh(currSnapLayer);
			foreach(MeshModel *mp,this->md()->meshList) {
				meshSetVisibility(mp,false);
			}
			meshSetVisibility(mm(),true);

			takeSnapTile=true;
			repaint();
			currSnapLayer++;
		}

		//cleanup
		foreach(MeshModel *mp,this->md()->meshList) {
			meshSetVisibility(mp,true);
		}
		ss.counter++;
	}
	else
	{
		takeSnapTile=true;
		update();
	}
}

// Slot called when the current mesh has changed.
void GLArea::updateLayer()
{
	//if we have an edit tool open, notify it that the current layer has changed
	if(iEdit)
	{
		if(iEdit->isSingleMeshEdit())
			assert(lastModelEdited);  // if there is an editor that works on a single mesh
		// last model edited should always be set when start edit is called

		iEdit->LayerChanged(*this->md(), *lastModelEdited, this);

		//now update the last model edited
		//TODO this is not the best design....   iEdit should maybe keep track of the model on its own
		lastModelEdited = this->md()->mm();
	}
	// if the layer has changed update also the decoration.
	updateDecoration();
}

void GLArea::updateDecoration()
{
	foreach(QAction *p , iDecoratorsList)
	{
		MeshDecorateInterface * decorInterface = qobject_cast<MeshDecorateInterface *>(p->parent());
		decorInterface->endDecorate(p, *this->md(),this->glas.currentGlobalParamSet,this);
		decorInterface->startDecorate(p,*this->md(), this->glas.currentGlobalParamSet,this);
	}
}


void GLArea::setCurrentEditAction(QAction *editAction)
{
	assert(editAction);
	currentEditor = editAction;

	iEdit = actionToMeshEditMap.value(currentEditor);
	assert(iEdit);
	lastModelEdited = this->md()->mm();
	if (!iEdit->StartEdit(*this->md(), this))
	{
		//iEdit->EndEdit(*(this->md()->mm()), this);
		endEdit();
	}
	else
	{
		log->Logf(GLLogStream::SYSTEM,"Started Mode %s", qPrintable(currentEditor->text()));
		if(mm()!=NULL)
			mm()->meshModified() = true;
		else assert(!iEdit->isSingleMeshEdit());
	}
}


bool GLArea::readyToClose()
{
	if (md()->hasBeenModified())
	{		
		QMessageBox::StandardButton ret=QMessageBox::question(
			this,  tr("MeshLab"), tr("Project '%1' modified.\n\nClose without saving?").arg(md()->docLabel()),
			QMessageBox::Yes|QMessageBox::No,
			QMessageBox::No);
		if(ret==QMessageBox::No)	// don't close please!
			return false;
	}
	if(getCurrentEditAction()) 
		endEdit();
	if (iRenderer) 
		iRenderer->Finalize(currentShader, this->md(), this);
	if(targetTex) glDeleteTextures(1, &targetTex);
	emit glareaClosed();
	return true;
}

void GLArea::keyReleaseEvent ( QKeyEvent * e )
{
	if(!isRaster() || isRaster())
	{
		e->ignore();
		if(iEdit && !suspendedEditor)  iEdit->keyReleaseEvent(e,*mm(),this);
		else{
			if(e->key()==Qt::Key_Control) trackball.ButtonUp(QT2VCG(Qt::NoButton, Qt::ControlModifier ) );
			if(e->key()==Qt::Key_Shift) trackball.ButtonUp(QT2VCG(Qt::NoButton, Qt::ShiftModifier ) );
			if(e->key()==Qt::Key_Alt) trackball.ButtonUp(QT2VCG(Qt::NoButton, Qt::AltModifier ) );
		}
	}
}

void GLArea::keyPressEvent ( QKeyEvent * e )
{
	if(!isRaster() || isRaster())
	{
		e->ignore();
		if(iEdit && !suspendedEditor)  iEdit->keyPressEvent(e,*mm(),this);
		else{
			if(e->key()==Qt::Key_Control) trackball.ButtonDown(QT2VCG(Qt::NoButton, Qt::ControlModifier ) );
			if(e->key()==Qt::Key_Shift) trackball.ButtonDown(QT2VCG(Qt::NoButton, Qt::ShiftModifier ) );
			if(e->key()==Qt::Key_Alt) trackball.ButtonDown(QT2VCG(Qt::NoButton, Qt::AltModifier ) );
		}
	}
}

void GLArea::mousePressEvent(QMouseEvent*e)
{
	e->accept();	
	if(!this->hasFocus()) this->setFocus();

	if(!isRaster() || isRaster())
	{
		if( (iEdit && !suspendedEditor) )
			iEdit->mousePressEvent(e,*mm(),this);
		else {
			if ((e->modifiers() & Qt::ShiftModifier) && (e->modifiers() & Qt::ControlModifier) &&
				(e->button()==Qt::LeftButton) )
				activeDefaultTrackball=false;
			else activeDefaultTrackball=true;

			if (isDefaultTrackBall())
			{
				if(QApplication::keyboardModifiers () & Qt::Key_Control) trackball.ButtonDown(QT2VCG(Qt::NoButton, Qt::ControlModifier ) );
				else trackball.ButtonUp  (QT2VCG(Qt::NoButton, Qt::ControlModifier ) );
				if(QApplication::keyboardModifiers () & Qt::Key_Shift) trackball.ButtonDown(QT2VCG(Qt::NoButton, Qt::ShiftModifier ) );
				else trackball.ButtonUp  (QT2VCG(Qt::NoButton, Qt::ShiftModifier ) );
				if(QApplication::keyboardModifiers () & Qt::Key_Alt) trackball.ButtonDown(QT2VCG(Qt::NoButton, Qt::AltModifier ) );
				else trackball.ButtonUp  (QT2VCG(Qt::NoButton, Qt::AltModifier ) );

				trackball.MouseDown(e->x(),height()-e->y(), QT2VCG(e->button(), e->modifiers() ) );
			}
			else trackball_light.MouseDown(e->x(),height()-e->y(), QT2VCG(e->button(), Qt::NoModifier ) );
		}
	}
	update();
}

void GLArea::mouseMoveEvent(QMouseEvent*e)
{
	if(!isRaster() || isRaster())
	{
		if( (iEdit && !suspendedEditor) )
			iEdit->mouseMoveEvent(e,*mm(),this);
		else {
			if (isDefaultTrackBall())
			{
				trackball.MouseMove(e->x(),height()-e->y());
				setCursorTrack(trackball.current_mode);
			}
			else trackball_light.MouseMove(e->x(),height()-e->y());
		}
		update();
	}

}

// When mouse is released we set the correct mouse cursor
void GLArea::mouseReleaseEvent(QMouseEvent*e)
{
	//clearFocus();
	activeDefaultTrackball=true;
	if( (iEdit && !suspendedEditor) )
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
	setFocus();
	if( (iEdit && !suspendedEditor) )
	{
		iEdit->wheelEvent(e,*mm(),this);
	}
	else
	{
		const int WHEEL_STEP = 120;
		float notch = e->delta()/ float(WHEEL_STEP);
		switch(e->modifiers())
		{
		case Qt::ControlModifier+Qt::ShiftModifier     : clipRatioFar  = math::Clamp( clipRatioFar*powf(1.2f, notch),0.01f,5000.0f); break;
		case Qt::ControlModifier                       : clipRatioNear = math::Clamp(clipRatioNear*powf(1.2f, notch),0.01f,50.0f); break;
		case Qt::ShiftModifier                         : fov = math::Clamp(fov+1.2f*notch,5.0f,90.0f); break;
		case Qt::AltModifier                           : glas.pointSize = math::Clamp(glas.pointSize*powf(1.2f, notch),0.01f,150.0f);
			foreach(MeshModel * mp, this->md()->meshList)
				mp->glw.SetHintParamf(GLW::HNPPointSize,glas.pointSize);
			break;
		default:
			if(isRaster())
				this->opacity = math::Clamp( opacity*powf(1.2f, notch),0.1f,1.0f);
			else
				trackball.MouseWheel( e->delta()/ float(WHEEL_STEP));
			break;
		}
	}
	update();
}


void GLArea::mouseDoubleClickEvent ( QMouseEvent * e )
{
	if(!isRaster() || isRaster())
	{
		hasToPick=true;
		pointToPick=Point2i(e->x(),height()-e->y());
	}
	update();
}

void GLArea::focusInEvent ( QFocusEvent * e )
{
	QWidget::focusInEvent(e);
	emit currentViewerChanged(id);
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
		curMap["PickMode"]=QCursor(QPixmap(":/images/cursors/plain_pick.png"),1,1);
	}
	if(tm) setCursor(curMap[tm->Name()]);
	else
		if(hasToGetPickPos) setCursor(curMap["PickMode"]);
		else setCursor(curMap[""]);

}

void GLArea::setDrawMode(vcg::GLW::DrawMode mode)
{
	rm.drawMode = mode;
	update();
}


void GLArea::setColorMode(vcg::GLW::ColorMode mode)
{
	rm.colorMode = mode;
	update();
}
void GLArea::updateTexture()
{
	hasToUpdateTexture = true;
}

// compute the next highest power of 2 of 32-bit v
int GLArea::RoundUpToTheNextHighestPowerOf2(unsigned int v)
{
  v--;
  v |= v >> 1;
  v |= v >> 2;
  v |= v >> 4;
  v |= v >> 8;
  v |= v >> 16;
  v++;
return v;
}
/** initTexture loads all the required textures (if necessary).
It is called every time in the glpaint.
It assumes that:
- there is a shared gl wrapper for all the contexts (same texture id for different glareas)
- the values stored in the glwrapper for the texture id are an indicator if there is the need of loading a texture (0 means load that texture)

*/

void GLArea::initTexture()
{
	if(hasToUpdateTexture)
	{
		foreach (MeshModel *mp,this->md()->meshList)
			mp->glw.TMId.clear();

		qDebug("Beware: deleting the texutres could lead to problems for shared textures.");
		hasToUpdateTexture = false;
	}

	foreach (MeshModel *mp, this->md()->meshList)
	{
		if(!mp->cm.textures.empty() && mp->glw.TMId.empty()){
			glEnable(GL_TEXTURE_2D);
			GLint MaxTextureSize;
			glGetIntegerv(GL_MAX_TEXTURE_SIZE,&MaxTextureSize);
			QString unexistingtext = "In mesh file <i>" + mp->fullName() + "</i> : Failure loading textures:<br>";
			bool sometextfailed = false;
			for(unsigned int i =0; i< mp->cm.textures.size();++i)
			{
				QImage img, imgScaled, imgGL;
				bool res = img.load(mp->cm.textures[i].c_str());
				sometextfailed = sometextfailed || !res;
				if(!res)
				{
					// Note that sometimes (in collada) the texture names could have been encoded with a url-like style (e.g. replacing spaces with '%20') so making some other attempt could be harmless
					QString ConvertedName = QString(mp->cm.textures[i].c_str()).replace(QString("%20"), QString(" "));
					res = img.load(ConvertedName);
					if(!res)
					{
						this->Logf(0,"Failure of loading texture %s",mp->cm.textures[i].c_str());
						unexistingtext += "<font color=red>" + QString(mp->cm.textures[i].c_str()) + "</font><br>";
					}
					else 
						this->Logf(0,"Warning, texture loading was successful only after replacing %%20 with spaces;\n Loaded texture %s instead of %s",qPrintable(ConvertedName),mp->cm.textures[i].c_str());
					mp->glw.TMId.push_back(0);
					glGenTextures( 1, (GLuint*)&(mp->glw.TMId.back()) );
					glBindTexture( GL_TEXTURE_2D, mp->glw.TMId.back() );
					glTexImage2D( GL_TEXTURE_2D, 0, 3, 0, 0, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0 );
				}
				if(!res && QString(mp->cm.textures[i].c_str()).endsWith("dds",Qt::CaseInsensitive))
				{
					qDebug("DDS binding!");
					int newTexId = bindTexture(QString(mp->cm.textures[i].c_str()));
					mp->glw.TMId.push_back(newTexId);
				}
				if(res)
				{
					// image has to be scaled to a 2^n size. We choose the first 2^N >= picture size.
					int bestW=RoundUpToTheNextHighestPowerOf2(img.width()  );
					int bestH=RoundUpToTheNextHighestPowerOf2(img.height() );
					while(bestW>MaxTextureSize) bestW /=2;
					while(bestH>MaxTextureSize) bestH /=2;

					Logf(GLLogStream::SYSTEM,"Loading textures");
					Logf(GLLogStream::SYSTEM,"	Texture[ %3i ] =  '%s' ( %6i x %6i ) -> ( %6i x %6i )",	i,mp->cm.textures[i].c_str(), img.width(), img.height(),bestW,bestH);
					imgScaled=img.scaled(bestW,bestH,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
					imgGL=convertToGLFormat(imgScaled);
					mp->glw.TMId.push_back(0);
					qDebug("      	will be loaded as GL texture id %i  ( %i x %i )",mp->glw.TMId.back() ,imgGL.width(), imgGL.height());
					glGenTextures( 1, (GLuint*)&(mp->glw.TMId.back()) );
					glBindTexture( GL_TEXTURE_2D, mp->glw.TMId.back() );
					glTexImage2D( GL_TEXTURE_2D, 0, 3, imgGL.width(), imgGL.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, imgGL.bits() );
					gluBuild2DMipmaps(GL_TEXTURE_2D, 3, imgGL.width(), imgGL.height(), GL_RGBA, GL_UNSIGNED_BYTE, imgGL.bits() );
				}
				if(glas.textureMagFilter == 0 ) 	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
				else	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
				if(glas.textureMinFilter == 0 ) 	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
				else	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );

				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			}
			if (sometextfailed)
				QMessageBox::warning(this,"Texture files has not been correctly loaded",unexistingtext);
		}
		glDisable(GL_TEXTURE_2D);
	}
	
}

void GLArea::setTextureMode(vcg::GLW::TextureMode mode)
{
	rm.textureMode = mode;
	update();
}

void GLArea::setLight(bool state)
{
	rm.lighting = state;
	update();
}

void GLArea::setLightMode(bool state,LightingModel lmode)
{
	switch(lmode)
	{
	case LDOUBLE:		rm.doubleSideLighting = state;	break;
	case LFANCY:		rm.fancyLighting = state; break;
	}
	update();
}

void GLArea::setBackFaceCulling(bool enabled)
{
	rm.backFaceCull = enabled;
	update();
}

void GLArea::setSelectFaceRendering(bool enabled)
{
	rm.selectedFace = enabled;
	update();
}
void GLArea::setSelectVertRendering(bool enabled)
{
	rm.selectedVert = enabled;
	update();
}

void GLArea::setLightModel()
{
	if (rm.lighting)
	{
		glEnable(GL_LIGHTING);

		if (rm.doubleSideLighting)
			glEnable(GL_LIGHT1);
		else
			glDisable(GL_LIGHT1);

		glLightfv(GL_LIGHT0, GL_AMBIENT, Color4f::Construct(glas.baseLightAmbientColor).V());
		glLightfv(GL_LIGHT0, GL_DIFFUSE, Color4f::Construct(glas.baseLightDiffuseColor).V());
		glLightfv(GL_LIGHT0, GL_SPECULAR,Color4f::Construct(glas.baseLightSpecularColor).V());

		glLightfv(GL_LIGHT1, GL_AMBIENT, Color4f::Construct(glas.baseLightAmbientColor).V());
		glLightfv(GL_LIGHT1, GL_DIFFUSE, Color4f::Construct(glas.baseLightDiffuseColor).V());
		glLightfv(GL_LIGHT1, GL_SPECULAR,Color4f::Construct(glas.baseLightSpecularColor).V());
		if(rm.fancyLighting)
		{
			glLightfv(GL_LIGHT0, GL_DIFFUSE, Color4f::Construct(glas.fancyFLightDiffuseColor).V());
			glLightfv(GL_LIGHT1, GL_DIFFUSE, Color4f::Construct(glas.fancyBLightDiffuseColor).V());
		}
	}
	else glDisable(GL_LIGHTING);
}

void GLArea::setView()
{
	glViewport(0,0, this->width(),this->height());
	curSiz.setWidth(this->width());
	curSiz.setHeight(this->height());

	GLfloat fAspect = (GLfloat)curSiz.width()/ curSiz.height();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// This parameter is the one that controls:
	// HOW LARGE IS THE TRACKBALL ICON ON THE SCREEN.
	float viewRatio = 1.75f;

	float cameraDist = viewRatio / tanf(math::ToRad(fov*.5f));  // the distance between the center of the trackball and the viewer.

	if(fov==5) cameraDist = 1000; // small hack for orthographic projection where camera distance is rather meaningless...

	nearPlane = cameraDist - 2.f*clipRatioNear;
	farPlane =  cameraDist + 10.f*clipRatioFar;
	if(nearPlane<=cameraDist*.1f) nearPlane=cameraDist*.1f;

	if (!takeSnapTile)
	{
		if(fov==5)	glOrtho( -viewRatio*fAspect, viewRatio*fAspect, -viewRatio, viewRatio, cameraDist - 2.f*clipRatioNear, cameraDist+2.f*clipRatioFar);
		else		gluPerspective(fov, fAspect, nearPlane, farPlane);
	}
	else	setTiledView(fov, viewRatio, fAspect, nearPlane, farPlane, cameraDist);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, cameraDist,0, 0, 0, 0, 1, 0);
}

void GLArea::setTiledView(GLdouble fovY, float viewRatio, float fAspect, GLdouble zNear, GLdouble zFar,  float cameraDist)
{
	if(fovY<=5)
	{
		GLdouble fLeft   = -viewRatio*fAspect;
		GLdouble fRight  =  viewRatio*fAspect;
		GLdouble fBottom = -viewRatio;
		GLdouble fTop    =  viewRatio;

		GLdouble tDimX = fabs(fRight-fLeft) / totalCols;
		GLdouble tDimY = fabs(fTop-fBottom) / totalRows;


		glOrtho(fLeft   + tDimX * tileCol, fLeft   + tDimX * (tileCol+1),     /* left, right */
			fBottom + tDimY * tileRow, fBottom + tDimY * (tileRow+1),     /* bottom, top */
			cameraDist - 2.f*clipRatioNear, cameraDist+2.f*clipRatioFar);
	}
	else
	{
		GLdouble fTop    = zNear * tan(math::ToRad(fovY/2.0));
		GLdouble fRight  = fTop * fAspect;
		GLdouble fBottom = -fTop;
		GLdouble fLeft   = -fRight;

		// tile Dimension
		GLdouble tDimX = fabs(fRight-fLeft) / totalCols;
		GLdouble tDimY = fabs(fTop-fBottom) / totalRows;

		glFrustum(fLeft   + tDimX * tileCol, fLeft   + tDimX * (tileCol+1),
			fBottom + tDimY * tileRow, fBottom + tDimY * (tileRow+1), zNear, zFar);
	}
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

void GLArea::resetTrackBall()
{
	trackball.Reset();
	float newScale= 3.0f/this->md()->bbox().Diag();
	trackball.track.sca = newScale;
	trackball.track.tra =  -this->md()->bbox().Center();
	update();
}

void GLArea::hideEvent(QHideEvent * /*event*/)
{
	trackball.current_button=0;
}

void GLArea::sendViewPos(QString name)
{
#ifndef VCG_USE_EIGEN
	Point3f pos=  trackball.track.InverseMatrix() *Inverse(trackball.camera.model) *Point3f(0,0,0);
#else
	Point3f pos=  Eigen::Transform3f(trackball.track.InverseMatrix()) * Eigen::Transform3f(Inverse(trackball.camera.model)).translation();
#endif
	emit transmitViewPos(name, pos);
}

void GLArea::sendSurfacePos(QString name)
{
	qDebug("sendSurfacePos %s",qPrintable(name));
	nameToGetPickPos = name;
	hasToGetPickPos=true;
}

void GLArea::sendViewDir(QString name)
{
	Point3f dir= getViewDir();
	emit transmitViewDir(name,dir);
}

void GLArea::sendMeshShot(QString name)
{
	Shotf curShot=this->md()->mm()->cm.shot;
	emit transmitShot(name, curShot);
}

void GLArea::sendMeshMatrix(QString name)
{
	Matrix44f mat=this->md()->mm()->cm.Tr;
	emit transmitMatrix(name, mat);
}

void GLArea::sendViewerShot(QString name)
{
	Shotf curShot=shotFromTrackball().first;
	emit transmitShot(name, curShot);
}
void GLArea::sendRasterShot(QString name)
{
	Shotf curShot = vcg::Shotf();
	if (this->md()->rm() != NULL)
		curShot = this->md()->rm()->shot;
	emit transmitShot(name, curShot);
}

void GLArea::sendCameraPos( QString name )
{
	Point3f dir= trackball.camera.ViewPoint();
	emit transmitCameraPos(name,dir);
}

Point3f GLArea::getViewDir()
{
	vcg::Matrix44f rotM;
	trackball.track.rot.ToMatrix(rotM);
	vcg::Invert(rotM);
	return rotM*vcg::Point3f(0,0,1);
}

void GLArea::updateCustomSettingValues( RichParameterSet& rps )
{
	glas.updateGlobalParameterSet(rps);
	this->update();
}

void GLArea::initGlobalParameterSet( RichParameterSet * defaultGlobalParamSet)
{
	GLAreaSetting::initGlobalParameterSet(defaultGlobalParamSet);
}

//Don't alter the state of the other elements in the visibility map
void GLArea::updateMeshSetVisibilities()
{
	meshVisibilityMap.clear();
	foreach(MeshModel * mp, this->md()->meshList)
	{
		//Insert the new pair in the map; If the key is already in the map, its value will be overwritten
		meshVisibilityMap.insert(mp->id(),mp->visible);
	}
}

//Don't alter the state of the other elements in the visibility map
void GLArea::updateRasterSetVisibilities()
{
	//Align rasterVisibilityMap state with rasterList state
	//Deleting from the map the visibility of the deleted rasters 
	QMapIterator<int, bool> i(rasterVisibilityMap);
	while (i.hasNext()) {
		i.next();
		bool found =false;
		foreach(RasterModel * rp, this->md()->rasterList)
		{
			if(rp->id() == i.key())
			{
				found = true;
				break;
			}
		}
		if(!found)
			rasterVisibilityMap.remove(i.key());
	}

	foreach(RasterModel * rp, this->md()->rasterList)
	{
		//Insert the new pair in the map;If the key is already in the map, its value will be overwritten
		rasterVisibilityMap.insert(rp->id(),rp->visible);
	}
}

void GLArea::meshSetVisibility(MeshModel *mp, bool visibility)
{
	mp->visible=visibility;
	meshVisibilityMap[mp->id()]=visibility;
}

void GLArea::addRasterSetVisibility(int rasterId, bool visibility)
{
	rasterVisibilityMap.insert(rasterId,visibility);
}

// --------------- Raster view -------------------------------------
void GLArea::setIsRaster(bool viewMode){
	_isRaster= viewMode;
}

// this slot is called when someone press the showraster button on the toolbar
void GLArea::showRaster(bool resetViewFlag)
{
	if(!this->isRaster())
	{
		lastViewBeforeRasterMode = this->viewToText();
		setIsRaster(true);
		loadRaster(md()->rm()->id() );
	} else
	{
		this->setIsRaster(false);
		QDomDocument doc("StringDoc");
		doc.setContent(lastViewBeforeRasterMode);
		if(resetViewFlag) this->loadViewFromViewStateFile(doc);
		else this->update();
	}
}

void GLArea::loadRaster(int id)
{
	foreach(RasterModel *rm, this->md()->rasterList)
		if(rm->id()==id)
		{
			this->md()->setCurrentRaster(id);
			setTarget(rm->currentPlane->image);
			//load his shot or a default shot

			if (rm->shot.IsValid())
			{
				fov = rm->shot.GetFovFromFocal();

				float cameraDist = getCameraDistance();
				Matrix44f rotFrom;
				rm->shot.Extrinsics.Rot().ToMatrix(rotFrom);

				Point3f p1 = rotFrom*(vcg::Point3f::Construct(rm->shot.Extrinsics.Tra()));

				Point3f p2 = (Point3f(0,0,cameraDist));

				trackball.track.sca =fabs(p2.Z()/p1.Z());
				loadShot(QPair<Shotf, float> (rm->shot,trackball.track.sca));
			}
			else
				createOrthoView("Front");
		}
}

void GLArea::drawTarget() 
{
	if(!targetTex) return;

	if(this->md()->rm()==0) return;
	QImage &curImg = this->md()->rm()->currentPlane->image;
	float imageRatio = float(curImg.width())/float(curImg.height());
	float screenRatio = float(this->width())/float(this->height());
	//set orthogonal view
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(-1.0f*screenRatio, 1.0f*screenRatio, -1, 1);

	glColor4f(1, 1, 1, opacity);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, targetTex);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);	//first point
	glVertex3f(-1.0f*imageRatio, -1.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);	//second point
	glVertex3f(1.0f*imageRatio, -1.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);	//third point
	glVertex3f(1.0f*imageRatio, 1.0f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);	//fourth point
	glVertex3f(-1.0f*imageRatio, 1.0f, 0.0f);
	glEnd();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);

	//restore view
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}


void GLArea::setTarget(QImage &image) {

	if (targetTex) {
		glDeleteTextures(1, &targetTex);
		targetTex = 0;
	}
	// create texture
	glGenTextures(1, &targetTex);
	QImage tximg = QGLWidget::convertToGLFormat(image);
	glBindTexture(GL_TEXTURE_2D, targetTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, tximg.width(), tximg.height(),
		0, GL_RGBA, GL_UNSIGNED_BYTE, tximg.bits());

	glBindTexture(GL_TEXTURE_2D, 0);
}

// --------------- Methods involving shots -------------------------------------

float GLArea::getCameraDistance()
{
	// This parameter is the one that controls:
	// HOW LARGE IS THE TRACKBALL ICON ON THE SCREEN.
	float viewRatio = 1.75f;
	float cameraDist = viewRatio / tanf(vcg::math::ToRad(fov*.5f));

	return cameraDist;
}

void GLArea::initializeShot(Shotf &shot)
{
	shot.Intrinsics.PixelSizeMm[0]=0.036916077;
	shot.Intrinsics.PixelSizeMm[1]=0.036916077;

	shot.Intrinsics.DistorCenterPx[0]=width()/2;
	shot.Intrinsics.DistorCenterPx[1]=height()/2;
	shot.Intrinsics.CenterPx[0]=width()/2;
	shot.Intrinsics.CenterPx[1]=height()/2;
	shot.Intrinsics.ViewportPx[0]=width();
	shot.Intrinsics.ViewportPx[1]=height();

	double viewportYMm = shot.Intrinsics.PixelSizeMm[1]*shot.Intrinsics.ViewportPx[1];
	float defaultFov=60.0;
	shot.Intrinsics.FocalMm = viewportYMm/(2*tanf(vcg::math::ToRad(defaultFov/2))); //27.846098mm

	shot.Extrinsics.SetIdentity();
}

bool GLArea::viewFromFile() 
{
	QString filename = QFileDialog::getOpenFileName(this, tr("Load Project"), "./", tr("Xml Files (*.xml)"));

	QFile qf(filename);
	QFileInfo qfInfo(filename);

	if( !qf.open(QIODevice::ReadOnly ) )
		return false;

	QString project_path = qfInfo.absoluteFilePath();

	QDomDocument doc("XmlDocument");    //It represents the XML document
	if(!doc.setContent( &qf ))	
		return false;

	QString type = doc.doctype().name();

	//TextAlign file project
	if(type == "RegProjectML")   loadShotFromTextAlignFile(doc);
	//View State file
	else if(type == "ViewState") loadViewFromViewStateFile(doc);

	qDebug("End file reading");
	qf.close();

	return true;
}

void GLArea::loadShotFromTextAlignFile(const QDomDocument &doc)
{
	QDomElement root = doc.documentElement();
	QDomNode node;
	Shotf shot;

	node = root.firstChild();

	//Devices
	while(!node.isNull()){
		if(QString::compare(node.nodeName(),"Device")==0)
		{
			QString type = node.attributes().namedItem("type").nodeValue();
			if (type== "GlImageWidget")
			{
				//Aligned Image
				if(QString::compare(node.attributes().namedItem("aligned").nodeValue(),"1")==0){
					QDomNode nodeb = node.firstChild();
					ReadShotFromOLDXML(shot,nodeb);
				}
			}
		}
		node = node.nextSibling();
	}

	//Adjust params for Meshlab settings

	//resize viewport
	int w = shot.Intrinsics.ViewportPx[0];
	int h = shot.Intrinsics.ViewportPx[1];

	shot.Intrinsics.DistorCenterPx[0]=w/2;
	shot.Intrinsics.DistorCenterPx[1]=h/2;
	shot.Intrinsics.CenterPx[0]=w/2;
	shot.Intrinsics.CenterPx[1]=h/2;
	shot.Intrinsics.ViewportPx[0]=w;
	shot.Intrinsics.ViewportPx[1]=h;

	// The shot loaded from TextAlign doesn't have a scale. Trackball needs it.
	// The scale factor is computed as the ratio between cameraDistance and the z coordinate of the translation 
	// introduced by the shot.
	fov = shot.GetFovFromFocal();

	float cameraDist = getCameraDistance();

	Matrix44f rotFrom;
	shot.Extrinsics.Rot().ToMatrix(rotFrom);

	Point3f p1 = rotFrom*(vcg::Point3f::Construct(shot.Extrinsics.Tra()));

	Point3f p2 = (Point3f(0,0,cameraDist));

	trackball.track.sca =fabs(p2.Z()/p1.Z());

	loadShot(QPair<Shotf, float> (shot,trackball.track.sca));

}

/* 
ViewState file is an xml file format created by Meshlab with the action "copyToClipboard"
*/
void GLArea::loadViewFromViewStateFile(const QDomDocument &doc)
{
	Shotf shot;
	QDomElement root = doc.documentElement();
	QDomNode node = root.firstChild();

	while(!node.isNull())
	{
		if (QString::compare(node.nodeName(),"VCGCamera")==0)
			ReadShotFromQDomNode<Shotf>(shot,node);
		else if (QString::compare(node.nodeName(),"CamParam")==0)
			ReadShotFromOLDXML<Shotf>(shot,node);

		else if (QString::compare(node.nodeName(),"ViewSettings")==0)
		{
			QDomNamedNodeMap attr = node.attributes();
			trackball.track.sca = attr.namedItem("TrackScale").nodeValue().section(' ',0,0).toFloat();
			nearPlane = attr.namedItem("NearPlane").nodeValue().section(' ',0,0).toFloat();
			farPlane = attr.namedItem("FarPlane").nodeValue().section(' ',0,0).toFloat();      
			fov = shot.GetFovFromFocal();
			clipRatioNear = (getCameraDistance()-nearPlane)/2.0f ;
			clipRatioFar = (farPlane-getCameraDistance())/10.0f ;

		}
		else if (QString::compare(node.nodeName(),"Render")==0)
		{
			QDomNamedNodeMap attr = node.attributes();
			rm.drawMode = (vcg::GLW::DrawMode) (attr.namedItem("DrawMode").nodeValue().section(' ',0,0).toInt());
			rm.colorMode = (vcg::GLW::ColorMode) (attr.namedItem("ColorMode").nodeValue().section(' ',0,0).toInt());
			rm.textureMode = (vcg::GLW::TextureMode) (attr.namedItem("TextureMode").nodeValue().section(' ',0,0).toInt());
			rm.lighting = (attr.namedItem("Lighting").nodeValue().section(' ',0,0).toInt() != 0);
			rm.backFaceCull = (attr.namedItem("BackFaceCull").nodeValue().section(' ',0,0).toInt() != 0);
			rm.doubleSideLighting = (attr.namedItem("DoubleSideLighting").nodeValue().section(' ',0,0).toInt() != 0);
			rm.fancyLighting = (attr.namedItem("FancyLighting").nodeValue().section(' ',0,0).toInt() != 0);
			rm.selectedFace = (attr.namedItem("SelectedFace").nodeValue().section(' ',0,0).toInt() != 0);
			rm.selectedVert = (attr.namedItem("SelectedVert").nodeValue().section(' ',0,0).toInt() != 0);
		}
		node = node.nextSibling();
	}

	loadShot(QPair<Shotf, float> (shot,trackball.track.sca));
}
QString GLArea::viewToText()
{
	QString docString;

	Shotf shot = shotFromTrackball().first;

	QDomDocument doc("ViewState");
	QDomElement root = doc.createElement("project");
	doc.appendChild( root );

	QDomElement shotElem = WriteShotToQDomNode(shot,doc);
	root.appendChild(shotElem);

	QDomElement settingsElem = doc.createElement( "ViewSettings" );
	settingsElem.setAttribute( "TrackScale", trackball.track.sca);
	settingsElem.setAttribute( "NearPlane", nearPlane);
	settingsElem.setAttribute( "FarPlane", farPlane);
	root.appendChild(settingsElem);

	QDomElement renderElem = doc.createElement( "Render");
	renderElem.setAttribute("DrawMode",rm.drawMode);
	renderElem.setAttribute("ColorMode",rm.colorMode);
	renderElem.setAttribute("TextureMode",rm.textureMode);
	renderElem.setAttribute("Lighting",rm.lighting);
	renderElem.setAttribute("BackFaceCull",rm.backFaceCull);
	renderElem.setAttribute("DoubleSideLighting",rm.doubleSideLighting);
	renderElem.setAttribute("FancyLighting",rm.fancyLighting);
	renderElem.setAttribute("SelectedFace",rm.selectedFace);
	renderElem.setAttribute("SelectedVert",rm.selectedVert);
	root.appendChild(renderElem);

	return doc.toString();
}

void GLArea::viewToClipboard()
{
	QApplication::clipboard()->setText(this->viewToText());
}

void GLArea::viewFromClipboard()
{
	QClipboard *clipboard = QApplication::clipboard();
	QString shotString = clipboard->text();
	QDomDocument doc("StringDoc");  
	doc.setContent(shotString);
	loadViewFromViewStateFile(doc);
}

QPair<vcg::Shotf,float> GLArea::shotFromTrackball()
{
	Shotf shot;
	initializeShot(shot);

	double viewportYMm=shot.Intrinsics.PixelSizeMm[1]*shot.Intrinsics.ViewportPx[1];
	shot.Intrinsics.FocalMm = viewportYMm/(2*tanf(vcg::math::ToRad(fov/2)));

	float cameraDist = getCameraDistance();

	//add the translation introduced by gluLookAt() (0,0,cameraDist), in order to have te same view---------------
	//T(gl)*S*R*T(t) => SR(gl+t) => S R (S^(-1)R^(-1)gl + t)
	//Add translation S^(-1) R^(-1)(gl)
	//Shotd doesn't introduce scaling
	//---------------------------------------------------------------------
	shot.Extrinsics.SetTra( shot.Extrinsics.Tra() + (Inverse(shot.Extrinsics.Rot())*Point3f(0, 0, cameraDist)));

	vcg::Shotf newShot = track2ShotCPU<float>(shot, &trackball);

	return QPair<Shotf, float> (newShot,trackball.track.sca);
}
void GLArea::viewFromCurrentShot(QString kind)
{
	Shotf localShot;
	if(kind=="Mesh" && this->md()->mm())   localShot = this->md()->mm()->cm.shot;
	if(kind=="Raster" && this->md()->rm()) localShot = this->md()->rm()->shot;
	if(!localShot.IsValid())
	{
		this->log->Logf(GLLogStream::SYSTEM, "Unable to set Shot from current %s",qPrintable(kind));
		return;
	}

	loadShot(QPair<Shotf, float>(localShot,trackball.track.sca));
}


void GLArea::loadShot(const QPair<vcg::Shotf,float> &shotAndScale){

	Shotf shot = shotAndScale.first;

	fov = shot.GetFovFromFocal();

	float cameraDist = getCameraDistance();

	//reset trackball. The point of view must be set only by the shot
	trackball.Reset();
	trackball.track.sca = shotAndScale.second;

	/*Point3f point = this->md()->bbox().Center();
	Point3f p1 = ((trackball.track.Matrix()*(point-trackball.center))- Point3f(0,0,cameraDist));*/
	shot2Track(shot, cameraDist,trackball);

	//Expressing the translation along Z with a scale factor k
	//Point3f p2 = ((trackball.track.Matrix()*(point-trackball.center))- Point3f(0,0,cameraDist));

	////k is the ratio between the distances along z of two correspondent points (before and after the traslation) 
	////from the point of view
	//float k= abs(p2.Z()/p1.Z());

	//float sca= trackball.track.sca/k;
	//Point3f tra = trackball.track.tra;
	//
	//// Apply this formula:
	//// SR(t+p) -v = k[S'R'(t'+p) -v] forall p, R=R', k is a costant
	//// SR(t) -v = k[S'R(t') -v]
	//// t' = 1/k* S'^-1St + (k-1)/k S'^-1*R^-1v
	//Matrix44f s0 = Matrix44f().SetScale(trackball.track.sca,trackball.track.sca, trackball.track.sca);
	//Matrix44f s1 = Matrix44f().SetScale(sca, sca, sca);
	//Matrix44f r;
	//trackball.track.rot.ToMatrix(r);
	//Matrix44f rapM = Matrix44f().SetScale(1/k, 1/k, 1/k);
	//Matrix44f rapM2 = Matrix44f().SetScale(1-1/k, 1-1/k, 1-1/k);
	//Point3f t1 = rapM*Inverse(s1)*s0*tra + rapM2*Inverse(s1)*Inverse(r)*Point3f(0,0,cameraDist);

	//trackball.track.sca =sca;
	//trackball.track.tra =t1 /*+ tb.track.rot.Inverse().Rotate(glLookAt)*/ ;


	update();
}

void GLArea::createOrthoView(QString dir)
{
	Shotf view;
	initializeShot(view);

	fov =5;
	double viewportYMm = view.Intrinsics.PixelSizeMm[1]*view.Intrinsics.ViewportPx[1];
	view.Intrinsics.FocalMm = viewportYMm/(2*tanf(vcg::math::ToRad(fov/2))); //27.846098 equivalente a circa 60 gradi

	trackball.Reset();
	float newScale= 3.0f/this->md()->bbox().Diag();
	trackball.track.sca = newScale;
	trackball.track.tra =  -this->md()->bbox().Center();

	vcg::Matrix44f rot;

	if(dir == tr("Top"))
		rot.SetRotateDeg(90,Point3f(1,0,0));
	else if(dir == tr("Bottom"))
		rot.SetRotateDeg(90,Point3f(-1,0,0));
	else if(dir == tr("Left"))
		rot.SetRotateDeg(90,Point3f(0,1,0));
	else if(dir == tr("Right"))
		rot.SetRotateDeg(90,Point3f(0,-1,0));
	else if(dir == tr("Front"))
		rot.SetRotateDeg(0,Point3f(0,1,0));
	else if(dir == tr("Back"))
		rot.SetRotateDeg(180,Point3f(0,1,0));

	view.Extrinsics.SetRot(rot);

	float cameraDist = getCameraDistance();

	//add the translation introduced by gluLookAt() (0,0,cameraDist), in order to have te same view---------------
	//T(gl)*S*R*T(t) => SR(gl+t) => S R (S^(-1)R^(-1)gl + t)
	//Add translation S^(-1) R^(-1)(gl)
	//Shotd doesn't introduce scaling
	//---------------------------------------------------------------------
	view.Extrinsics.SetTra( view.Extrinsics.Tra() + (Inverse(view.Extrinsics.Rot())*Point3f(0, 0, cameraDist)));

	Shotf shot = track2ShotCPU(view, &trackball);

	QPair<Shotf,float> shotAndScale = QPair<Shotf,float> (shot, trackball.track.sca);
	loadShot(shotAndScale);
}

MultiViewer_Container * GLArea::mvc()
{ 
	QObject * curParent = this->parent();
	while(qobject_cast<MultiViewer_Container *>(curParent) == 0)
	{
		if (curParent != NULL)
			curParent = curParent->parent();
		else
			return NULL;
	}
	return qobject_cast<MultiViewer_Container *>(curParent);
}


MainWindow * GLArea::mw()
{
	QObject * curParent = this->parent();
	while(qobject_cast<MainWindow *>(curParent) == 0)
	{
		curParent = curParent->parent();
	}
	return qobject_cast<MainWindow *>(curParent);
}

bool GLArea::showInterruptButton() const
{
	return interrbutshow;
}

void GLArea::showInterruptButton( const bool& show )
{
	interrbutshow = show;
}

void GLArea::completeUpdateRequested()
{
	if ((this->md() != NULL) && (this->md()->rm() != NULL))
		loadRaster(this->md()->rm()->id());
	//if (md()->mm() != NULL)
	//	trackball.center = md()->mm()->cm.bbox.Center();
	update();
}
