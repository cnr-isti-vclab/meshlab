/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//None of this should happen if we are compiling c, not c++
#ifdef __cplusplus
#include <GL/glew.h>
#include <QtScript>

#include <QDir>
#include <QMainWindow>
#include <QMdiArea>
#include <QStringList>
#include <QColorDialog>
#include "../common/pluginmanager.h"
#include "../common/scriptinterface.h"
#include "glarea.h"
#include "layerDialog.h"
#include "stdpardialog.h"
#include "xmlstdpardialog.h"
#include "xmlgeneratorgui.h"

#define MAXRECENTFILES 4

class QAction;
class QActionGroup;
class QMenu;
class QScrollArea;
class QSignalMapper;
class QProgressDialog;
class QHttp;


class MainWindow : public QMainWindow, MainWindowInterface
{
	Q_OBJECT

public:
	// callback function to execute a filter
	void executeFilter(QAction *action, RichParameterSet &srcpar, bool isPreview);
	void executeFilter(MeshLabXMLFilterContainer* mfc, EnvWrap& env, bool  isPreview);

  MainWindow();
	static bool QCallBack(const int pos, const char * str);
	const QString appName() const {return tr("MeshLab v")+appVer(); }
  const QString appVer() const {return tr("1.3.2_64bit"); }

signals:
	void dispatchCustomSettings(RichParameterSet& rps);
	void filterExecuted();

private slots:
  GLArea* newProject(const QString& projName = QString());
  void saveProject();

public slots:
  bool importMesh(QString fileName=QString());
  bool importRaster(const QString& fileImg = QString());
  bool openProject(QString fileName=QString());
  bool appendProject(QString fileName=QString());
  void updateCustomSettings();


  void delCurrentMesh();
  void delCurrentRaster();
private slots:
  void endEdit();
  void updateDocumentScriptBindings();
  void loadAndInsertXMLPlugin(const QString& xmlpath,const QString& scriptname);
  void postFilterExecution(/*MeshLabXMLFilterContainer* mfc*/);
  //void evaluateExpression(const Expression& exp,Value** res);
  void updateProgressBar(const int pos,const QString& text);

public:
  bool exportMesh(QString fileName,MeshModel* mod,const bool saveAllPossibleAttributes);
  bool loadMesh(const QString& fileName,MeshIOInterface *pCurrentIOPlugin,MeshModel* mm,int& mask,RichParameterSet* prePar);
  bool loadMeshWithStandardParams(QString& fullPath,MeshModel* mm);

private slots:
	//////////// Slot Menu File //////////////////////
  void reload();
  void reloadAllMesh();
  void openRecentMesh();
	void openRecentProj();
	bool saveAs(QString fileName = QString(),const bool saveAllPossibleAttributes = false);
	bool save(const bool saveAllPossibleAttributes = false);
	bool saveSnapshot();
	///////////Slot Menu Edit ////////////////////////
	void applyEditMode();
	void suspendEditMode();
	///////////Slot Menu Filter ////////////////////////
	void startFilter();
	void applyLastFilter();
	void runFilterScript();
	void showFilterScript();
	void showXMLPluginEditorGui();
  void showTooltip(QAction*);
  /////////// Slot Menu Render /////////////////////
	void renderBbox();
	void renderPoint();
	void renderWire();
	void renderFlat();
	void renderFlatLine();
	void renderHiddenLines();
	void renderSmooth();
	void renderTexture();
	void setLight();
	void setDoubleLighting();
	void setFancyLighting();
    void setColorMode(QAction *qa);
	void applyRenderMode();
	//void applyColorMode();
	void toggleBackFaceCulling();
  void toggleSelectFaceRendering();
  void toggleSelectVertRendering();
  void applyDecorateMode();
	///////////Slot Menu View ////////////////////////
	void fullScreen();
	void showToolbarFile();
	void showToolbarRender();
	void showInfoPane();
	void showTrackBall();
	void resetTrackBall();
	void showLayerDlg();
	void showRaster();
	///////////Slot Menu Windows /////////////////////
	void updateWindowMenu();
    void updateMenus();
	void updateSubFiltersMenu(const bool createmenuenabled,const bool validmeshdoc);
    void updateMenuItems(QMenu* menu,const bool enabled);
	void updateStdDialog();
	void updateXMLStdDialog();
	void setSplit(QAction *qa);
	void setUnsplit();
	void linkViewers();
	void viewFrom(QAction *qa);
	void readViewFromFile();
  void viewFromCurrentMeshShot();
  void viewFromCurrentRasterShot();
  void copyViewToClipBoard();
	void pasteViewFromClipboard();

