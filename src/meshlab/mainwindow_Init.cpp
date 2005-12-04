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
/****************************************************************************
History

$Log$
Revision 1.15  2005/12/04 17:47:18  davide_portelli
Added menu windows->Next and Shortcut "CTRL+PageDown"
Added reset trackbal Shortcut "CTRL+H"
Optimize fullscreen

Revision 1.14  2005/12/04 16:51:57  glvertex
Changed some action accelerator keys
Renamed preference menu and dialog

Revision 1.13  2005/12/04 15:25:22  glvertex
added texture action in menu render

Revision 1.12  2005/12/04 14:45:30  glvertex
gla now is a local variable used only if needed
texture button now works properly

Revision 1.11  2005/12/04 02:44:39  davide_portelli
Added texture icon in toolbar

Revision 1.10  2005/12/04 00:22:46  cignoni
Switched from progresBar widget to progressbar dialog

Revision 1.9  2005/12/03 23:53:27  cignoni
Re added filter and io plugins

Revision 1.8  2005/12/03 23:40:31  davide_portelli
Added FullScreen menu and TrackBall->Reset trackBall

Revision 1.7  2005/12/03 23:25:10  ggangemi
re-added meshcolorizeplugin support

Revision 1.6  2005/12/03 19:05:39  davide_portelli
Added About menu.

Revision 1.5  2005/12/03 17:04:34  glvertex
Added backface culling action and slots
Added shortcuts for fancy and double lighting

Revision 1.4  2005/12/03 16:07:14  glvertex
Added samples for core-plugin calls

Revision 1.3  2005/12/02 17:39:07  glvertex
modified plugin import code. old plugins have been disabled cause of new interface.

Revision 1.2  2005/12/02 11:57:59  glvertex
- show log
- show info area
- show trackball
- some renaming

Revision 1.1  2005/12/01 02:24:50  davide_portelli
Mainwindow Splitted----->[ mainwindow_Init.cpp ]&&[ mainwindow_RunTime.cpp ]

Revision 1.46  2005/11/30 16:26:56  cignoni
All the modification, restructuring seen during the 30/12 lesson...

Revision 1.45  2005/11/29 18:32:56  alemochi
Added customize menu to change colors of environment

Revision 1.44  2005/11/29 11:22:23  vannini
Added experimental snapshot saving function

Revision 1.43  2005/11/28 21:05:37  alemochi
Added menu preferences and configurable background

Revision 1.42  2005/11/28 01:06:04  davide_portelli
Now GLA contains a list of RenderMode, instead of a single RenderMode.
Thus it is possible to have more active RenderMode (MeshRenderInterface)
at the same time, and when there are many opened windows, the menù of rendering
is consisting.

Revision 1.41  2005/11/27 18:36:58  buzzelli
changed applyImportExport method in order to handle correctly the case of no opened subwindows

Revision 1.40  2005/11/27 04:09:53  glvertex
- Added full support for import/export plugins
- Added ViewLog Action and Slot (not working as well)
- Minor changes and clean up

Revision 1.39  2005/11/26 17:05:13  glvertex
Optimized SetLight method
Now using Log constants instead of int values in logging lines

Revision 1.37  2005/11/26 14:09:15  alemochi
Added double side lighting and fancy lighting (working only double side+fancy)

Revision 1.36  2005/11/25 18:15:10  ggangemi
Moved MeshColorizeInterface menu entries under "Color" sub-menu

Revision 1.35  2005/11/25 17:41:52  alemochi
Added categorization to render menu

Revision 1.34  2005/11/25 16:23:02  ggangemi
Added MeshColorizeInterface plugins support code

Revision 1.33  2005/11/25 16:02:28  davide_portelli
Added consistency of toolbar and menu with openGL

Revision 1.30  2005/11/25 12:58:58  alemochi
Added new icon that represent the changing of lighting state and modified function.

Revision 1.29  2005/11/25 11:55:59  alemochi
Added function to Enable/Disable lighting (work in progress)

Revision 1.28  2005/11/25 02:47:18  davide_portelli
Some cleanup

Revision 1.27  2005/11/24 15:46:57  davide_portelli
Added the check icon for menu Render->Show Normals

Revision 1.26  2005/11/24 14:59:31  davide_portelli
Correct a little bug in menu filter

Revision 1.25  2005/11/24 09:48:37  cignoni
changed invocation of applyfilter

Revision 1.24  2005/11/24 01:38:36  cignoni
Added new plugins intefaces, tested with shownormal render mode

Revision 1.23  2005/11/23 00:25:06  glvertex
Reverted plugin interface to prev version

Revision 1.22  2005/11/23 00:03:10  cignoni
Changed names view->render, added renderModeGroup to implement radio styles menu entries

Revision 1.21  2005/11/22 17:10:53  glvertex
MeshFilter Plugin STRONGLY reviewed and changed

Revision 1.20  2005/11/22 01:22:45  davide_portelli
Added the render menu.
I have rename the following variable:
viewModePoints      <====>    viewModePointsAct
viewModeWire        <====>    viewModeWireAct
viewModeLines       <====>    viewModeHiddenLinesAct
viewModeFlatLines  <====>    viewModeFlatLinesAct
viewModeFlat          <====>    viewModeFlatAct
viewModeSmooth    <====>    viewModeSmoothAct

Revision 1.19  2005/11/22 00:03:21  davide_portelli
Correct a little bug in menu filter

Revision 1.18  2005/11/21 22:08:25  cignoni
added a cast to a 8bit char from unicode using locale

Revision 1.17  2005/11/20 19:33:09  glvertex
Logging filters events (still working on...)

Revision 1.16  2005/11/20 18:54:33  davide_portelli
Improved menu behavior

Revision 1.15  2005/11/20 17:57:26  davide_portelli
Modification the menu:
- Menu disable when not there is active windows
- Menu enable when there is active windows

Revision 1.14  2005/11/20 04:34:34  davide_portelli
Adding in the file menù, the list of the last open file (Recent File).

Revision 1.13  2005/11/19 18:15:20  glvertex
- Some bug removed.
- Interface more friendly.
- Background.

Revision 1.12  2005/11/19 12:14:20  glvertex
Some cleanup and renaming

Revision 1.11  2005/11/19 04:59:12  davide_portelli
I have added the modifications to the menù view and to the menu windows:
- View->Toolbar->File ToolBar
- View->Toolbar->Render ToolBar
- windows->Close
- windows->Close All
- windows->and the list of the open windows

Revision 1.10  2005/11/18 18:25:35  alemochi
Rename function in glArea.h

Revision 1.9  2005/11/18 18:10:28  alemochi
Aggiunto slot cambiare la modalita' di rendering

Revision 1.8  2005/11/18 02:12:04  glvertex
- Added icons to the project file [meshlab.qrc]
- Enabled renderToolbar with icons

Revision 1.7  2005/11/18 00:36:50  davide_portelli
Added View->Toolbar and Windows->Tile and Windows->Cascade

Revision 1.6  2005/11/17 22:15:52  davide_portelli
Added menu View->Tile

Revision 1.5  2005/11/17 20:51:00  davide_portelli
Added Help->About Plugins

Revision 1.4  2005/11/16 23:18:54  cignoni
Added plugins management

Revision 1.3  2005/10/31 17:17:47  cignoni
Sketched the interface  of the odd/even refine function

Revision 1.2  2005/10/22 10:23:28  mariolatronico
on meshlab.pro added a lib dependency , on other files modified #include directive to allow compilation on case sensitive o.s.

Revision 1.1  2005/10/18 10:38:02  cignoni
First rough version. It simply load a mesh.

****************************************************************************/


