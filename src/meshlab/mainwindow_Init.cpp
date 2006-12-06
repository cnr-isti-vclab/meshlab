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

#include "meshmodel.h"
#include "interfaces.h"
#include "mainwindow.h"
#include "glarea.h"
#include "plugindialog.h"
#include "customDialog.h"	
#include "saveSnapshotDialog.h"	

QProgressBar *MainWindow::qb;

MainWindow::MainWindow()
{
	workspace = new QWorkspace(this);
	setCentralWidget(workspace);
	windowMapper = new QSignalMapper(this);	
	// Permette di passare da una finestra all'altra e tenere aggiornato il workspace
	connect(windowMapper, SIGNAL(mapped(QWidget *)),workspace, SLOT(setActiveWindow(QWidget *)));
	// Quando si passa da una finestra all'altra aggiorna lo stato delle toolbar e dei menu
	connect(workspace, SIGNAL(windowActivated(QWidget *)),this, SLOT(updateMenus()));
	connect(workspace, SIGNAL(windowActivated(QWidget *)),this, SLOT(updateWindowMenu()));

  httpReq=new QHttp(this);
  //connect(httpReq, SIGNAL(requestFinished(int,bool)), this, SLOT(connectionFinished(int,bool)));
  connect(httpReq, SIGNAL(done(bool)), this, SLOT(connectionDone(bool)));

	createActions();
	createMenus();
	createToolBars();
	updateMenus();
  setAcceptDrops(true);
  workspace->setAcceptDrops(true);
	setWindowTitle(appName());
	loadPlugins();
  setStatusBar(new QStatusBar());
  globalStatusBar()=statusBar();
  qb=new QProgressBar(this);
  qb->setMaximum(100);
  qb->setMinimum(0);
  statusBar()->addPermanentWidget(qb,0);
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

  closeAct = new QAction(tr("&Close"), this);
	closeAct->setShortcutContext(Qt::ApplicationShortcut);
	closeAct->setShortcut(Qt::CTRL+Qt::Key_C);
	connect(closeAct, SIGNAL(triggered()),workspace, SLOT(closeActiveWindow()));


  reloadAct = new QAction(QIcon(":/images/reload.png"),tr("&Reload"), this);
	reloadAct->setShortcutContext(Qt::ApplicationShortcut);
	reloadAct->setShortcut(Qt::CTRL+Qt::Key_R);
	connect(reloadAct, SIGNAL(triggered()), this, SLOT(reload()));


	saveAsAct = new QAction(QIcon(":/images/save.png"),tr("&Save As..."), this);
	saveAsAct->setShortcutContext(Qt::ApplicationShortcut);
	saveAsAct->setShortcut(Qt::CTRL+Qt::Key_S);
	connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

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
	setSelectionRenderingAct->setShortcut(Qt::CTRL+Qt::Key_S);
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

	showLogAct= new QAction (tr("Show &Infos"), this);
	showLogAct->setCheckable(true);
	connect(showLogAct, SIGNAL(triggered()), this, SLOT(showLog()));

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

  //////////////Action Menu EDIT /////////////////////////////////////////////////////////////////////////
  endEditModeAct = new QAction (QIcon(":/images/no_edit.png"),tr("Not editing"), this);
	endEditModeAct->setShortcut(Qt::Key_Escape);
	endEditModeAct->setCheckable(true);
	endEditModeAct->setChecked(true);
	connect(endEditModeAct, SIGNAL(triggered()), this, SLOT(endEditMode()));

	//////////////Action Menu WINDOWS /////////////////////////////////////////////////////////////////////////
	windowsTileAct = new QAction(tr("&Tile"), this);
	connect(windowsTileAct, SIGNAL(triggered()), workspace, SLOT(tile()));

	windowsCascadeAct = new QAction(tr("&Cascade"), this);
	connect(windowsCascadeAct, SIGNAL(triggered()), workspace, SLOT(cascade()));

  windowsNextAct = new QAction(tr("&Next"), this);
	connect(windowsNextAct, SIGNAL(triggered()), workspace, SLOT(activateNextWindow()));

	closeAllAct = new QAction(tr("Close &All Windows"), this);
	connect(closeAllAct, SIGNAL(triggered()),workspace, SLOT(closeAllWindows()));

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
}

void MainWindow::createToolBars()
{
	mainToolBar = addToolBar(tr("Standard"));
	mainToolBar->setIconSize(QSize(32,32));
	mainToolBar->addAction(openAct);
	mainToolBar->addAction(reloadAct);
	mainToolBar->addAction(saveAsAct);
	mainToolBar->addAction(saveSnapshotAct);

	renderToolBar = addToolBar(tr("Render"));
	//renderToolBar->setIconSize(QSize(32,32));
	renderToolBar->addActions(renderModeGroupAct->actions());
	renderToolBar->addAction(renderModeTextureAct);
	renderToolBar->addAction(setLightAct);
	renderToolBar->addAction(setSelectionRenderingAct);

  editToolBar = addToolBar(tr("Edit"));
	editToolBar->addAction(endEditModeAct);

}


