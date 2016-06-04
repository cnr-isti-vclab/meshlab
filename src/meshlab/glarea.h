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

#include <common/interfaces.h>
#include "glarea_setting.h"
#include "snapshotsetting.h"
#include "multiViewer_Container.h"

#define SSHOT_BYTES_PER_PIXEL 4

enum LightingModel{LDOUBLE,LFANCY};

class MeshModel;
class MainWindow;

class GLArea : public QGLWidget
{
    Q_OBJECT

    //typedef vcg::Shot<double> Shot;

public:
    GLArea(QWidget *parent,MultiViewer_Container *mvcont, RichParameterSet *current);
    ~GLArea();
    static void initGlobalParameterSet( RichParameterSet * /*globalparam*/);

private:
    int id;  //the very important unique id of each subwindow.
    MultiViewer_Container* parentmultiview;

public:
    int getId() {return id;}
    // Layer Management stuff.

    //MultiViewer_Container *mvc()
    MultiViewer_Container * mvc()
    {
        return parentmultiview;
    }

    void requestForRenderingAttsUpdate( int meshid,vcg::GLMeshAttributesInfo::ATT_NAMES attname )
    {
        if (parentmultiview != NULL)
        {
            MLSceneGLSharedDataContext* cont = parentmultiview->sharedDataContext();
            if (cont != NULL)
            {
                vcg::GLMeshAttributesInfo::RendAtts atts;
                atts[attname] = true;
                cont->meshAttributesUpdated(meshid,false,atts);
                cont->manageBuffers(meshid);
            }
        }
    }


    MainWindow * mw();

    MeshModel *mm(){ if (mvc() == NULL) return NULL;return mvc()->meshDoc.mm();}
    inline MeshDocument *md() {if (mvc() == NULL) return NULL;return &(mvc()->meshDoc);}

    vcg::Trackball trackball;
    vcg::Trackball trackball_light;
    void Logf(int Level, const char * f, ... );

    GLAreaSetting glas;
    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    QAction *getLastAppliedFilter()							{return lastFilterRef;}
    void		setLastAppliedFilter(QAction *qa)		{lastFilterRef = qa;}

    ////RenderMode*  getCurrentRenderMode();
    //RenderMode* getCurrentRenderMode()
    //{
    //    if ((md() != NULL) && (md()->mm() != NULL))
    //    {
    //        QMap<int,RenderMode>::iterator it = rendermodemap.find(md()->mm()->id());
    //        if (it != rendermodemap.end())
    //            return &it.value();
    //    }
    //    return NULL;
    //}

    void updateFps(float deltaTime);

    bool isCurrent() { if (mvc() == NULL) return false;return mvc()->currentId == this->id;}

    void showTrackBall(bool b)		{trackBallVisible = b; update();}
    bool isHelpVisible()      {return helpVisible;}
    bool isTrackBallVisible()		{return trackBallVisible;}
    bool isDefaultTrackBall()   {return activeDefaultTrackball;}
    void saveSnapshot();
    void toggleHelpVisible()      {helpVisible = !helpVisible; update();}
  /*  void setBackFaceCulling(bool enabled);
    void setLight(bool state);
    void setLightMode(bool state,LightingModel lmode);*/
    
   // void setLightModel(RenderMode& rm);
    void setView();

    int RenderForSelection(int pickX, int pickY);

    // Stores for each mesh what are the per Mesh active decorations
    QMap<int, QList<QAction *> > iPerMeshDecoratorsListMap;

    QList<QAction *> iPerDocDecoratorlist;
    QList<QAction *> &iCurPerMeshDecoratorList() { assert(this->md()->mm()) ; return iPerMeshDecoratorsListMap[this->md()->mm()->id()]; }

    void setRenderer(MeshRenderInterface *rend, QAction *shader){	iRenderer = rend; currentShader = shader;}
    MeshRenderInterface * getRenderer() { return iRenderer; }
    QAction* getCurrentShaderAction() {return currentShader;}
    
    

    // Edit Mode management
    // In the glArea we can have a active Editor that can toggled into a ''suspendeed'' state
    // in which the mouse event are redirected back to the GLArea to drive the camera trackball
    // The decorate function of the current active editor is still called.
    // EndEdit is called only when you press again the same button or when you change editor.
public slots:
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
    void updateCustomSettingValues(RichParameterSet& rps);

    void endEdit(){
        if(iEdit && currentEditor)
        {
            if (mm() != NULL)
                iEdit->EndEdit(*mm(),this,parentmultiview->sharedDataContext());
        }
        iEdit= 0;
        currentEditor=0;
        setCursorTrack(0);
        update();
        emit updateMainWindowMenus();
    }

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

