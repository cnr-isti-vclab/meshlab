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


#include "../common/interfaces.h"

#include <QtGui>
#include <QToolBar>
#include <QProgressBar>
#include <QHttp>
#include <QFileOpenEvent>
#include <QFile>
#include <QtXml>
#include "mainwindow.h"
#include "glarea.h"
//#include "plugindialog.h"
//#include "customDialog.h"
//#include "saveSnapshotDialog.h"
//#include "ui_congratsDialog.h"

QProgressBar *MainWindow::qb;

MainWindow::MainWindow()
{
	//workspace = new QWorkspace(this);
	mdiarea = new QMdiArea(this);
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

        QIcon icon;
        icon.addPixmap(QPixmap(":images/eye48.png"));
        setWindowIcon(icon);

	PM.loadPlugins(defaultGlobalParams);
	// Now load from the registry the settings and  merge the hardwired values got from the PM.loadPlugins with the ones found in the registry.
	loadMeshLabSettings();
	createActions();
	createToolBars();
	createMenus();
	stddialog = 0;
	setAcceptDrops(true);
	mdiarea->setAcceptDrops(true);
	setWindowTitle(appName());
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

	backFaceCullAct 	  = new QAction(tr("BackFace &Culling"),this);
	backFaceCullAct->setCheckable(true);
	backFaceCullAct->setShortcutContext(Qt::ApplicationShortcut);
	backFaceCullAct->setShortcut(Qt::CTRL+Qt::Key_K);
	connect(backFaceCullAct, SIGNAL(triggered()), this, SLOT(toggleBackFaceCulling()));

  setSelectFaceRenderingAct 	  = new QAction(QIcon(":/images/selected_face.png"),tr("Selected Face Rendering"),this);
  setSelectFaceRenderingAct->setCheckable(true);
  setSelectFaceRenderingAct->setShortcutContext(Qt::ApplicationShortcut);
  connect(setSelectFaceRenderingAct, SIGNAL(triggered()), this, SLOT(toggleSelectFaceRendering()));

  setSelectVertRenderingAct 	  = new QAction(QIcon(":/images/selected_vert.png"),tr("Selected Vertex Rendering"),this);
  setSelectVertRenderingAct->setCheckable(true);
  setSelectVertRenderingAct->setShortcutContext(Qt::ApplicationShortcut);
  connect(setSelectVertRenderingAct, SIGNAL(triggered()), this, SLOT(toggleSelectVertRendering()));

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

	/*showLayerDlgAct =  new QAction (QIcon(":/images/layers.png"),tr("Show Layer Dialog"), this);
	showLayerDlgAct->setCheckable(true);
	showLayerDlgAct->setChecked(true);
	connect(showLayerDlgAct, SIGNAL(triggered()), this, SLOT(showLayerDlg()));*/


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
	/*mainToolBar->addAction(saveSnapshotAct);
	mainToolBar->addAction(showLayerDlgAct);*/

	renderToolBar = addToolBar(tr("Render"));
	//renderToolBar->setIconSize(QSize(32,32));
	renderToolBar->addActions(renderModeGroupAct->actions());
	renderToolBar->addAction(renderModeTextureAct);
	renderToolBar->addAction(setLightAct);
  renderToolBar->addAction(setSelectFaceRenderingAct);
  renderToolBar->addAction(setSelectVertRenderingAct);

	editToolBar = addToolBar(tr("Edit"));
	editToolBar->addAction(suspendEditModeAct);
	editToolBar->addSeparator();

	filterToolBar = addToolBar(tr("Action"));

	/*foreach(MeshEditInterfaceFactory *iEditFactory,PM.meshEditFactoryPlugins())
	{		
		foreach(QAction* editAction, iEditFactory->actions())
		{
			if(!editAction->icon().isNull())
			{
				editToolBar->addAction(editAction);
			} else qDebug() << "action was null";
		}
	}*/
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

	/*fileMenu->addSeparator();
	fileMenu->addAction(saveSnapshotAct);*/
	separatorAct = fileMenu->addSeparator();

	for (int i = 0; i < MAXRECENTFILES; ++i) fileMenu->addAction(recentFileActs[i]);
	updateRecentFileActions();
	fileMenu->addSeparator();
	fileMenu->addAction(exitAct);

	//////////////////// Menu Edit //////////////////////////////////////////////////////////////////////////
	/*editMenu = menuBar()->addMenu(tr("&Edit"));
	editMenu->addAction(suspendEditModeAct);*/

  //////////////////// Menu Filter //////////////////////////////////////////////////////////////////////////
	/*filterMenu = menuBar()->addMenu(tr("Fi&lters"));
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
	filterMenuTexture = filterMenu->addMenu(tr("Texture"));*/
	

	//////////////////// Menu Render //////////////////////////////////////////////////////////////////////////
	renderMenu		= menuBar()->addMenu(tr("&Render"));

	renderModeMenu=renderMenu->addMenu(tr("Render &Mode"));
	renderModeMenu->addAction(backFaceCullAct);
	renderModeMenu->addActions(renderModeGroupAct->actions());
	renderModeMenu->addAction(renderModeTextureAct);
  renderModeMenu->addAction(setSelectFaceRenderingAct);
  renderModeMenu->addAction(setSelectVertRenderingAct);

	lightingModeMenu=renderMenu->addMenu(tr("&Lighting"));
	lightingModeMenu->addAction(setLightAct);
	lightingModeMenu->addAction(setDoubleLightingAct);
	lightingModeMenu->addAction(setFancyLightingAct);

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
	//viewMenu->addAction(showLayerDlgAct);

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

	/*fillFilterMenu();
	fillEditMenu();*/
	fillRenderMenu();
	//fillDecorateMenu();
}

