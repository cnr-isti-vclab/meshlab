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
Revision 1.93  2008/04/18 17:39:34  cignoni
added menus for the new filter classes (smoothing, normal, quality)

Revision 1.92  2008/04/10 11:07:42  cignoni
added tooltip to decorations

Revision 1.91  2008/03/10 09:39:51  cignoni
better disabling of functionalities when no mesh is loaded

Revision 1.90  2008/03/08 17:25:10  cignoni
added define for disabling auto test of updated versions

Revision 1.89  2008/02/06 09:56:37  cignoni
Corrected bug in the display of plugins info

Revision 1.88  2008/01/06 20:44:19  cignoni
added correct paths for QT dynlib loading

Revision 1.87  2008/01/04 00:46:28  cignoni
Changed the decoration framework. Now it accept a, global, parameter set. Added static calls for finding important directories in a OS independent way.

Revision 1.86  2007/12/27 09:43:08  cignoni
moved colorize filters to a submenu of the filter menu

Revision 1.85  2007/12/13 00:18:28  cignoni
added meshCreation class of filter, and the corresponding menu new under file

Revision 1.84  2007/11/20 11:51:28  cignoni
added layer icon

Revision 1.83  2007/11/09 11:26:44  cignoni
hide the stddialog at the beginning please.

Revision 1.82  2007/11/05 13:49:52  cignoni
better managment of the filter parameter dialog (stddialog)

Revision 1.81  2007/10/23 08:23:10  cignoni
removed reserved shortcut ctrl+s from selection drawing.

Revision 1.80  2007/10/06 23:29:51  cignoni
corrected management of suspeneded editing actions. Added filter toolbar

Revision 1.79  2007/10/02 07:59:33  cignoni
New filter interface. Hopefully more clean and easy to use.

Revision 1.78  2007/07/10 07:19:22  cignoni
** Serious Changes **
again on the MeshDocument, the management of multiple meshes, layers, and per mesh transformation

Revision 1.77  2007/05/16 15:02:05  cignoni
Better management of toggling between edit actions and camera movement

Revision 1.76  2007/05/14 10:46:04  cignoni
Added cngrt dialog

Revision 1.75  2007/04/16 09:24:37  cignoni
** big change **
Added Layers managemnt.
Interfaces are changing...

Revision 1.74  2007/03/27 12:20:16  cignoni
Revamped logging iterface, changed function names in automatic parameters, better selection handling

Revision 1.73  2007/03/20 16:22:34  cignoni
Big small change in accessing mesh interface. First step toward layers

Revision 1.72  2007/03/12 15:23:59  cignoni
Safer dir search for plugins for mac

Revision 1.71  2007/03/03 02:03:25  cignoni
Reformatted lower bar, added number of selected faces. Updated about dialog

Revision 1.70  2007/02/28 00:05:12  cignoni
Added Bug submitting menu

Revision 1.69  2007/02/26 12:03:43  cignoni
Added Help online and check for updates

Revision 1.68  2007/02/26 01:21:46  cignoni
no more snapping dialog, better search for plugins

Revision 1.67  2007/02/08 16:04:18  cignoni
Corrected behaviour of edit actions

Revision 1.66  2006/12/13 17:37:02  pirosu
Added standard plugin window support

Revision 1.65  2006/12/06 00:48:17  cignoni
Improved managment of http answer

Revision 1.64  2006/11/29 00:53:43  cignoni
Improved logging and added web based version checking

Revision 1.63  2006/11/09 08:16:23  cignoni
Bug in the http communication

Revision 1.62  2006/11/08 15:51:56  cignoni
Added kvert to the saved vars

Revision 1.61  2006/11/08 01:35:59  cignoni
still on the web logging

Revision 1.60  2006/11/08 01:04:48  cignoni
First version with http communications

Revision 1.59  2006/10/26 12:07:30  corsini
add lighting properties option

Revision 1.58  2006/07/08 06:37:47  cignoni
Many small bugs correction (esc crash, info in about, obj loading progress,fullscreen es)

Revision 1.57  2006/06/18 21:27:49  cignoni
Progress bar redesigned, now integrated in the workspace window

Revision 1.56  2006/06/15 13:05:57  cignoni
added Filter History Dialogs

Revision 1.55  2006/06/12 15:20:44  cignoni
Initial Dragdrop support (still not working,,,)

Revision 1.54  2006/05/26 04:09:52  cignoni
Still debugging 0.7

Revision 1.53  2006/05/25 04:57:45  cignoni
Major 0.7 release. A lot of things changed. Colorize interface gone away, Editing and selection start to work.
Optional data really working. Clustering decimation totally rewrote. History start to work. Filters organized in classes.