    void currentViewerChanged(int currentId);

public slots:

    // Called when we change layer, notifies the edit tool if one is open
    void manageCurrentMeshChange();
    // Called when we modify the document
    void updateAllPerMeshDecorators();

public:
    void focusInEvent ( QFocusEvent * event );

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
    bool readyToClose();
    float lastRenderingTime() { return lastTime;}
    void drawGradient();
    void drawLight();
    float getFov() { return fov; }
    bool showInterruptButton() const;
    void showInterruptButton(const bool& show);


    // the following pairs of slot/signal implements a very simple message passing mechanism.
    // a widget that has a pointer to the glarea call the sendViewDir() slot and
    // setup a connect to recive the transmitViewDir signal that actually contains the point3f.
    // This mechanism is used to get the view direction/position and picking point on surface in the filter parameter dialog.
    // See the Point3fWidget code.
signals :
    void transmitViewDir(QString name, vcg::Point3f dir);
    void transmitViewPos(QString name, vcg::Point3f dir);
    void transmitSurfacePos(QString name,vcg::Point3f dir);
	void transmitPickedPos(QString name, vcg::Point2f dir);
    void transmitCameraPos(QString name,vcg::Point3f dir);
    void transmitShot(QString name, Shotm);
    void transmitMatrix(QString name, Matrix44m);
    void updateLayerTable();

public slots:
    void sendViewPos(QString name);
    void sendSurfacePos(QString name);
	void sendPickedPos(QString name);
    void sendViewDir(QString name);
    void sendCameraPos(QString name);
    void sendMeshShot(QString name);
    void sendMeshMatrix(QString name);
    void sendViewerShot(QString name);
    void sendRasterShot(QString name);


    

public:
    vcg::Point3f getViewDir();
    bool	infoAreaVisible;		// Draws the lower info area ?
    bool  suspendedEditor;
protected:

    void initializeGL();

    void displayInfo(QPainter *painter);
    void displayRealTimeLog(QPainter *painter);

    void displayMatrix(QPainter *painter, QRect areaRect);
    void displayViewerHighlight();
    void displayHelp();

    QString GetMeshInfoString();
    void paintEvent(QPaintEvent *event);
    void keyReleaseEvent ( QKeyEvent * e );
    void keyPressEvent ( QKeyEvent * e );
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent ( QMouseEvent * event ) ;
    void wheelEvent(QWheelEvent*e);
    void tabletEvent(QTabletEvent *e);
    void hideEvent(QHideEvent * event);

private:
    void setLightingColors(const MLPerViewGLOptions& opts);

    QMap<QString,QCursor> curMap;
    void pasteTile();
    void setTiledView(GLdouble fovY, float viewRatio, float fAspect, GLdouble zNear, GLdouble zFar, float cameraDist);

    bool helpVisible;				// Help on screen
    bool trackBallVisible;		// Draws the trackball ?
    bool activeDefaultTrackball; // keep track on active trackball
    bool hasToUpdateTexture;			// has to reload textures at the next redraw
    bool hasToPick;							// has to pick during the next redraw.
    bool hasToGetPickPos;							// if we are waiting for a double click for getting a surface position that has to be sent back using signal/slots (for parameters)
	bool hasToGetPickCoords;							// if we are waiting for a double click for getting a position on the GLArea that has to be sent back using signal/slots (for parameters)
	bool hasToSelectMesh;							// if we are waiting for a double click for getting a surface position that has to be sent back using signal/slots (for parameters)
    QString nameToGetPickPos;         // the name of the parameter that has asked for the point on the surface
	QString nameToGetPickCoords;         // the name of the parameter that has asked for the point on the GLArea
    bool interrbutshow;
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
    inline MLSceneGLSharedDataContext* getSceneGLSharedContext() {return ((mvc() != NULL)? mvc()->sharedDataContext() : NULL);}

    // view setting variables
    float fov;
    float clipRatioFar;
    float clipRatioNear;
    float nearPlane;
    float farPlane;
    SnapshotSetting ss;

    // Store for each mesh if it is visible for the current viewer.
    QMap<int, bool> meshVisibilityMap;

    // Store for each raster if it is visible for the current viewer.
    QMap<int, bool> rasterVisibilityMap;

    // Add an entry in the mesh visibility map
    void meshSetVisibility(MeshModel *mp, bool visibility);

    // Add an entry in the raster visibility map
    void addRasterSetVisibility(int rasterId, bool visibility);

public slots:
    void updateMeshSetVisibilities();
    void updateRasterSetVisibilities();

private slots:
    void meshAdded(int index);
    void meshRemoved(int index);

private:
    float cfps;
    float lastTime;

