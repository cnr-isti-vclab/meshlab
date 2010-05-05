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

#include <QDir>
#include <QMainWindow>
#include <QMdiArea>
#include <QStringList>
#include <QColorDialog>
#include "../common/pluginmanager.h"
#include "glarea.h"
#include "stdpardialog.h"
#include "multiViewer_Container.h"

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
	//void executeFilter(QAction *action, RichParameterSet &srcpar, bool isPreview);

  MainWindow();
	static bool QCallBack(const int pos, const char * str);
	const QString appName() const {return tr("MeshLab v")+appVer(); }
	const QString appVer() const {return tr("1.2.3b"); }

signals:
	void dispatchCustomSettings(RichParameterSet& rps);

public slots:

	bool open(QString fileName=QString(), GLArea *gla=0);
	bool openIn(QString fileName=QString());
	bool openProject(QString fileName=QString());
	void saveProject();
	void delCurrentMesh();
	void updateGL();
	void endEdit();
	void updateCustomSettings();

private slots:

	//////////// Slot Menu File //////////////////////
	void reload();
	void openRecentFile();
	bool saveAs(QString fileName = QString());
	bool save();
	//bool saveSnapshot();
	/////////////Slot Menu Edit ////////////////////////
	//void applyEditMode();
	//void suspendEditMode();
	///////////Slot Menu Filter ////////////////////////
	//void startFilter();
	//void applyLastFilter();
	//void runFilterScript();
	//void showFilterScript();
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
	void loadShot();
  //void applyDecorateMode();
	///////////Slot Menu View ////////////////////////
	void fullScreen();
	void showToolbarFile();
	void showToolbarRender();
	void showInfoPane();
	void showTrackBall();
	void resetTrackBall();
	void showLayerDlg();
	///////////Slot Menu Windows /////////////////////
	void updateWindowMenu();
	void updateMenus();
	void updateStdDialog();

	///////////Slot Menu Preferences /////////////////
	//void setCustomize();
	void setSplit(QAction *qa);
	void setUnsplit();
	///////////Slot Menu Help ////////////////////////
	/*void about();
	void aboutPlugins();*/
	void helpOnline();
	void helpOnscreen();
	void submitBug();
	void checkForUpdates(bool verboseFlag=true);

	///////////Slot General Purpose ////////////////////////

	void dropEvent ( QDropEvent * event );
	void dragEnterEvent(QDragEnterEvent *);
	void connectionDone(bool status);

	///////////Solt Wrapper for QMdiArea //////////////////
	void wrapSetActiveSubWindow(QWidget* window);
private:
    void createStdPluginWnd(); // this one is
	void initGlobalParameters();
    void createActions();
	void createMenus();
	/*void fillFilterMenu();
	void fillDecorateMenu();*/
	void fillRenderMenu();
	//void fillEditMenu();
	void createToolBars();
	void loadMeshLabSettings();
	void loadPlugins();
	void keyPressEvent(QKeyEvent *);
	void updateRecentFileActions();
	void setCurrentFile(const QString &fileName);
	void addToMenu(QList<QAction *>, QMenu *menu, const char *slot);


	QHttp *httpReq;
	QBuffer myLocalBuf;
	int idHost;
	int idGet;
	bool VerboseCheckingFlag;

	MeshlabStdDialog *stddialog;
	static QProgressBar *qb;
	QMdiArea *mdiarea;
	QSignalMapper *windowMapper;
	QDir pluginsDir;
	QStringList pluginFileNames;

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
	GLArea *GLA() const {
	  if(mdiarea->currentSubWindow()==0) return 0;
	  MultiViewer_Container *mvc = qobject_cast<MultiViewer_Container *>(mdiarea->currentSubWindow());
	  if(!mvc) 
		  mvc = qobject_cast<MultiViewer_Container *>(mdiarea->currentSubWindow()->widget());
	  GLArea *glw =  (GLArea*)(mvc->currentView());//qobject_cast<GLArea*>qobject_cast(mvc->currentView());
	  if(glw) 
		  return glw;
	  else return 0;
	}

	const PluginManager& pluginManager() const { return PM; }

  //QMap<QString, QAction *> filterMap; // a map to retrieve an action from a name. Used for playing filter scripts.
  static QStatusBar *&globalStatusBar()
  {
    static QStatusBar *_qsb=0;
    return _qsb;
  }
	//QMenu* layerMenu() { return filterMenuLayer; }

