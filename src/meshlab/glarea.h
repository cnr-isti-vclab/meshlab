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

#include <vcg/space/plane3.h>
#include <vcg/space/line3.h>
#include <vcg/math/matrix44.h>
#include <wrap/gl/math.h>
#include <wrap/gui/trackball.h>
#include <vcg/math/shot.h>
#include <wrap/gl/shot.h>

#include <QTimer>
#include <QTime>

#include <common/plugins/interfaces/render_plugin.h>
#include <common/plugins/interfaces/decorate_plugin.h>
#include <common/plugins/interfaces/edit_plugin.h>
#include <common/ml_shared_data_context/ml_shared_data_context.h>
#include "glarea_setting.h"
#include "snapshotsetting.h"
#include "multiViewer_Container.h"
#include <common/ml_selection_buffers.h>
#include "ml_default_decorators.h"

#include <QOpenGLWidget>

#define SSHOT_BYTES_PER_PIXEL 4

enum LightingModel{LDOUBLE,LFANCY};

class MeshModel;
class MainWindow;

class GLArea : public QOpenGLWidget
{
	Q_OBJECT

	//typedef vcg::Shot<double> Shot;

public:
	GLArea(MultiViewer_Container *mvcont, RichParameterList *current);
	~GLArea();

	static void initGlobalParameterList(RichParameterList& /*globalparam*/);

	int getId() {return id;}

	// Layer Management stuff.

	void requestForRenderingAttsUpdate( int meshid,MLRenderingData::ATT_NAMES attname );


	MainWindow * mw();

	MeshModel *mm()
	{
		if (mvc() == nullptr)
			return nullptr;
		return mvc()->meshDoc.mm();
	}

	template <typename... Ts>
	void Logf(int Level, const char * f, Ts&&... ts);

	void Log(int Level, const char * f);

	QSize minimumSizeHint() const;
	QSize sizeHint() const;

	const QAction *getLastAppliedFilter();
	void setLastAppliedFilter(const QAction *qa);

	void updateFps(float deltaTime);

	bool isCurrent();

	void showTrackBall(bool b);
	bool isHelpVisible();
	bool isTrackBallVisible();
	bool isEditorSuspended();
	bool isDefaultTrackBall();
	void saveSnapshot();
	void toggleHelpVisible();
	/*  void setBackFaceCulling(bool enabled);
	void setLight(bool state);
	void setLightMode(bool state,LightingModel lmode);*/

	// void setLightModel(RenderMode& rm);
	void setView();

	int RenderForSelection(int pickX, int pickY);

	QList<QAction *> &iCurPerMeshDecoratorList() { assert(this->md()->mm()) ; return iPerMeshDecoratorsListMap[this->md()->mm()->id()]; }

	void setRenderer(RenderPlugin *rend, QAction *shader){	iRenderer = rend; currentShader = shader;}
	RenderPlugin * getRenderer() { return iRenderer; }
	QAction* getCurrentShaderAction() {return currentShader;}

	void focusInEvent ( QFocusEvent * event );

	//call when the editor changes
	void setCurrentEditAction(QAction *editAction);

	//get the currently active edit action
	QAction * getCurrentEditAction() { return currentEditor; }

	//get the currently active mesh editor
	EditTool * getCurrentMeshEditor() { return iEdit; }

	//see if this glAarea has a MESHEditInterface for this action
	bool editorExistsForAction(QAction *editAction){ return actionToMeshEditMap.contains(editAction); }

	//add a MeshEditInterface for the given action
	void addMeshEditor(QAction *editAction, EditTool *editor){ actionToMeshEditMap.insert(editAction, editor); }
	bool readyToClose();
	float lastRenderingTime() { return lastTime; }
	void drawGradient();
	void drawLight();
	float& getFov() { return fov; }
	float  getFov() const { return fov; }
	vcg::Matrix44f trackballMatrix() const { return trackball.Matrix(); }
	vcg::Similarityf trackballTrack() const { return trackball.track; }
	vcg::Matrix44f trackballLightMatrix() const { return trackball_light.Matrix(); }
	bool showInterruptButton() const;
	void showInterruptButton(const bool& show);
	bool isInfoAreaVisible() const;
	void setInfoAreaVisibility(bool vis);

	vcg::Point3f getViewDir();

	// Add an entry in the mesh visibility map
	void meshSetVisibility(MeshModel& mp, bool visibility);

	// Add an entry in the raster visibility map
	void addRasterSetVisibility(int rasterId, bool visibility);

	// void getPerDocGlobalRenderingData(MLRenderingData& dt) const;
	// void setPerDocGlobalRenderingData(const MLRenderingData& dt);

	bool isRaster() {return _isRaster;}
	void setIsRaster(bool viewMode);
	void loadRaster(int id);

	void setTarget(QImage &image);