#include <QtGui>
#include <QToolBar>
#include <QProgressBar>


#include "meshmodel.h"
#include "interfaces.h"
#include "mainwindow.h"
#include "glarea.h"
#include "plugindialog.h"
#include "customDialog.h"				


QProgressDialog *MainWindow::qb;

MainWindow::MainWindow()
{
	workspace = new QWorkspace(this);
	setCentralWidget(workspace);
	windowMapper = new QSignalMapper(this);
	
	// Permette di passare da una finestra all'altra e tenere aggiornato il workspace
	connect(windowMapper, SIGNAL(mapped(QWidget *)),workspace, SLOT(setActiveWindow(QWidget *)));
	
	// Quando si passa da una finestra all'altra aggiorna lo stato delle toolbar e dei menu
	connect(workspace, SIGNAL(windowActivated(QWidget *)),this, SLOT(updateMenus()));

	createActions();
	createMenus();
	createToolBars();
	updateMenus();
	addToolBar(mainToolBar);
	addToolBar(renderToolBar);
	setWindowTitle(tr("MeshLab v0.3"));
	loadPlugins();
	if(QCoreApplication::instance ()->argc()>1){
		open(QCoreApplication::instance ()->argv()[1]);
	}
	else{ 
		QTimer::singleShot(500, this, SLOT(open()));
	}
  qb=new QProgressDialog(this);
  qb->setMaximum(100);
  qb->setMinimum(0);
  qb->hide();
}