Revision 1.52  2006/04/18 06:57:34  zifnab1974
syntax errors for gcc 3.4.5 resolved

Revision 1.51  2006/04/12 15:12:18  cignoni
Added Filter classes (cleaning, meshing etc)

Revision 1.50  2006/02/25 13:43:39  ggangemi
Action "None" is now exported from MeshRenderPlugin

Revision 1.49  2006/02/24 08:21:00  cignoni
yet another attempt to solve the QProgressDialog issue. Now trying with qt->reset.

Revision 1.48  2006/02/22 10:20:09  cignoni
Changed progressbar->hide  into close to avoid 100% cpu use.

Revision 1.47  2006/02/17 11:17:23  glvertex
- Moved closeAction in FileMenu
- Minor changes

Revision 1.46  2006/02/13 16:18:04  cignoni
Minor edits.

Revision 1.45  2006/02/01 12:44:42  glvertex
- Disabled EDIT menu when no editing tools loaded
- Solved openig bug when running by command line

Revision 1.44  2006/01/31 15:25:13  davide_portelli
Added short key lastFilter
****************************************************************************/


#include <QtGui>
#include <QToolBar>
#include <QProgressBar>
#include <QHttp>
#include <QFileOpenEvent>


#include "meshmodel.h"
#include "interfaces.h"
#include "mainwindow.h"
#include "glarea.h"
#include "plugindialog.h"
#include "customDialog.h"
#include "saveSnapshotDialog.h"
#include "ui_congratsDialog.h"

QProgressBar *MainWindow::qb;

MainWindow::MainWindow()
{
	//workspace = new QWorkspace(this);
	mdiarea = new QMdiArea(this),
	//setCentralWidget(workspace);
	setCentralWidget(mdiarea);
	windowMapper = new QSignalMapper(this);
	// Permette di passare da una finestra all'altra e tenere aggiornato il workspace
	connect(windowMapper, SIGNAL(mapped(QWidget*)),this, SLOT(wrapSetActiveSubWindow(QWidget *)));
	// Quando si passa da una finestra all'altra aggiorna lo stato delle toolbar e dei menu
	connect(mdiarea, SIGNAL(subWindowActivated(QMdiSubWindow *)),this, SLOT(updateMenus()));
	connect(mdiarea, SIGNAL(subWindowActivated(QMdiSubWindow *)),this, SLOT(updateWindowMenu()));
	connect(mdiarea, SIGNAL(subWindowActivated(QMdiSubWindow *)),this, SLOT(updateStdDialog()));

	httpReq=new QHttp(this);
	//connect(httpReq, SIGNAL(requestFinished(int,bool)), this, SLOT(connectionFinished(int,bool)));
	connect(httpReq, SIGNAL(done(bool)), this, SLOT(connectionDone(bool)));

	createActions();
	createMenus();
	createToolBars();
	stddialog = 0;
	setAcceptDrops(true);
	mdiarea->setAcceptDrops(true);
	setWindowTitle(appName());
	loadPlugins();
	setStatusBar(new QStatusBar(this));
	globalStatusBar()=statusBar();
	qb=new QProgressBar(this);
	qb->setMaximum(100);
	qb->setMinimum(0);
	//qb->reset();
	statusBar()->addPermanentWidget(qb,0);
	updateMenus();

	//qb->setAutoClose(true);
	//qb->setMinimumDuration(0);
	//qb->reset();
}