    QImage snapBuffer;
    bool takeSnapTile;

    enum AnimMode { AnimNone, AnimSpin, AnimInterp};
    AnimMode animMode;
    int tileCol, tileRow, totalCols, totalRows;   // snapshot: total number of subparts and current subpart rendered
    int  currSnapLayer;            // snapshot: total number of layers and current layer rendered
    void setCursorTrack(vcg::TrackMode *tm);

    //-----------Raster support----------------------------
private:
    bool _isRaster; // true if the viewer is a RasterViewer, false if is a MeshViewer; default value is false.

    int zoomx, zoomy;
    bool zoom;
    float opacity;
    GLuint targetTex;           // here we store the reference image. The raster image is rendered as a texture
    QString lastViewBeforeRasterMode; // keep the view immediately before switching to raster mode

public:
    bool isRaster() {return _isRaster;}
    void setIsRaster(bool viewMode);
    void loadRaster(int id);

    void setTarget(QImage &image);

private:
    void drawTarget();

    //-----------Shot support----------------------------
public:
    QPair<Shotm, float > shotFromTrackball();
    void viewFromCurrentShot(QString kind);
    bool viewFromFile();
    void createOrthoView(QString);
    void viewToClipboard();
    QString viewToText();
    void viewFromClipboard();
    void loadShot(const QPair<Shotm, float> &) ;

private:

    float getCameraDistance();

    // This parameter is the one that controls HOW LARGE IS THE TRACKBALL ICON ON THE SCREEN.
    inline float viewRatio() const { return 1.75f; }
    inline float clipRatioNearDefault() const { return 0.3f; }
    inline float fovDefault() const { return 60.f; }
    void initializeShot(Shotm &shot);
    void loadShotFromTextAlignFile(const QDomDocument &doc);
    void loadViewFromViewStateFile(const QDomDocument &doc);


    /*
    Given a shot "refCamera" and a trackball "track", computes a new shot which is equivalent
    to apply "refCamera" o "track" (via GPU).
    */
    template <class T>
    vcg::Shot<T> track2ShotGPU(vcg::Shot<T> &refCamera, vcg::Trackball *track){
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
    to apply "refCamera" o "track" (via CPU).
    */
    template <class T>
    vcg::Shot<T> track2ShotCPU(vcg::Shot<T> &refCamera, vcg::Trackball *track){
        vcg::Shot<T> view;

        double _near, _far;
        _near = 0.1;
        _far = 100;

        //get shot extrinsics matrix
        vcg::Matrix44<T> shotExtr;
        refCamera.GetWorldToExtrinsicsMatrix().ToMatrix(shotExtr);

        vcg::Matrix44<T> model2;
        model2 = (shotExtr)* vcg::Matrix44<T>::Construct(track->Matrix());
        vcg::Matrix44<T> model;
        model2.ToMatrix(model);

        //get translation out of modelview
        vcg::Point3<T> tra;
        tra[0] = model[0][3]; tra[1] = model[1][3]; tra[2] = model[2][3];
        model[0][3] = model[1][3] = model[2][3] = 0;

        //get pure rotation out of modelview
        double det = model.Determinant();
        double idet = 1/pow(det, 1/3.0); //inverse of the determinant
        model *= idet;
        model[3][3] = 1;
        view.Extrinsics.SetRot(model);

        //get pure translation out of modelview
        vcg::Matrix44<T> imodel = model;
        vcg::Transpose(imodel);
        tra = -(imodel*tra);
        tra *= idet;
        view.Extrinsics.SetTra(vcg::Point3<T>::Construct(tra));

        //use same current intrinsics
        view.Intrinsics = refCamera.Intrinsics;

        return view;
    }

    /*
    Given a shot "from" and a trackball "track", updates "track" with "from" extrinsics.
    A traslation involving cameraDistance is included. This is necessary to compensate a trasformation that OpenGL performs
    at the end of the graphic pipeline.
    */
    template <class T>
    void shot2Track(const vcg::Shot<T> &from, const float cameraDist, vcg::Trackball &tb){

        vcg::Quaternion<T> qfrom; qfrom.FromMatrix(from.Extrinsics.Rot());

        tb.track.rot = vcg::Quaternionf::Construct(qfrom);
        tb.track.tra =  (vcg::Point3f::Construct(-from.Extrinsics.Tra()));
        tb.track.tra += vcg::Point3f::Construct(tb.track.rot.Inverse().Rotate(vcg::Point3f(0,0,cameraDist)))*(1/tb.track.sca);
    }
   
};


#endif
