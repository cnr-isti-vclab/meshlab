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
Revision 1.66  2007/02/26 11:57:19  cignoni
Re enabled on screen help, moved back far plane

Revision 1.65  2007/02/26 01:20:59  cignoni
cursor added

Revision 1.64  2007/02/05 19:25:05  gfrei
added float getFov()

Revision 1.63  2006/11/09 08:15:59  cignoni
Restored classical dark background

Revision 1.62  2006/11/08 15:40:10  cignoni
Restored original checca colors

Revision 1.61  2006/10/26 12:06:16  corsini
add GLlightSettings structure

Revision 1.60  2006/10/10 19:55:02  cignoni
Corrected trackball bug, changed default background color.

Revision 1.59  2006/06/13 13:50:01  cignoni
Cleaned FPS management

Revision 1.58  2006/06/12 15:18:36  cignoni
toggle between last editing mode

Revision 1.57  2006/05/25 04:57:45  cignoni
Major 0.7 release. A lot of things changed. Colorize interface gone away, Editing and selection start to work.
Optional data really working. Clustering decimation totally rewrote. History start to work. Filters organized in classes.

Revision 1.56  2006/02/16 10:09:34  cignoni
Removed unnecessary stuff (modifiers)

Revision 1.55  2006/02/13 15:37:18  cignoni
Restructured some functions (pasteTile, wheelevent,lightmode)
Added DoubleClick for zoom and center. Restructured all the keyboard modifier (removed currentButton)
Removed various gl state leaking

Revision 1.54  2006/02/03 15:58:21  glvertex
Added getFont() inline method

Revision 1.53  2006/01/27 12:41:21  glvertex
Removed HUGE memory leaks. The model is now deallocated when the window is colsed.

Revision 1.52  2006/01/25 15:38:10  glvertex
- Restyling part II
- Font resizing works better
- Some renaming

Revision 1.51  2006/01/25 03:57:15  glvertex
- Code cleaning and restyling
- Some bugs removed on resizing
- A lot of changes in paintGL
****************************************************************************/
#ifndef GLAREA_H
#define GLAREA_H
#include <GL/glew.h>
#include <QTimer>
#include <QGLWidget>
#include <QTime>
#include <QtGui>
#include <vcg/space/plane3.h>
#include <vcg/space/line3.h>
#include <vcg/math/matrix44.h>
#include <wrap/gl/math.h>
#include <wrap/gl/trimesh.h>
#include <wrap/gui/trackball.h>

#include "GLLogStream.h"
#include "meshmodel.h"
#include "interfaces.h"
#include "filterscript.h"

#define SSHOT_BYTES_PER_PIXEL 4

enum LightingModel{LDOUBLE,LFANCY};

class GLLightSetting
{
public:

	GLfloat ambient[4];
	GLfloat diffuse[4];
	GLfloat specular[4];

	GLfloat ambientFancyBack[4];
	GLfloat diffuseFancyBack[4];
	GLfloat specularFancyBack[4];

	GLfloat ambientFancyFront[4];
	GLfloat diffuseFancyFront[4];
	GLfloat specularFancyFront[4];

	GLLightSetting()
	{
		// defaults
		ambient[0] = 0.2f; ambient[1] = 0.2f; ambient[2] = 0.2f; ambient[3] = 1.0f;
		diffuse[0] = 0.8f; diffuse[1] = 0.8f; diffuse[2] = 0.8f; diffuse[3] = 1.0f;
		specular[0] = 1.0f; specular[1] = 1.0f; specular[2] = 1.0f; specular[3] = 1.0f;

		ambientFancyBack[0] = 0.0f;
		ambientFancyBack[1] = 0.0f;
		ambientFancyBack[2] = 0.0f;
		ambientFancyBack[3] = 1.0f;

		diffuseFancyBack[0] = 1.0f;
		diffuseFancyBack[1] = 0.7f;
		diffuseFancyBack[2] = 0.7f;
		diffuseFancyBack[3] = 1.0f;

		specularFancyBack[0] = 1.0f;
		specularFancyBack[1] = 1.0f;
		specularFancyBack[2] = 1.0f;
		specularFancyBack[3] = 1.0f;

		ambientFancyFront[0] = 0.0f;
		ambientFancyFront[1] = 0.0f;
		ambientFancyFront[2] = 0.0f;
		ambientFancyFront[3] = 1.0f;

		diffuseFancyFront[0] = 0.7f;
		diffuseFancyFront[1] = 0.7f;
		diffuseFancyFront[2] = 1.0f;
		diffuseFancyFront[3] = 1.0f;

		specularFancyFront[0] = 1.0f;
		specularFancyFront[1] = 1.0f;
		specularFancyFront[2] = 1.0f;
		specularFancyFront[3] = 1.0f;
	}
};

class ColorSetting
{
public:
	
	Color4b bColorBottom;
	Color4b bColorTop;
	Color4b lColor;

	ColorSetting()
	{
		bColorBottom=Color4b(64,64,128,1);	
		bColorTop=Color4b(0,0,0,1);
		lColor=Color4b(128,16,16,1);

    //bColorBottom=Color4b(192,192,192,1);	
		//bColorTop=Color4b(255,255,255,1);
		//lColor=Color4b(128,16,16,1);
	};
};

class SnapshotSetting
{
public:
	QString outdir;
	QString basename;
	int counter;
	int resolution;
		
	SnapshotSetting()
	{
		outdir=".";
		basename="snapshot";
		counter=0;
		resolution=1;
	};
};