void MainWindow::createActions()
{
	//////////////Action Menu File ////////////////////////////////////////////////////////////////////////////
	openAct = new QAction(QIcon(":/images/open.png"),tr("&Open..."), this);
	openAct->setShortcutContext(Qt::ApplicationShortcut);
	openAct->setShortcut(Qt::CTRL+Qt::Key_O);
	connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

	openInAct = new QAction(QIcon(":/images/open.png"),tr("&Open as new layer..."), this);
	connect(openInAct, SIGNAL(triggered()), this, SLOT(openIn()));

	openProjectAct = new QAction(QIcon(":/images/openPrj.png"),tr("&Open project..."), this);
	connect(openProjectAct, SIGNAL(triggered()), this, SLOT(openProject()));

	closeAct = new QAction(tr("&Close"), this);
	closeAct->setShortcutContext(Qt::ApplicationShortcut);
	//closeAct->setShortcut(Qt::CTRL+Qt::Key_C);
	connect(closeAct, SIGNAL(triggered()),mdiarea, SLOT(closeActiveSubWindow()));

	reloadAct = new QAction(QIcon(":/images/reload.png"),tr("&Reload"), this);
	reloadAct->setShortcutContext(Qt::ApplicationShortcut);
	reloadAct->setShortcut(Qt::CTRL+Qt::Key_R);
	connect(reloadAct, SIGNAL(triggered()), this, SLOT(reload()));

	saveAct = new QAction(QIcon(":/images/save.png"),tr("&Save"), this);
	saveAct->setShortcutContext(Qt::ApplicationShortcut);
	saveAct->setShortcut(Qt::CTRL+Qt::Key_S);
	connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));


	saveAsAct = new QAction(QIcon(":/images/save.png"),tr("Save As..."), this);
	saveAsAct->setShortcutContext(Qt::ApplicationShortcut);
	//saveAsAct->setShortcut(Qt::CTRL+Qt::Key_S);
	connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

	saveProjectAct = new QAction(QIcon(":/images/savePrj.png"),tr("&Save Project..."), this);
	connect(saveProjectAct, SIGNAL(triggered()), this, SLOT(saveProject()));

	saveSnapshotAct = new QAction(QIcon(":/images/snapshot.png"),tr("Save snapsho&t"), this);
	connect(saveSnapshotAct, SIGNAL(triggered()), this, SLOT(saveSnapshot()));

	for (int i = 0; i < MAXRECENTFILES; ++i) {
		recentFileActs[i] = new QAction(this);
		recentFileActs[i]->setVisible(false);
		connect(recentFileActs[i], SIGNAL(triggered()),this, SLOT(openRecentFile()));
	}

	exitAct = new QAction(tr("E&xit"), this);
	exitAct->setShortcut(Qt::CTRL+Qt::Key_Q);
	connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

	//////////////Render Actions for Toolbar and Menu /////////////////////////////////////////////////////////
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

	renderModeHiddenLinesAct  = new QAction(QIcon(":/images/backlines.png"),tr("&Hidden Lines"),renderModeGroupAct);
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
	connect(renderModeSmoothAct, SIGNAL(triggered()), this, SLOT(renderSmooth()));

	renderModeTextureAct  = new QAction(QIcon(":/images/textures.png"),tr("&Texture"),this);
	renderModeTextureAct->setCheckable(true);
	connect(renderModeTextureAct, SIGNAL(triggered()), this, SLOT(renderTexture()));

	setLightAct	  = new QAction(QIcon(":/images/lighton.png"),tr("&Light on/off"),this);
	setLightAct->setCheckable(true);
	connect(setLightAct, SIGNAL(triggered()), this, SLOT(setLight()));

	setDoubleLightingAct= new QAction(tr("&Double side lighting"),this);
	setDoubleLightingAct->setCheckable(true);
	setDoubleLightingAct->setShortcutContext(Qt::ApplicationShortcut);
	setDoubleLightingAct->setShortcut(Qt::CTRL+Qt::Key_D);
	connect(setDoubleLightingAct, SIGNAL(triggered()), this, SLOT(setDoubleLighting()));

	setFancyLightingAct   = new QAction(tr("&Fancy Lighting"),this);
	setFancyLightingAct->setCheckable(true);
	setFancyLightingAct->setShortcutContext(Qt::ApplicationShortcut);
	setFancyLightingAct->setShortcut(Qt::CTRL+Qt::Key_F);
	connect(setFancyLightingAct, SIGNAL(triggered()), this, SLOT(setFancyLighting()));

	setLightingPropertiesAct  = new QAction(tr("Lighting &Properties"), this);
	setLightingPropertiesAct->setShortcutContext(Qt::ApplicationShortcut);
	setLightingPropertiesAct->setShortcut(Qt::CTRL+Qt::Key_P);
	connect(setLightingPropertiesAct, SIGNAL(triggered()), this, SLOT(setLightingProperties()));

	backFaceCullAct 	  = new QAction(tr("BackFace &Culling"),this);
	backFaceCullAct->setCheckable(true);
	backFaceCullAct->setShortcutContext(Qt::ApplicationShortcut);
	backFaceCullAct->setShortcut(Qt::CTRL+Qt::Key_K);
	connect(backFaceCullAct, SIGNAL(triggered()), this, SLOT(toggleBackFaceCulling()));

	setSelectionRenderingAct 	  = new QAction(QIcon(":/images/selected.png"),tr("Selected Face Rendering"),this);
	setSelectionRenderingAct->setCheckable(true);
	setSelectionRenderingAct->setShortcutContext(Qt::ApplicationShortcut);
	//setSelectionRenderingAct->setShortcut(Qt::CTRL+Qt::Key_S);
	connect(setSelectionRenderingAct, SIGNAL(triggered()), this, SLOT(toggleSelectionRendering()));

	//////////////Action Menu View ////////////////////////////////////////////////////////////////////////////
	fullScreenAct = new QAction (tr("&FullScreen"), this);
	fullScreenAct->setCheckable(true);
	fullScreenAct->setShortcutContext(Qt::ApplicationShortcut);
	fullScreenAct->setShortcut(Qt::ALT+Qt::Key_Return);
	connect(fullScreenAct, SIGNAL(triggered()), this, SLOT(fullScreen()));

	showToolbarStandardAct = new QAction (tr("&Standard"), this);
	showToolbarStandardAct->setCheckable(true);
	showToolbarStandardAct->setChecked(true);
	connect(showToolbarStandardAct, SIGNAL(triggered()), this, SLOT(showToolbarFile()));

	showToolbarRenderAct = new QAction (tr("&Render"), this);
	showToolbarRenderAct->setCheckable(true);
	showToolbarRenderAct->setChecked(true);
	connect(showToolbarRenderAct, SIGNAL(triggered()), this, SLOT(showToolbarRender()));

	showInfoPaneAct= new QAction (tr("Show Info &Pane"), this);
	showInfoPaneAct->setCheckable(true);
	connect(showInfoPaneAct, SIGNAL(triggered()), this, SLOT(showInfoPane()));


	showTrackBallAct = new QAction (tr("Show &Trackball"), this);
	showTrackBallAct->setCheckable(true);
	connect(showTrackBallAct, SIGNAL(triggered()), this, SLOT(showTrackBall()));

	resetTrackBallAct = new QAction (tr("Reset &Trackball"), this);
	resetTrackBallAct->setShortcutContext(Qt::ApplicationShortcut);
	resetTrackBallAct->setShortcut(Qt::CTRL+Qt::Key_H);
	connect(resetTrackBallAct, SIGNAL(triggered()), this, SLOT(resetTrackBall()));

	showLayerDlgAct =  new QAction (QIcon(":/images/layers.png"),tr("Show Layer Dialog"), this);
	showLayerDlgAct->setCheckable(true);
	showLayerDlgAct->setChecked(true);
	connect(showLayerDlgAct, SIGNAL(triggered()), this, SLOT(showLayerDlg()));


	//////////////Action Menu EDIT /////////////////////////////////////////////////////////////////////////
	suspendEditModeAct = new QAction (QIcon(":/images/no_edit.png"),tr("Not editing"), this);
	suspendEditModeAct->setShortcut(Qt::Key_Escape);
	suspendEditModeAct->setCheckable(true);
	suspendEditModeAct->setChecked(true);
	connect(suspendEditModeAct, SIGNAL(triggered()), this, SLOT(suspendEditMode()));

	//////////////Action Menu WINDOWS /////////////////////////////////////////////////////////////////////////
	windowsTileAct = new QAction(tr("&Tile"), this);
	connect(windowsTileAct, SIGNAL(triggered()), mdiarea, SLOT(tileSubWindows()));

	windowsCascadeAct = new QAction(tr("&Cascade"), this);
	connect(windowsCascadeAct, SIGNAL(triggered()), mdiarea, SLOT(cascadeSubWindows()));

	windowsNextAct = new QAction(tr("&Next"), this);
	connect(windowsNextAct, SIGNAL(triggered()), mdiarea, SLOT(activateNextSubWindow()));

	closeAllAct = new QAction(tr("Close &All Windows"), this);
	connect(closeAllAct, SIGNAL(triggered()),mdiarea, SLOT(closeAllSubWindows()));

	//////////////Action Menu Filters /////////////////////////////////////////////////////////////////////
	lastFilterAct = new QAction(tr("Apply filter"),this);
	lastFilterAct->setShortcutContext(Qt::ApplicationShortcut);
	lastFilterAct->setShortcut(Qt::CTRL+Qt::Key_L);
	lastFilterAct->setEnabled(false);
	connect(lastFilterAct, SIGNAL(triggered()), this, SLOT(applyLastFilter()));

	showFilterScriptAct = new QAction(tr("Show current filter script"),this);
	showFilterScriptAct->setEnabled(true);
	connect(showFilterScriptAct, SIGNAL(triggered()), this, SLOT(showFilterScript()));

	//////////////Action Menu Preferences /////////////////////////////////////////////////////////////////////
	setCustomizeAct	  = new QAction(tr("&Options..."),this);
	connect(setCustomizeAct, SIGNAL(triggered()), this, SLOT(setCustomize()));

	//////////////Action Menu About ///////////////////////////////////////////////////////////////////////////
	aboutAct = new QAction(tr("&About"), this);
	connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

	aboutPluginsAct = new QAction(tr("About &Plugins"), this);
	connect(aboutPluginsAct, SIGNAL(triggered()), this, SLOT(aboutPlugins()));

	onlineHelpAct = new QAction(tr("Online &Documentation"), this);
	connect(onlineHelpAct, SIGNAL(triggered()), this, SLOT(helpOnline()));

	submitBugAct = new QAction(tr("Submit Bug"), this);
	connect(submitBugAct, SIGNAL(triggered()), this, SLOT(submitBug()));

	onscreenHelpAct = new QAction(tr("On screen quick help"), this);
	connect(onscreenHelpAct, SIGNAL(triggered()), this, SLOT(helpOnscreen()));

	checkUpdatesAct = new QAction(tr("Check for updates"), this);
	connect(checkUpdatesAct, SIGNAL(triggered()), this, SLOT(checkForUpdates()));
}