void MainWindow::createActions()
{
	//////////////Action Menu File //////////////////////////////////////////////////////////////
  openAct = new QAction(QIcon(":/images/open.png"),tr("&Open..."), this);
	openAct->setShortcut(tr("Ctrl+O"));
	connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

	saveAsAct = new QAction(QIcon(":/images/save.png"),tr("&Save As..."), this);
	saveAsAct->setShortcut(tr("Ctrl+S"));
	connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

	saveSnapshotAct = new QAction(QIcon(":/images/save.png"),tr("&Save snapshot"), this);
	connect(saveSnapshotAct, SIGNAL(triggered()), this, SLOT(saveSnapshot()));



	for (int i = 0; i < MAXRECENTFILES; ++i) {
		recentFileActs[i] = new QAction(this);
		recentFileActs[i]->setVisible(false);
		connect(recentFileActs[i], SIGNAL(triggered()),this, SLOT(openRecentFile()));
	}
	
	exitAct = new QAction(tr("E&xit"), this);
	exitAct->setShortcut(tr("Ctrl+Q"));
	connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

	//////////////Render Actions for Toolbar and Menu /////////////////////////////////////////
	renderModeGroupAct = new QActionGroup(this);
	
	renderBboxAct	  = new QAction(QIcon(":/images/bbox.png"),tr("&Bounding box"), renderModeGroupAct);
	renderBboxAct->setCheckable(true);
	connect(renderBboxAct, SIGNAL(triggered()), this, SLOT(renderBbox()));
	
	
	renderModePointsAct	  = new QAction(QIcon(":/images/points.png"),tr("&Points"), renderModeGroupAct);
	renderModePointsAct->setCheckable(true);
	connect(renderModePointsAct, SIGNAL(triggered()), this, SLOT(renderPoint()));

	renderModeWireAct		  = new QAction(QIcon(":/images/wire.png"),tr("&Wireframe"), renderModeGroupAct);
	renderModeWireAct->setCheckable(true);
	connect(renderModeWireAct, SIGNAL(triggered()), this, SLOT(renderWire()));

	renderModeHiddenLinesAct		  = new QAction(QIcon(":/images/backlines.png"),tr("&Hidden Lines"), renderModeGroupAct);
	renderModeHiddenLinesAct->setCheckable(true);
	connect(renderModeHiddenLinesAct, SIGNAL(triggered()), this, SLOT(renderHiddenLines()));

	renderModeFlatLinesAct = new QAction(QIcon(":/images/flatlines.png"),tr("Flat &Lines"), renderModeGroupAct);
	renderModeFlatLinesAct->setCheckable(true);
	connect(renderModeFlatLinesAct, SIGNAL(triggered()), this, SLOT(renderFlatLine()));

	renderModeFlatAct		  = new QAction(QIcon(":/images/flat.png"),tr("&Flat"), renderModeGroupAct);
	renderModeFlatAct->setCheckable(true);
	connect(renderModeFlatAct, SIGNAL(triggered()), this, SLOT(renderFlat()));

	renderModeSmoothAct	  = new QAction(QIcon(":/images/smooth.png"),tr("&Smooth"), renderModeGroupAct);
	renderModeSmoothAct->setCheckable(true);
	renderModeSmoothAct->setChecked(true);
	connect(renderModeSmoothAct, SIGNAL(triggered()), this, SLOT(renderSmooth()));

	renderModeTextureAct  = new QAction(QIcon(":/images/textures.png"),tr("&Texture"),this);
	renderModeTextureAct->setCheckable(true);
	renderModeTextureAct->setChecked(false);
	connect(renderModeTextureAct, SIGNAL(triggered()), this, SLOT(renderTexture()));

	setLightAct	  = new QAction(QIcon(":/images/lighton.png"),tr("&Light on/off"),this);
	setLightAct->setCheckable(true);
	setLightAct->setChecked(true);
	connect(setLightAct, SIGNAL(triggered()), this, SLOT(setLight()));

	setDoubleLightingAct= new QAction(tr("&Double side lighting"),this);
	setDoubleLightingAct->setCheckable(true);
	setDoubleLightingAct->setShortcut(tr("Ctrl+D"));
	connect(setDoubleLightingAct, SIGNAL(triggered()), this, SLOT(setDoubleLighting()));

	setFancyLightingAct	  = new QAction(tr("&Fancy Lighting"),this);
	setFancyLightingAct->setCheckable(true);
	setFancyLightingAct->setShortcut(tr("Ctrl+F"));
	connect(setFancyLightingAct, SIGNAL(triggered()), this, SLOT(setFancyLighting()));

	backFaceCullAct 	  = new QAction(tr("BackFace &Culling"),this);
	backFaceCullAct->setCheckable(true);
	backFaceCullAct->setShortcut(tr("Ctrl+K"));
	connect(backFaceCullAct, SIGNAL(triggered()), this, SLOT(toggleBackFaceCulling()));
	
	//////////////Action Menu View /////////////////////////////////////////////////////////////
	fullScreenAct = new QAction (tr("&FullScreen"), this);
	fullScreenAct->setCheckable(true);
	fullScreenAct->setChecked(false);
	connect(fullScreenAct, SIGNAL(triggered()), this, SLOT(fullScreen()));

	showToolbarStandardAct = new QAction (tr("&Standard"), this);
	showToolbarStandardAct->setCheckable(true);
	showToolbarStandardAct->setChecked(true);
	connect(showToolbarStandardAct, SIGNAL(triggered()), this, SLOT(showToolbarFile()));

	showToolbarRenderAct = new QAction (tr("&Render"), this);
	showToolbarRenderAct->setCheckable(true);
	showToolbarRenderAct->setChecked(true);
	connect(showToolbarRenderAct, SIGNAL(triggered()), this, SLOT(showToolbarRender()));

	showLogAct= new QAction (tr("Show &Infos"), this);
	showLogAct->setCheckable(true);
	connect(showLogAct, SIGNAL(triggered()), this, SLOT(showLog()));

	showInfoPaneAct= new QAction (tr("Show Info &Pane"), this);
	showInfoPaneAct->setCheckable(true);
	connect(showInfoPaneAct, SIGNAL(triggered()), this, SLOT(showInfoPane()));


	showTrackBallAct = new QAction (tr("Show &Trackball"), this);
	showTrackBallAct->setCheckable(true);
	showTrackBallAct->setChecked(true);
	connect(showTrackBallAct, SIGNAL(triggered()), this, SLOT(showTrackBall()));

	resetTrackBallAct = new QAction (tr("Reset &Trackball"), this);
	resetTrackBallAct->setShortcut(tr("Ctrl+H"));
	connect(resetTrackBallAct, SIGNAL(triggered()), this, SLOT(resetTrackBall()));

	//////////////Action Menu Windows /////////////////////////////////////////////////////////
	windowsTileAct = new QAction(tr("&Tile"), this);
	connect(windowsTileAct, SIGNAL(triggered()), workspace, SLOT(tile()));

	windowsCascadeAct = new QAction(tr("&Cascade"), this);
	connect(windowsCascadeAct, SIGNAL(triggered()), workspace, SLOT(cascade()));

	windowsNextAct = new QAction(tr("&Next"), this);
	windowsNextAct->setShortcut(Qt::CTRL+Qt::Key_PageDown);
	connect(windowsNextAct, SIGNAL(triggered()), workspace, SLOT(activateNextWindow()));


	closeAct = new QAction(tr("Cl&ose"), this);
	connect(closeAct, SIGNAL(triggered()),workspace, SLOT(closeActiveWindow()));

	closeAllAct = new QAction(tr("Close &All"), this);
	connect(closeAllAct, SIGNAL(triggered()),workspace, SLOT(closeAllWindows()));
	
	//////////////Action Menu Preferences //////////////////////////////////////////////////////
	setCustomizeAct	  = new QAction(tr("&Options..."),this);
	connect(setCustomizeAct, SIGNAL(triggered()), this, SLOT(setCustomize()));

	//////////////Action Menu About ////////////////////////////////////////////////////////////
	aboutAct = new QAction(tr("&About"), this);
	connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

	aboutPluginsAct = new QAction(tr("About &Plugins"), this);
	connect(aboutPluginsAct, SIGNAL(triggered()), this, SLOT(aboutPlugins()));
}

