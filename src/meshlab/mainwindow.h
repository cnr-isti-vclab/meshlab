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

#include "../common/pluginmanager.h"
#include "../common/scriptinterface.h"

#include <wrap/qt/qt_thread_safe_memory_info.h>

#include "glarea.h"
#include "layerDialog.h"
#include "stdpardialog.h"
#include "xmlstdpardialog.h"
#include "xmlgeneratorgui.h"
#include "multiViewer_Container.h"
#include "ml_render_gui.h"

#include <QtScript>
#include <QDir>
#include <QMainWindow>
#include <QMdiArea>
#include <QStringList>
#include <QColorDialog>
#include <QMdiSubWindow>
#include <QSplitter>

#define MAXRECENTFILES 4

class QAction;
class QActionGroup;
class QMenu;
class QScrollArea;
class QSignalMapper;
class QProgressDialog;
class QNetworkAccessManager;
class QNetworkReply;
class QToolBar;

class MainWindowSetting
{
public:
    static void initGlobalParameterSet(RichParameterSet* gblset);
    void updateGlobalParameterSet( RichParameterSet& rps );

    std::ptrdiff_t maxgpumem;
    inline static QString maximumDedicatedGPUMem() {return "MeshLab::System::maxGPUMemDedicatedToGeometry";}

    bool permeshtoolbar;
    inline static QString perMeshRenderingToolBar() {return "MeshLab::GUI::perMeshToolBar";}

    bool highprecision;
    inline static QString highPrecisionRendering() {return "MeshLab::System::highPrecisionRendering";}

    size_t perbatchprimitives;
    inline static QString perBatchPrimitives() {return "MeshLab::System::perBatchPrimitives";}

    std::ptrdiff_t maxTextureMemory;
    inline static QString maxTextureMemoryParam()  {return "MeshLab::System::maxTextureMemory";}
};

class MainWindow : public QMainWindow, public MainWindowInterface
{
    Q_OBJECT

public:
    // callback function to execute a filter
    void executeFilter(QAction *action, RichParameterSet &srcpar, bool isPreview = false);
    //parexpval is a string map containing the parameter expression values set in the filter's dialog.
    //These parameter expression values will be evaluated when the filter will start.
    void executeFilter(MeshLabXMLFilterContainer*,const QMap<QString,QString>& parexpval , bool  isPreview = false);


    MainWindow();
    ~MainWindow();
    static bool QCallBack(const int pos, const char * str);
    //const QString appName() const {return tr("MeshLab v")+appVer(); }
    //const QString appVer() const {return tr("1.3.2"); }
    MainWindowSetting mwsettings;
signals:
    void dispatchCustomSettings(RichParameterSet& rps);
    void filterExecuted();
    void updateLayerTable();

protected:
	void showEvent(QShowEvent *event);

private slots:
    void newProject(const QString& projName = QString());
    void saveProject();
    
    void meshAdded(int mid);
    void meshRemoved(int mid);


public slots:
    bool importMeshWithLayerManagement(QString fileName=QString());
    bool importRaster(const QString& fileImg = QString());
    bool openProject(QString fileName=QString());
    bool appendProject(QString fileName=QString());
    void updateCustomSettings();
	void updateLayerDialog();
	bool addRenderingDataIfNewlyGeneratedMesh(int meshid);

	void updateRenderingDataAccordingToActions(int meshid, const QList<MLRenderingAction*>& acts);
	void updateRenderingDataAccordingToActionsToAllVisibleLayers(const QList<MLRenderingAction*>& acts);
	void updateRenderingDataAccordingToActions(int meshid, MLRenderingAction* act, QList<MLRenderingAction*>& acts);
	void updateRenderingDataAccordingToAction(int meshid, MLRenderingAction* act);
	void updateRenderingDataAccordingToActionToAllVisibleLayers(MLRenderingAction* act);
	void updateRenderingDataAccordingToAction(int meshid, MLRenderingAction* act,bool);
	void updateRenderingDataAccordingToActions(QList<MLRenderingGlobalAction*> actlist);

private:
	void updateRenderingDataAccordingToActionsCommonCode(int meshid, const QList<MLRenderingAction*>& acts);
	void updateRenderingDataAccordingToActionCommonCode(int meshid, MLRenderingAction* act);

private slots:
	void documentUpdateRequested();
    bool importMesh(QString fileName=QString(), bool isareload = false);
    void endEdit();
    void updateDocumentScriptBindings();
    void loadAndInsertXMLPlugin(const QString& xmlpath,const QString& scriptname);
    void postFilterExecution(/*MeshLabXMLFilterContainer* mfc*/);
    //void evaluateExpression(const Expression& exp,Value** res);
    void updateProgressBar(const int pos,const QString& text);
    void updateTexture(int meshid);
public:

