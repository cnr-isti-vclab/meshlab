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
#include <vcg/math/shot.h>
#include <wrap/gl/shot.h>

#include "../common/interfaces.h"
#include "../common/filterscript.h"
//#include "layerDialog.h"  **
#include "glarea_setting.h"
#include "viewer.h"
#include "multiViewer_Container.h"

//mathematics
#include <vcg/math/quaternion.h>

#define SSHOT_BYTES_PER_PIXEL 4

enum LightingModel{LDOUBLE,LFANCY};


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
class GLArea : public QGLWidget, public Viewer
{
	Q_OBJECT

	typedef vcg::Shot<double> Shot;

public:
    GLArea(QWidget *parent, MultiViewer_Container *mvcont, RichParameterSet *current, int id, MeshDocument *meshDoc);
	~GLArea();
	static void initGlobalParameterSet( RichParameterSet * /*globalparam*/);
private:


public:
	//LayerDialog *layerDialog; **
  // Layer Management stuff. 
	MeshDocument *meshDoc;
	MeshModel *mm(){return meshDoc->mm();}
	MultiViewer_Container *mvc;
 
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
	
	void setFileName(QString name)	
    {
    mm()->setFileName(name);
		setWindowTitle(QFileInfo(name).fileName()+tr("[*]"));
		//layerDialog->setWindowTitle("Layer Dialog for "+QFileInfo(name).fileName());			
	  
    ss.basename=QFileInfo(getFileName()).baseName().append("Snap");
	}
	
	RenderMode &  getCurrentRenderMode()		{return rm;}

    const SnapshotSetting& getSnapshotSetting()	{/*ss.dx=vpWidth; ss.dy=vpHeight;*/ return ss;}
	void updateFps(float deltaTime);
	
	void showTrackBall(bool b)		{trackBallVisible = b; updateGL();}
	bool isHelpVisible()      {return helpVisible;}  
	bool isTrackBallVisible()		{return trackBallVisible;}
	bool isDefaultTrackBall()   {return activeDefaultTrackball;}

	bool isCurrent() { return mvc->currentId == id;}

	void toggleHelpVisible()      {helpVisible = !helpVisible; update();}  
	void setBackFaceCulling(bool enabled);
	void setSnapshotSetting(const SnapshotSetting & s);
	void setLight(bool state);
	void setLightMode(bool state,LightingModel lmode);
	void saveSnapshot();
	void setLightModel();
	void setView();
    void resetTrackBall();
	void loadShot();
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

	void currentViewerChanged(int currentId);         // set this viewer as current
  
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

public:
	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent2(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);

protected:
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
private:
	float cfps;
	float lastTime;
	
	SnapshotSetting ss;
	QImage snapBuffer;
	bool takeSnapTile;
  
	enum AnimMode { AnimNone, AnimSpin, AnimInterp};
	AnimMode animMode; 
	int tileCol, tileRow, totalCols, totalRows;
	void setCursorTrack(vcg::TrackMode *tm);

	void initializeShot(Shot &shot);
	void readShotFromFile(Shot &shot);

	/*
	Given a shot "refCamera" and a trackball "track", computes a new shot which is equivalent
	to apply "refCamera" o "track".
	*/
	template <class T>
	vcg::Shot<T> getShotFromTrack(vcg::Shot<T> &refCamera, vcg::Trackball *track){
		vcg::Shot<T> view;

		double _near, _far;
		_near = 0.1;
		_far = 100;

		//get OpenGL modelview matrix after applying the trackball
		GlShot<vcg::Shot<T> >::SetView(refCamera, _near, _far);
		glPushMatrix();
		track->GetView();
		track->Apply();
		vcg::Matrix44d model;
		glGetv(GL_MODELVIEW_MATRIX, model);
		glPopMatrix();
		GlShot<vcg::Shot<T> >::UnsetView();

		//get translation out of modelview
		vcg::Point3d tra;
		tra[0] = model[0][3]; tra[1] = model[1][3]; tra[2] = model[2][3];
		model[0][3] = model[1][3] = model[2][3] = 0;

		//get pure rotation out of modelview
		double det = model.Determinant();
		double idet = 1/pow(det, 1/3.0); //inverse of the determinant
		model *= idet;
		model[3][3] = 1;
		view.Extrinsics.SetRot(model);

		//get pure translation out of modelview
		vcg::Matrix44d imodel = model;
		vcg::Transpose(imodel);
		tra = -(imodel*tra);
		tra *= idet;
		view.Extrinsics.SetTra(vcg::Point3<T>::Construct(tra));

		//use same current intrinsics
		view.Intrinsics = refCamera.Intrinsics;

		return view;
	}