private:
	//////// ToolBars ///////////////
	QToolBar *mainToolBar;
	QToolBar *renderToolBar;
	/*QToolBar *editToolBar;
	QToolBar *filterToolBar;*/

	///////// Menus ///////////////
	QMenu *fileMenu;
	QMenu *fileMenuNew;
	/*QMenu *filterMenu;
	QMenu *filterMenuSelect;
  QMenu *filterMenuClean;
  QMenu *filterMenuRemeshing;
	QMenu *filterMenuColorize;
  QMenu *filterMenuSmoothing;
  QMenu *filterMenuQuality;
	QMenu *filterMenuNormal;
  QMenu *filterMenuLayer;
  QMenu *filterMenuRangeMap;
  QMenu *filterMenuPointSet;
  QMenu *filterMenuSampling;
	QMenu *filterMenuTexture;

	QMenu *editMenu;*/

  //Render Menu and SubMenu ////
	QMenu *shadersMenu;
	QMenu *renderMenu;
	QMenu *renderModeMenu;
	QMenu *lightingModeMenu;
	QMenu *colorModeMenu;
	QMenu *splitModeMenu;

	//View Menu and SubMenu //////
	QMenu *viewMenu;
	QMenu *trackBallMenu;
	QMenu *logMenu;
	QMenu *toolBarMenu;
	//////////////////////////////
	QMenu *windowsMenu;
	QMenu *preferencesMenu;
	QMenu *helpMenu;


	//////////// Actions Menu File ///////////////////////
	QAction *openAct;
	QAction *openInAct,*openProjectAct;
	QAction *closeAct;
	QAction *reloadAct;
	QAction *saveAct,*saveAsAct,*saveProjectAct;
	QAction *saveSnapshotAct;
	QAction *lastFilterAct;
	QAction *runFilterScriptAct;
	QAction *showFilterScriptAct;
	QAction *recentFileActs[MAXRECENTFILES];
	QAction *separatorAct;
	QAction *exitAct;
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

	QAction *loadShotAct;
	///////////Actions Menu View ////////////////////////
	QAction *fullScreenAct;
	QAction *showToolbarStandardAct;
	QAction *showToolbarRenderAct;
	QAction *showInfoPaneAct;
	QAction *showTrackBallAct;
	QAction *resetTrackBallAct;
	QAction *showLayerDlgAct;
	///////////Actions Menu Windows /////////////////////
	QAction *windowsTileAct;
	QAction *windowsCascadeAct;
	QAction *windowsNextAct;
	QAction *closeAllAct;
	///////////Actions Menu Preferences /////////////////
	QAction *setCustomizeAct;
	QAction *setSplitHAct;
	QAction *setSplitVAct;
    QActionGroup *setSplitGroupAct;
	QAction *setUnsplitAct;

	///////////Actions Menu Help ////////////////////////
	QAction *aboutAct;
	QAction *aboutPluginsAct;
	QAction *submitBugAct;
	QAction *onlineHelpAct;
	QAction *onscreenHelpAct;
	QAction *checkUpdatesAct;
	////////////////////////////////////////////////////
};

class FileOpenEater : public QObject
{
Q_OBJECT

public:
FileOpenEater() {noEvent=true;}
MainWindow *mainWindow;
bool noEvent;

protected:

bool eventFilter(QObject *obj, QEvent *event)
 {
	 if (event->type() == QEvent::FileOpen) {
						noEvent=false;
						QFileOpenEvent *fileEvent = static_cast<QFileOpenEvent*>(event);
						mainWindow->open(fileEvent->file());
						// QMessageBox::information(0,"Meshlab",fileEvent->file());
						return true;
        } else {
             // standard event processing
             return QObject::eventFilter(obj, event);
         }
     }
 };

#endif
#endif
