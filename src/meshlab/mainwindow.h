/****************************************************************************
**
** Copyright (C) 2005-2005 Trolltech AS. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
** information about Qt Commercial License Agreements.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
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
class QProgressBar;

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
	void RenderBbox();
	void RenderPoint();
	void RenderWire();
	void RenderFlat();
	void RenderFlatLine();
	void RenderHiddenLines();
	void RenderSmooth();
	void SetLight();
	void SetDoubleLighting();
	void SetFancyLighting();
	void applyRenderMode();
	void applyColorMode();
	///////////Slot Menu View ////////////////////////
	void showToolbarFile();
	void showToolbarRender();
	void showLog();
	void showInfoPane();
	void showTrackBall();
	///////////Slot Menu Windows /////////////////////
	void windowsTile();
	void windowsCascade();
	void updateWindowMenu();
	void updateMenus();
	///////////Slot Menu Preferences /////////////////
	void SetCustomize();
	///////////Slot Menu Help ////////////////////////
	void about();
	void aboutPlugins();	




private:
	void createActions();
	void createMenus();
	void createToolBars();
	void loadPlugins();
	void updateRecentFileActions();				
	void setCurrentFile(const QString &fileName);			
	void addToMenu(QObject *plugin, const QStringList &texts, QMenu *menu,
								const char *member, QActionGroup *actionGroup = 0,bool chackable = false);
 


	
	static QProgressBar *qb;
	QWorkspace *workspace;
	QSignalMapper *windowMapper;
  GLArea *GLA(){return qobject_cast<GLArea *>(workspace->activeWindow()); }
	GLArea *gla;
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
	QActionGroup *renderModeGroup;
	QAction *renderBboxAct;
	QAction *renderModePointsAct;
	QAction *renderModeWireAct;
	QAction *renderModeHiddenLinesAct;
	QAction *renderModeFlatLinesAct;
	QAction *renderModeFlatAct;
	QAction *renderModeSmoothAct;
	QAction *setDoubleLightingAct;
	QAction *setFancyLightingAct;
	QAction *setLightAct;
	///////////Action Menu View ////////////////////////
	QAction *showToolbarStandardAct;
	QAction *showToolbarRenderAct;
	QAction *showLogAct;
	QAction *showInfoPaneAct;
	QAction *showTrackBallAct;
	///////////Action Menu Windows /////////////////////
	QAction *windowsTileAct;
	QAction *windowsCascadeAct;
	QAction *closeAct;
	QAction *closeAllAct;
	///////////Action Menu Preferences /////////////////
	QAction *setCustomizeAct;
	///////////Action Menu Help ////////////////////////
	QAction *aboutAct;
	QAction *aboutQtAct;
	QAction *aboutPluginsAct;
	/////////////////////////////////////////////
	vector<QAction *> TotalRenderList;
	////////////////////////////////////////////
};

#endif
