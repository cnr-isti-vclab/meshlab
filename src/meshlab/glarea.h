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
Revision 1.51  2006/01/25 03:57:15  glvertex
- Code cleaning and restyling
- Some bugs removed on resizing
- A lot of changes in paintGL

Revision 1.50  2006/01/25 00:56:51  alemochi
Added trackball to change directional lighting

Revision 1.49  2006/01/24 17:19:36  alemochi
Added help on screen (key F1)

Revision 1.48  2006/01/23 08:56:49  fmazzant
added GetMeshInfoString(mask meshmodel).
This member shows the information of the Mesh in terms of VC,VQ,FC,FQ,WT
where:
VC = VertColor,VQ = VertQuality,FC = FaceColor,FQ = FaceQuality,WT = WedgTexCoord

Revision 1.47  2006/01/17 16:35:27  glvertex
Added Scalable fonts

Revision 1.46  2006/01/13 10:12:59  alemochi
Added control to fov and changed behavior

Revision 1.45  2006/01/12 22:14:39  alemochi
added vertigo effect and moveable near clip plane


****************************************************************************/
#ifndef GLAREA_H
#define GLAREA_H
#include <wrap/gl/trimesh.h>
#include <wrap/gui/trackball.h>
#include <QTimer>
#include <QWidget>
#include <QGLWidget>
#include <QTime>
#include <QtGui>
#include <vcg/space/point3.h>
#include <vcg/space/plane3.h>
#include <vcg/space/line3.h>
#include <vcg/math/matrix44.h>
#include <vcg/math/matrix44.h>
#include <wrap/gl/math.h>
//#include <../wrap/gl/math.h>

#include "GLLogStream.h"

#include "meshmodel.h"
#include "interfaces.h"

#define SSHOT_BYTES_PER_PIXEL 4

enum LightingModel{LDOUBLE,LFANCY};

class RenderMode
{
public:
  	vcg::GLW::DrawMode	drawMode;
  	vcg::GLW::ColorMode	colorMode;
  	vcg::GLW::TextureMode	textureMode;

    bool lighting;
    bool backFaceCull;
    bool doubleSideLighting;  
    bool fancyLighting;
    bool castShadow;
    vcg::Point3f lightDir;
		
		
		RenderMode()
		{
      drawMode	= GLW::DMSmooth;
			colorMode = GLW::CMNone;
			textureMode = GLW::TMNone;

			lighting = true;
			backFaceCull = false;
			doubleSideLighting = false;
			fancyLighting = false;
			castShadow = false;
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
	
	enum ButtonPressed { BUTTON_NONE   = 0x0000, 
		WHEEL         = 0x0008,
		KEY_SHIFT     = 0x0010, 
		KEY_SHIFTWHEEL= 0x0018, 
		KEY_CTRL      = 0x0020, 
		KEY_ALT       = 0x0040, 
		};

	MeshModel *mm;
	vcg::Trackball trackball;
	vcg::Trackball trackball_light;
	GLLogStream log;
	short currLogLevel;

	int currentWidth;
	int currentHeight;
	QSize minimumSizeHint() const;
	QSize sizeHint() const;

	QAction *getLastAppliedFilter()							{return lastFilterRef;}
	void		setLastAppliedFilter(QAction *qa)		{lastFilterRef = qa;}

	QString getFileName()							{return fileName;}
	void		setFileName(QString name)	{fileName = name;}

	short		getLogLevel()												{return currLogLevel;}
	void		setLogLevel(short lvl)	{currLogLevel = lvl;}
	
	RenderMode &  getCurrentRenderMode()		{return rm;}
	const ColorSetting& getCustomSetting()		const {return cs;}
	const SnapshotSetting& getSnapshotSetting()	{/*ss.dx=vpWidth; ss.dy=vpHeight;*/ return ss;}
	void updateFps();
	void renderFps();
	void renderHelpOnScreen();
	void showLog(bool b)			{logVisible = b; updateGL();}
	void showInfoArea(bool b)		{infoAreaVisible = b; updateGL();}
	void showTrackBall(bool b)		{trackBallVisible = b; updateGL();}
	bool isLogVisible()				{return logVisible;}
	bool isHelpVisible()      {return helpVisible;}  
	bool isInfoAreaVisible()		{return infoAreaVisible;}
	bool isTrackBallVisible()		{return trackBallVisible;}
	bool isDefaultTrackBall()   {return activeDefaultTrackball;}

	void setBackFaceCulling(bool enabled);
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

	void setRenderer(MeshRenderInterface *rend, QAction *shader){	iRenderer = rend; currentSharder = shader;}
	MeshRenderInterface * getRenderer() { return iRenderer; }

	void setEdit(MeshEditInterface *edit){	iEdit = edit; }

protected:
	void initializeGL();
	void initTexture();
	void displayModelInfo();
	QString GetMeshInfoString(int mask);
	void paintGL();
	void resizeGL(int width, int height);
	void keyPressEvent ( QKeyEvent * e );  
	void keyReleaseEvent ( QKeyEvent * e ); 
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent*e);
	
private:
	void pasteTile();
	void myGluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);
    
	bool	logVisible;				  // Prints out log infos ?
	bool  helpVisible;				// Help on screen	
	bool	infoAreaVisible;		// Draws the lower info area ?
	bool	trackBallVisible;		// Draws the trackball ?
	bool  activeDefaultTrackball; // keep track on active trackball
	
	//shader support
	MeshRenderInterface *iRenderer;
	QAction *currentSharder;
	QAction *lastFilterRef; // reference to last filter applied
	QFont	qFont;			//font settings


	// Editing support
	MeshEditInterface *iEdit;

	RenderMode rm;
	ColorSetting cs;
	int currentButton;
	float cfps;
	float fov;
	float objDist;
	float clipRatioFar;
	float clipRatioNear;
	float nearPlane;
	float farPlane;
  QTime time;
	int deltaTime;
	int lastTime;
	int currentTime;
	float fpsVector[10];

	QString fileName;
	
	SnapshotSetting ss;
	QImage snapBuffer;
	QImage tileBuffer;
	bool takeSnapTile;
	int tileCol, tileRow, totalCols, totalRows;
	int vcgFlag;
};


#endif