void MainWindow::createToolBars()
{
	mainToolBar = addToolBar(tr("Standard"));
	mainToolBar->setIconSize(QSize(32,32));
	mainToolBar->addAction(openAct);
	mainToolBar->addAction(reloadAct);
	mainToolBar->addAction(saveAct);
	mainToolBar->addAction(saveSnapshotAct);
	mainToolBar->addAction(showLayerDlgAct);

	renderToolBar = addToolBar(tr("Render"));
	//renderToolBar->setIconSize(QSize(32,32));
	renderToolBar->addActions(renderModeGroupAct->actions());
	renderToolBar->addAction(renderModeTextureAct);
	renderToolBar->addAction(setLightAct);
	renderToolBar->addAction(setSelectionRenderingAct);

	editToolBar = addToolBar(tr("Edit"));
	editToolBar->addAction(suspendEditModeAct);
	editToolBar->addSeparator();

	filterToolBar = addToolBar(tr("Action"));
}


void MainWindow::createMenus()
{
	//////////////////// Menu File ////////////////////////////////////////////////////////////////////////////
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(openAct);
	fileMenu->addAction(openInAct);
	fileMenu->addAction(openProjectAct);
	fileMenu->addAction(closeAct);
	fileMenu->addAction(reloadAct);
	fileMenu->addAction(saveAct);
	fileMenu->addAction(saveAsAct);
	fileMenu->addAction(saveProjectAct);

	fileMenuNew = fileMenu->addMenu(tr("New"));

	fileMenu->addSeparator();
	fileMenu->addAction(saveSnapshotAct);
	separatorAct = fileMenu->addSeparator();

	for (int i = 0; i < MAXRECENTFILES; ++i) fileMenu->addAction(recentFileActs[i]);
	updateRecentFileActions();
	fileMenu->addSeparator();
	fileMenu->addAction(exitAct);

	//////////////////// Menu Edit //////////////////////////////////////////////////////////////////////////
	editMenu = menuBar()->addMenu(tr("&Edit"));
	editMenu->addAction(suspendEditModeAct);

  //////////////////// Menu Filter //////////////////////////////////////////////////////////////////////////
	filterMenu = menuBar()->addMenu(tr("Fi&lters"));
	filterMenu->addAction(lastFilterAct);
	filterMenu->addAction(showFilterScriptAct);
	filterMenu->addSeparator();
	filterMenuSelect = filterMenu->addMenu(tr("Selection"));
	filterMenuClean  = filterMenu->addMenu(tr("Cleaning and Repairing"));
	filterMenuRemeshing = filterMenu->addMenu(tr("Remeshing, simplification and reconstruction"));
	filterMenuColorize = filterMenu->addMenu(tr("Color Creation and Processing"));
	filterMenuSmoothing = filterMenu->addMenu(tr("Smoothing, Fairing and Deformation"));
	filterMenuQuality = filterMenu->addMenu(tr("Quality Measure and computations"));
	filterMenuNormal = filterMenu->addMenu(tr("Normals, Curvatures and Orientation"));
	filterMenuLayer = filterMenu->addMenu(tr("Layer and Attribute Management"));
	filterMenuRangeMap = filterMenu->addMenu(tr("Range Map"));
  filterMenuPointSet = filterMenu->addMenu(tr("Point Set"));
  filterMenuSampling = filterMenu->addMenu(tr("Sampling"));

	//////////////////// Menu Render //////////////////////////////////////////////////////////////////////////
	renderMenu		= menuBar()->addMenu(tr("&Render"));

	renderModeMenu=renderMenu->addMenu(tr("Render &Mode"));
	renderModeMenu->addAction(backFaceCullAct);
	renderModeMenu->addActions(renderModeGroupAct->actions());
	renderModeMenu->addAction(renderModeTextureAct);
	renderModeMenu->addAction(setSelectionRenderingAct);

	lightingModeMenu=renderMenu->addMenu(tr("&Lighting"));
	lightingModeMenu->addAction(setLightAct);
	lightingModeMenu->addAction(setDoubleLightingAct);
	lightingModeMenu->addAction(setFancyLightingAct);
	lightingModeMenu->addAction(setLightingPropertiesAct);

	// Color SUBmenu
	colorModeMenu = renderMenu->addMenu(tr("&Color"));

	colorModeGroupAct = new QActionGroup(this);	colorModeGroupAct->setExclusive(true);

	colorModeNoneAct = new QAction(QString("&None"),colorModeGroupAct);
	colorModeNoneAct->setCheckable(true);
	colorModeNoneAct->setChecked(true);

        colorModePerMeshAct = new QAction(QString("Per &Mesh"),colorModeGroupAct);
        colorModePerMeshAct->setCheckable(true);

	colorModePerVertexAct = new QAction(QString("Per &Vertex"),colorModeGroupAct);
	colorModePerVertexAct->setCheckable(true);

	colorModePerFaceAct = new QAction(QString("Per &Face"),colorModeGroupAct);
	colorModePerFaceAct->setCheckable(true);


	colorModeMenu->addAction(colorModeNoneAct);
        colorModeMenu->addAction(colorModePerMeshAct);
	colorModeMenu->addAction(colorModePerVertexAct);
	colorModeMenu->addAction(colorModePerFaceAct);

	connect(colorModeGroupAct, SIGNAL(triggered(QAction *)), this, SLOT(setColorMode(QAction *)));

	// Shaders SUBmenu
	shadersMenu = renderMenu->addMenu(tr("&Shaders"));

	renderMenu->addSeparator();

	//////////////////// Menu View ////////////////////////////////////////////////////////////////////////////
	viewMenu		= menuBar()->addMenu(tr("&View"));
	viewMenu->addAction(fullScreenAct);
	viewMenu->addAction(showLayerDlgAct);

	trackBallMenu = viewMenu->addMenu(tr("&Trackball"));
	trackBallMenu->addAction(showTrackBallAct);
	trackBallMenu->addAction(resetTrackBallAct);

	logMenu = viewMenu->addMenu(tr("&Info"));
	logMenu->addAction(showInfoPaneAct);

	toolBarMenu	= viewMenu->addMenu(tr("&ToolBars"));
	toolBarMenu->addAction(showToolbarStandardAct);
	toolBarMenu->addAction(showToolbarRenderAct);
	connect(toolBarMenu,SIGNAL(aboutToShow()),this,SLOT(updateMenus()));

	//////////////////// Menu Windows /////////////////////////////////////////////////////////////////////////
	windowsMenu = menuBar()->addMenu(tr("&Windows"));
	connect(windowsMenu, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));
	menuBar()->addSeparator();

	//////////////////// Menu Preferences /////////////////////////////////////////////////////////////////////
	preferencesMenu=menuBar()->addMenu(tr("&Tools"));
	preferencesMenu->addAction(setCustomizeAct);

	//////////////////// Menu Help ////////////////////////////////////////////////////////////////
	helpMenu = menuBar()->addMenu(tr("&Help"));
	helpMenu->addAction(aboutAct);
	helpMenu->addAction(aboutPluginsAct);
	helpMenu->addAction(onlineHelpAct);
	helpMenu->addAction(onscreenHelpAct);
	helpMenu->addAction(submitBugAct);
	helpMenu->addAction(checkUpdatesAct);
}