    bool exportMesh(QString fileName,MeshModel* mod,const bool saveAllPossibleAttributes);
    bool loadMesh(const QString& fileName,MeshIOInterface *pCurrentIOPlugin,MeshModel* mm,int& mask,RichParameterSet* prePar,const Matrix44m &mtr=Matrix44m::Identity(), bool isareload = false);

	void computeRenderingDataOnLoading(MeshModel* mm,bool isareload);

	bool loadMeshWithStandardParams(QString& fullPath, MeshModel* mm, const Matrix44m &mtr = Matrix44m::Identity(),bool isareload = false);
    
    void defaultPerViewRenderingData(MLRenderingData& dt) const;
    void getRenderingData(int mid,MLRenderingData& dt) const;
    void setRenderingData(int mid,const MLRenderingData& dt);

	unsigned int viewsRequiringRenderingActions(int meshid,MLRenderingAction* act);

    void updateSharedContextDataAfterFilterExecution(int postcondmask,int fclasses,bool& newmeshcreated);

private slots:
	void closeCurrentDocument();
    //////////// Slot Menu File //////////////////////
    void reload();
    void reloadAllMesh();
    void openRecentMesh();
    void openRecentProj();
    bool saveAs(QString fileName = QString(),const bool saveAllPossibleAttributes = false);
    bool save(const bool saveAllPossibleAttributes = false);
    bool saveSnapshot();
    void changeFileExtension(const QString&);
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

    void applyRenderMode();
    void applyDecorateMode();

    void switchOffDecorator(QAction* );
    ///////////Slot Menu View ////////////////////////
    void fullScreen();
    void showToolbarFile();
    void showInfoPane();
    void showTrackBall();
    void resetTrackBall();
    void showLayerDlg(bool visible);
    void showRaster();
    ///////////Slot Menu Windows /////////////////////
    void updateWindowMenu();
    void updateMenus();
    void updateSubFiltersMenu(const bool createmenuenabled,const bool validmeshdoc);
    void updateMenuItems(QMenu* menu,const bool enabled);
    void updateStdDialog();
    void updateXMLStdDialog();
    void enableDocumentSensibleActionsContainer(const bool enable);
    //void updatePerViewApplicationStatus();
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
	void sendUsAMail();
	void checkForUpdates(bool verboseFlag=true);
    //void updatePerViewApplicationStatus();

    void dropEvent ( QDropEvent * event );
    void dragEnterEvent(QDragEnterEvent *);
    void connectionDone(QNetworkReply *reply);
    void sendHistory();
    ///////////Solt Wrapper for QMdiArea //////////////////
    void wrapSetActiveSubWindow(QWidget* window);
	void switchCurrentContainer(QMdiSubWindow *);
    void scriptCodeExecuted(const QScriptValue& val,const int time,const QString& output);
    
    void updateGPUMemBar(int,int);

	void updateLog();
private:
    void addRenderingSystemLogInfo(unsigned mmid);
    int longestActionWidthInMenu(QMenu* m,const int longestwidth);
    int longestActionWidthInMenu( QMenu* m);
    int longestActionWidthInAllMenus();
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

    void initDocumentMeshRenderState(MeshLabXMLFilterContainer* mfc);
    void initDocumentRasterRenderState(MeshLabXMLFilterContainer* mfc);
	void setCurrentMeshBestTab();


    QNetworkAccessManager *httpReq;
    /*QBuffer myLocalBuf;*/
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
    vcg::QtThreadSafeMemoryInfo* gpumeminfo;
    QProgressBar* nvgpumeminfo;

    //QMap<QThread*,Env*> envtobedeleted;

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