//void MainWindow::fillFilterMenu()
//{
//	foreach(MeshFilterInterface *iFilter,PM.meshFilterPlugins())
//	{
//		foreach(QAction *filterAction, iFilter->actions())
//		{
//			filterAction->setToolTip(iFilter->filterInfo(filterAction));
//			connect(filterAction,SIGNAL(triggered()),this,SLOT(startFilter()));
//			int filterClass = iFilter->getClass(filterAction);
//
//			if( (filterClass & MeshFilterInterface::FaceColoring) || (filterClass & MeshFilterInterface::VertexColoring) ) filterMenuColorize->addAction(filterAction);
//			if(filterClass & MeshFilterInterface::Selection) filterMenuSelect->addAction(filterAction);
//			if(filterClass &  MeshFilterInterface::Cleaning ) filterMenuClean->addAction(filterAction);
//			if(filterClass &  MeshFilterInterface::Remeshing ) filterMenuRemeshing->addAction(filterAction);
//			if(filterClass &  MeshFilterInterface::Smoothing ) filterMenuSmoothing->addAction(filterAction);
//			if(filterClass &  MeshFilterInterface::Normal ) filterMenuNormal->addAction(filterAction);
//			if( (filterClass &  MeshFilterInterface::Quality ) || (filterClass & MeshFilterInterface::Measure  )	)	 filterMenuQuality->addAction(filterAction);
//			if(filterClass &  MeshFilterInterface::Layer ) filterMenuLayer->addAction(filterAction);
//			if(filterClass & MeshFilterInterface::MeshCreation ) fileMenuNew->addAction(filterAction);
//			if(filterClass & MeshFilterInterface::RangeMap )	filterMenuRangeMap->addAction(filterAction);
//			if(filterClass & MeshFilterInterface::PointSet )	filterMenuPointSet->addAction(filterAction);
//			if(filterClass & MeshFilterInterface::Sampling )	filterMenuSampling->addAction(filterAction);
//			if(filterClass & MeshFilterInterface::Texture)		filterMenuTexture->addAction(filterAction);
//			 //  MeshFilterInterface::Generic :
//			if(filterClass == 0) filterMenu->addAction(filterAction);
//
//			if(!filterAction->icon().isNull())
//				filterToolBar->addAction(filterAction);
//		}
//	}	
//}
//
//void MainWindow::fillDecorateMenu()
//{
//	foreach(MeshDecorateInterface *iDecorate,PM.meshDecoratePlugins())
//	{
//		foreach(QAction *decorateAction, iDecorate->actions())
//		{
//			connect(decorateAction,SIGNAL(triggered()),this,SLOT(applyDecorateMode()));
//			decorateAction->setToolTip(iDecorate->filterInfo(decorateAction));
//			renderMenu->addAction(decorateAction);
//		}
//	}
//}

void MainWindow::fillRenderMenu()
{
	foreach(MeshRenderInterface *iRender,PM.meshRenderPlugins())
	{
		addToMenu(iRender->actions(), shadersMenu, SLOT(applyRenderMode()));
	}
}

//void MainWindow::fillEditMenu()
//{
//	foreach(MeshEditInterfaceFactory *iEditFactory,PM.meshEditFactoryPlugins())
//	{		
//		foreach(QAction* editAction, iEditFactory->actions())
//		{
//			editMenu->addAction(editAction);
//
//			connect(editAction, SIGNAL(triggered()), this, SLOT(applyEditMode()));
//			//editActionList.push_back(editAction);
//		}
//	}
//}