void MainWindow::loadPlugins()
{
	pluginsDir=QDir(getPluginDirPath());
	// without adding the correct library path in the mac the loading of jpg (done via qt plugins) fails
	qApp->addLibraryPath(getPluginDirPath());
	qApp->addLibraryPath(getBaseDirPath());

	qDebug( "Current Plugins Dir: %s ",qPrintable(pluginsDir.absolutePath()));
	foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
		QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
		QObject *plugin = loader.instance();

		if (plugin) {
		  MeshFilterInterface *iFilter = qobject_cast<MeshFilterInterface *>(plugin);
			if (iFilter)
      {
        QAction *filterAction;
        foreach(filterAction, iFilter->actions())
        {
					//qDebug("Processing action %s",qPrintable(filterAction->text()) );
					//qDebug("          (%s)", qPrintable(iFilter->filterInfo(filterAction)) );
					filterMap[filterAction->text()]=filterAction;
          filterAction->setToolTip(iFilter->filterInfo(filterAction));
          connect(filterAction,SIGNAL(triggered()),this,SLOT(startFilter()));
          int filterClass = iFilter->getClass(filterAction);

          if( (filterClass & MeshFilterInterface::FaceColoring) ||
              (filterClass & MeshFilterInterface::VertexColoring) )
              		filterMenuColorize->addAction(filterAction);
					if(filterClass & MeshFilterInterface::Selection)
              		filterMenuSelect->addAction(filterAction);
					if(filterClass &  MeshFilterInterface::Cleaning )
              		filterMenuClean->addAction(filterAction);
					if(filterClass &  MeshFilterInterface::Remeshing )
              		filterMenuRemeshing->addAction(filterAction);
					if(filterClass &  MeshFilterInterface::Smoothing )
              		filterMenuSmoothing->addAction(filterAction);
					if(filterClass &  MeshFilterInterface::Normal )
              		filterMenuNormal->addAction(filterAction);
					if(filterClass &  MeshFilterInterface::Quality )
              		filterMenuQuality->addAction(filterAction);
					if(filterClass &  MeshFilterInterface::Layer )
              		filterMenuLayer->addAction(filterAction);
					if(filterClass & MeshFilterInterface::MeshCreation )
              		fileMenuNew->addAction(filterAction);
					if(filterClass & MeshFilterInterface::RangeMap )
              		filterMenuRangeMap->addAction(filterAction);
          if(filterClass & MeshFilterInterface::PointSet )
                  filterMenuPointSet->addAction(filterAction);
          if(filterClass & MeshFilterInterface::Sampling )
									filterMenuSampling->addAction(filterAction);
					if(filterClass == 0) //  MeshFilterInterface::Generic :
									filterMenu->addAction(filterAction);

					if(!filterAction->icon().isNull())
										filterToolBar->addAction(filterAction);
        }
       }
		  MeshIOInterface *iIO = qobject_cast<MeshIOInterface *>(plugin);
			if (iIO) meshIOPlugins.push_back(iIO);

			MeshDecorateInterface *iDecorator = qobject_cast<MeshDecorateInterface *>(plugin);
			if (iDecorator){
				QAction *decoratorAction;
				decoratorActionList+=iDecorator->actions();
				foreach(decoratorAction, iDecorator->actions())
						{
								connect(decoratorAction,SIGNAL(triggered()),this,SLOT(applyDecorateMode()));
								decoratorAction->setToolTip(iDecorator->Info(decoratorAction));
								renderMenu->addAction(decoratorAction);
						}
			}

			MeshRenderInterface *iRender = qobject_cast<MeshRenderInterface *>(plugin);
			if (iRender)
			  addToMenu(iRender->actions(), shadersMenu, SLOT(applyRenderMode()));
/*
			MeshEditInterface *iEdit = qobject_cast<MeshEditInterface *>(plugin);
			QAction *editAction;
			if (iEdit)
        foreach(editAction, iEdit->actions())
        {
			    editMenu->addAction(editAction);
          if(!editAction->icon().isNull())
              editToolBar->addAction(editAction);
          connect(editAction,SIGNAL(triggered()),this,SLOT(applyEditMode()));
          editActionList.push_back(editAction);
        }
*/		
			MeshEditInterfaceFactory *iEditFactory = qobject_cast<MeshEditInterfaceFactory *>(plugin);
			QAction *editAction = 0;
			if(iEditFactory)
			{
				//qDebug() << "Here with filename:" << fileName;
				
				foreach(editAction, iEditFactory->actions())
				{
					editMenu->addAction(editAction);
					if(!editAction->icon().isNull())
					{
						editToolBar->addAction(editAction);
					} else qDebug() << "action was null";
					
					connect(editAction, SIGNAL(triggered()), this, SLOT(applyEditMode()));
					editActionList.push_back(editAction);
				}
			}
			
			pluginFileNames += fileName;
		} else
		{
			qDebug() << "error loading plugin with filename:" << fileName;
			qDebug() << loader.errorString();
        }
	}
	filterMenu->setEnabled(!filterMenu->actions().isEmpty() && mdiarea->activeSubWindow());

}

