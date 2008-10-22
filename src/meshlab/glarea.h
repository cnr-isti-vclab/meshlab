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

$Log: glarea.h,v $
Revision 1.85  2008/04/22 14:54:38  bernabei
Added support for tablet events

Revision 1.84  2008/04/04 10:07:10  cignoni
Solved namespace ambiguities caused by the removal of a silly 'using namespace' in meshmodel.h

Revision 1.83  2008/01/16 01:41:01  cignoni
added endedit slot

Revision 1.82  2008/01/04 18:23:24  cignoni
Corrected a wrong type (glwidget instead of glarea) in the decoration callback.

Revision 1.81  2008/01/04 00:46:28  cignoni
Changed the decoration framework. Now it accept a, global, parameter set. Added static calls for finding important directories in a OS independent way.

Revision 1.80  2007/12/13 00:18:28  cignoni
added meshCreation class of filter, and the corresponding menu new under file

Revision 1.79  2007/11/05 22:38:57  cignoni
Remove static map of cursors that caused the annoying deallocation bug under vs2005

Revision 1.78  2007/10/23 07:15:13  cignoni
switch to selection rendering done by slot and signals

Revision 1.77  2007/10/17 08:31:39  cignoni
better label and automatic suspension after the end of the measurement using slots

Revision 1.76  2007/10/06 23:29:51  cignoni
corrected management of suspeneded editing actions. Added filter toolbar

Revision 1.75  2007/07/10 07:19:12  cignoni
** Serious Changes **
again on the MeshDocument, the management of multiple meshes, layers, and per mesh transformation

Revision 1.74  2007/05/16 15:02:04  cignoni
Better management of toggling between edit actions and camera movement

Revision 1.73  2007/04/16 09:24:37  cignoni
** big change **
Added Layers managemnt.
Interfaces are changing...

Revision 1.72  2007/03/26 08:24:10  zifnab1974
When a user minimizes the window using a shortcut that uses modifiers (alt, ctrl, shift), the state of the button remained "pressed" after the window was reraised. Added a hideevent which resets the button state.

Revision 1.71  2007/03/09 11:08:09  mariolatronico
Removed unnecessary modifier

Revision 1.70  2007/03/05 11:12:55  cignoni
correct management of release of keyboard modifiers

Revision 1.69  2007/03/03 02:03:25  cignoni
Reformatted lower bar, added number of selected faces. Updated about dialog

Revision 1.68  2007/03/03 00:13:48  cignoni
quick patch of font size

Revision 1.67  2007/02/26 15:05:49  cignoni
syntax error

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