void MainWindow::loadPlugins()
{
	//PM.loadPlugins();
//	pluginsDir=QDir(getPluginDirPath());
//	// without adding the correct library path in the mac the loading of jpg (done via qt plugins) fails
//	qApp->addLibraryPath(getPluginDirPath());
//	qApp->addLibraryPath(getBaseDirPath());
//	
//	QStringList pluginfilters;
//#if defined(Q_OS_WIN)
//	pluginfilters << "*.dll";		
//#elif defined(Q_OS_MAC)
//	pluginfilters << "*.dylib";		
//#else
//#endif
//	pluginsDir.setNameFilters(pluginfilters);
//
//	qDebug( "Current Plugins Dir: %s ",qPrintable(pluginsDir.absolutePath()));
//	foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
//		QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
//		QObject *plugin = loader.instance();
//
//		if (plugin) {
//		  MeshFilterInterface *iFilter = qobject_cast<MeshFilterInterface *>(plugin);
//			if (iFilter)
//      {
//
//        QAction *filterAction;
//        
//				foreach(filterAction, iFilter->actions())
//        {
//					//qDebug("Processing action %s",qPrintable(filterAction->text()) );
//					//qDebug("          (%s)", qPrintable(iFilter->filterInfo(filterAction)) );
//					iFilter->initGlobalParameterSet(filterAction,defaultGlobalParams);
//					filterMap[filterAction->text()]=filterAction;
//          filterAction->setToolTip(iFilter->filterInfo(filterAction));
//          connect(filterAction,SIGNAL(triggered()),this,SLOT(startFilter()));
//          int filterClass = iFilter->getClass(filterAction);
//
//          if( (filterClass & MeshFilterInterface::FaceColoring) ||
//              (filterClass & MeshFilterInterface::VertexColoring) )
//              		filterMenuColorize->addAction(filterAction);
//					if(filterClass & MeshFilterInterface::Selection)
//              		filterMenuSelect->addAction(filterAction);
//					if(filterClass &  MeshFilterInterface::Cleaning )
//              		filterMenuClean->addAction(filterAction);
//					if(filterClass &  MeshFilterInterface::Remeshing )
//              		filterMenuRemeshing->addAction(filterAction);
//					if(filterClass &  MeshFilterInterface::Smoothing )
//              		filterMenuSmoothing->addAction(filterAction);
//					if(filterClass &  MeshFilterInterface::Normal )
//              		filterMenuNormal->addAction(filterAction);
//					if( (filterClass &  MeshFilterInterface::Quality ) ||
//              (filterClass & MeshFilterInterface::Measure  )	)				
//              		filterMenuQuality->addAction(filterAction);
//					if(filterClass &  MeshFilterInterface::Layer )
//              		filterMenuLayer->addAction(filterAction);
//					if(filterClass & MeshFilterInterface::MeshCreation )
//              		fileMenuNew->addAction(filterAction);
//					if(filterClass & MeshFilterInterface::RangeMap )
//              		filterMenuRangeMap->addAction(filterAction);
//          if(filterClass & MeshFilterInterface::PointSet )
//                  filterMenuPointSet->addAction(filterAction);
//          if(filterClass & MeshFilterInterface::Sampling )
//									filterMenuSampling->addAction(filterAction);
//			if (filterClass & MeshFilterInterface::Texture)
//				filterMenuTexture->addAction(filterAction);
//					if(filterClass == 0) //  MeshFilterInterface::Generic :
//									filterMenu->addAction(filterAction);
//
//					if(!filterAction->icon().isNull())
//										filterToolBar->addAction(filterAction);
//        }
//       }
//		  MeshIOInterface *iIO = qobject_cast<MeshIOInterface *>(plugin);
//			if (iIO) 
//				meshIOPlugins.push_back(iIO);
//
//			MeshDecorateInterface *iDecorator = qobject_cast<MeshDecorateInterface *>(plugin);
//			if (iDecorator){
//				QAction *decoratorAction;
//				decoratorActionList+=iDecorator->actions();
//				foreach(decoratorAction, iDecorator->actions())
//						{
//							iDecorator->initGlobalParameterSet(decoratorAction,&defaultGlobalParams);
//								connect(decoratorAction,SIGNAL(triggered()),this,SLOT(applyDecorateMode()));
//								decoratorAction->setToolTip(iDecorator->Info(decoratorAction));
//								renderMenu->addAction(decoratorAction);
//						}
//			}
//
//			MeshRenderInterface *iRender = qobject_cast<MeshRenderInterface *>(plugin);
//			if (iRender)
//			  addToMenu(iRender->actions(), shadersMenu, SLOT(applyRenderMode()));
///*
//			MeshEditInterface *iEdit = qobject_cast<MeshEditInterface *>(plugin);
//			QAction *editAction;
//			if (iEdit)
//        foreach(editAction, iEdit->actions())
//        {
//			    editMenu->addAction(editAction);
//          if(!editAction->icon().isNull())
//              editToolBar->addAction(editAction);
//          connect(editAction,SIGNAL(triggered()),this,SLOT(applyEditMode()));
//          editActionList.push_back(editAction);
//        }
//*/		
//			MeshEditInterfaceFactory *iEditFactory = qobject_cast<MeshEditInterfaceFactory *>(plugin);
//			QAction *editAction = 0;
//			if(iEditFactory)
//			{
//				//qDebug() << "Here with filename:" << fileName;
//				
//				foreach(editAction, iEditFactory->actions())
//				{
//					editMenu->addAction(editAction);
//					if(!editAction->icon().isNull())
//					{
//						editToolBar->addAction(editAction);
//					} else qDebug() << "action was null";
//					
//					connect(editAction, SIGNAL(triggered()), this, SLOT(applyEditMode()));
//					editActionList.push_back(editAction);
//				}
//			}
//			
//			pluginFileNames += fileName;
//		} else
//		{
//			qDebug() << "error loading plugin with filename:" << fileName;
//			qDebug() << loader.errorString();
//        }
//	}
//	filterMenu->setEnabled(!filterMenu->actions().isEmpty() && mdiarea->activeSubWindow());
//
}


