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
Revision 1.32  2005/12/05 12:16:46  ggangemi
iRendersList -> iDecoratorsList

Revision 1.31  2005/12/05 10:27:39  vannini
Snapshot in png format instead of ppm

Revision 1.30  2005/12/04 22:19:48  alemochi
Added in Info Pane number of triangles and vertices

Revision 1.29  2005/12/04 17:47:18  davide_portelli
Added menu windows->Next and Shortcut "CTRL+PageDown"
Added reset trackbal Shortcut "CTRL+H"
Optimize fullscreen

Revision 1.28  2005/12/04 10:43:45  glvertex
Some cleanup and renaming

Revision 1.27  2005/12/03 17:04:34  glvertex
Added backface culling action and slots
Added shortcuts for fancy and double lighting

Revision 1.26  2005/12/03 16:26:45  vannini
New code for snapshot and tiled rendering (disabled by default because not fully functional)

Revision 1.25  2005/12/03 16:05:18  glvertex
solved some compatilbility issues

Revision 1.24  2005/12/02 15:30:36  alemochi
Changed fps, added a control

Revision 1.23  2005/12/02 13:51:43  alemochi
Changed fps (problem with initialization fps)

Revision 1.22  2005/12/02 11:57:59  glvertex
- show log
- show info area
- show trackball
- some renaming

Revision 1.21  2005/12/02 00:52:10  cignoni
Added support for textures

Revision 1.20  2005/12/01 17:20:48  vannini
Added basic tiled rendering functions
saveSnapshot saves a 4x resolution snapshot

Revision 1.19  2005/12/01 03:27:33  glvertex
- Cleaned lighting code
- Minimum size now is (400,300)
- Added rendering log area on the bottom of the screen
- Solved zBuff conflicts during rendering texts

Revision 1.18  2005/11/30 16:26:56  cignoni
All the modification, restructuring seen during the 30/12 lesson...

Revision 1.17  2005/11/30 00:43:19  alemochi
FPS modified (not work correctly)

Revision 1.16  2005/11/30 00:21:07  alemochi
Added function to display fps

Revision 1.15  2005/11/29 18:32:56  alemochi
Added customize menu to change colors of environment

Revision 1.14  2005/11/29 11:22:23  vannini
Added experimental snapshot saving function

Revision 1.13  2005/11/28 21:05:37  alemochi
Added menu preferences and configurable background

Revision 1.12  2005/11/28 01:06:04  davide_portelli
Now GLA contains a list of RenderMode, instead of a single RenderMode.
Thus it is possible to have more active RenderMode (MeshRenderInterface)
at the same time, and when there are many opened windows, the menù of rendering
is consisting.

Revision 1.11  2005/11/27 03:50:58  glvertex
- Added method setColorMode
- Now getCurrentRenderMode is inline method

Revision 1.10  2005/11/26 16:53:54  glvertex
getRenderState --> getCurrentRenderMode

Revision 1.9  2005/11/26 14:09:15  alemochi
Added double side lighting and fancy lighting (working only double side+fancy)

Revision 1.8  2005/11/25 11:55:59  alemochi
Added function to Enable/Disable lighting (work in progress)

Revision 1.7  2005/11/24 01:38:36  cignoni
Added new plugins intefaces, tested with shownormal render mode

Revision 1.6  2005/11/20 19:30:50  glvertex
- Added lighting parameters (still working on...)
- Added logging events

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
#ifndef GLAREA_H
#define GLAREA_H
#include <wrap/gl/trimesh.h>
#include <wrap/gui/trackball.h>
#include <QTimer>
#include <QWidget>
#include <QGLWidget>
#include <QTime>

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

    bool Lighting;
    bool BackFaceCull;
    bool DoubleSideLighting;  
    bool FancyLighting;
    bool CastShadow;
    vcg::Point3f LightDir;
		
		
		RenderMode()
		{
      drawMode	= GLW::DMSmooth;
			colorMode = GLW::CMNone;
			textureMode = GLW::TMNone;

			Lighting = true;
			BackFaceCull = false;
			DoubleSideLighting = false;
			FancyLighting = false;
			CastShadow = false;
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

class MeshModel;
class GLArea : public QGLWidget
{
	Q_OBJECT


public:
	GLArea(QWidget *parent = 0);
	~GLArea(){}
	
	MeshModel *mm;
	vcg::Trackball trackball;
	GLLogStream log;

	int currentWidth;
	int currentHeight;
	QSize minimumSizeHint() const;
	QSize sizeHint() const;
	
	RenderMode &  getCurrentRenderMode()	{return rm;}
	const ColorSetting& getCustomSetting()			const {return cs;}
	void updateFps();
	void renderFps();
	
	void showLog(bool b)			{logVisible = b; updateGL();}
	void showInfoArea(bool b)		{infoAreaVisible = b; updateGL();}
	void showTrackBall(bool b)		{trackBallVisible = b; updateGL();}
	bool isLogVisible()				{return logVisible;}
	bool isInfoAreaVisible()		{return infoAreaVisible;}
	bool isTrackBallVisible()		{return trackBallVisible;}

	void setBackFaceCulling(bool enabled);
	void setCustomSetting(const ColorSetting & s);
	void setDrawMode(vcg::GLW::DrawMode mode);
	void setColorMode(vcg::GLW::ColorMode mode);
	void setTextureMode(vcg::GLW::TextureMode mode);
	void setLight(bool state);
	void setLightMode(bool state,LightingModel lmode);
	bool saveSnapshot(QString path);
	void SetLightModel();
	void resetTrackBall();
	list<pair<QAction *,MeshDecorateInterface *> > *iDecoratorsList;

protected:
	void initializeGL();
	void initTexture();
	void DisplayModelInfo();
	void paintGL();
	void resizeGL(int width, int height);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent*e);
	
private:
	bool pasteTile();
	void myGluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);
    
	bool	logVisible;				// Prints out log infos ?
	bool	infoAreaVisible;		// Draws the lower info area ?
	bool	trackBallVisible;		// Draws the trackball ?
	RenderMode rm;
	ColorSetting cs;
	float cfps;
	QTime time;
	int deltaTime;
	int lastTime;
	int currentTime;
	float fpsVector[10];
	
	char *snapBuffer;
	char *tileBuffer;
	bool takeSnapTile;
	int vpWidth, vpHeight, tileCol, tileRow, totalCols, totalRows;

};


#endif
