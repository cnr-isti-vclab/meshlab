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



#include <common/interfaces.h>

#include "glarea.h"
#include "mainwindow.h"
#include "multiViewer_Container.h"
#include "ml_default_decorators.h"

#include <QFileDialog>
#include <QClipboard>
#include <QLocale>

#include <wrap/gl/picking.h>
#include <wrap/qt/trackball.h>
#include <wrap/qt/col_qt_convert.h>
#include <wrap/qt/shot_qt.h>
#include <wrap/qt/checkGLError.h>
#include <wrap/qt/gl_label.h>
#include <wrap/io_trimesh/export_ply.h>
#include <wrap/io_trimesh/import_ply.h>
#include<QOpenGLContext>


using namespace std;
using namespace vcg;

GLArea::GLArea(QWidget *parent, MultiViewer_Container *mvcont, RichParameterSet *current)
    : QGLWidget(parent,mvcont->sharedDataContext()),interrbutshow(false)
{
	makeCurrent();
    parentmultiview = mvcont;
    this->updateCustomSettingValues(*current);
    animMode=AnimNone;
    iRenderer=0; //Shader support
    iEdit=0;
    currentEditor=0;
    suspendedEditor=false;
    lastModelEdited = 0;
    cfps=0;
    lastTime=0;
    hasToPick=false;
    hasToSelectMesh=false;
    hasToGetPickPos=false;
    //hasToUpdateTexture=false;
    helpVisible=false;
    takeSnapTile=false;
    activeDefaultTrackball=true;
    infoAreaVisible = true;
    trackBallVisible = true;
    currentShader = NULL;
    lastFilterRef = NULL;
    //lastEditRef = NULL;
    setAttribute(Qt::WA_DeleteOnClose,true);
    fov = fovDefault();
    clipRatioFar = 5;
    clipRatioNear = clipRatioNearDefault();
    nearPlane = .2f;
    farPlane = 5.f;

    //if (mvcont != NULL)
    //    shared = mvcont->sharedDataContext();

    id = mvcont->getNextViewerId();

    updateMeshSetVisibilities();
    updateRasterSetVisibilities();
    setAutoFillBackground(false);

    //Raster support
    _isRaster =false;
    opacity = 0.5;
    zoom = false;
    targetTex = 0;

    connect(this->md(), SIGNAL(currentMeshChanged(int)), this, SLOT(manageCurrentMeshChange()),Qt::QueuedConnection);
    connect(this->md(), SIGNAL(meshDocumentModified()), this, SLOT(updateAllPerMeshDecorators()),Qt::QueuedConnection);
    connect(this->md(), SIGNAL(meshSetChanged()), this, SLOT(updateMeshSetVisibilities()));
    connect(this->md(), SIGNAL(rasterSetChanged()), this, SLOT(updateRasterSetVisibilities()));
    connect(this->md(),SIGNAL(documentUpdated()),this,SLOT(completeUpdateRequested()));
    connect(this, SIGNAL(updateLayerTable()), this->mw(), SIGNAL(updateLayerTable()));
    connect(md(),SIGNAL(meshRemoved(int)),this,SLOT(meshRemoved(int)));

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
	lastloadedraster = -1;
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
	makeCurrent();
    if(this->md()==0) return;

    char buf[4096];
    va_list marker;
    va_start( marker, f );

    vsprintf(buf,f,marker);
    va_end( marker );
    this->md()->Log.Log(Level,buf);
}

QSize GLArea::minimumSizeHint() const {return QSize(400,300);}
QSize GLArea::sizeHint() const				{return QSize(400,300);}


void GLArea::initializeGL()
{
	makeCurrent();
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
	//doneCurrent();
}

void GLArea::pasteTile()
{
    QString outfile;
	makeCurrent();
    glPushAttrib(GL_ENABLE_BIT);
	bool useAlfa = ss.background==1;
	QImage tileBuffer = grabFrameBuffer(useAlfa).mirrored(false, true);
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

        uchar *snapPtr = snapBuffer.bits() + (tileBuffer.bytesPerLine() * tileCol) + ((totalCols * tileRow) * tileBuffer.byteCount());
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
                    this->Logf(GLLogStream::SYSTEM, "Snapshot saved to %s",outfile.toLocal8Bit().constData());
                    if(ss.addToRasters)
                    {
						// get current transform, before is reset by the following importRaster
						Shotm shot_tmp = shotFromTrackball().first;
                        mw()->importRaster(outfile);

                        RasterModel *rastm = md()->rm();
						rastm->shot = shot_tmp;
                        float ratio=(float)rastm->currentPlane->image.height()/(float)rastm->shot.Intrinsics.ViewportPx[1];
                        rastm->shot.Intrinsics.ViewportPx[0]=rastm->currentPlane->image.width();
                        rastm->shot.Intrinsics.ViewportPx[1]=rastm->currentPlane->image.height();
                        rastm->shot.Intrinsics.PixelSizeMm[1]/=ratio;
                        rastm->shot.Intrinsics.PixelSizeMm[0]/=ratio;
                        rastm->shot.Intrinsics.CenterPx[0]= rastm->shot.Intrinsics.ViewportPx[0]/2.0;
                        rastm->shot.Intrinsics.CenterPx[1]= rastm->shot.Intrinsics.ViewportPx[1]/2.0;

						//importRaster has destroyed the original trackball state, now we restore it
						loadShot(QPair<Shotm, float>(shot_tmp, trackball.track.sca));
                    }
                }
                else
                {
                    Logf(GLLogStream::WARNING,"Error saving %s",outfile.toLocal8Bit().constData());
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
	makeCurrent();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-1,1,-1,1,-1,1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

	if (!takeSnapTile)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glColor(glas.backgroundTopColor);  glVertex2f(-1, 1);
		glColor(glas.backgroundBotColor);  glVertex2f(-1, -1);
		glColor(glas.backgroundTopColor);  glVertex2f(1, 1);
		glColor(glas.backgroundBotColor);  glVertex2f(1, -1);
		glEnd();
	}
	else
	{
		float xb = (-tileCol * 2) - 1;
		float yb = (-tileRow * 2) - 1;
		float xt = ((totalCols - tileCol) * 2) - 1;
		float yt = ((totalRows - tileRow) * 2) - 1;

		glBegin(GL_TRIANGLE_STRIP);
		glColor(glas.backgroundTopColor);  glVertex2f(xb, yt);
		glColor(glas.backgroundBotColor);  glVertex2f(xb, yb);
		glColor(glas.backgroundTopColor);  glVertex2f(xt, yt);
		glColor(glas.backgroundBotColor);  glVertex2f(xt, yb);
		glEnd();
	}

    glPopAttrib();
    glPopMatrix(); // restore modelview
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
	//doneCurrent();
}

void GLArea::drawLight()
{
	makeCurrent();
    // ============== LIGHT TRACKBALL ==============
    // Apply the trackball for the light direction
    glPushMatrix();
    trackball_light.GetView();
    trackball_light.Apply();

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
    if(!isDefaultTrackBall()) trackball_light.DrawPostApply();

}
int GLArea::RenderForSelection(int pickX, int pickY)
{
	makeCurrent();
    if (mvc() == NULL)
        return -1;

	MLSceneGLSharedDataContext* datacont = mvc()->sharedDataContext();
	if (datacont == NULL)
		return -1;

    int sz = int( md()->meshList.size())*5;
    GLuint *selectBuf =new GLuint[sz];
    glSelectBuffer(sz, selectBuf);
    glRenderMode(GL_SELECT);
    glInitNames();

    /* Because LoadName() won't work with no names on the stack */
    glPushName(-1);
    double mp[16];

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT,viewport);
    glPushAttrib(GL_TRANSFORM_BIT);
    glMatrixMode(GL_PROJECTION);
    glGetDoublev(GL_PROJECTION_MATRIX ,mp);
    glPushMatrix();
    glLoadIdentity();
    gluPickMatrix(pickX, pickY, 4, 4, viewport);
    glMultMatrixd(mp);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    /*if (shared->highPrecisionRendering())
        glTranslate(-shared->globalSceneCenter());*/

    foreach(MeshModel * mp, this->md()->meshList)
    {
        glLoadName(mp->id());
		
		datacont->setMeshTransformationMatrix(mp->id(), mp->cm.Tr);
		datacont->draw(mp->id(), context());
    }

    long hits;
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    hits = glRenderMode(GL_RENDER);
    glPopAttrib();

    std::vector< std::pair<double,unsigned int> > H;
    for(long ii=0;ii<hits;ii++){
        H.push_back( std::pair<double,unsigned int>(selectBuf[ii*4+1]/4294967295.0,selectBuf[ii*4+3]));
    }
    std::sort(H.begin(),H.end());
    delete [] selectBuf;
    if(hits==0) return -1;
    return H.front().second;
}