void MainWindow::addToMenu(QList<QAction *> actionList, QMenu *menu, const char *slot)
{
	foreach (QAction *a, actionList)
	{
		connect(a,SIGNAL(triggered()),this,slot);
		menu->addAction(a);
	}
}

// this function update the app settings with the current recent file list
// and update the loaded mesh counter
void MainWindow::setCurrentFile(const QString &fileName)
{
	QSettings settings;
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

	settings.setValue("totalKV",          settings.value("totalKV",0).toInt()           + (GLA()->mm()->cm.vn)/1000);
	settings.setValue("loadedMeshCounter",settings.value("loadedMeshCounter",0).toInt() + 1);

	int loadedMeshCounter    = settings.value("loadedMeshCounter",20).toInt();
	int connectionInterval   = settings.value("connectionInterval",20).toInt();
	int lastComunicatedValue = settings.value("lastComunicatedValue",0).toInt();

	if(loadedMeshCounter-lastComunicatedValue>connectionInterval && !myLocalBuf.isOpen())
  {
#if not defined(__DISABLE_AUTO_STATS__)
		checkForUpdates(false);
#endif
		int congratsMeshCounter = settings.value("congratsMeshCounter",0).toInt();
		if(loadedMeshCounter > congratsMeshCounter + 100 )
			{
				QFile txtFile(":/images/100mesh.html");
				txtFile.open(QIODevice::ReadOnly | QIODevice::Text);
				QString tttt=txtFile.readAll();
				// This preference values store when you did the last request for a mail
				settings.setValue("congratsMeshCounter",loadedMeshCounter);

				QDialog *congratsDialog = new QDialog();
				Ui::CongratsDialog temp;
				temp.setupUi(congratsDialog);

				temp.buttonBox->addButton("Send Mail", QDialogButtonBox::AcceptRole);
				temp.congratsTextEdit->setHtml(tttt);
				congratsDialog->exec();
				if(congratsDialog->result()==QDialog::Accepted)
					QDesktopServices::openUrl(QUrl("mailto:p.cignoni@isti.cnr.it?subject=[MeshLab] Reporting Info on MeshLab Usage"));
			}
	}
}