void MainWindow::loadMeshLabSettings()
{
  // I have already loaded the plugins so the default for the plugins are already in.
	// we just miss the globals default of meshlab itself
	GLArea::initGlobalParameterSet(& defaultGlobalParams);
	
	QSettings settings;
	QStringList klist = settings.allKeys();

  // 1) load saved values
	for(int ii = 0;ii < klist.size();++ii)
	{
		QDomDocument doc;
        doc.setContent(settings.value(klist.at(ii)).toString());

		QString st = settings.value(klist.at(ii)).toString();
        QDomElement docElem = doc.firstChild().toElement();

		RichParameter* rpar = NULL;
        if(!docElem.isNull())
		{
            bool ret = RichParameterFactory::create(docElem,&rpar);
            if (!ret)
            {
                qDebug("Warning Ignored parameter '%s' = '%s'. Malformed.", qPrintable(docElem.attribute("name")),qPrintable(docElem.attribute("value")));
                continue;
            }
            if (!defaultGlobalParams.hasParameter(rpar->name))
            {
                qDebug("Warning Ignored parameter %s. In the saved parameters there are ones that are not in the HardWired ones. "
                       "It happens if you are running MeshLab with only a subset of the plugins. ",qPrintable(rpar->name));
            }
            else currentGlobalParams.addParam(rpar);
		}
	}

	// 2) eventually fill missing values with the hardwired defaults 
	for(int ii = 0;ii < defaultGlobalParams.paramList.size();++ii)
	{
		qDebug("Searching param[%i] %s of the default into the loaded settings. ",ii,qPrintable(defaultGlobalParams.paramList.at(ii)->name));
		if (!currentGlobalParams.hasParameter(defaultGlobalParams.paramList.at(ii)->name))
		{
			qDebug("Warning! a default param was not found in the saved settings. This should happen only on the first run...");
			RichParameterCopyConstructor v;
			defaultGlobalParams.paramList.at(ii)->accept(v);
			currentGlobalParams.paramList.push_back(v.lastCreated);
			
			QDomDocument doc("MeshLabSettings");
			RichParameterXMLVisitor vxml(doc);
			v.lastCreated->accept(vxml);
			doc.appendChild(vxml.parElem);
			QString docstring =  doc.toString();
			QSettings setting;
			setting.setValue(v.lastCreated->name,QVariant(docstring));
		}	
	}
	
	emit dispatchCustomSettings(currentGlobalParams);
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

				/*QDialog *congratsDialog = new QDialog();
				Ui::CongratsDialog temp;
				temp.setupUi(congratsDialog);

				temp.buttonBox->addButton("Send Mail", QDialogButtonBox::AcceptRole);
				temp.congratsTextEdit->setHtml(tttt);
				congratsDialog->exec();
				if(congratsDialog->result()==QDialog::Accepted)
					QDesktopServices::openUrl(QUrl("mailto:p.cignoni@isti.cnr.it?subject=[MeshLab] Reporting Info on MeshLab Usage"));*/
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