void MainWindow::createMenus()
{
	//////////////////// Menu File ////////////////////////////////////////////////////////////////////////////
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(openAct);
	fileMenu->addAction(closeAct);
	fileMenu->addAction(reloadAct);
	fileMenu->addAction(saveAsAct);

	
	fileMenu->addSeparator();
		fileMenu->addAction(saveSnapshotAct);
	separatorAct = fileMenu->addSeparator();
	
	for (int i = 0; i < MAXRECENTFILES; ++i) fileMenu->addAction(recentFileActs[i]);
	updateRecentFileActions();
	fileMenu->addSeparator();
	fileMenu->addAction(exitAct);
	
	//////////////////// Menu Edit //////////////////////////////////////////////////////////////////////////
	editMenu = menuBar()->addMenu(tr("&Edit"));
	editMenu->addAction(endEditModeAct);
	
  //////////////////// Menu Filter //////////////////////////////////////////////////////////////////////////
	filterMenu = menuBar()->addMenu(tr("Fi&lters"));
	filterMenu->addAction(lastFilterAct);
  filterMenu->addAction(showFilterScriptAct);
	filterMenu->addSeparator();
	filterMenuSelect = filterMenu->addMenu(tr("Select"));
	filterMenuClean  = filterMenu->addMenu(tr("Clean"));
	filterMenuRemeshing = filterMenu->addMenu(tr("Remeshing"));


	
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

	colorModePerVertexAct = new QAction(QString("Per &Vertex"),colorModeGroupAct);
	colorModePerVertexAct->setCheckable(true);

	colorModePerFaceAct = new QAction(QString("Per &Face"),colorModeGroupAct); 
	colorModePerFaceAct->setCheckable(true);
	

	colorModeMenu->addAction(colorModeNoneAct);
	colorModeMenu->addAction(colorModePerVertexAct);
	colorModeMenu->addAction(colorModePerFaceAct);

	connect(colorModeGroupAct, SIGNAL(triggered(QAction *)), this, SLOT(setColorMode(QAction *)));

	// Shaders SUBmenu
	shadersMenu = renderMenu->addMenu(tr("&Shaders"));

	renderMenu->addSeparator();

	//////////////////// Menu View ////////////////////////////////////////////////////////////////////////////
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
			//MeshColorizeInterface *iColor = qobject_cast<MeshColorizeInterface *>(plugin);
						
		  MeshFilterInterface *iFilter = qobject_cast<MeshFilterInterface *>(plugin);
			if (iFilter)
      { 
        QAction *filterAction;
        foreach(filterAction, iFilter->actions())
        {
          filterMap[filterAction->text()]=filterAction;
          connect(filterAction,SIGNAL(triggered()),this,SLOT(applyFilter()));
          switch(iFilter->getClass(filterAction))
          {
            case MeshFilterInterface::FaceColoring : 
            case MeshFilterInterface::VertexColoring : 
              		colorModeMenu->addAction(filterAction); break;
            case MeshFilterInterface::Selection : 
              		filterMenuSelect->addAction(filterAction); 
                  if(!filterAction->icon().isNull())
                      editToolBar->addAction(filterAction);
            break;
            case MeshFilterInterface::Cleaning : 
              		filterMenuClean->addAction(filterAction); break;
            case MeshFilterInterface::Remeshing : 
              		filterMenuRemeshing->addAction(filterAction); break;
            case MeshFilterInterface::Generic : 
            default:
              		filterMenu->addAction(filterAction); break;
          }  
        }
       }
		  MeshIOInterface *iIO = qobject_cast<MeshIOInterface *>(plugin);
			if (iIO)
				meshIOPlugins.push_back(iIO);

			MeshDecorateInterface *iDecorator = qobject_cast<MeshDecorateInterface *>(plugin);
			if (iDecorator){
				TotalDecoratorsList=iDecorator->actions();
				addToMenu(iDecorator->actions(), renderMenu, SLOT(applyDecorateMode()));
			}

			MeshRenderInterface *iRender = qobject_cast<MeshRenderInterface *>(plugin);
			if (iRender)
			  addToMenu(iRender->actions(), shadersMenu, SLOT(applyRenderMode()));

			MeshEditInterface *iEdit = qobject_cast<MeshEditInterface *>(plugin);
      QAction *editAction;
			if (iEdit)
        foreach(editAction, iEdit->actions())
        {
			    editMenu->addAction(editAction);
          if(!editAction->icon().isNull())
              editToolBar->addAction(editAction);
          connect(editAction,SIGNAL(triggered()),this,SLOT(applyEditMode()));
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

  int totalKV=settings.value("totalKV",0).toInt();
  settings.setValue("totalKV",totalKV+(GLA()->mm->cm.vn)/1000);
  int connectionInterval=settings.value("connectionInterval",20).toInt();
  settings.setValue("connectionInterval",connectionInterval);
  int loadedMeshCounter=settings.value("loadedMeshCounter",0).toInt();
  settings.setValue("loadedMeshCounter",loadedMeshCounter+1);
  int savedMeshCounter=settings.value("savedMeshCounter",0).toInt();
  int lastComunicatedValue=settings.value("lastComunicatedValue",0).toInt();
  QString UID=settings.value("UID",QString("")).toString();
  if(UID.isEmpty())
  {
    UID=QUuid::createUuid ().toString();
    settings.setValue("UID",UID);
  }
  if(loadedMeshCounter-lastComunicatedValue>connectionInterval && !myLocalBuf.isOpen())
  {
#ifdef _DEBUG_PHP
    QString BaseCommand("/~cignoni/meshlab_d.php");
#else 
    QString BaseCommand("/~cignoni/meshlab.php");
#endif
#ifdef Q_WS_WIN    
    QString OS="Win";
#elif Q_WS_MAC
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
}


void MainWindow::connectionDone(bool status)
{
        QString answer=myLocalBuf.data();
        if(answer.left(3)==QString("NEW"))
          QMessageBox::information(this,"MeshLab Version Checking",answer.remove(0,3));
        myLocalBuf.close();
        //QMessageBox::information(this,"Remote Counter",QString("Updated!"));
        QSettings settings;
        int loadedMeshCounter=settings.value("loadedMeshCounter",0).toInt();
        settings.setValue("lastComunicatedValue",loadedMeshCounter);
}