void MainWindow::checkForUpdates(bool verboseFlag)
{
	VerboseCheckingFlag=verboseFlag;
	QSettings settings;
	int totalKV=settings.value("totalKV",0).toInt();
	int connectionInterval=settings.value("connectionInterval",20).toInt();
	settings.setValue("connectionInterval",connectionInterval);
	int loadedMeshCounter=settings.value("loadedMeshCounter",0).toInt();
	int savedMeshCounter=settings.value("savedMeshCounter",0).toInt();
	QString UID=settings.value("UID",QString("")).toString();
	if(UID.isEmpty())
	{
		UID=QUuid::createUuid ().toString();
		settings.setValue("UID",UID);
	}

#ifdef _DEBUG_PHP
    QString BaseCommand("/~cignoni/meshlab_d.php");
#else
    QString BaseCommand("/~cignoni/meshlab.php");
#endif

#ifdef Q_WS_WIN
    QString OS="Win";
#elif defined( Q_WS_MAC)
    QString OS="Mac";
#else
    QString OS="Lin";
#endif
    QString message=BaseCommand+QString("?code=%1&count=%2&scount=%3&totkv=%4&ver=%5&os=%6").arg(UID).arg(loadedMeshCounter).arg(savedMeshCounter).arg(totalKV).arg(appVer()).arg(OS);
    idHost=httpReq->setHost("vcg.isti.cnr.it"); // id == 1
    bool ret=myLocalBuf.open(QBuffer::WriteOnly);
    if(!ret) QMessageBox::information(this,"Meshlab",QString("Failed opening of internal buffer"));
    idGet=httpReq->get(message,&myLocalBuf);     // id == 2

}