class LayerDialog;

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
	
	vcg::Color4b bColorBottom;
	vcg::Color4b bColorTop;
	vcg::Color4b lColor;

	ColorSetting()
	{
		bColorBottom=vcg::Color4b(64,64,128,1);	
		bColorTop=vcg::Color4b(0,0,0,1);
		lColor=vcg::Color4b(128,16,16,1);

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
	~GLArea();

private:
public:
	LayerDialog *layerDialog;
  // Layer Management stuff. 
	MeshDocument meshDoc;
	MeshModel *mm(){return meshDoc.mm();}
 
	vcg::Trackball trackball;
	vcg::Trackball trackball_light;
	GLLogStream log;
	short currLogLevel;
    FilterScript filterHistory;

	QSize curSiz;
	QSize minimumSizeHint() const;
	QSize sizeHint() const;
	//QFont getFont() {return qFont;}

	QAction *getLastAppliedFilter()							{return lastFilterRef;}
	void		setLastAppliedFilter(QAction *qa)		{lastFilterRef = qa;}

	QString getFileName()							{return QString(mm()->fileName.c_str());}
	void		setFileName(QString name)	
    {
    mm()->fileName = qPrintable(name); 
		setWindowTitle(QFileInfo(name).fileName()+tr("[*]"));
					
    ss.basename=QFileInfo(getFileName()).baseName().append("Snap");
	}

	short		getLogLevel()												{return currLogLevel;}
	void		setLogLevel(short lvl)	{currLogLevel = lvl;}
	
	RenderMode &  getCurrentRenderMode()		{return rm;}
	const ColorSetting& getCustomSetting()		const {return cs;}
	GLLightSetting getLightSettings(){return ls;}
	void setLightSettings(GLLightSetting glls){ls = glls;}
	const SnapshotSetting& getSnapshotSetting()	{/*ss.dx=vpWidth; ss.dy=vpHeight;*/ return ss;}
	void updateFps(float deltaTime);
	
	void showTrackBall(bool b)		{trackBallVisible = b; updateGL();}
	bool isHelpVisible()      {return helpVisible;}  
	bool isTrackBallVisible()		{return trackBallVisible;}
	bool isDefaultTrackBall()   {return activeDefaultTrackball;}

	void toggleHelpVisible()      {helpVisible = !helpVisible; update();}  
	void setBackFaceCulling(bool enabled);
	void setCustomSetting(const ColorSetting & s);
	void setSnapshotSetting(const SnapshotSetting & s);
	void setLight(bool state);
	void setLightMode(bool state,LightingModel lmode);
	void saveSnapshot();
	void setLightModel();
	void setView();
	void resetTrackBall();
	std::list<std::pair<QAction *,FilterParameterSet *> > *iDecoratorsList;

	void setRenderer(MeshRenderInterface *rend, QAction *shader){	iRenderer = rend; currentShader = shader;}
	MeshRenderInterface * getRenderer() { return iRenderer; }

  // Edit Mode management
	// In the glArea we can have a active Editor that can toggled into a ''suspendeed'' state 
	// in which the mouse event are redirected back to the GLArea to drive the camera trackball
	// The decorate function of the current active editor is still called.
  // EndEdit is called only when you press again the same button or when you change editor.
	  
public slots:
	void updateTexture(); // slot for forcing the texture reload. 
		
	//slots for changing the draw rendering and texturing mode
	void setDrawMode(vcg::GLW::DrawMode mode); 
	void setColorMode(vcg::GLW::ColorMode mode);
	void setTextureMode(vcg::GLW::TextureMode mode);
	
	void endEdit(){	
		if(iEdit && currentEditor) 
		{
			iEdit->EndEdit(*mm(),this);
		}
		iEdit= 0; 
		currentEditor=0; 
		setCursorTrack(0); 
		update(); 
	} 

	void setSelectionRendering(bool enabled);

  void suspendEditToggle()
		{	
			if(currentEditor==0) return;
			static QCursor qc;
			if(suspendedEditor) {
							suspendedEditor=false; 
							setCursor(qc); 
			}	else {
							suspendedEditor=true;
							qc=cursor(); 
							setCursorTrack(0); 						
			}
		}
  
signals:
	void updateMainWindowMenus(); //updates the menus of the meshlab MainWindow

  
public slots:

	// Called when we change layer, notifies the edit tool if one is open
	void setCurrentlyActiveLayer(int meshId);
	
public:
	
	//call when the editor changes
	void setCurrentEditAction(QAction *editAction);
	
	//get the currently active edit action
	QAction * getCurrentEditAction() { return currentEditor; }
	
	//get the currently active mesh editor
	MeshEditInterface * getCurrentMeshEditor() { return iEdit; }
	
	//see if this glAarea has a MESHEditInterface for this action
	bool editorExistsForAction(QAction *editAction){ return actionToMeshEditMap.contains(editAction); }
	
	//add a MeshEditInterface for the given action
	void addMeshEditor(QAction *editAction, MeshEditInterface *editor){ actionToMeshEditMap.insert(editAction, editor); }
	
	void closeEvent(QCloseEvent *event);
	float lastRenderingTime() { return lastTime;}

	float getFov() { return fov; }
// the following pair of slot/signal implements a very simple message passing mechanism.
// a widget that has a pointer to the glarea call the sendViewDir() slot and 
// setup a connect to recive the transmitViewDir signal that actually contains the point3f.
// See the Point3fWidget code.
signals :
		void transmitViewDir(vcg::Point3f dir);
public slots:
		void sendViewDir() {
			vcg::Point3f dir= getViewDir();
			emit transmitViewDir(dir);
		}
public:
	vcg::Point3f getViewDir() {
		vcg::Matrix44f rotM; 
		trackball.track.rot.ToMatrix(rotM); 
		//vcg::Matrix44f::Invert(rotM);
		vcg::Invert(rotM);
		return rotM*vcg::Point3f(0,0,1);
	}

	bool	infoAreaVisible;		// Draws the lower info area ?
	bool  suspendedEditor;
protected:

	void initializeGL();
	void initTexture();
	void displayInfo();
	void displayHelp();

	QString GetMeshInfoString();
	void paintGL();
	void resizeGL(int width, int height);
	void keyReleaseEvent ( QKeyEvent * e ); 
	void keyPressEvent ( QKeyEvent * e ); 
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseDoubleClickEvent ( QMouseEvent * event ) ;
	void wheelEvent(QWheelEvent*e);
	void tabletEvent(QTabletEvent *e);
	bool drawSelection;

	void hideEvent(QHideEvent * event);

private:
	QMap<QString,QCursor> curMap;
	void pasteTile();
	void myGluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);
    
	bool  helpVisible;				// Help on screen	
	bool	trackBallVisible;		// Draws the trackball ?
	bool  activeDefaultTrackball; // keep track on active trackball
	bool	hasToUpdateTexture;			// has to reload textures at the next redraw
	bool  hasToPick;							// has to pick during the next redraw.
	vcg::Point2i pointToPick;

	//shader support
	MeshRenderInterface *iRenderer;
	QAction *currentShader;
	QAction *lastFilterRef; // reference to last filter applied
	QFont	qFont;			//font settings

	// Editing support
	MeshEditInterface *iEdit;
	QAction *currentEditor;
	QAction *suspendedEditRef; // reference to last Editing Mode Used 
	QMap<QAction*, MeshEditInterface*> actionToMeshEditMap;

public:
	RenderMode rm;
	ColorSetting cs;
	GLLightSetting ls;
	// view setting variables
	float fov;
	float ratio;
	//float objDist; 
	float clipRatioFar;
	float clipRatioNear;
	float nearPlane;
	float farPlane;
  float pointSize;
private:
	float cfps;
	float lastTime;
	
	SnapshotSetting ss;
	QImage snapBuffer;
	QImage tileBuffer;
	bool takeSnapTile;
  
	enum AnimMode { AnimNone, AnimSpin, AnimInterp};
	AnimMode animMode; 
	int tileCol, tileRow, totalCols, totalRows;
	void setCursorTrack(vcg::TrackMode *tm);

	
};


#endif