	///////////Slot PopUp Menu Handles /////////////////////
	void splitFromHandle(QAction * qa);
	void unsplitFromHandle(QAction * qa);

	///////////Slot Menu Preferences /////////////////
	void setCustomize();
	///////////Slot Menu Help ////////////////////////
	void about();
	void aboutPlugins();
	void helpOnline();
	void helpOnscreen();
	void submitBug();
	void checkForUpdates(bool verboseFlag=true);

	///////////Slot General Purpose ////////////////////////

	void dropEvent ( QDropEvent * event );
	void dragEnterEvent(QDragEnterEvent *);
	void connectionDone(bool status);
	void sendHistory();

	///////////Solt Wrapper for QMdiArea //////////////////
	void wrapSetActiveSubWindow(QWidget* window);
	void scriptCodeExecuted(const QScriptValue& val,const int time,const QString& output);
private:
    void createStdPluginWnd(); // this one is
	void createXMLStdPluginWnd();
	void initGlobalParameters();
    void createActions();
	void createMenus();
	void initSearchEngine();
	void initItemForSearching(QAction* act);
	void initMenuForSearching(QMenu* menu);
	void fillFilterMenu();
	void fillDecorateMenu();
	void fillRenderMenu();
	void fillEditMenu();
	void createToolBars();
	void loadMeshLabSettings();
  // void loadPlugins();
	void keyPressEvent(QKeyEvent *);
	void updateRecentFileActions();
	void updateRecentProjActions();
  void saveRecentFileList(const QString &fileName);
	void saveRecentProjectList(const QString &projName);
	void addToMenu(QList<QAction *>, QMenu *menu, const char *slot);

	void initDocumentMeshRenderState(MeshLabXMLFilterContainer* mfc,EnvWrap &env );
	void initDocumentRasterRenderState(MeshLabXMLFilterContainer* mfc, EnvWrap &env );

	QHttp *httpReq;
	QBuffer myLocalBuf;
	int idHost;
	int idGet;
	bool VerboseCheckingFlag;

	MeshlabStdDialog *stddialog;
	MeshLabXMLStdDialog* xmldialog;
	static QProgressBar *qb;

	QMdiArea *mdiarea;
	LayerDialog *layerDialog;
	PluginGeneratorGUI* plugingui;
	QSignalMapper *windowMapper;


    PluginManager PM;

		/* 
		Note this part should be detached from MainWindow just like the loading plugin part.
		
		For each running instance of meshlab, for the global params we have default (hardwired) values and current(saved,modified) values. 
		At the start up the initGlobalParameterSet function (of decorations and of glarea and of ... ) is called with the empty RichParameterSet defaultGlobalParams (to collect the default values) 
		At the start up the currentGlobalParams is filled with the values saved in the registry.
	*/
	
	RichParameterSet currentGlobalParams;
	RichParameterSet defaultGlobalParams;
	
	QByteArray toolbarState;								//stato delle toolbar e dockwidgets

	QDir lastUsedDirectory;  //This will hold the last directory that was used to load/save a file/project in

public:

  MeshDocument *meshDoc() {
    assert(currentViewContainer());
    return &currentViewContainer()->meshDoc;
  }

  const RichParameterSet& currentGlobalPars() const { return currentGlobalParams; }
  RichParameterSet& currentGlobalPars() { return currentGlobalParams; }
  const RichParameterSet& defaultGlobalPars() const { return defaultGlobalParams; }

	GLArea *GLA() const {
//	  if(mdiarea->currentSubWindow()==0) return 0;
    MultiViewer_Container *mvc = currentViewContainer();
    if(!mvc) return 0;
    GLArea *glw =  qobject_cast<GLArea*>(mvc->currentView());
	  return glw;
	}