    //vcg::GLW::TextureMode getBestTextureRenderModePerMesh(const int meshid);
    //void setBestTextureModePerMesh(RenderModeAction* textact,const int meshid, RenderMode& rm);
public:
    PluginManager PM;

    MeshDocument *meshDoc() {
        if (currentViewContainer() != NULL)
			return &currentViewContainer()->meshDoc;
		return NULL;
    }

    inline vcg::QtThreadSafeMemoryInfo* memoryInfoManager() const {return gpumeminfo;} 
    const RichParameterSet& currentGlobalPars() const { return currentGlobalParams; }
    RichParameterSet& currentGlobalPars() { return currentGlobalParams; }
    const RichParameterSet& defaultGlobalPars() const { return defaultGlobalParams; }

    GLArea *GLA() const {
        //	  if(mdiarea->currentSubWindow()==0) return 0;
        MultiViewer_Container *mvc = currentViewContainer();
        if(!mvc) return 0;
        return  mvc->currentgla;
    }

    MultiViewer_Container* currentViewContainer() const {
  //     /* MultiViewer_Container *mvc = dynamic_cast<MultiViewer_Container *>(mdiarea->currentSubWindow());
  //      if(mvc) return mvc;*/
  //      if(mdiarea->currentSubWindow()!=0 )
		//{
  //          Splitter* split = reinterpret_cast<Splitter*>(mdiarea->currentSubWindow()->widget());
		//	if (split->isMultiViewerContainer())
		//		return reinterpret_cast<MultiViewer_Container*>(split);
  //      }
  //      /*QList<QMdiSubWindow *> subwinList=mdiarea->subWindowList();
  //      foreach(QMdiSubWindow *subwinPtr,subwinList)
  //      {
  //          MultiViewer_Container *mvc = dynamic_cast<MultiViewer_Container *>(subwinPtr);
  //          if(mvc) return mvc;
  //          if(mvc==0 && subwinPtr!=0){
  //              mvc = dynamic_cast<MultiViewer_Container *>(subwinPtr->widget());
  //              if(mvc) return mvc;
  //          }
  //      }*/

        return _currviewcontainer;
    }
    
    const PluginManager& pluginManager() const { return PM; }

    static QStatusBar *&globalStatusBar()
    {
        static QStatusBar *_qsb=0;
        return _qsb;
    }
    
    void setHandleMenu(QPoint point, Qt::Orientation orientation, QSplitter *origin);
    QMenu* meshLayerMenu() { return filterMenuMeshLayer; }
    QMenu* rasterLayerMenu() { return filterMenuRasterLayer; }

private:
    //the xml filters run in a different thread. The xmlfiltertimer starts on executeFilter and stops on postFilterExecution
    //function linked to the thread finished signal.
    QTime xmlfiltertimer;
    WordActionsMapAccessor wama;
    //////// ToolBars ///////////////
    QToolBar *mainToolBar;
    QToolBar *decoratorToolBar;
    QToolBar *editToolBar;
    QToolBar *filterToolBar;
    QToolBar *searchToolBar;
	MLRenderingGlobalToolbar* globrendtoolbar;
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

    //View Menu and SubMenu //////
    QMenu *viewMenu;
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
    SearchMenu* searchMenu;

    //////////// Actions Menu File ///////////////////////
    QAction *newProjectAct;
    QAction *openProjectAct, *appendProjectAct, *saveProjectAct;
    QAction *importMeshAct, *exportMeshAct, *exportMeshAsAct;
    QAction *importRasterAct;
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
   
    ///////////Actions Menu View ////////////////////////
    QAction *fullScreenAct;
    QAction *showToolbarStandardAct;
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
	QAction *viewTopYAct;
	QAction *viewBottomYAct;
	QAction *viewLeftYAct;
	QAction *viewRightYAct;
	QAction *viewFrontYAct;
	QAction *viewBackYAct;
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
    static QString getDecoratedFileName(const QString& name);

	MultiViewer_Container* _currviewcontainer;
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
            mainWindow->importMeshWithLayerManagement(fileEvent->file());
            qDebug("event fileopen %s",qPrintable(fileEvent->file()));
            return true;
        } else {
            // standard event processing
            return QObject::eventFilter(obj, event);
        }
    }
};


#endif
#endif