void MainWindow::createToolBars()
{
	mainToolBar = addToolBar(tr("Standard"));
	mainToolBar->setIconSize(QSize(32,32));
	mainToolBar->addAction(openAct);
	mainToolBar->addAction(saveAsAct);
	mainToolBar->addAction(saveSnapshotAct);

	renderToolBar = addToolBar(tr("Render"));
	renderToolBar->setIconSize(QSize(32,32));
	renderToolBar->addActions(renderModeGroupAct->actions());
	renderToolBar->addAction(renderModeTextureAct);
	renderToolBar->addAction(setLightAct);
}


void MainWindow::createMenus()
{
	//////////////////// Menu File ////////////////////////////////////////////////////////////////
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(openAct);
	fileMenu->addAction(saveAsAct);
	fileMenu->addAction(saveSnapshotAct);

	separatorAct = fileMenu->addSeparator();
	for (int i = 0; i < MAXRECENTFILES; ++i) fileMenu->addAction(recentFileActs[i]);
	updateRecentFileActions();
	fileMenu->addSeparator();
	fileMenu->addAction(exitAct);
	
	//////////////////// Menu Filter //////////////////////////////////////////////////////////////
	filterMenu = menuBar()->addMenu(tr("&Filter"));
	
	//////////////////// Menu Render //////////////////////////////////////////////////////////////
	renderMenu		= menuBar()->addMenu(tr("&Render"));
  //renderMenu->addActions(renderModeGroupAct->actions());
	//renderMenu->addAction(setLightAct);
//  renderMenu->addAction(renderModeAct);
//  renderMenu->addAction(lightingModeAct);
//	lightingModeAct->addAction(setLightAct);
	renderModeMenu=renderMenu->addMenu(tr("Render Mode"));
	renderModeMenu->addAction(backFaceCullAct);
	renderModeMenu->addActions(renderModeGroupAct->actions());
	renderModeMenu->addAction(renderModeTextureAct);
	
	lightingModeMenu=renderMenu->addMenu(tr("Lighting"));
	lightingModeMenu->addAction(setLightAct);
	lightingModeMenu->addAction(setDoubleLightingAct);
	lightingModeMenu->addAction(setFancyLightingAct);

	textureModeMenu=renderMenu->addMenu(tr("Texture"));

	colorModeMenu=renderMenu->addMenu(tr("Color"));

	//////////////////// Menu View ////////////////////////////////////////////////////////////////
	viewMenu		= menuBar()->addMenu(tr("&View"));
	viewMenu->addAction(fullScreenAct);

	trackBallMenu = viewMenu->addMenu(tr("&Trackball"));
	trackBallMenu->addAction(showTrackBallAct);
	trackBallMenu->addAction(resetTrackBallAct);
	
	logMenu = viewMenu->addMenu(tr("&Info"));
	logMenu->addAction(showInfoPaneAct);
	logMenu->addAction(showLogAct);

	toolBarMenu	= viewMenu->addMenu(tr("&ToolBars"));
	toolBarMenu->addAction(showToolbarStandardAct);
	toolBarMenu->addAction(showToolbarRenderAct);
	

	//////////////////// Menu Windows /////////////////////////////////////////////////////////////
	windowsMenu = menuBar()->addMenu(tr("&Windows"));
	connect(windowsMenu, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));
	menuBar()->addSeparator();

	//////////////////// Menu Preferences /////////////////////////////////////////////////////////////
	preferencesMenu=menuBar()->addMenu(tr("&Tools"));
	preferencesMenu->addAction(setCustomizeAct);

	//////////////////// Menu Help ////////////////////////////////////////////////////////////////
	helpMenu = menuBar()->addMenu(tr("&Help"));
	helpMenu->addAction(aboutAct);
	helpMenu->addAction(aboutPluginsAct);
}

