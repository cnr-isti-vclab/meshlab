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

#include "../common/interfaces.h"
#include "../common/filterscript.h"
#include "layerDialog.h"
#include "glarea_setting.h"

#define SSHOT_BYTES_PER_PIXEL 4

enum LightingModel{LDOUBLE,LFANCY};


class SnapshotSetting
{
public:
	QString outdir;
	QString basename;
	int counter;
	int resolution;
  bool transparentBackground;
		
	SnapshotSetting()
	{
		outdir=".";
		basename="snapshot";
		counter=0;
		resolution=1;
    transparentBackground=true;
	};
};

class MeshModel;
class GLArea : public QGLWidget
{
	Q_OBJECT

public:
    GLArea(QWidget *parent, RichParameterSet *current);
	~GLArea();
	static void initGlobalParameterSet( RichParameterSet * /*globalparam*/);
private:


public:
	LayerDialog *layerDialog;
  // Layer Management stuff. 
	MeshDocument meshDoc;
	MeshModel *mm(){return meshDoc.mm();}
 
	vcg::Trackball trackball;
	vcg::Trackball trackball_light;
	GLLogStream log;
	FilterScript filterHistory;
    GLAreaSetting glas;
	QSize curSiz;
	QSize minimumSizeHint() const;
	QSize sizeHint() const;

	QAction *getLastAppliedFilter()							{return lastFilterRef;}
	void		setLastAppliedFilter(QAction *qa)		{lastFilterRef = qa;}

    QString getFileName()							{return QString(mm()->shortName());}
	void		setFileName(QString name)	
    {
    mm()->setFileName(name);
		setWindowTitle(QFileInfo(name).fileName()+tr("[*]"));
		layerDialog->setWindowTitle("Layer Dialog for "+QFileInfo(name).fileName());			
	  
    ss.basename=QFileInfo(getFileName()).baseName().append("Snap");
	}
	
	RenderMode &  getCurrentRenderMode()		{return rm;}

	void updateFps(float deltaTime);
	
	void showTrackBall(bool b)		{trackBallVisible = b; updateGL();}
	bool isHelpVisible()      {return helpVisible;}  
	bool isTrackBallVisible()		{return trackBallVisible;}
	bool isDefaultTrackBall()   {return activeDefaultTrackball;}

	void toggleHelpVisible()      {helpVisible = !helpVisible; update();}  
	void setBackFaceCulling(bool enabled);
	void setLight(bool state);
	void setLightMode(bool state,LightingModel lmode);
	void saveSnapshot();
	void setLightModel();
	void setView();
    void resetTrackBall();
	std::list<QAction *> iDecoratorsList;

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
	void updateCustomSettingValues(RichParameterSet& rps);

	void endEdit(){	
		if(iEdit && currentEditor) 
		{
			iEdit->EndEdit(*mm(),this);
		}
		iEdit= 0; 
		currentEditor=0; 
		setCursorTrack(0); 
		update(); 
		emit updateMainWindowMenus();
	} 

  void setSelectFaceRendering(bool enabled);
  void setSelectVertRendering(bool enabled);

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
	void glareaClosed();					//someone has closed the glarea
  
public slots:

	// Called when we change layer, notifies the edit tool if one is open
    void updateLayer();
	
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
	void drawGradient();
	void drawLight();
	float getFov() { return fov; }
	
// the following pairs of slot/signal implements a very simple message passing mechanism.
// a widget that has a pointer to the glarea call the sendViewDir() slot and 
// setup a connect to recive the transmitViewDir signal that actually contains the point3f.
// This mechanism is used to get the view direction/position and picking point on surface in the filter parameter dialog.
// See the Point3fWidget code.
signals :
		void transmitViewDir(QString name, vcg::Point3f dir);
		void transmitViewPos(QString name, vcg::Point3f dir);
		void transmitSurfacePos(QString name,vcg::Point3f dir);
		void transmitCameraPos(QString name,vcg::Point3f dir);
public slots:
		void sendViewPos(QString name);
		void sendSurfacePos(QString name);
		void sendViewDir(QString name);
		void sendCameraPos(QString name);


public:
	vcg::Point3f getViewDir(); 
	bool	infoAreaVisible;		// Draws the lower info area ?
	bool  suspendedEditor;
protected:

	void initializeGL();
	void initTexture();
	void displayInfo();
	void displayHelp();

	QString GetMeshInfoString();
	void paintGL();
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
	void setTiledView(GLdouble fovY, float viewRatio, float fAspect, GLdouble zNear, GLdouble zFar, float cameraDist);
    
	bool  helpVisible;				// Help on screen	
	bool	trackBallVisible;		// Draws the trackball ?
	bool  activeDefaultTrackball; // keep track on active trackball
	bool	hasToUpdateTexture;			// has to reload textures at the next redraw
	bool  hasToPick;							// has to pick during the next redraw.
	bool hasToGetPickPos;							// if we are waiting for a double click for getting a surface position that has to be sent back using signal/slots (for parameters)
	QString nameToGetPickPos;         // the name of the parameter that has asked for the point on the surface 
	
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

	//the last model that start edit was called with
	MeshModel *lastModelEdited;
	
public:
	RenderMode rm;
    // view setting variables
	float fov;
	float clipRatioFar;
	float clipRatioNear;
	float nearPlane;
	float farPlane;
  float pointSize;
  SnapshotSetting ss;
private:
	float cfps;
	float lastTime;
	
  QImage snapBuffer;
	bool takeSnapTile;
  
	enum AnimMode { AnimNone, AnimSpin, AnimInterp};
	AnimMode animMode; 
	int tileCol, tileRow, totalCols, totalRows;
	void setCursorTrack(vcg::TrackMode *tm);

	
};


#endif