void GLArea::paintEvent(QPaintEvent* /*event*/)
{
    if (mvc() == NULL) 
        return;
    QPainter painter(this);
    painter.beginNativePainting();
#ifdef Q_OS_MAC
    glGetError();  // This seems required because on osx painter.beginNativePainting() trashes the err state of opengl
#endif    
    
    makeCurrent();

    if(!isValid()) 
        return;

    QTime time;
    time.start();

    /*if(!this->md()->isBusy())
    {
        initTexture(hasToUpdateTexture);
        hasToUpdateTexture=false;
    }*/

    glClearColor(1.0,1.0,1.0,0.0);
	if (takeSnapTile && (ss.background == 3))
		glClearColor(0.0, 0.0, 0.0, 0.0);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    setView();  // Set Modelview and Projection matrix
    if((!takeSnapTile) || (takeSnapTile && (ss.background==0)) )
        drawGradient();  // draws the background

    drawLight();

    glPushMatrix();
    // Finally apply the Trackball for the model
    trackball.GetView();
    trackball.Apply();

    glPushMatrix();

    if(!this->md()->isBusy())
    {
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        
        
        if ((iRenderer) && (parentmultiview != NULL))
        {
            MLSceneGLSharedDataContext* shared = parentmultiview->sharedDataContext();
            if (shared != NULL)
            {
                MLSceneGLSharedDataContext::PerMeshRenderingDataMap dt;
                shared->getRenderInfoPerMeshView(context(),dt);
        
                iRenderer->Render(currentShader, *this->md(),dt,this);

                MLDefaultMeshDecorators defdec(mw());
               
                foreach(MeshModel * mp, this->md()->meshList)
                {
                    if ((mp != NULL) && (meshVisibilityMap[mp->id()]))
                    {
                        QList<QAction *>& tmpset = iPerMeshDecoratorsListMap[mp->id()];
                        for( QList<QAction *>::iterator it = tmpset.begin(); it != tmpset.end();++it)
                        {
                            MeshDecorateInterface * decorInterface = qobject_cast<MeshDecorateInterface *>((*it)->parent());
                            decorInterface->decorateMesh(*it,*mp,this->glas.currentGlobalParamSet,this,&painter,md()->Log);
                        }
                        MLRenderingData meshdt;
                        shared->getRenderInfoPerMeshView(mp->id(),context(),meshdt);
                        defdec.decorateMesh(*mp,meshdt,&painter,md()->Log);
                    }
                }
            }
        }
        else
        {
            if(hasToSelectMesh) // right mouse click you have to select a mesh
            {
                int newId=RenderForSelection(pointToPick[0],pointToPick[1]);
                if(newId>=0)
                {
                    Logf(0,"Selected new Mesh %i",newId);
                    md()->setCurrentMesh(newId);
					if (mw() != NULL)
						mw()->updateLayerDialog();
                    //update();
                }
                hasToSelectMesh=false;
            }
            MLSceneGLSharedDataContext* datacont = mvc()->sharedDataContext();
            if (datacont == NULL)
                return;

            foreach(MeshModel * mp, this->md()->meshList)
            {
                if (meshVisibilityMap[mp->id()])
                {
                    MLRenderingData curr;
                    datacont->getRenderInfoPerMeshView(mp->id(),context(),curr);
                    MLPerViewGLOptions opts;
                    if (curr.get(opts) == false)
                        throw MLException(QString("GLArea: invalid MLPerViewGLOptions"));
                    setLightingColors(opts);
                    
                    
                    if(opts._back_face_cull)
                        glEnable(GL_CULL_FACE);
                    else
                        glDisable(GL_CULL_FACE);

                    datacont->setMeshTransformationMatrix(mp->id(),mp->cm.Tr);
					datacont->draw(mp->id(),context());
                }
            }
			foreach(MeshModel * mp, this->md()->meshList)
			{
				if (meshVisibilityMap[mp->id()])
				{
					MLRenderingData curr;
					MLDefaultMeshDecorators defdec(mw());
					datacont->getRenderInfoPerMeshView(mp->id(), context(), curr);
					defdec.decorateMesh(*mp, curr, &painter, md()->Log);

					QList<QAction *>& tmpset = iPerMeshDecoratorsListMap[mp->id()];
					for (QList<QAction *>::iterator it = tmpset.begin(); it != tmpset.end(); ++it)
					{
						MeshDecorateInterface * decorInterface = qobject_cast<MeshDecorateInterface *>((*it)->parent());
						decorInterface->decorateMesh(*it, *mp, this->glas.currentGlobalParamSet, this, &painter, md()->Log);
					}
				}
			}
        }
		if (iEdit) {
			iEdit->setLog(&md()->Log);
			iEdit->Decorate(*mm(), this, &painter);
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

	if (hasToPick && hasToGetPickCoords)
	{
		Point2f pp(pointToPick[0], pointToPick[1]);
		hasToPick = false;
		emit transmitPickedPos(nameToGetPickCoords, pp);
		hasToGetPickCoords = false;
	}

    foreach(QAction * p , iPerDocDecoratorlist)
    {
        MeshDecorateInterface * decorInterface = qobject_cast<MeshDecorateInterface *>(p->parent());
        decorInterface->decorateDoc(p,*this->md(),this->glas.currentGlobalParamSet, this,&painter,md()->Log);
    }

    // we want to write scene-space the point picked with double-click in the log
    // we have to do it now, before leaving this transformation space
    // we hook to the same mechanism double-click will be managed later on to move trackball
    if(hasToPick && !hasToGetPickPos)
    {
        Point3f pp;
        if(Pick<Point3f>(pointToPick[0],pointToPick[1],pp))
        {
            // write picked point in the log
            Logf(0,"Recentering on point [%f %f %f] [%d,%d]",pp[0],pp[1],pp[2],pointToPick[0],pointToPick[1]);
        }
    }

    glPopMatrix(); // We restore the state to immediately before the trackball
    //If it is a raster viewer draw the image as a texture
	if (isRaster())
	{
		if ((md()->rm() != NULL) && (lastloadedraster != md()->rm()->id()))
			loadRaster(md()->rm()->id());
		drawTarget();
	}

	// Double click move picked point to center
    // It has to be done in the before trackball space (we MOVE the trackball itself...)
    if(hasToPick && !hasToGetPickPos)
    {
        Point3f pp;
        hasToPick=false;
        if(Pick<Point3f>(pointToPick[0],pointToPick[1],pp))
        {
            trackball.MouseUp(pointToPick[0],pointToPick[1], vcg::Trackball::BUTTON_NONE );
            trackball.Translate(-pp);
            trackball.Scale(1.25f);

            QCursor::setPos(mapToGlobal(QPoint(width()/2+2,height()/2+2)));
        }
    }

    // ...and take a snapshot
    if (takeSnapTile) pasteTile();

    // Finally display HELP if requested
    if (isHelpVisible()) 
		displayHelp();

    //Draw highlight if it is the current viewer
    if(mvc()->currentId==id)
        displayViewerHighlight();

    QString error = checkGLError::makeString("There are gl errors: ");
    if(!error.isEmpty()) {
        Logf(GLLogStream::WARNING,qPrintable(error));
    }
    //check if viewers are linked
    MainWindow *window = qobject_cast<MainWindow *>(QApplication::activeWindow());
    if(window && window->linkViewersAct->isChecked() && mvc()->currentId==id)
        mvc()->updateTrackballInViewers();

	// Draw the log area background
		// on the bottom of the glArea
	if (infoAreaVisible)
	{
		glPushAttrib(GL_ENABLE_BIT);
		glDisable(GL_DEPTH_TEST);
		renderingFacilityString();
		displayInfo(&painter);
		displayRealTimeLog(&painter);
		updateFps(time.elapsed());
		glPopAttrib();
	}
	//doneCurrent();
	glFlush();
	glFinish();
    painter.endNativePainting();
}

void GLArea::displayMatrix(QPainter *painter, QRect areaRect)
{
	makeCurrent();
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
	makeCurrent();
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
    //mQMultiMap<QString,std::pair<QString,QString> >::const_iterator it = md()->Log.RealTimeLogText.constBegin();it != md()->Log.RealTimeLogText.constEnd();++it)
    foreach(QString keyIt, md()->Log.RealTimeLogText.uniqueKeys() )
    {
        QList< QPair<QString,QString> > valueList = md()->Log.RealTimeLogText.values(keyIt);
        QPair<QString,QString> itVal;
        // the map contains pairs of meshname, text
        // the meshname is used only to disambiguate when there are more than two boxes with the same title
        foreach(itVal,  valueList)
        {
            QString HeadName = keyIt;
            if(md()->Log.RealTimeLogText.count(keyIt)>1)
                HeadName += " - "+itVal.first;
            doc.clear();
            doc.setDocumentMargin(margin*0.75);
            QColor textColor = Qt::white;
            QColor headColor(200,200,200);
            doc.setHtml("<font color=\"" + headColor.name() + "\" size=\"+1\" ><p><i><b>" + HeadName + "</b></i></p></font>"
                                                                                                       "<font color=\"" + textColor.name() + "\"             >" + itVal.second + "</font>");
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
    }

    // After the rederaw we clear the RealTimeLog buffer!
    md()->Log.RealTimeLogText.clear();
    painter->restore();
    painter->beginNativePainting();
}

void GLArea::displayInfo(QPainter *painter)
{
	makeCurrent();
    if ((mvc() == NULL) || (md() == NULL))
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
    QFontMetrics metrics = QFontMetrics(qFont);
    int border = qMax(4, metrics.leading()) / 2;
    int numLines = 6;

    float barHeight = ((metrics.height() + metrics.leading())*numLines) + 2 * border;

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
            QLocale engLocale(QLocale::English, QLocale::UnitedStates);
            col1Text += QString("Mesh: %1\n").arg(mm()->label());
            col1Text += "Vertices: " + engLocale.toString(mm()->cm.vn) + " \n";
            col1Text += "Faces: " + engLocale.toString(mm()->cm.fn) + " \n";
        }
        else
        {
            QLocale engLocale(QLocale::English, QLocale::UnitedStates);
            QFileInfo inf = mm()->label();
			col1Text += QString("Current Mesh: %1\n").arg(inf.completeBaseName());
            col1Text += "Vertices: " + engLocale.toString(mm()->cm.vn) + "    (" + engLocale.toString(this->md()->vn()) + ") \n";
            col1Text += "Faces: " + engLocale.toString(mm()->cm.fn) + "    (" + engLocale.toString(this->md()->fn()) + ") \n";
        }
        
        
		int svn = 0;
		int sfn = 0;

		if (mm() != NULL)
		{
			svn = mm()->cm.svn;
			sfn = mm()->cm.sfn;
		}

        QLocale engLocale(QLocale::English, QLocale::UnitedStates);
		col1Text += "Selection: v: " + engLocale.toString(svn) + " f: " + engLocale.toString(sfn) + " \n";
                
        col1Text += GetMeshInfoString();

        if(fov>5) col0Text += QString("FOV: %1\n").arg(fov);
        else col0Text += QString("FOV: Ortho\n");
        if ((cfps>0) && (cfps<1999))
            col0Text += QString("FPS: %1\n").arg(cfps,7,'f',1);

		col0Text += renderfacility;

        if (clipRatioNear!=clipRatioNearDefault())
            col0Text += QString("\nClipping Near:%1\n").arg(clipRatioNear,7,'f',2);
        painter->drawText(Column_1, Qt::AlignLeft | Qt::TextWordWrap, col1Text);
        painter->drawText(Column_0, Qt::AlignLeft | Qt::TextWordWrap, col0Text);
        if(mm()->cm.Tr != Matrix44m::Identity() ) displayMatrix(painter, Column_2);
    }
    painter->restore();
    painter->beginNativePainting();
    //glPopAttrib();
}

void GLArea::renderingFacilityString()
{
	
	renderfacility.clear();
	makeCurrent();
	if (md()->size() > 0)
	{
		enum RenderingType { FULL_BO, MIXED, FULL_IMMEDIATE_MODE };
		RenderingType rendtype = FULL_IMMEDIATE_MODE;

		if (parentmultiview != NULL)
		{
			MLSceneGLSharedDataContext* shared = parentmultiview->sharedDataContext();
			if (shared != NULL)
			{
				int hh = 0;
				foreach(MeshModel* meshmod, md()->meshList)
				{
					if (shared->isBORenderingAvailable(meshmod->id()))
					{
						rendtype = MIXED;
						if ((rendtype == MIXED) && (hh == md()->meshList.size() - 1))
							rendtype = FULL_BO;
					}
					++hh;
				}
			}
		}

		switch (rendtype)
		{
			case(FULL_BO):
			{
				renderfacility += QString("BO_RENDERING");
				break;
			}
			case(MIXED):
			{
				renderfacility += QString("MIXED_RENDERING");
				break;
			}
			case(FULL_IMMEDIATE_MODE):
			{
				renderfacility += QString("IMMEDIATE_MODE_RENDERING");
				break;
			}
		}
	}
}

void GLArea::displayViewerHighlight()
{
	makeCurrent();
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


void GLArea::displayHelp()
{
	makeCurrent();
    static QString tableText;
    if(tableText.isEmpty())
    {
        QFile helpFile(":/images/onscreenHelp.txt");
        if(helpFile.open(QFile::ReadOnly))
            tableText=helpFile.readAll();
        else assert(0);
#ifdef Q_OS_MAC
        tableText.replace("Ctrl","Command");
#endif
    }
    md()->Log.RealTimeLog("Quick Help","",tableText);
}


void GLArea::saveSnapshot()
{
	makeCurrent();
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
			for (int tilenum = 0; tilenum < (ss.resolution*ss.resolution); tilenum++)
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
void GLArea::manageCurrentMeshChange()
{
    //if we have an edit tool open, notify it that the current layer has changed
    if(iEdit)
    {
        if(iEdit->isSingleMeshEdit())
            assert(lastModelEdited);  // if there is an editor that works on a single mesh
        // last model edited should always be set when start edit is called

        iEdit->LayerChanged(*this->md(), *lastModelEdited, this,parentmultiview->sharedDataContext());

        //now update the last model edited
        //TODO this is not the best design....   iEdit should maybe keep track of the model on its own
        lastModelEdited = this->md()->mm();
    }
    emit this->updateMainWindowMenus();
    // if the layer has changed update also the decoration.
    //	updateAllPerMeshDecorators();
}

///// Execute a end/start pair for all the PerMesh decorator that are active in this glarea.
///// It is used when the document is changed or when some parameter changes
///// Note that it is rather inefficient. Such work should be done only once for each decorator.
//void GLArea::updateAllPerMeshDecorators()
//{
//    MeshDocument* mdoc = md();
//    if (mdoc == NULL)
//        return;
//	makeCurrent();
//    
//    for (QMap<int, QList<QAction *> >::iterator i = iPerMeshDecoratorsListMap.begin(); i != iPerMeshDecoratorsListMap.end(); ++i )
//    {
//        
//        MeshModel *m = md()->getMesh(i.key());
//        foreach(QAction *p , i.value())
//        {
//            MeshDecorateInterface * decorInterface = qobject_cast<MeshDecorateInterface *>(p->parent());
//            decorInterface->endDecorate  (p, *m,this->glas.currentGlobalParamSet,this);
//            decorInterface->setLog(&md()->Log);
//            decorInterface->startDecorate(p,*m, this->glas.currentGlobalParamSet,this);
//        }
//    }
//
//    MultiViewer_Container* viewcont = mvc();
//    if (viewcont == NULL)
//        return;
//
//    MLSceneGLSharedDataContext* shared = viewcont->sharedDataContext();
//    if (shared == NULL)
//        return;
//
//    MLDefaultMeshDecorators defdec(mw());
//    for(MeshModel* mm = mdoc->nextMesh();mm != NULL;mm = mdoc->nextMesh(mm))
//    {
//        MLRenderingData dt;
//        shared->getRenderInfoPerMeshView(mm->id(),context(),dt);
//        defdec.cleanMeshDecorationData(*mm,dt);
//        defdec.initMeshDecorationData(*mm,dt);
//    }
//}


void GLArea::updateAllDecorators()
{
	updateAllPerMeshDecorators();
	if (md() == NULL)
		return;
	foreach(QAction * p, iPerDocDecoratorlist)
	{
		MeshDecorateInterface * decorInterface = qobject_cast<MeshDecorateInterface *>(p->parent());
		decorInterface->endDecorate(p, *md(), this->glas.currentGlobalParamSet, this);
		decorInterface->setLog(&md()->Log);
		decorInterface->startDecorate(p, *md(), this->glas.currentGlobalParamSet, this);
	}
	if (mvc() != NULL)
		mvc()->updateAllViewer();
}

void GLArea::setCurrentEditAction(QAction *editAction)
{
	if ((parentmultiview == NULL) || (parentmultiview->sharedDataContext() == NULL))
		return;

	makeCurrent();
    assert(editAction);
    currentEditor = editAction;

    iEdit = actionToMeshEditMap.value(currentEditor);
	if (iEdit == NULL)
		return;

    lastModelEdited = this->md()->mm();

	/*_oldvalues.clear();
	parentmultiview->sharedDataContext()->getRenderInfoPerMeshView(context(), _oldvalues);*/

	MLRenderingData dt;
	if (iEdit->isSingleMeshEdit())
	{
		if (md()->mm() != NULL)
		{
			parentmultiview->sharedDataContext()->getRenderInfoPerMeshView(md()->mm()->id(), context(), dt);
			iEdit->suggestedRenderingData(*(md()->mm()), dt);
			MLPoliciesStandAloneFunctions::disableRedundatRenderingDataAccordingToPriorities(dt);
			parentmultiview->sharedDataContext()->setRenderingDataPerMeshView(md()->mm()->id(), context(), dt);
			parentmultiview->sharedDataContext()->manageBuffers(md()->mm()->id());
		}
	}
	else
	{
		foreach(MeshModel* mm, md()->meshList)
		{
			if (mm != NULL)
			{
				parentmultiview->sharedDataContext()->getRenderInfoPerMeshView(mm->id(), context(), dt);
				iEdit->suggestedRenderingData(*(mm), dt);
				MLPoliciesStandAloneFunctions::disableRedundatRenderingDataAccordingToPriorities(dt);
				parentmultiview->sharedDataContext()->setRenderingDataPerMeshView(mm->id(), context(), dt);
				parentmultiview->sharedDataContext()->manageBuffers(mm->id());
			}
		}
	}
	if (mw() != NULL)
		mw()->updateLayerDialog();
    if (!iEdit->StartEdit(*this->md(), this,parentmultiview->sharedDataContext()))
    {
        //iEdit->EndEdit(*(this->md()->mm()), this);
        endEdit();
    }
    else
    {
        Logf(GLLogStream::SYSTEM,"Started Mode %s", qPrintable(currentEditor->text()));
		if(mm()!=NULL)
            mm()->meshModified() = true;
        else assert(!iEdit->isSingleMeshEdit());
		update();
    }
}


bool GLArea::readyToClose()
{
	makeCurrent();
    // Now do the actual closing of the glArea
	if (getCurrentEditAction())
	{
		endEdit();
		md()->meshDocStateData().clear();
	}
    if (iRenderer)
        iRenderer->Finalize(currentShader, this->md(), this);

    // Now manage the closing of the decorator set;
    foreach(QAction* act, iPerDocDecoratorlist)
    {
        MeshDecorateInterface* mdec = qobject_cast<MeshDecorateInterface*>(act->parent());
        mdec->endDecorate(act,*md(),glas.currentGlobalParamSet,this);
        mdec->setLog(NULL);
    }
    iPerDocDecoratorlist.clear();
    QSet<QAction *> dectobeclose;
    for(QMap<int, QList<QAction *> >::iterator it = iPerMeshDecoratorsListMap.begin();it != iPerMeshDecoratorsListMap.end();++it)
    {
        foreach(QAction* curract,it.value())
            dectobeclose.insert(curract);
    }

    for(QSet<QAction *>::iterator it = dectobeclose.begin();it != dectobeclose.end();++it)
    {
        MeshDecorateInterface* mdec = qobject_cast<MeshDecorateInterface*>((*it)->parent());
        if (mdec != NULL)
        {
            mdec->endDecorate(*it,*md(),glas.currentGlobalParamSet,this);
            mdec->setLog(NULL);
        }
    }
    dectobeclose.clear();
    iPerMeshDecoratorsListMap.clear();
    if(targetTex) glDeleteTextures(1, &targetTex);
    emit glareaClosed();
    return true;
}

void GLArea::keyReleaseEvent ( QKeyEvent * e )
{
	makeCurrent();
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
	makeCurrent();
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
	makeCurrent();
    e->accept();
	if (!this->hasFocus())
	{
		this->setFocus();
	}

	{
		if ((iEdit != NULL) && !suspendedEditor)
		{
			iEdit->mousePressEvent(e, *mm(), this);
		}
		else
		{
			if (e->button() == Qt::RightButton) // Select a new current mesh
			{
				hasToSelectMesh = true;
				this->pointToPick = Point2i(QT2VCG_X(this, e), QT2VCG_Y(this, e));
			}
			else
			{
				if ((e->modifiers() & Qt::ShiftModifier) &&
					(e->modifiers() & Qt::ControlModifier) &&
					(e->button() == Qt::LeftButton))
					activeDefaultTrackball = false;
				else activeDefaultTrackball = true;

				if (isDefaultTrackBall())
				{
					if (QApplication::keyboardModifiers() & Qt::Key_Control) trackball.ButtonDown(QT2VCG(Qt::NoButton, Qt::ControlModifier));
					else trackball.ButtonUp(QT2VCG(Qt::NoButton, Qt::ControlModifier));
					if (QApplication::keyboardModifiers() & Qt::Key_Shift) trackball.ButtonDown(QT2VCG(Qt::NoButton, Qt::ShiftModifier));
					else trackball.ButtonUp(QT2VCG(Qt::NoButton, Qt::ShiftModifier));
					if (QApplication::keyboardModifiers() & Qt::Key_Alt) trackball.ButtonDown(QT2VCG(Qt::NoButton, Qt::AltModifier));
					else trackball.ButtonUp(QT2VCG(Qt::NoButton, Qt::AltModifier));

					trackball.MouseDown(QT2VCG_X(this, e), QT2VCG_Y(this, e), QT2VCG(e->button(), e->modifiers()));
				}
				else trackball_light.MouseDown(QT2VCG_X(this, e), QT2VCG_Y(this, e), QT2VCG(e->button(), Qt::NoModifier));
			}
		}
	}
    update();
}

void GLArea::mouseMoveEvent(QMouseEvent*e)
{
	makeCurrent();
    if( (iEdit && !suspendedEditor) )
        iEdit->mouseMoveEvent(e,*mm(),this);
    else {
        if (isDefaultTrackBall())
        {
            trackball.MouseMove(QT2VCG_X(this,e), QT2VCG_Y(this,e));
            setCursorTrack(trackball.current_mode);
        }
        else trackball_light.MouseMove(QT2VCG_X(this,e), QT2VCG_Y(this,e));
    }
    update();
}

// When mouse is released we set the correct mouse cursor
void GLArea::mouseReleaseEvent(QMouseEvent*e)
{
	makeCurrent();
    //clearFocus();
    activeDefaultTrackball=true;
    if( (iEdit && !suspendedEditor) )
        iEdit->mouseReleaseEvent(e,*mm(),this);
    else {
        if (isDefaultTrackBall()) trackball.MouseUp(QT2VCG_X(this,e), QT2VCG_Y(this,e), QT2VCG(e->button(), e->modifiers() ) );
        else trackball_light.MouseUp(QT2VCG_X(this,e), QT2VCG_Y(this,e), QT2VCG(e->button(),e->modifiers()) );
        setCursorTrack(trackball.current_mode);
    }

    update();
}

//Processing of tablet events, interesting only for painting plugins
void GLArea::tabletEvent(QTabletEvent*e)
{
	makeCurrent();
    if(iEdit && !suspendedEditor) iEdit->tabletEvent(e,*mm(),this);
    else e->ignore();
}

void GLArea::wheelEvent(QWheelEvent*e)
{
	makeCurrent();
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
        //        case Qt::ControlModifier+Qt::ShiftModifier     : clipRatioFar  = math::Clamp( clipRatioFar*powf(1.1f, notch),0.01f,50000.0f); break;
        case Qt::ControlModifier: 
            {
                clipRatioNear = math::Clamp(clipRatioNear*powf(1.1f, notch),0.01f,500.0f); 
                break;
            }
        case Qt::ShiftModifier: 
            {
                fov = math::Clamp(fov+1.2f*notch,5.0f,90.0f); 
                break;
            }
        case Qt::AltModifier:
            { 
                glas.pointSize = math::Clamp(glas.pointSize*powf(1.2f, notch),0.01f, MLPerViewGLOptions::maxPointSize());
                MLSceneGLSharedDataContext* cont = mvc()->sharedDataContext();
                if (cont != NULL)
                {
					foreach(MeshModel * mp, this->md()->meshList)
					{
						MLRenderingData dt;
						cont->getRenderInfoPerMeshView(mp->id(), context(), dt);
						MLPerViewGLOptions opt;
						dt.get(opt);
						opt._perpoint_pointsize = glas.pointSize;
						opt._perpoint_pointsmooth_enabled = glas.pointSmooth;
						opt._perpoint_pointattenuation_enabled = glas.pointDistanceAttenuation;
						cont->setGLOptions(mp->id(), context(), opt);
					}
					if (mw() != NULL)
						mw()->updateLayerDialog();
                }
                break;
            }
        default:
            {            
                if(isRaster())
                    this->opacity = math::Clamp( opacity*powf(1.2f, notch),0.1f,1.0f);
                else
                    trackball.MouseWheel( e->delta()/ float(WHEEL_STEP));
                break;
            }
        }
    }
    update();
}


void GLArea::mouseDoubleClickEvent ( QMouseEvent * e )
{
	makeCurrent();
	if ((iEdit == NULL) || suspendedEditor)
	{
		hasToPick = true;
		pointToPick = Point2i(QT2VCG_X(this, e), QT2VCG_Y(this, e));
	}
    update();
}

void GLArea::focusInEvent ( QFocusEvent * e )
{
	makeCurrent();
    QWidget::focusInEvent(e);
    emit currentViewerChanged(id);
}

void GLArea::setCursorTrack(vcg::TrackMode *tm)
{
	makeCurrent();
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

void GLArea::setDecorator(QString name, bool state)
{
	makeCurrent();
    updateDecorator(name, false, state);
}

void GLArea::toggleDecorator(QString name)
{
	makeCurrent();
    updateDecorator(name, true, false);
}


void GLArea::updateDecorator(QString name, bool toggle, bool stateToSet)
{
	makeCurrent();
    MeshDecorateInterface *iDecorateTemp = this->mw()->PM.getDecoratorInterfaceByName(name);
    QAction *action = iDecorateTemp->action(name);

    if(iDecorateTemp->getDecorationClass(action)== MeshDecorateInterface::PerDocument)
    {
        bool found=this->iPerDocDecoratorlist.removeOne(action);
        if(found)
        {
            if(toggle || stateToSet==false){
                iDecorateTemp->endDecorate(action,*md(),glas.currentGlobalParamSet,this);
                iDecorateTemp->setLog(NULL);
                this->Logf(GLLogStream::SYSTEM,"Disabled Decorate mode %s",qPrintable(action->text()));
            } else
                this->Logf(GLLogStream::SYSTEM,"Trying to disable an already disabled Decorate mode %s",qPrintable(action->text()));
        }
        else{
            if(toggle || stateToSet==true){
                iDecorateTemp->setLog(&(this->md()->Log));
                bool ret = iDecorateTemp->startDecorate(action,*md(), glas.currentGlobalParamSet, this);
                if(ret) {
                    this->iPerDocDecoratorlist.push_back(action);
                    this->Logf(GLLogStream::SYSTEM,"Enabled Decorate mode %s",qPrintable(action->text()));
                }
                else this->Logf(GLLogStream::SYSTEM,"Failed start of Decorate mode %s",qPrintable(action->text()));
            } else
                this->Logf(GLLogStream::SYSTEM,"Trying to enable an already enabled Decorate mode %s",qPrintable(action->text()));
        }
    }

    if(iDecorateTemp->getDecorationClass(action)== MeshDecorateInterface::PerMesh)
    {
        MeshModel &currentMeshModel = *mm();
        bool found=this->iCurPerMeshDecoratorList().removeOne(action);
        if(found)
        {
            if(toggle || stateToSet==false){
                iDecorateTemp->endDecorate(action,currentMeshModel,glas.currentGlobalParamSet,this);
                iDecorateTemp->setLog(NULL);
                this->Logf(0,"Disabled Decorate mode %s",qPrintable(action->text()));
            } else
                this->Logf(GLLogStream::SYSTEM,"Trying to disable an already disabled Decorate mode %s",qPrintable(action->text()));
        }
        else{
            if(toggle || stateToSet==true){
                QString errorMessage;
                if (iDecorateTemp->isDecorationApplicable(action,currentMeshModel,errorMessage)) 
                {
                    iDecorateTemp->setLog(&md()->Log);
                    bool ret = iDecorateTemp->startDecorate(action,currentMeshModel, glas.currentGlobalParamSet, this);
                    if(ret) {
                        this->iCurPerMeshDecoratorList().push_back(action);
                        this->Logf(GLLogStream::SYSTEM,"Enabled Decorate mode %s",qPrintable(action->text()));
                    }
                    else this->Logf(GLLogStream::SYSTEM,"Failed Decorate mode %s",qPrintable(action->text()));
                } else
                    this->Logf(GLLogStream::SYSTEM,"Error in Decorate mode %s: %s",qPrintable(action->text()),qPrintable(errorMessage));

            }
        }
    }
}

//void GLArea::setDrawMode(vcg::GLW::DrawMode mode)
//{
//	makeCurrent();
//	for(QMap<int,RenderMode>::iterator it = rendermodemap.begin();it != rendermodemap.end();++it)
//		it.value().drawMode = mode;
//	update();
//}
//
//void GLArea::setDrawMode(RenderMode& rm,vcg::GLW::DrawMode mode )
//{
//	makeCurrent();
//    rm.drawMode = mode;
//    update();
//}
//
//void GLArea::setColorMode(vcg::GLW::ColorMode mode)
//{
//	makeCurrent();
//    for(QMap<int,RenderMode>::iterator it = rendermodemap.begin();it != rendermodemap.end();++it)
//        it.value().colorMode = mode;
//    update();
//}
//
//void GLArea::setColorMode( RenderMode& rm,vcg::GLW::ColorMode mode )
//{
//	makeCurrent();
//    rm.colorMode = mode;
//    update();
//}
//
//void GLArea::setTextureMode(vcg::GLW::TextureMode mode)
//{
//	makeCurrent();
//    for(QMap<int,RenderMode>::iterator it = rendermodemap.begin();it != rendermodemap.end();++it)
//        it.value().textureMode = mode;
//    update();
//}
//
//void GLArea::setTextureMode(RenderMode& rm,vcg::GLW::TextureMode mode)
//{
//	makeCurrent();
//    rm.textureMode = mode;
//    update();
//}
//
//void GLArea::setLight(bool setState)
//{
//	makeCurrent();
//    for(QMap<int,RenderMode>::iterator it = rendermodemap.begin();it != rendermodemap.end();++it)
//        it.value().lighting = setState;
//    update();
//}
//
//void GLArea::setLightMode(bool state,LightingModel lmode)
//{
//	makeCurrent();
//    for(QMap<int,RenderMode>::iterator it = rendermodemap.begin();it != rendermodemap.end();++it)
//    {
//        switch(lmode)
//        {
//        case LDOUBLE:		it.value().doubleSideLighting = state;	break;
//        case LFANCY:		it.value().fancyLighting = state; break;
//        }
//    }
//    update();
//}
//
//void GLArea::setBackFaceCulling(bool enabled)
//{
//	makeCurrent();
//    for(QMap<int,RenderMode>::iterator it = rendermodemap.begin();it != rendermodemap.end();++it)
//        it.value().backFaceCull = enabled;
//    update();
//}
//
void GLArea::setLightingColors(const MLPerViewGLOptions& opts)
{
    makeCurrent();
    if (opts._double_side_lighting || opts._fancy_lighting)
        glEnable(GL_LIGHT1);
    else
        glDisable(GL_LIGHT1);

	//WARNING!!!!! ALL THIS DATA SHOULD BE MOVED INSIDE THE OPTS....IN THE NEXT MESHLAB RELEASE....MAYBE.....
    glLightfv(GL_LIGHT0, GL_AMBIENT, Color4f::Construct(glas.baseLightAmbientColor).V());
    glLightfv(GL_LIGHT0, GL_DIFFUSE, Color4f::Construct(glas.baseLightDiffuseColor).V());
    glLightfv(GL_LIGHT0, GL_SPECULAR,Color4f::Construct(glas.baseLightSpecularColor).V());

    glLightfv(GL_LIGHT1, GL_AMBIENT, Color4f::Construct(glas.baseLightAmbientColor).V());
    glLightfv(GL_LIGHT1, GL_DIFFUSE, Color4f::Construct(glas.baseLightDiffuseColor).V());
    glLightfv(GL_LIGHT1, GL_SPECULAR,Color4f::Construct(glas.baseLightSpecularColor).V());
    if(opts._fancy_lighting)
    {
        glLightfv(GL_LIGHT0, GL_DIFFUSE, Color4f::Construct(glas.fancyFLightDiffuseColor).V());
        glLightfv(GL_LIGHT1, GL_DIFFUSE, Color4f::Construct(glas.fancyBLightDiffuseColor).V());
    }
	//*********************************************************************************************************
}

void GLArea::setView()
{
	makeCurrent();
    glViewport(0,0, (GLsizei) QTLogicalToDevice(this,width()),(GLsizei) QTLogicalToDevice(this,height()));

    GLfloat fAspect = (GLfloat)width()/height();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    Matrix44f mtTr; mtTr.SetTranslate( trackball.center);
    Matrix44f mtSc; mtSc.SetScale(4.0f,4.0f,4.0f);
    Matrix44f mt = mtSc * mtTr * trackball.Matrix() *(-mtTr);
    //    Matrix44f mt =  trackball.Matrix();

    Box3m bb;
    bb.Add(Matrix44m::Construct(mt),this->md()->bbox());
    float cameraDist = this->getCameraDistance();

    if(fov<=5) cameraDist = 8.0f; // small hack for orthographic projection where camera distance is rather meaningless...

    nearPlane = cameraDist*clipRatioNear;
    farPlane = cameraDist + max(viewRatio(),float(-bb.min[2]));
    if(nearPlane<=cameraDist*.01f) nearPlane=cameraDist*.01f;

    //    qDebug("tbcenter %f %f %f",trackball.center[0],trackball.center[1],trackball.center[2]);
    //    qDebug("camera dist %f far  %f",cameraDist, farPlane);
    //    qDebug("Bb %f %f %f - %f %f %f", bb.min[0], bb.min[1], bb.min[2], bb.max[0], bb.max[1], bb.max[2]);

    if (!takeSnapTile)
    {
        if(fov==5)	glOrtho( -viewRatio()*fAspect, viewRatio()*fAspect, -viewRatio(), viewRatio(),  nearPlane, farPlane);
        else		gluPerspective(fov, fAspect, nearPlane, farPlane);
    }
    else	setTiledView(fov, viewRatio(), fAspect, nearPlane, farPlane, cameraDist);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, cameraDist,0, 0, 0, 0, 1, 0);
}

void GLArea::setTiledView(GLdouble fovY, float viewRatio, float fAspect, GLdouble zNear, GLdouble zFar,  float /*cameraDist*/)
{
	makeCurrent();
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
                zNear, zFar);
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
    const int avgSize =10;
    static float fpsVector[avgSize];
    static int j=0;
    float averageFps=0;
    if (deltaTime>0) {
        fpsVector[j]=deltaTime;
        j=(j+1) % avgSize;
    }
    for (int i=0;i<avgSize;i++) averageFps+=fpsVector[i];
    cfps=1000.0f/(averageFps/avgSize);
    lastTime=deltaTime;
}