void MainWindow::loadPlugins()
{
	pluginsDir = QDir(qApp->applicationDirPath());
#if defined(Q_OS_WIN)
	if (pluginsDir.dirName() == "debug" || pluginsDir.dirName() == "release")
		pluginsDir.cdUp();
#elif defined(Q_OS_MAC)
	if (pluginsDir.dirName() == "MacOS") {
		pluginsDir.cdUp();
		pluginsDir.cdUp();
		pluginsDir.cdUp();
	}
#endif
	pluginsDir.cd("plugins");

	foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
		QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
		QObject *plugin = loader.instance();
		if (plugin) {

// OLD VERSION
			//MeshFilterInterface *iFilter = qobject_cast<MeshFilterInterface *>(plugin);
			//if (iFilter)
			//	addToMenu(plugin, iFilter->filters(), filterMenu, SLOT(applyFilter()));

			//MeshRenderInterface *iRender = qobject_cast<MeshRenderInterface *>(plugin);
			//if (iRender)
			//	addToMenu(plugin, iRender->modes(), renderMenu, SLOT(applyRenderMode()),0,true);

			//// MeshColorizeInterface test 
			//MeshColorizeInterface *iColor = qobject_cast<MeshColorizeInterface *>(plugin);
			//if (iColor)
			//	addToMenu(plugin, iColor->colorsFrom(), colorModeMenu, SLOT(applyColorMode()),0,true);     

			//MeshIOInterface *iIO = qobject_cast<MeshIOInterface *>(plugin);
			//if (iIO)
			//{
			//	fileMenu->addSeparator();
			//	addToMenu(plugin, iIO->formats(), fileMenu, SLOT(applyImportExport()));
			//	//fileMenu->addSeparator();
			//
			//}
// NEW VERSION

			
			MeshColorizeInterface *iColor = qobject_cast<MeshColorizeInterface *>(plugin);
			if (iColor)
			addToMenu(iColor->actions(), colorModeMenu, SLOT(applyColorMode()));
			
			MeshRenderInterface *iDummy = qobject_cast<MeshRenderInterface *>(plugin);
			if(iDummy)
				addToMenu(iDummy->actions(),renderMenu,SLOT(applyRenderMode()));

		  MeshFilterInterface *iFilter = qobject_cast<MeshFilterInterface *>(plugin);
			if (iFilter)
				addToMenu(iFilter->actions(), filterMenu, SLOT(applyFilter()));

		  MeshIOInterface *iIO = qobject_cast<MeshIOInterface *>(plugin);
			if (iIO)
			{
				fileMenu->addSeparator();
				addToMenu(iIO->formats(), fileMenu, SLOT(applyImportExport()));
			  fileMenu->addSeparator();
			}


			pluginFileNames += fileName;
		}
	}
	filterMenu->setEnabled(!filterMenu->actions().isEmpty() && workspace->activeWindow());
}

void MainWindow::addToMenu(QList<QAction *> actionList, QMenu *menu, const char *slot)
{
	foreach (QAction *a, actionList)
	{
		connect(a,SIGNAL(triggered()),this,slot);
		a->setCheckable(true);
		menu->addAction(a);
	}
// OLD LOOP CORE		
//		QAction *action = new QAction(text, plugin);
//		TotalRenderList.push_back(action);
//		connect(action, SIGNAL(triggered()), this, member);
//		action->setCheckable(chackable);
//		menu->addAction(action);
//
//		if (actionGroup) {
//			action->setCheckable(true);
//			actionGroup->addAction(action);
//		}
}

void MainWindow::setCurrentFile(const QString &fileName)
{
	QSettings settings("Recent Files");
	QStringList files = settings.value("recentFileList").toStringList();
	files.removeAll(fileName);
	files.prepend(fileName);
	while (files.size() > MAXRECENTFILES)
		files.removeLast();

	settings.setValue("recentFileList", files);

	foreach (QWidget *widget, QApplication::topLevelWidgets()) {
		MainWindow *mainWin = qobject_cast<MainWindow *>(widget);
		if (mainWin) mainWin->updateRecentFileActions();
	}
}