class MeshModel;
class GLArea : public QGLWidget
{
	Q_OBJECT


public:
	GLArea(QWidget *parent = 0);
	~GLArea(){}
	
	MeshModel *mm;
	vcg::Trackball trackball;
	vcg::Trackball trackball_light;
	GLLogStream log;
	short currLogLevel;
    FilterScript filterHistory;

	QSize curSiz;
	QSize minimumSizeHint() const;
	QSize sizeHint() const;
	QFont getFont() {return qFont;}

	QAction *getLastAppliedFilter()							{return lastFilterRef;}
	QAction *getLastAppliedEdit()							{return lastEditRef;}
	void		setLastAppliedFilter(QAction *qa)		{lastFilterRef = qa;}
	void		setLastAppliedEdit(QAction *qa)		  {lastEditRef = qa;}

	QString getFileName()							{return fileName;}
	void		setFileName(QString name)	
    {
    fileName = name; 
    ss.basename=QFileInfo(fileName).baseName().append("Snap");
	}

	short		getLogLevel()												{return currLogLevel;}
	void		setLogLevel(short lvl)	{currLogLevel = lvl;}
	
	RenderMode &  getCurrentRenderMode()		{return rm;}
	const ColorSetting& getCustomSetting()		const {return cs;}
	GLLightSetting getLightSettings(){return ls;}
	void setLightSettings(GLLightSetting glls){ls = glls;}
	const SnapshotSetting& getSnapshotSetting()	{/*ss.dx=vpWidth; ss.dy=vpHeight;*/ return ss;}
	void updateFps(float deltaTime);
	
	void showLog(bool b)			{logVisible = b; updateGL();}
	void showInfoArea(bool b)		{infoAreaVisible = b; updateGL();}
	void showTrackBall(bool b)		{trackBallVisible = b; updateGL();}
	bool isLogVisible()				{return logVisible;}
	bool isHelpVisible()      {return helpVisible;}  
	bool isInfoAreaVisible()		{return infoAreaVisible;}
	bool isTrackBallVisible()		{return trackBallVisible;}
	bool isDefaultTrackBall()   {return activeDefaultTrackball;}

	bool toggleHelpVisible()      {helpVisible = !helpVisible; update();}  
	void setBackFaceCulling(bool enabled);
	void setSelectionRendering(bool enabled);
	void setCustomSetting(const ColorSetting & s);
	void setSnapshotSetting(const SnapshotSetting & s);
	void setDrawMode(vcg::GLW::DrawMode mode);
	void setColorMode(vcg::GLW::ColorMode mode);
	void setTextureMode(vcg::GLW::TextureMode mode);
	void setLight(bool state);
	void setLightMode(bool state,LightingModel lmode);
	void saveSnapshot();
	void setLightModel();
	void setView();
	void resetTrackBall();
	list<pair<QAction *,MeshDecorateInterface *> > *iDecoratorsList;

	void setRenderer(MeshRenderInterface *rend, QAction *shader){	iRenderer = rend; currentShader = shader;}
	MeshRenderInterface * getRenderer() { return iRenderer; }

	void setEdit(MeshEditInterface *edit, QAction *editor){	iEdit = edit; currentEditor=editor;}
	QAction * getEditAction() { return currentEditor; }
	void endEdit(){	iEdit = 0; currentEditor=0; setCursorTrack(0);update();}///

	void closeEvent(QCloseEvent *event);
    float lastRenderingTime() { return lastTime;}

	float getFov() { return fov; }

protected:

	void initializeGL();
	void initTexture();
	void displayMeshInfo();
	void displayEnvInfo();
	void displayHelp();

	QString GetMeshInfoString(int mask);
	void paintGL();
	void resizeGL(int width, int height);
	void keyReleaseEvent ( QKeyEvent * e ); 
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
  void mouseDoubleClickEvent ( QMouseEvent * event ) ;
	void wheelEvent(QWheelEvent*e);
	bool drawSelection;

private:

	void pasteTile();
	void myGluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);
    
	bool	logVisible;				  // Prints out log infos ?
	bool  helpVisible;				// Help on screen	
	bool	infoAreaVisible;		// Draws the lower info area ?
	bool	trackBallVisible;		// Draws the trackball ?
	bool  activeDefaultTrackball; // keep track on active trackball
	bool  hasToPick;         // has to pick during the next redraw.
	Point2i pointToPick;

	//shader support
	MeshRenderInterface *iRenderer;
	QAction *currentShader;
	QAction *lastFilterRef; // reference to last filter applied
	QAction *lastEditRef; // reference to last Editing Mode Used 
	QFont	qFont;			//font settings

	// Editing support
	MeshEditInterface *iEdit;
	QAction *currentEditor;

	RenderMode rm;
	ColorSetting cs;
	GLLightSetting ls;
	float fov;
	//float objDist;
	float clipRatioFar;
	float clipRatioNear;
	float nearPlane;
	float farPlane;

private:
	float cfps;
    float lastTime;

	QString fileName;
	
	SnapshotSetting ss;
	QImage snapBuffer;
	QImage tileBuffer;
	bool takeSnapTile;
  
  enum AnimMode { AnimNone, AnimSpin, AnimInterp};
  AnimMode animMode; 
	int tileCol, tileRow, totalCols, totalRows;
	void GLArea::setCursorTrack(vcg::TrackMode *tm);

	
};


#endif