  MultiViewer_Container* currentViewContainer() const {
    MultiViewer_Container *mvc = qobject_cast<MultiViewer_Container *>(mdiarea->currentSubWindow());
    if(mvc) return mvc;
    if(mvc==0 && mdiarea->currentSubWindow()!=0 ){
			mvc = qobject_cast<MultiViewer_Container *>(mdiarea->currentSubWindow()->widget());
      if(mvc) return mvc;
		}
    QList<QMdiSubWindow *> subwinList=mdiarea->subWindowList();
    foreach(QMdiSubWindow *subwinPtr,subwinList)
    {
        MultiViewer_Container *mvc = qobject_cast<MultiViewer_Container *>(subwinPtr);
        if(mvc) return mvc;
        if(mvc==0 && subwinPtr!=0){
          mvc = qobject_cast<MultiViewer_Container *>(subwinPtr->widget());
          if(mvc) return mvc;
        }
    }

    return 0;
	}


	void setHandleMenu(QPoint p, Qt::Orientation o, QSplitter *origin);

	const PluginManager& pluginManager() const { return PM; }

  static QStatusBar *&globalStatusBar()
  {
    static QStatusBar *_qsb=0;
    return _qsb;
  }
    QMenu* meshLayerMenu() { return filterMenuMeshLayer; }
	QMenu* rasterLayerMenu() { return filterMenuRasterLayer; }
	

private:
	WordActionsMapAccessor wama;
	//////// ToolBars ///////////////
	QToolBar *mainToolBar;
	QToolBar *renderToolBar;
	QToolBar *editToolBar;
	QToolBar *filterToolBar;
	QToolBar *searchToolBar;

	///////// Menus ///////////////
	QMenu *fileMenu;
  QMenu *filterMenu;
  QMenu* recentProjMenu;
  QMenu* recentFileMenu;

  QMenu *filterMenuSelect;
  QMenu *filterMenuClean;
  QMenu *filterMenuCreate;
  QMenu *filterMenuRemeshing;
  QMenu *filterMenuPolygonal;
  QMenu *filterMenuColorize;
  QMenu *filterMenuSmoothing;
  QMenu *filterMenuQuality; 
  QMenu *filterMenuMeshLayer;
  QMenu *filterMenuRasterLayer;
  QMenu *filterMenuNormal;
  QMenu *filterMenuRangeMap;
  QMenu *filterMenuPointSet;
  QMenu *filterMenuSampling;
  QMenu *filterMenuTexture; 
  QMenu *filterMenuCamera;

	QMenu *editMenu;

  //Render Menu and SubMenu ////
	QMenu *shadersMenu;
	QMenu *renderMenu;
	QMenu *renderModeMenu;
	QMenu *lightingModeMenu;
	QMenu *colorModeMenu;

	//View Menu and SubMenu //////
	QMenu *viewMenu;
	QMenu *trackBallMenu;
	QMenu *logMenu;
	QMenu *toolBarMenu;
	//////////////////////////////
	QMenu *windowsMenu;
	QMenu *preferencesMenu;
	QMenu *helpMenu;
	QMenu *splitModeMenu;
	QMenu *viewFromMenu;
	//////////// Split/Unsplit Menu from handle///////////
	QMenu *handleMenu;
	QMenu *splitMenu;
	QMenu *unSplitMenu;
	////////// Search Shortcut ////////////////
	QShortcut* searchShortCut;
	MyToolButton* searchButton;
	//////////// Actions Menu File ///////////////////////
  QAction *newProjectAct;
  QAction *openProjectAct, *appendProjectAct, *saveProjectAct, *saveProjectAsAct;
  QAction  *importMeshAct,   *exportMeshAct,  *exportMeshAsAct;
  QAction  *importRasterAct;
  QAction *closeProjectAct;
  QAction *reloadMeshAct;
  QAction *reloadAllMeshAct;
  QAction *saveSnapshotAct;
  QAction *recentFileActs[MAXRECENTFILES];
  QAction *recentProjActs[MAXRECENTFILES];
	QAction *separatorAct;
	QAction *exitAct;
  //////
  QAction *lastFilterAct;
  QAction *runFilterScriptAct;
  QAction *showFilterScriptAct;
  QAction* showFilterEditAct;
  /////////// Actions Menu Edit  /////////////////////
  QAction *suspendEditModeAct;
	/////////// Actions Menu Render /////////////////////
	QActionGroup *renderModeGroupAct;
	QAction *renderBboxAct;
	QAction *renderModePointsAct;
	QAction *renderModeWireAct;
	QAction *renderModeHiddenLinesAct;
	QAction *renderModeFlatLinesAct;
	QAction *renderModeFlatAct;
	QAction *renderModeSmoothAct;
	QAction *renderModeTextureAct;
	QAction *setDoubleLightingAct;
	QAction *setFancyLightingAct;
  QAction *setLightAct;
	QAction *backFaceCullAct;
  QAction *setSelectFaceRenderingAct;
  QAction *setSelectVertRenderingAct;

