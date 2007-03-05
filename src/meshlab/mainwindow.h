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
/****************************************************************************
  History
$Log$
Revision 1.81  2007/03/05 12:23:01  cignoni
v.1.0.0 string

Revision 1.80  2007/03/03 02:03:25  cignoni
Reformatted lower bar, added number of selected faces. Updated about dialog

Revision 1.79  2007/02/28 00:05:13  cignoni
Added Bug submitting menu

Revision 1.78  2007/02/26 12:03:44  cignoni
Added Help online and check for updates

Revision 1.77  2007/02/08 16:04:18  cignoni
Corrected behaviour of edit actions

Revision 1.76  2007/01/13 02:00:51  cignoni
Ver 0.9.1b

Revision 1.75  2006/12/21 21:24:54  cignoni
version 0.9.1

Revision 1.74  2006/12/13 17:37:02  pirosu
Added standard plugin window support

Revision 1.73  2006/12/12 11:16:08  cignoni
changed version string 0.9RC -> 0.9

Revision 1.72  2006/12/06 21:38:56  cignoni
0.9RC

Revision 1.71  2006/11/29 00:53:43  cignoni
Improved logging and added web based version checking

Revision 1.70  2006/11/08 01:04:48  cignoni
First version with http communications

Revision 1.69  2006/10/26 12:07:12  corsini
add lighting properties option

Revision 1.68  2006/06/27 08:07:42  cignoni
Restructured plugins interface for simplifying the server

Revision 1.67  2006/06/18 21:27:49  cignoni
Progress bar redesigned, now integrated in the workspace window

Revision 1.66  2006/06/16 01:26:07  cignoni
Added Initial Filter Script Dialog

Revision 1.65  2006/06/15 13:05:57  cignoni
added Filter History Dialogs

Revision 1.64  2006/06/12 15:20:44  cignoni
Initial Dragdrop support (still not working,,,)

Revision 1.63  2006/05/25 04:57:45  cignoni
Major 0.7 release. A lot of things changed. Colorize interface gone away, Editing and selection start to work.
Optional data really working. Clustering decimation totally rewrote. History start to work. Filters organized in classes.

Revision 1.62  2006/04/12 15:12:18  cignoni
Added Filter classes (cleaning, meshing etc)

Revision 1.61  2006/02/17 11:17:23  glvertex
- Moved closeAction in FileMenu
- Minor changes

Revision 1.60  2006/02/01 12:44:42  glvertex
- Disabled EDIT menu when no editing tools loaded
- Solved openig bug when running by command line

Revision 1.59  2006/01/19 11:54:15  fmazzant
cleaned up code & cleaned up history log

Revision 1.58  2006/01/19 11:21:12  fmazzant
deleted old savemaskobj & old MaskObj

Revision 1.57  2006/01/17 13:47:45  fmazzant
update interface meshio : formats -> importFormats() & exportFormts

Revision 1.56  2006/01/14 00:51:06  davide_portelli
A little change

Revision 1.55  2006/01/07 11:04:49  glvertex
Added Apply Last Filter action

****************************************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDir>
#include <QMainWindow>
#include <QWorkspace>
#include <QStringList>
#include <QColorDialog>
#include "meshmodel.h"
#include "glarea.h"
#include "stdpardialog.h"

#define MAXRECENTFILES 4

class QAction;
class QActionGroup;
class QMenu;
class QScrollArea;
class QSignalMapper;
class QProgressDialog;
class QHttp;
class MeshlabStdDialog;
class MeshlabStdDialogFrame;



class MainWindow : public QMainWindow,MainWindowInterface
{
	Q_OBJECT

public:
	// callback function to execute a filter
  void executeFilter(QAction *action,FilterParameter *srcpar);

  MainWindow();
   static bool QCallBack(const int pos, const char * str);
	 const QString appName() const {return tr("MeshLab v")+appVer(); }
   const QString appVer() const {return tr("1.0.0"); }

  // MaskObj maskobj;

public slots:
 void open(QString fileName=QString());

  
private slots:

	//////////// Slot Menu File //////////////////////
  void reload();
	void openRecentFile();							
	bool saveAs();
	bool saveSnapshot(); 
	///////////Slot Menu Edit ////////////////////////
  void applyEditMode();
	void endEditMode();
	///////////Slot Menu Filter ////////////////////////
	void applyFilter();
	void applyLastFilter();
	void runFilterScript();
	void showFilterScript();
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
	void setLightingProperties();
	void setColorMode(QAction *qa);
	void applyRenderMode();
	void applyColorMode();
	void toggleBackFaceCulling();
  void toggleSelectionRendering();
	void applyDecorateMode();
	///////////Slot Menu View ////////////////////////
	void fullScreen();
	void showToolbarFile();
	void showToolbarRender();
	void showInfoPane();
	void showTrackBall();
	void resetTrackBall();
	///////////Slot Menu Windows /////////////////////
	void updateWindowMenu();
	void updateMenus();
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

private:
	void createStdPluginWnd();
	void createActions();
	void createMenus();
	void createToolBars();
	void loadPlugins();
	void keyPressEvent(QKeyEvent *);
	void updateRecentFileActions();				
	void setCurrentFile(const QString &fileName);			
	void addToMenu(QList<QAction *>, QMenu *menu, const char *slot);
	//void LoadKnownFilters(QStringList &filters, QHash<QString, int> &allKnownFormats, int type);


	QHttp *httpReq;
  QBuffer myLocalBuf;
  int idHost;
  int idGet;
  bool VerboseCheckingFlag;
	  
	static QProgressBar *qb;
	QWorkspace *workspace;
	QSignalMapper *windowMapper;
  QDir pluginsDir;
	QStringList pluginFileNames;
	std::vector<MeshIOInterface*> meshIOPlugins;
  QList<QAction *> editActionList;
	QByteArray toolbarState;								//stato delle toolbar e dockwidgets
	MeshlabStdDialog *stddialog;

public:
  GLArea *GLA() const {return qobject_cast<GLArea *>(workspace->activeWindow()); }
  QMap<QString, QAction *> filterMap; // a map to retrieve an action from a name. Used for playing filter scripts.
  static QStatusBar *&globalStatusBar()
  {
    static QStatusBar *_qsb=0;
    return _qsb;
  }
private:	
	//////// ToolBars ///////////////
	QToolBar *mainToolBar;
	QToolBar *renderToolBar;
	QToolBar *editToolBar;

	///////// Menus ///////////////
	QMenu *fileMenu;
	QMenu *filterMenu;
	QMenu *filterMenuSelect;
  QMenu *filterMenuClean;
  QMenu *filterMenuRemeshing;
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


	//////////// Actions Menu File ///////////////////////
	QAction *openAct;
	QAction *closeAct;
	QAction *reloadAct;
	QAction *saveAsAct;
	QAction *saveSnapshotAct;
	QAction *lastFilterAct;
	QAction *runFilterScriptAct;
	QAction *showFilterScriptAct;
	QAction *recentFileActs[MAXRECENTFILES];
	QAction *separatorAct;										
	QAction *exitAct;
	/////////// Actions Menu Edit  /////////////////////
  QAction *endEditModeAct;
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
	QAction *setLightingPropertiesAct;
	QAction *setLightAct;
	QAction *backFaceCullAct;
  QAction *setSelectionRenderingAct;

	QActionGroup *colorModeGroupAct;
	QAction *colorModeNoneAct;
	QAction *colorModePerVertexAct;
	QAction *colorModePerFaceAct;
	///////////Actions Menu View ////////////////////////
	QAction *fullScreenAct;
	QAction *showToolbarStandardAct;
	QAction *showToolbarRenderAct;
	QAction *showInfoPaneAct;
	QAction *showTrackBallAct;
	QAction *resetTrackBallAct;
	///////////Actions Menu Windows /////////////////////
	QAction *windowsTileAct;
	QAction *windowsCascadeAct;
	QAction *windowsNextAct;
	QAction *closeAllAct;
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
	QList<QAction *> TotalDecoratorsList;
	////////////////////////////////////////////////////
};
#endif