void GLArea::resetTrackBall()
{
	makeCurrent();
    trackball.Reset();
    float newScale= 3.0f/this->md()->bbox().Diag();
    trackball.track.sca = newScale;
    trackball.track.tra.Import(-this->md()->bbox().Center());
    clipRatioNear = clipRatioNearDefault();
	if (!isRaster())
		fov=fovDefault();
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

void GLArea::sendPickedPos(QString name)
{
	qDebug("sendPickedPos %s", qPrintable(name));
	nameToGetPickCoords = name;
	hasToGetPickCoords = true;
}

void GLArea::sendViewDir(QString name)
{
    Point3f dir= getViewDir();
    emit transmitViewDir(name,dir);
}

void GLArea::sendMeshShot(QString name)
{
    Shotm curShot=this->md()->mm()->cm.shot;
    emit transmitShot(name, curShot);
}

void GLArea::sendMeshMatrix(QString name)
{
    Matrix44m mat=this->md()->mm()->cm.Tr;
    emit transmitMatrix(name, mat);
}

void GLArea::sendViewerShot(QString name)
{
    Shotm curShot=shotFromTrackball().first;
    emit transmitShot(name, curShot);
}
void GLArea::sendRasterShot(QString name)
{
    Shotm curShot;
    if (this->md()->rm() != NULL)
        curShot = this->md()->rm()->shot;
    emit transmitShot(name, curShot);
}

void GLArea::sendCameraPos( QString name )
{
	Point3f pos = trackball.camera.ViewPoint();
	emit transmitCameraPos(name, pos);
}

void GLArea::sendTrackballPos(QString name)
{
	Point3f pos = -trackball.track.tra;
	emit transmitTrackballPos(name, pos);
}

Point3f GLArea::getViewDir()
{
    vcg::Matrix44f rotM;
    trackball.track.rot.ToMatrix(rotM);
    return vcg::Inverse(rotM)*vcg::Point3f(0,0,1);
}

void GLArea::updateCustomSettingValues( RichParameterSet& rps )
{
	makeCurrent();
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

//void GLArea::getPerDocGlobalRenderingData(MLRenderingData& dt) const
//{
//	dt = _perdocglobaldt;
//}
//
//void GLArea::setPerDocGlobalRenderingData(const MLRenderingData& dt)
//{
//	_perdocglobaldt = dt;
//}

// --------------- Raster view -------------------------------------
void GLArea::setIsRaster(bool viewMode){
    _isRaster= viewMode;
}

// this slot is called when someone press the showraster button on the toolbar
void GLArea::showRaster(bool resetViewFlag)
{
	makeCurrent();
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
	lastloadedraster = id;
    foreach(RasterModel *rm, this->md()->rasterList)
        if(rm->id()==id)
        {
            this->md()->setCurrentRaster(id);
            if (rm->currentPlane->image.isNull())
            {
                Logf(0,"Image file %s has not been correctly loaded, a fake image is going to be shown.",rm->currentPlane->fullPathFileName.toUtf8().constData());
                rm->currentPlane->image.load(":/images/dummy.png");
            }
            setTarget(rm->currentPlane->image);
            //load his shot or a default shot

            if (rm->shot.IsValid())
            {
				fov = (rm->shot.Intrinsics.cameraType == 0) ? rm->shot.GetFovFromFocal() : 5.0;

                float cameraDist = getCameraDistance();
                Matrix44f rotFrom;
                rm->shot.Extrinsics.Rot().ToMatrix(rotFrom);

				// this code seems useless, and if the camera translation is[0 0 0] (or even just with a small z), there is a division by zero
                //Point3f p1 = rotFrom*(vcg::Point3f::Construct(rm->shot.Extrinsics.Tra()));
				//Point3f p2 = (Point3f(0,0,cameraDist));
				//trackball.track.sca =fabs(p2.Z()/p1.Z());

                loadShot(QPair<Shotm, float> (rm->shot,trackball.track.sca));
            }
            else
                createOrthoView("Front");
        }
}

void GLArea::drawTarget()
{
	makeCurrent();
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
	makeCurrent();
    if (image.isNull())
        return;
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
    float cameraDist = viewRatio() / tanf(vcg::math::ToRad(fov*.5f));
    return cameraDist;
}

void GLArea::initializeShot(Shotm &shot)
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
    shot.Intrinsics.FocalMm = viewportYMm/(2*tanf(vcg::math::ToRad(this->fovDefault()/2.0f))); //27.846098mm

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
    Shotm shot;

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
	fov = (shot.Intrinsics.cameraType == 0) ? shot.GetFovFromFocal() : 5.0;

    float cameraDist = getCameraDistance();

    Matrix44f rotFrom;
    shot.Extrinsics.Rot().ToMatrix(rotFrom);

	// this code seems useless, and if the camera translation is[0 0 0] (or even just with a small z), there is a division by zero
	//Point3f p1 = rotFrom*(vcg::Point3f::Construct(rm->shot.Extrinsics.Tra()));
	//Point3f p2 = (Point3f(0,0,cameraDist));
	//trackball.track.sca =fabs(p2.Z()/p1.Z());

    loadShot(QPair<Shotm, float> (shot,trackball.track.sca));

}

/*
ViewState file is an xml file format created by Meshlab with the action "copyToClipboard"
*/
void GLArea::loadViewFromViewStateFile(const QDomDocument &doc)
{
    Shotm shot;
    QDomElement root = doc.documentElement();
    QDomNode node = root.firstChild();

    while(!node.isNull())
    {
        if (QString::compare(node.nodeName(),"VCGCamera")==0)
            ReadShotFromQDomNode(shot,node);
        else if (QString::compare(node.nodeName(),"CamParam")==0)
            ReadShotFromOLDXML(shot,node);

        else if (QString::compare(node.nodeName(),"ViewSettings")==0)
        {
            QDomNamedNodeMap attr = node.attributes();
            trackball.track.sca = attr.namedItem("TrackScale").nodeValue().section(' ',0,0).toFloat();
            nearPlane = attr.namedItem("NearPlane").nodeValue().section(' ',0,0).toFloat();
            farPlane = attr.namedItem("FarPlane").nodeValue().section(' ',0,0).toFloat();
			fov = (shot.Intrinsics.cameraType == 0) ? shot.GetFovFromFocal() : 5.0;
            clipRatioNear = nearPlane/getCameraDistance();
            clipRatioFar = farPlane/getCameraDistance();
        }
        /*else if (QString::compare(node.nodeName(),"Render")==0)
        {
        QDomNamedNodeMap attr = node.attributes();
        rm.drawMode = (vcg::GLW::DrawMode) (attr.namedItem("DrawMode").nodeValue().section(' ',0,0).toInt());
        rm.colorMode = (vcg::GLW::ColorMode) (attr.namedItem("ColorMode").nodeValue().section(' ',0,0).toInt());
        rm.textureMode = (vcg::GLW::TextureMode) (attr.namedItem("TextureMode").nodeValue().section(' ',0,0).toInt());
        rm.lighting = (attr.namedItem("Lighting").nodeValue().section(' ',0,0).toInt() != 0);
        rm.backFaceCull = (attr.namedItem("BackFaceCull").nodeValue().section(' ',0,0).toInt() != 0);
        rm.doubleSideLighting = (attr.namedItem("DoubleSideLighting").nodeValue().section(' ',0,0).toInt() != 0);
        rm.fancyLighting = (attr.namedItem("FancyLighting").nodeValue().section(' ',0,0).toInt() != 0);
        }*/
        node = node.nextSibling();
    }

    loadShot(QPair<Shotm, float> (shot,trackball.track.sca));
}
QString GLArea::viewToText()
{
    QString docString;

    Shotm shot = shotFromTrackball().first;

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

    /*QDomElement renderElem = doc.createElement( "Render");
    renderElem.setAttribute("DrawMode",rm.drawMode);
    renderElem.setAttribute("ColorMode",rm.colorMode);
    renderElem.setAttribute("TextureMode",rm.textureMode);
    renderElem.setAttribute("Lighting",rm.lighting);
    renderElem.setAttribute("BackFaceCull",rm.backFaceCull);
    renderElem.setAttribute("DoubleSideLighting",rm.doubleSideLighting);
    renderElem.setAttribute("FancyLighting",rm.fancyLighting);
    root.appendChild(renderElem);*/

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

QPair<Shotm,float> GLArea::shotFromTrackball()
{
    Shotm shot;
    initializeShot(shot);

    double viewportYMm=shot.Intrinsics.PixelSizeMm[1]*shot.Intrinsics.ViewportPx[1];
    shot.Intrinsics.FocalMm = viewportYMm/(2*tanf(vcg::math::ToRad(fov/2)));

	// in MeshLab, fov < 5.0 means orthographic camera
	if (fov > 5.0)
		shot.Intrinsics.cameraType = 0; //perspective
	else
		shot.Intrinsics.cameraType = 1; //orthographic

    float cameraDist = getCameraDistance();

    //add the translation introduced by gluLookAt() (0,0,cameraDist), in order to have te same view---------------
    //T(gl)*S*R*T(t) => SR(gl+t) => S R (S^(-1)R^(-1)gl + t)
    //Add translation S^(-1) R^(-1)(gl)
    //Shotd doesn't introduce scaling
    //---------------------------------------------------------------------
    shot.Extrinsics.SetTra( shot.Extrinsics.Tra() + (Inverse(shot.Extrinsics.Rot())*Point3m(0, 0, cameraDist)));

    Shotm newShot = track2ShotCPU<Shotm::ScalarType>(shot, &trackball);

    return QPair<Shotm, float> (newShot,trackball.track.sca);
}
void GLArea::viewFromCurrentShot(QString kind)
{
    Shotm localShot;
    if(kind=="Mesh" && this->md()->mm())   localShot = this->md()->mm()->cm.shot;
    if(kind=="Raster" && this->md()->rm()) localShot = this->md()->rm()->shot;
    if(!localShot.IsValid())
    {
        this->Logf(GLLogStream::SYSTEM, "Unable to set Shot from current %s",qPrintable(kind));
        return;
    }

    loadShot(QPair<Shotm, float>(localShot,trackball.track.sca));
}


void GLArea::loadShot(const QPair<Shotm,float> &shotAndScale){

    Shotm shot = shotAndScale.first;

	fov = (shot.Intrinsics.cameraType == 0) ? shot.GetFovFromFocal() : 5.0;

    float cameraDist = getCameraDistance();

    //reset trackball. The point of view must be set only by the shot
    trackball.Reset();
    trackball.track.sca = shotAndScale.second;

    shot2Track(shot, cameraDist, trackball);

	/*Point3f point = this->md()->bbox().Center();
	Point3f p1 = ((trackball.track.Matrix()*(point-trackball.center))- Point3f(0,0,cameraDist));*/
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
    Shotm view;
    initializeShot(view);

    fov =5;
    double viewportYMm = view.Intrinsics.PixelSizeMm[1]*view.Intrinsics.ViewportPx[1];
    view.Intrinsics.FocalMm = viewportYMm/(2*tanf(vcg::math::ToRad(fov/2))); //27.846098 equivalente a circa 60 gradi

    trackball.Reset();
    float newScale= 3.0f/this->md()->bbox().Diag();
    trackball.track.sca = newScale;
    trackball.track.tra.Import(-this->md()->bbox().Center());

    Matrix44m rot;

    if(dir == tr("Top"))
        rot.SetRotateDeg(90,Point3m(1,0,0));
    else if(dir == tr("Bottom"))
        rot.SetRotateDeg(90,Point3m(-1,0,0));
    else if(dir == tr("Left"))
        rot.SetRotateDeg(90,Point3m(0,1,0));
    else if(dir == tr("Right"))
        rot.SetRotateDeg(90,Point3m(0,-1,0));
    else if(dir == tr("Front"))
        rot.SetRotateDeg(0,Point3m(0,1,0));
    else if(dir == tr("Back"))
        rot.SetRotateDeg(180,Point3m(0,1,0));
	// scene uses "engineering" reference system, with Z as vertical axis
	else if (dir == tr("Top (Z is up)"))
		rot.SetRotateDeg(0, Point3m(1, 0, 0));
	else if (dir == tr("Bottom (Z is up)"))
		rot.SetRotateDeg(180, Point3m(1, 0, 0));
	else if (dir == tr("Left (Z is up)"))
		rot = Matrix44m().SetRotateDeg(90, Point3m(0, 1, 0)) * Matrix44m().SetRotateDeg(90, Point3m(-1, 0, 0));
	else if (dir == tr("Right (Z is up)"))
		rot = Matrix44m().SetRotateDeg(90, Point3m(0, -1, 0)) * Matrix44m().SetRotateDeg(90, Point3m(-1, 0, 0));
	else if (dir == tr("Front (Z is up)"))
		rot.SetRotateDeg(90, Point3m(-1, 0, 0));
	else if (dir == tr("Back (Z is up)"))
		rot = Matrix44m().SetRotateDeg(90, Point3m(1, 0, 0)) * Matrix44m().SetRotateDeg(180, Point3m(0, 1, 0));

    view.Extrinsics.SetRot(rot);

    float cameraDist = getCameraDistance();

    //add the translation introduced by gluLookAt() (0,0,cameraDist), in order to have te same view---------------
    //T(gl)*S*R*T(t) => SR(gl+t) => S R (S^(-1)R^(-1)gl + t)
    //Add translation S^(-1) R^(-1)(gl)
    //Shotd doesn't introduce scaling
    //---------------------------------------------------------------------
    view.Extrinsics.SetTra( view.Extrinsics.Tra() + (Inverse(view.Extrinsics.Rot())*Point3m(0, 0, cameraDist)));

    Shotm shot = track2ShotCPU(view, &trackball);

    QPair<Shotm,float> shotAndScale = QPair<Shotm,float> (shot, trackball.track.sca);
    loadShot(shotAndScale);

	this->Logf(GLLogStream::SYSTEM, "View scene from %s", qPrintable(dir));
}

void GLArea::toggleOrtho()
{
	if (fov == 5.0)
		fov = 35.0;
	else
		fov = 5.0;

	update();
}

void GLArea::trackballStep(QString dir)
{
	float stepAngle = M_PI / 12.0;

	if (dir == tr("Horizontal +"))
		trackball.track.rot = Quaternionf(-stepAngle, Point3f(0.0, 1.0, 0.0)) * trackball.track.rot;
	else if (dir == tr("Horizontal -"))
		trackball.track.rot = Quaternionf( stepAngle, Point3f(0.0, 1.0, 0.0)) * trackball.track.rot;
	else if (dir == tr("Vertical +"))
		trackball.track.rot = Quaternionf(-stepAngle, Point3f(1.0, 0.0, 0.0)) * trackball.track.rot;
	else if (dir == tr("Vertical -"))
		trackball.track.rot = Quaternionf( stepAngle, Point3f(1.0, 0.0, 0.0)) * trackball.track.rot;
	else if (dir == tr("Axial +"))
		trackball.track.rot = Quaternionf(-stepAngle, Point3f(0.0, 0.0, 1.0)) * trackball.track.rot;
	else if (dir == tr("Axial -"))
		trackball.track.rot = Quaternionf( stepAngle, Point3f(0.0, 0.0, 1.0)) * trackball.track.rot;

	update();
}


//MultiViewer_Container * GLArea::mvc()
//{
//	QObject * curParent = this->parent();
//	while(qobject_cast<MultiViewer_Container *>(curParent) == 0)
//	{
//		if (curParent != NULL)
//			curParent = curParent->parent();
//		else
//			return NULL;
//	}
//	return qobject_cast<MultiViewer_Container *>(curParent);
//}


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
    makeCurrent();
    if ((this->md() != NULL) && (this->md()->rm() != NULL))
        loadRaster(this->md()->rm()->id());
    //if (md()->mm() != NULL)
    //	trackball.center = md()->mm()->cm.bbox.Center();
    update();
    //doneCurrent();
}

//RenderMode* GLArea::getCurrentRenderMode()
//{
//	if ((md() != NULL) && (md()->mm() != NULL))
//	{
//		QMap<int,RenderMode>::iterator it = rendermodemap.find(md()->mm()->id());
//		if (it != rendermodemap.end())
//			return &it.value();
//	}
//	return NULL;
//}

void GLArea::meshAdded( int /*index*/)
{
    emit updateLayerTable();
}

void GLArea::meshRemoved( int /*index*/ )
{
    emit updateLayerTable();
}

void GLArea::setupTextureEnv( GLuint textid )
{
    makeCurrent();
    glPushAttrib(GL_ENABLE_BIT);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,textid);
    if(glas.textureMagFilter == 0 ) 	
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    else	
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    if(glas.textureMinFilter == 0 ) 	
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    else	
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glBindTexture(GL_TEXTURE_2D,0);
    glPopAttrib();
}

MainWindow * GLArea::mw()
{
    QObject * curParent = parent();
    while(qobject_cast<MainWindow *>(curParent) == 0)
    {
        curParent = curParent->parent();
    }
    return qobject_cast<MainWindow *>(curParent);
}