	/*
	Given a shot "refCamera" and a trackball "track", computes a new shot which is equivalent
	to apply "refCamera" o "track".
	*/
	template <class T>
	vcg::Shot<T> getShotFromTrack2(vcg::Shot<T> &refCamera, vcg::Trackball *track){
		vcg::Shot<T> view;

		double _near, _far;
		_near = 0.1;
		_far = 100;

		//---------------------- ALTERED--------------------

		// //get OpenGL modelview matrix after applying the trackball
		// GlShot<vcg::Shot<T> >::SetView(refCamera, _near, _far);
		//	glPushMatrix();
		//	track->GetView();
		//	track->Apply();
		//	vcg::Matrix44d model;
		//	glGetv(GL_MODELVIEW_MATRIX, model);
		//	glPopMatrix();
		// GlShot<vcg::Shot<T> >::UnsetView();

		//GlShot<vcg::Shot<T> >::SetView(refCamera, _near, _far); //mi serve per la proj di refcamera... altrimenti come la ottengo?
		vcg::Matrix44f shotExtr;
		refCamera.GetWorldToExtrinsicsMatrix().ToMatrix(shotExtr);

		////legge la matrice di proj settata prima
		//vcg::Matrix44f proj; 
		//glGetv(GL_PROJECTION_MATRIX,proj);
		//GlShot<vcg::Shot<T> >::UnsetView();

		////aggiorna la matrice di rototraslazione di track
		//int viewport[] = {0,0,width(),height()};
		//track->camera.SetView(proj.V(),shotExtr.V(), viewport);

		vcg::Matrix44f model2;
		model2 = (shotExtr)* track->Matrix();
		vcg::Matrix44d model;
		model2.ToMatrix(model);

		//---------------------- ORIGINAL--------------------

		//get translation out of modelview
		vcg::Point3d tra;
		tra[0] = model[0][3]; tra[1] = model[1][3]; tra[2] = model[2][3];
		model[0][3] = model[1][3] = model[2][3] = 0;

		//get pure rotation out of modelview
		double det = model.Determinant();
		double idet = 1/pow(det, 1/3.0); //inverse of the determinant
		model *= idet;
		model[3][3] = 1;
		view.Extrinsics.SetRot(model);

		//get pure translation out of modelview
		vcg::Matrix44d imodel = model;
		vcg::Transpose(imodel);
		tra = -(imodel*tra);
		tra *= idet;
		view.Extrinsics.SetTra(vcg::Point3<T>::Construct(tra));

		//use same current intrinsics
		view.Intrinsics = refCamera.Intrinsics;

		return view;
	}

	/*
	Given a Shot "from", and a trackball "tb" replaces "tb" with a trackball "tb'" such that:
	"from" o "tb" = "tb'"
	*/
	template <class T>
	void Shot2Track(const vcg::Shot<T> &from, const float cameraDist, vcg::Trackball &tb){
		
		Shot id;

		vcg::Quaterniond qto;     qto.FromMatrix(id.Extrinsics.Rot());
		vcg::Quaterniond qfrom; qfrom.FromMatrix(from.Extrinsics.Rot());

		/*float sca=tb.track.sca;
		tb.track.sca=1;*/
		tb.track.tra += ( tb.track.rot.Inverse().Rotate( vcg::Point3f::Construct(-from.Extrinsics.Tra()) + tb.center ) ) / tb.track.sca;
		tb.track.rot = vcg::Quaternionf::Construct(qto.Inverse() * qfrom) * tb.track.rot;
		tb.track.tra -= ( tb.track.rot.Inverse().Rotate( vcg::Point3f::Construct(-  id.Extrinsics.Tra()) + tb.center ) ) / tb.track.sca;

		//aggiustare sca e tra per mettere il centro della trackbal al punto giusto
	}
};


#endif