void MainWindow::connectionDone(bool /* status */)
{
	QString answer=myLocalBuf.data();
	if(answer.left(3)==QString("NEW"))
		QMessageBox::information(this,"MeshLab Version Checking",answer.remove(0,3));
	else if (VerboseCheckingFlag) QMessageBox::information(this,"MeshLab Version Checking","Your MeshLab version is the most recent one.");

	myLocalBuf.close();
	QSettings settings;
	int loadedMeshCounter=settings.value("loadedMeshCounter",0).toInt();
	settings.setValue("lastComunicatedValue",loadedMeshCounter);
}


void MainWindow::submitBug()
{
	QMessageBox mb(QMessageBox::NoIcon,tr("MeshLab"),tr("MeshLab"),QMessageBox::NoButton, this);
	//mb.setWindowTitle(tr("MeshLab"));
	QPushButton *submitBug = mb.addButton("Submit Bug",QMessageBox::AcceptRole);
	mb.addButton(QMessageBox::Cancel);
	mb.setText(tr("If Meshlab closed in unexpected way (e.g. it crashed badly) and"
						 "if you are able to repeat the bug, please consider to submit a report using the SourceForge tracking system.\n"
						  ) );
	mb.setInformativeText(	tr(
	         "Hints for a good, useful bug report:\n"
					 "- Be verbose and descriptive\n"
					 "- Report meshlab version and OS\n"
					 "- Describe the sequence of actions that bring you to the crash.\n"
					 "- Consider submitting the mesh file causing a particular crash.\n"
					 ) );

	mb.exec();

	if (mb.clickedButton() == submitBug)
		QDesktopServices::openUrl(QUrl("http://sourceforge.net/tracker/?func=add&group_id=149444&atid=774731"));

}

void MainWindow::wrapSetActiveSubWindow(QWidget* window){
	QMdiSubWindow* subwindow;
	subwindow = dynamic_cast<QMdiSubWindow*>(window);
	if(subwindow!= NULL){
		mdiarea->setActiveSubWindow(subwindow);
	}else{
		qDebug("Type of window is not a QMdiSubWindow*");
	}
}


