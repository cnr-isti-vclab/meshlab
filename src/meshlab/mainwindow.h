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
Revision 1.40  2005/12/04 14:45:30  glvertex
gla now is a local variable used only if needed
texture button now works properly

Revision 1.39  2005/12/04 02:44:39  davide_portelli
Added texture icon in toolbar

Revision 1.38  2005/12/04 00:22:46  cignoni
Switched from progresBar widget to progressbar dialog

Revision 1.37  2005/12/03 23:40:31  davide_portelli
Added FullScreen menu and TrackBall->Reset trackBall

Revision 1.36  2005/12/03 22:49:46  cignoni
Added copyright info

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

#define MAXRECENTFILES 4

class QAction;
class QActionGroup;
class QMenu;
class QScrollArea;
class QSignalMapper;
class QProgressDialog;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow();
  static bool QCallBack(const int pos, const char * str);
  
private slots:

	//////////// Slot Menu File //////////////////////
	void open(QString fileName=QString());
	void openRecentFile();							
	bool saveAs();
	bool saveSnapshot();
	void applyImportExport();
	///////////Slot Menu Filter ////////////////////////
	void applyFilter();
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
	void applyRenderMode();
	void applyColorMode();
	void toggleBackFaceCulling();
	///////////Slot Menu View ////////////////////////
	void fullScreen();
	void showToolbarFile();
	void showToolbarRender();
	void showLog();
	void showInfoPane();
	void showTrackBall();
	void resetTrackBall();
	///////////Slot Menu Windows /////////////////////
	void windowsTile();
	void windowsCascade();
	void updateWindowMenu();
	void updateMenus();
	///////////Slot Menu Preferences /////////////////
	void setCustomize();
	///////////Slot Menu Help ////////////////////////
	void about();
	void aboutPlugins();	




private:
	void createActions();
	void createMenus();
	void createToolBars();
	void loadPlugins();
	void keyPressEvent(QKeyEvent *);
	void updateRecentFileActions();				
	void setCurrentFile(const QString &fileName);			
	void addToMenu(QList<QAction *>, QMenu *menu, const char *slot);
 


	
	static QProgressDialog *qb;
	QWorkspace *workspace;
	QSignalMapper *windowMapper;
  GLArea *GLA(){return qobject_cast<GLArea *>(workspace->activeWindow()); }
	QDir pluginsDir;
	QStringList pluginFileNames;
	
	
	////////ToolBar//////////////
	QToolBar *mainToolBar;
	QToolBar *renderToolBar;


	///////// Menu /////////////
	QMenu *fileMenu;
	QMenu *filterMenu;
	//Render Menu and SubMenu ///
	QMenu *renderMenu;
	QMenu *renderModeMenu;
	QMenu *lightingModeMenu;
	QMenu *textureModeMenu;
	QMenu *colorModeMenu;
	//View Menu and SubMenu /////
	QMenu *viewMenu;
	QMenu *trackBallMenu;
	QMenu *logMenu;
	QMenu *toolBarMenu;
	////////////////////////
	QMenu *windowsMenu;
	QMenu *preferencesMenu; 
	QMenu *helpMenu;


	//////////// Action Menu File //////////////////////
	QAction *openAct;
	QAction *saveAsAct;
	QAction *saveSnapshotAct;
	QAction *recentFileActs[MAXRECENTFILES];
	QAction *separatorAct;										
	QAction *exitAct;
	/////////// Action Menu Render /////////////////////
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
	///////////Action Menu View ////////////////////////
	QAction *fullScreenAct;
	QAction *showToolbarStandardAct;
	QAction *showToolbarRenderAct;
	QAction *showLogAct;
	QAction *showInfoPaneAct;
	QAction *showTrackBallAct;
	QAction *resetTrackBallAct;
	///////////Action Menu Windows /////////////////////
	QAction *windowsTileAct;
	QAction *windowsCascadeAct;
	QAction *closeAct;
	QAction *closeAllAct;
	///////////Action Menu Preferences /////////////////
	QAction *setCustomizeAct;
	///////////Action Menu Help ////////////////////////
	QAction *aboutAct;
	QAction *aboutPluginsAct;
	/////////////////////////////////////////////
	vector<QAction *> TotalRenderList;
	////////////////////////////////////////////
};

#endif