	//-----------Shot support----------------------------
	QPair<Shotm, float > shotFromTrackball();
	void viewFromCurrentShot(QString kind);
	bool saveViewToFile();
	bool readViewFromFile();
	bool readViewFromFile(QString const& filename);
	void createOrthoView(QString);
	void toggleOrtho();
	void trackballStep(QString);
	void viewToClipboard();
	QString viewToText();
	void viewFromClipboard();
	void loadShot(const QPair<Shotm, float> &) ;
	void loadShotFromTextAlignFile(const QDomDocument &doc);
	void loadViewFromViewStateFile(const QDomDocument &doc);

	// TODO: make private these member attributes
	QList<QAction *> iPerDocDecoratorlist;

	SnapshotSetting ss;

	// Store for each mesh if it is visible for the current viewer.
	QMap<int, bool> meshVisibilityMap;

	// Store for each raster if it is visible for the current viewer.
	QMap<int, bool> rasterVisibilityMap;

	/***********************************************/
	/* Inline and templated members                */
	/***********************************************/
	// These members are defined in glarea.ipp, which contains templated and inlined members.
	// Some member functions are inlined because they are called also from plugins.
	// This is a design issue THAT MUST BE SOLVED.
	// Member functions available to plugins SHOULD NOT BE in the meshlab target, but should be in
	// an appropriate library.

	inline MultiViewer_Container* mvc();
	inline MeshDocument* md();

	inline MLSceneGLSharedDataContext* getSceneGLSharedContext();

	inline void updateSelection(int meshid, bool vertsel, bool facesel);

	/* WARNING!!!!! HORRIBLE THING!!!!!
	 * Added just to avoid to include the multiViewer_container.cpp file in a MeshLab plugins
	 * project in case it needs to update all the GLArea and not just the one passed as parameter
	 */

	inline void updateAllSiblingsGLAreas();

public slots:
	// Edit Mode management
	// In the glArea we can have a active Editor that can toggled into a ''suspendeed'' state
	// in which the mouse event are redirected back to the GLArea to drive the camera trackball
	// The decorate function of the current active editor is still called.
	// EndEdit is called only when you press again the same button or when you change editor.
	void setupTextureEnv(GLuint textid);
	void resetTrackBall();
	void showRaster(bool resetViewFlag);

	   //current raster will be reloaded and repaint function will be called
	void completeUpdateRequested();

	void setDecorator(QString name, bool state);
	void toggleDecorator(QString name);

	void updateDecorator(QString name, bool toggle, bool stateToSet);

	//slots for changing the draw rendering and texturing mode
	/* void setDrawMode(vcg::GLW::DrawMode mode);
	 void setDrawMode(RenderMode& rm,vcg::GLW::DrawMode mode );
	 void setColorMode(vcg::GLW::ColorMode mode);
	 void setColorMode(RenderMode& rm,vcg::GLW::ColorMode mode);
	 void setTextureMode(vcg::GLW::TextureMode mode);
	 void setTextureMode(RenderMode& rm,vcg::GLW::TextureMode mode);*/
	void updateCustomSettingValues(const RichParameterList& rps);

	void endEdit();

	void suspendEditToggle();

	//// Other slots

	void copyToClip();
	void pasteFromClip();

	// Called when we change layer, notifies the edit tool if one is open
	void manageCurrentMeshChange();
	// Called when we modify the document
	/// Execute a end/start pair for all the PerMesh decorator that are active in this glarea.
	/// It is used when the document is changed or when some parameter changes
	/// Note that it is rather inefficient. Such work should be done only once for each decorator.
	void updateAllPerMeshDecorators();

	void updatePerMeshDecorators(int);

	void updateAllDecorators();

	void sendViewPos(QString name);
	void sendSurfacePos(QString name);
	void sendPickedPos(QString name);
	void sendViewDir(QString name);
	void sendCameraPos(QString name);
	void sendMeshShot(QString name);
	void sendMeshMatrix(QString name);
	void sendViewerShot(QString name);
	void sendRasterShot(QString name);
	void sendTrackballPos(QString name);

	void updateMeshSetVisibilities();
	void updateRasterSetVisibilities();

signals:
	void updateMainWindowMenus(); //updates the menus of the meshlab MainWindow
	void glareaClosed();					//someone has closed the glarea
	void insertRenderingDataForNewlyGeneratedMesh(int);
	void currentViewerChanged(int currentId);
	void currentViewerRefreshed();

	// the following pairs of slot/signal implements a very simple message passing mechanism.
	// a widget that has a pointer to the glarea call the sendViewDir() slot and
	// setup a connect to receive the transmitViewDir signal that actually contains the point3f.
	// This mechanism is used to get the view direction/position and picking point on surface in the filter parameter dialog.
	// See the Point3fWidget code.
	void transmitViewDir(QString name, Point3m dir);
	void transmitViewPos(QString name, Point3m pos);
	void transmitSurfacePos(QString name, Point3m pos);
	void transmitPickedPos(QString name, Point2m pos);
	void transmitCameraPos(QString name, Point3m pos);
	void transmitTrackballPos(QString name, Point3m pos);
	void transmitShot(QString name, Shotm);
	void transmitMatrix(QString name, Matrix44m);
	void updateLayerTable();

protected:
	void initializeGL();

	void displayInfo(QPainter* painter);
	void displayRealTimeLog(QPainter* painter);