	QActionGroup *colorModeGroupAct;
	QAction *colorModeNoneAct;
        QAction *colorModePerMeshAct;
	QAction *colorModePerVertexAct;
	QAction *colorModePerFaceAct;
	///////////Actions Menu View ////////////////////////
	QAction *fullScreenAct;
	QAction *showToolbarStandardAct;
	QAction *showToolbarRenderAct;
	QAction *showInfoPaneAct;
	QAction *showTrackBallAct;
	QAction *resetTrackBallAct;
	QAction *showLayerDlgAct;
	QAction *showRasterAct;
	///////////Actions Menu Windows /////////////////////
	QAction *windowsTileAct;
	QAction *windowsCascadeAct;
	QAction *windowsNextAct;
	QAction *closeAllAct;
	QAction *setSplitHAct;
	QAction *setSplitVAct;
    QActionGroup *setSplitGroupAct;
	QAction *setUnsplitAct;
	///////////Actions Menu Windows -> Split/UnSplit from Handle ////////////////////////
	QActionGroup *splitGroupAct;
	QActionGroup *unsplitGroupAct;	

	QAction *splitUpAct;
	QAction *splitDownAct;

	QAction *unsplitUpAct;
	QAction *unsplitDownAct;

	QAction *splitRightAct;
	QAction *splitLeftAct;

	QAction *unsplitRightAct;
	QAction *unsplitLeftAct;

	///////////Actions Menu Windows -> View From ////////////////////////
	QActionGroup *viewFromGroupAct;
	QAction *viewTopAct;
	QAction *viewBottomAct;
	QAction *viewLeftAct;
	QAction *viewRightAct;
	QAction *viewFrontAct;
	QAction *viewBackAct;
  QAction *viewFromMeshAct;
  QAction *viewFromRasterAct;
	QAction *viewFromFileAct;

	///////////Actions Menu Windows -> Link/Copy/Paste View ////////////////////////
public:
	QAction *linkViewersAct;
private:
	QAction *copyShotToClipboardAct;
	QAction *pasteShotFromClipboardAct;

	///////////Actions Menu Preferences /////////////////
	QAction *setCustomizeAct;
	///////////Actions Menu Help ////////////////////////
	QAction *aboutAct;
	QAction *aboutPluginsAct;
	QAction *submitBugAct;
	QAction *onlineHelpAct;
	QAction *onscreenHelpAct;
	QAction *checkUpdatesAct;
	////////////////////////////////////////////////////
};


/// Event filter that is installed to intercept the open events sent directly by the Operative System
class FileOpenEater : public QObject
{
  Q_OBJECT

public:
  FileOpenEater(MainWindow *_mainWindow)
  {
    mainWindow= _mainWindow;
    noEvent=true;
  }

  MainWindow *mainWindow;
  bool noEvent;

protected:

  bool eventFilter(QObject *obj, QEvent *event)
  {
    if (event->type() == QEvent::FileOpen) {
      noEvent=false;
      QFileOpenEvent *fileEvent = static_cast<QFileOpenEvent*>(event);
      mainWindow->importMesh(fileEvent->file());
      return true;
    } else {
      // standard event processing
      return QObject::eventFilter(obj, event);
    }
  }
};

#endif
#endif