	void displayMatrix(QPainter* painter, QRect areaRect);
	void displayViewerHighlight();
	void displayHelp();

	QString GetMeshInfoString();
	void    paintEvent(QPaintEvent* event);
	void    keyReleaseEvent(QKeyEvent* e);
	void    keyPressEvent(QKeyEvent* e);
	void    mousePressEvent(QMouseEvent* event);
	void    mouseMoveEvent(QMouseEvent* event);
	void    mouseReleaseEvent(QMouseEvent* event);
	void    mouseDoubleClickEvent(QMouseEvent* event);
	void    wheelEvent(QWheelEvent* e);
	void    tabletEvent(QTabletEvent* e);
	void    hideEvent(QHideEvent* event);

private:
	int                    id; // the very important unique id of each subview.
	MultiViewer_Container* parentmultiview;

	vcg::Trackball trackball;
	vcg::Trackball trackball_light;
	GLAreaSetting  glas;

	// Stores for each mesh what are the per Mesh active decorations
	QMap<int, QList<QAction*>> iPerMeshDecoratorsListMap;

	QString renderfacility;

	QMap<QString, QCursor> curMap;

	bool infoAreaVisible = true; // Draws the lower info area
	bool helpVisible = false;            // Help on screen
	bool trackBallVisible = true; // Draws the trackball
	bool activeDefaultTrackball = true; // keep track on active trackball
	// bool hasToUpdateTexture;			// has to reload textures at the next redraw
	bool hasToPick = false;          // has to pick during the next redraw.
	bool hasToGetPickPos = false;    // if we are waiting for a double click for getting a surface position
							 // that has to be sent back using signal/slots (for parameters)
	bool hasToGetPickCoords; // if we are waiting for a double click for getting a position on the
							 // GLArea that has to be sent back using signal/slots (for parameters)
	bool hasToSelectMesh = false;    // if we are waiting for a double click for getting a surface position
							 // that has to be sent back using signal/slots (for parameters)
	QString
		nameToGetPickPos; // the name of the parameter that has asked for the point on the surface
	QString
		nameToGetPickCoords; // the name of the parameter that has asked for the point on the GLArea
	bool         interrbutshow = false;
	vcg::Point2i pointToPick;

	// shader support
	RenderPlugin*  iRenderer = nullptr;
	QAction*       currentShader = nullptr;
	const QAction* lastFilterRef = nullptr; // reference to last filter applied
	QFont          qFont;         // font settings

	// Editing support
	EditTool*                 iEdit = nullptr;
	QAction*                  currentEditor = nullptr;
	bool                      suspendedEditor = false;
	QAction*                  suspendedEditRef; // reference to last Editing Mode Used
	QMap<QAction*, EditTool*> actionToMeshEditMap;

	// the last model that start edit was called with
	MeshModel* lastModelEdited = nullptr;

	// view setting variables
	float clipRatioFar;
	float clipRatioNear;
	float nearPlane;
	float farPlane;

	unsigned int snapshotCounter;

	float cfps = 0;
	float lastTime = 0;

	QImage snapBuffer;
	bool   takeSnapTile = false;

	enum AnimMode { AnimNone, AnimSpin, AnimInterp };
	AnimMode animMode = AnimNone;
	int      tileCol, tileRow, totalCols,
		totalRows; // snapshot: total number of subparts and current subpart rendered

	bool _isRaster; // true if the viewer is a RasterViewer, false if is a MeshViewer; default false

	float   fov;
	int     zoomx, zoomy;
	bool    zoom;
	float   opacity;
	GLuint  targetTex; // store the reference image. The raster image is rendered as a texture
	QString lastViewBeforeRasterMode; // keep the view immediately before switching to raster mode

	int lastloadedraster;

	void renderingFacilityString();

	void setLightingColors(const MLPerViewGLOptions& opts);

	void pasteTile();
	void setTiledView(
		GLdouble fovY,
		float    viewRatio,
		float    fAspect,
		GLdouble zNear,
		GLdouble zFar,
		float    cameraDist);

	void setCursorTrack(vcg::TrackMode* tm);

	void drawTarget();

	float getCameraDistance();

	// This parameter is the one that controls HOW LARGE IS THE TRACKBALL ICON ON THE SCREEN.
	inline float viewRatio() const { return 1.75f; }
	inline float clipRatioNearDefault() const { return 0.1f; }
	inline float clipRatioFarDefault() const { return 500.0f; }
	inline float fovDefault() const { return 60.f; }
	void         initializeShot(Shotm& shot);

	template <class T>
	vcg::Shot<T> track2ShotGPU(vcg::Shot<T> &refCamera, vcg::Trackball *track);

	template <class T>
	vcg::Shot<T> track2ShotCPU(vcg::Shot<T> &refCamera, vcg::Trackball *track);

	template <class T>
	void shot2Track(const vcg::Shot<T> &from, const float cameraDist, vcg::Trackball &tb);

private slots:
	void meshAdded(int index);
	void meshRemoved(int index);
};

#include "glarea.ipp"

#endif
