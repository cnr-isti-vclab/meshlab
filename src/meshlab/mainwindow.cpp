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

	createActions();
	createMenus();
	createToolBars();
	updateMenus();
	addToolBar(mainToolBar);
	addToolBar(renderToolBar);
	setWindowTitle(tr("MeshLab v0.1"));
	loadPlugins();
	if(QCoreApplication::instance ()->argc()>1){
		open(QCoreApplication::instance ()->argv()[1]);
	}
	else{ 
		QTimer::singleShot(500, this, SLOT(open()));
	}
  qb=new QProgressBar(this);
  qb->setMaximum(100);
  qb->setMinimum(0);
  qb->hide();
}

void MainWindow::open(QString fileName)
{
	if (fileName.isEmpty()){
		fileName = QFileDialog::getOpenFileName(this,tr("Open File"),"../sample","Mesh files (*.ply *.off *.stl)");
	}
	if (!fileName.isEmpty()) {
		MeshModel *nm= new MeshModel();
		if(!nm->Open(fileName.toAscii())){
			QMessageBox::information(this, tr("Error"),tr("Cannot load %1.").arg(fileName));
    	delete nm;
			//return;
		}
		else{
			//VM.push_back(nm);
			gla=new GLArea(workspace);
			gla->mm=nm;
			gla->setWindowTitle(QFileInfo(fileName).fileName());   
			workspace->addWindow(gla);
			if(workspace->isVisible()) gla->showMaximized();
			else QTimer::singleShot(00, gla, SLOT(showMaximized()));
      //setCurrentFile(fileName);
      if(!gla->mm->cm.textures.empty()){
        QMessageBox::information(this, tr("Error"),tr("Cannot load %1.").arg(gla->mm->cm.textures[0].c_str()));
      }
			//return;
		}
	}
}

void MainWindow::openRecentFile()
{
	QAction *action = qobject_cast<QAction *>(sender());
	if (action)	open(action->data().toString());
}

bool MainWindow::saveAs()
{
	QString initialPath = QDir::currentPath() + "/untitled.png";

	QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), initialPath);
	if (fileName.isEmpty()) {
		return false;
	} else {
		//       return paintArea->saveImage(fileName, "png");
		return true;
	}
}

bool MainWindow::saveSnapshot()
{
	QString snapshotPath = "snapshot.ppm";

	bool ret=GLA()->saveSnapshot(snapshotPath);

	if (ret) 
		GLA()->log.Log(GLLogStream::Info,"Snapshot saved to %s",snapshotPath.toLocal8Bit().constData());
	else
		GLA()->log.Log(GLLogStream::Error,"Error saving snapshot %s",snapshotPath.toLocal8Bit().constData());

	return ret;
}
void MainWindow::about()
{
	QMessageBox::about(this, tr("About Plug & Paint"),
		tr("The <b>Plug & Paint</b> example demonstrates how to write Qt "
		"applications that can be extended through plugins."));
}

void MainWindow::aboutPlugins()
{
	PluginDialog dialog(pluginsDir.path(), pluginFileNames, this);
	dialog.exec();
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
	renderModeGroup = new QActionGroup(this);
	
	renderBboxAct	  = new QAction(QIcon(":/images/bbox.png"),tr("&Bounding box"), renderModeGroup);
	renderBboxAct->setCheckable(true);
	//renderBboxAct->setChecked(true);
	connect(renderBboxAct, SIGNAL(triggered()), this, SLOT(RenderBbox()));
	
	
	renderModePointsAct	  = new QAction(QIcon(":/images/points.png"),tr("&Points"), renderModeGroup);
	renderModePointsAct->setCheckable(true);
	connect(renderModePointsAct, SIGNAL(triggered()), this, SLOT(RenderPoint()));

	renderModeWireAct		  = new QAction(QIcon(":/images/wire.png"),tr("&Wireframe"), renderModeGroup);
	renderModeWireAct->setCheckable(true);
	connect(renderModeWireAct, SIGNAL(triggered()), this, SLOT(RenderWire()));

	renderModeHiddenLinesAct		  = new QAction(QIcon(":/images/backlines.png"),tr("&Hidden Lines"), renderModeGroup);
	renderModeHiddenLinesAct->setCheckable(true);
	connect(renderModeHiddenLinesAct, SIGNAL(triggered()), this, SLOT(RenderHiddenLines()));

	renderModeFlatLinesAct = new QAction(QIcon(":/images/flatlines.png"),tr("Flat &Lines"), renderModeGroup);
	renderModeFlatLinesAct->setCheckable(true);
	connect(renderModeFlatLinesAct, SIGNAL(triggered()), this, SLOT(RenderFlatLine()));

	renderModeFlatAct		  = new QAction(QIcon(":/images/flat.png"),tr("&Flat"), renderModeGroup);
	renderModeFlatAct->setCheckable(true);
	connect(renderModeFlatAct, SIGNAL(triggered()), this, SLOT(RenderFlat()));

	renderModeSmoothAct	  = new QAction(QIcon(":/images/smooth.png"),tr("&Smooth"), renderModeGroup);
	renderModeSmoothAct->setCheckable(true);
	renderModeSmoothAct->setChecked(true);
	connect(renderModeSmoothAct, SIGNAL(triggered()), this, SLOT(RenderSmooth()));

	setLightAct	  = new QAction(QIcon(":/images/lighton.png"),tr("&Light on/off"),this);
	setLightAct->setCheckable(true);
	setLightAct->setChecked(true);
	connect(setLightAct, SIGNAL(triggered()), this, SLOT(SetLight()));

	setDoubleLightingAct= new QAction(tr("&Double side lighting"),this);
	setDoubleLightingAct->setCheckable(true);
	setDoubleLightingAct->setChecked(false);
	connect(setDoubleLightingAct, SIGNAL(triggered()), this, SLOT(SetDoubleLighting()));

	setFancyLightingAct	  = new QAction(tr("&Fancy Lighting"),this);
	setFancyLightingAct->setCheckable(true);
	setFancyLightingAct->setChecked(false);
	connect(setFancyLightingAct, SIGNAL(triggered()), this, SLOT(SetFancyLighting()));

	setCustomizeAct	  = new QAction(tr("&Customize Colors"),this);
	connect(setCustomizeAct, SIGNAL(triggered()), this, SLOT(SetCustomize()));

	
	//////////////Action Menu View /////////////////////////////////////////////////////////////
	viewToolbarStandardAct = new QAction (tr("&Standard"), this);
	viewToolbarStandardAct->setCheckable(true);
	viewToolbarStandardAct->setChecked(true);
	connect(viewToolbarStandardAct, SIGNAL(triggered()), this, SLOT(viewToolbarFile()));

	viewToolbarRenderAct = new QAction (tr("&Render"), this);
	viewToolbarRenderAct->setCheckable(true);
	viewToolbarRenderAct->setChecked(true);
	connect(viewToolbarRenderAct, SIGNAL(triggered()), this, SLOT(viewToolbarRender()));

	viewLogAct= new QAction (tr("View &Log"), this);
	viewLogAct->setCheckable(true);
	viewLogAct->setChecked(false);
	connect(viewLogAct, SIGNAL(triggered()), this, SLOT(viewLog()));


	//////////////Action Menu Windows /////////////////////////////////////////////////////////
	windowsTileAct = new QAction(tr("&Tile"), this);
	connect(windowsTileAct, SIGNAL(triggered()), this, SLOT(windowsTile()));

	windowsCascadeAct = new QAction(tr("&Cascade"), this);
	connect(windowsCascadeAct, SIGNAL(triggered()), this, SLOT(windowsCascade()));

	closeAct = new QAction(tr("Cl&ose"), this);
	closeAct->setShortcut(tr("Ctrl+F4"));
	closeAct->setStatusTip(tr("Close the active window"));
	connect(closeAct, SIGNAL(triggered()),workspace, SLOT(closeActiveWindow()));

	closeAllAct = new QAction(tr("Close &All"), this);
	closeAllAct->setStatusTip(tr("Close all the windows"));
	connect(closeAllAct, SIGNAL(triggered()),workspace, SLOT(closeAllWindows()));

	//////////////Action Menu About ////////////////////////////////////////////////////////////
	aboutAct = new QAction(tr("&About"), this);
	connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

	aboutQtAct = new QAction(tr("About &Qt"), this);
	connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

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
	renderToolBar->addActions(renderModeGroup->actions());
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
  //renderMenu->addActions(renderModeGroup->actions());
	//renderMenu->addAction(setLightAct);
//  renderMenu->addAction(renderModeAct);
//  renderMenu->addAction(lightingModeAct);
//	lightingModeAct->addAction(setLightAct);
	renderModeMenu=renderMenu->addMenu(tr("Render Mode"));
	renderModeMenu->addActions(renderModeGroup->actions());
	
	lightingModeMenu=renderMenu->addMenu(tr("Lighting"));
	lightingModeMenu->addAction(setLightAct);
	lightingModeMenu->addAction(setDoubleLightingAct);
	lightingModeMenu->addAction(setFancyLightingAct);


	textureModeMenu=renderMenu->addMenu(tr("Texture"));
	colorModeMenu=renderMenu->addMenu(tr("Color"));
	

	//////////////////// Menu View ////////////////////////////////////////////////////////////////
	viewMenu		= menuBar()->addMenu(tr("&View"));
	viewMenu->addAction(viewLogAct);
	toolBarMenu	= viewMenu->addMenu(tr("&ToolBars"));
	toolBarMenu->addAction(viewToolbarStandardAct);
	toolBarMenu->addAction(viewToolbarRenderAct);
	

	//////////////////// Menu Windows /////////////////////////////////////////////////////////////
	windowsMenu = menuBar()->addMenu(tr("&Windows"));
	connect(windowsMenu, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));
	menuBar()->addSeparator();


	//////////////////// Menu Preferences /////////////////////////////////////////////////////////////
	preferencesMenu=menuBar()->addMenu(tr("&Preferences"));
	preferencesMenu->addAction(setCustomizeAct);

	//////////////////// Menu Help ////////////////////////////////////////////////////////////////
	helpMenu = menuBar()->addMenu(tr("&Help"));
	helpMenu->addAction(aboutAct);
	helpMenu->addAction(aboutQtAct);
	helpMenu->addAction(aboutPluginsAct);
}

void MainWindow::viewLog()
{

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

			MeshFilterInterface *iFilter = qobject_cast<MeshFilterInterface *>(plugin);
			if (iFilter)
				addToMenu(plugin, iFilter->filters(), filterMenu, SLOT(applyFilter()));

			MeshRenderInterface *iRender = qobject_cast<MeshRenderInterface *>(plugin);
			if (iRender)
				addToMenu(plugin, iRender->modes(), renderMenu, SLOT(applyRenderMode()),0,true);

			// MeshColorizeInterface test 
			MeshColorizeInterface *iColor = qobject_cast<MeshColorizeInterface *>(plugin);
			if (iColor)
				addToMenu(plugin, iColor->colorsFrom(), colorModeMenu, SLOT(applyColorMode()),0,true);     

			MeshIOInterface *iIO = qobject_cast<MeshIOInterface *>(plugin);
			if (iIO)
			{
				fileMenu->addSeparator();
				addToMenu(plugin, iIO->formats(), fileMenu, SLOT(applyImportExport()));
				//fileMenu->addSeparator();
			}


			pluginFileNames += fileName;
		}
	}
	filterMenu->setEnabled(!filterMenu->actions().isEmpty() && workspace->activeWindow());
}

void MainWindow::addToMenu(QObject *plugin, const QStringList &texts,QMenu *menu, const char *member,
													 QActionGroup *actionGroup,bool chackable)
{
	foreach (QString text, texts) {
		QAction *action = new QAction(text, plugin);
		TotalRenderList.push_back(action);
		connect(action, SIGNAL(triggered()), this, member);
		action->setCheckable(chackable);
		menu->addAction(action);

		if (actionGroup) {
			action->setCheckable(true);
			actionGroup->addAction(action);
		}
	}
}

void MainWindow::applyFilter()
{
	QAction *action = qobject_cast<QAction *>(sender());
	MeshFilterInterface *iFilter = qobject_cast<MeshFilterInterface *>(action->parent());
	qb->show();
	//iFilter->applyFilter(action->text(),*(GLA()->mm ),this);
	iFilter->applyFilter(action->text(),*(GLA()->mm ), GLA(),QCallBack);
	GLA()->log.Log(GLLogStream::Info,"Applied filter %s",action->text().toLocal8Bit().constData());// .data());
  qb->hide();
}


bool MainWindow::QCallBack(const int pos, const char * str)
{
  if(qb==0) return true;
  qb->setWindowTitle (str);
  qb->setValue(pos);
  qb->update();
  return true;
}

void MainWindow::applyRenderMode()
{
	QAction *action = qobject_cast<QAction *>(sender());
	MeshRenderInterface *iRenderTemp = qobject_cast<MeshRenderInterface *>(action->parent());
	if(GLA()->iRendersList==0){
		GLA()->iRendersList= new list<pair<QAction *,MeshRenderInterface *> >;
		GLA()->iRendersList->push_back(make_pair(action,iRenderTemp));
		GLA()->log.Log(GLLogStream::Info,"Enable Render mode %s",action->text().toLocal8Bit().constData());// .data());
	}else{
		bool found=false;
		pair<QAction *,MeshRenderInterface *> p;
		foreach(p,*GLA()->iRendersList){
			if(iRenderTemp==p.second && p.first->text()==action->text()){
				GLA()->iRendersList->remove(p);
				GLA()->log.Log(0,"Disabled Render mode %s",action->text().toLocal8Bit().constData());// .data());
				found=true;
			} 
		}
		if(!found){
			GLA()->iRendersList->push_back(make_pair(action,iRenderTemp));
			GLA()->log.Log(GLLogStream::Info,"Enable Render mode %s",action->text().toLocal8Bit().constData());// .data());
		}
	}
}


// MeshColorizeInterface test 
void MainWindow::applyColorMode()
{
	QAction *action = qobject_cast<QAction *>(sender());
	MeshColorizeInterface *iColor = qobject_cast<MeshColorizeInterface *>(action->parent());
	iColor->Compute(action->text(),*(GLA()->mm ), GLA());
	
	// when apply colorize we have to switch to a different color mode!!
	// Still not working
	GLA()->setColorMode(GLW::CMPerVert);

	GLA()->log.Log(GLLogStream::Info,"Applied colorize %s",action->text().toLocal8Bit().constData());// .data());
}

void MainWindow::applyImportExport()
{
	QAction *action = qobject_cast<QAction *>(sender());
	MeshIOInterface *iIO = qobject_cast<MeshIOInterface *>(action->parent());

	if(action->text().contains("Export"))
	{
		QString fileName;
		if(iIO->save(action->text(),fileName,*(GLA()->mm ),0,NULL,GLA()) )
		GLA()->log.Log(GLLogStream::Info,"File saved correctly");
	}

	if(action->text().contains("Import"))
	{
		int mask;
    qb->show();
		if (GLA() == NULL)
		{
			MeshModel *mm= new MeshModel();
			gla = new GLArea(workspace);
		
			QString fileName;
			if( iIO->open(action->text(), fileName, *mm ,mask,QCallBack,gla ) )
			{
				gla->mm=mm;	
				gla->setWindowTitle(QFileInfo(fileName).fileName());   
				workspace->addWindow(gla);
				
				if(workspace->isVisible()) gla->showMaximized();
				else QTimer::singleShot(00, gla, SLOT(showMaximized()));
		  
				if(!mm->cm.textures.empty())
					QMessageBox::information(this, tr("Error"),tr("Cannot load %1.").arg(gla->mm->cm.textures[0].c_str()));
        qb->hide();
				//GLA()->log.Log(GLLogStream::Info,"File loaded correctly");		
			}
			else
			{
				QMessageBox::information(this, tr("Error"),tr("Cannot load %1.").arg(fileName));
    		delete mm;
			}
		}
		else{
			QString fileName;
			if( iIO->open(action->text(), fileName,*(GLA()->mm ),mask,NULL,GLA()) )
			{	
				GLA()->log.Log(GLLogStream::Info,"File loaded correctly");
				if(!GLA()->mm->cm.textures.empty())
					QMessageBox::information(this, tr("Error"),tr("Cannot load %1.").arg(gla->mm->cm.textures[0].c_str()));

			}
		}
	}
}


void MainWindow::windowsTile(){
	workspace->tile();
}

void MainWindow::windowsCascade(){
	workspace->cascade();
}
void MainWindow::viewToolbarFile(){
		mainToolBar->setVisible(!mainToolBar->isVisible());
		viewToolbarStandardAct->setChecked(mainToolBar->isVisible());
}

void MainWindow::viewToolbarRender(){
	if(renderToolBar->isVisible()){
		renderToolBar->hide();
		viewToolbarRenderAct->setChecked(false);
	}else{
		renderToolBar->show();
		viewToolbarRenderAct->setChecked(true);
	}
}

void MainWindow::RenderBbox()        { GLA()->setDrawMode(GLW::DMBox     ); }
void MainWindow::RenderPoint()       { GLA()->setDrawMode(GLW::DMPoints  ); }
void MainWindow::RenderWire()        { GLA()->setDrawMode(GLW::DMWire    ); }
void MainWindow::RenderFlat()        { GLA()->setDrawMode(GLW::DMFlat    ); }
void MainWindow::RenderSmooth()      { GLA()->setDrawMode(GLW::DMSmooth  ); }
void MainWindow::RenderFlatLine()    { GLA()->setDrawMode(GLW::DMFlatWire); }
void MainWindow::RenderHiddenLines() { GLA()->setDrawMode(GLW::DMHidden  ); }


void MainWindow::SetFancyLighting()
{
	const RenderMode &rm=GLA()->getCurrentRenderMode();
	if (rm.FancyLighting) GLA()->setLightMode(false,LFANCY);
	else GLA()->setLightMode(true,LFANCY);
}

void MainWindow::SetDoubleLighting()
{
	const RenderMode &rm=GLA()->getCurrentRenderMode();
	if (rm.DoubleSideLighting) GLA()->setLightMode(false,LDOUBLE);
	else GLA()->setLightMode(true,LDOUBLE);
}

void MainWindow::SetLight()			     
{
// Is this check needed???
//	if (!GLA())
//		return;

	GLA()->setLight(!GLA()->getCurrentRenderMode().Lighting);
	updateMenus();
};


void MainWindow::SetCustomize()
{
	/*QColor backColor=QColorDialog::getColor(QColor(255,255,255,255),this);
	GLA()->setBackground(backColor);*/
	CustomDialog dialog(this);
	ColorSetting cs=GLA()->getCustomSetting();
	dialog.LoadCurrentSetting(cs.bColorBottom,cs.bColorTop,cs.lColor);
	if (dialog.exec()==QDialog::Accepted) 
	{
		cs.bColorBottom=dialog.GetBackgroundBottomColor();
		cs.bColorTop=dialog.GetBackgroundTopColor();
		cs.lColor=dialog.GetLogColor();
    GLA()->setCustomSetting(cs);	
	}
	
}


void MainWindow::updateWindowMenu()
{
	windowsMenu->clear();
	windowsMenu->addAction(closeAct);
	windowsMenu->addAction(closeAllAct);
	windowsMenu->addSeparator();
	windowsMenu->addAction(windowsTileAct);
	windowsMenu->addAction(windowsCascadeAct);

	QWidgetList windows = workspace->windowList();

	if(windows.size() > 0)
			windowsMenu->addSeparator();

	int i=0;
	foreach(QWidget *w,windows)
	{
		QString text = tr("&%1. %2").arg(i+1).arg(QFileInfo(w->windowTitle()).fileName());
		QAction *action  = windowsMenu->addAction(text);
		action->setCheckable(true);
		action->setChecked(w == workspace->activeWindow());
		// Connect the signal to activate the selected window
		connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
		windowMapper->setMapping(action, w);
		++i;
	}
}
void MainWindow::updateRecentFileActions()
{
	QSettings settings("Recent Files");
	QStringList files = settings.value("recentFileList").toStringList();
	int numRecentFiles = qMin(files.size(), (int)MAXRECENTFILES);
	for (int i = 0; i < numRecentFiles; ++i) {
		QString text = tr("&%1 %2").arg(i + 1).arg(QFileInfo(files[i]).fileName());
		recentFileActs[i]->setText(text);
		recentFileActs[i]->setData(files[i]);
		recentFileActs[i]->setVisible(true);
	}
	for (int j = numRecentFiles; j < MAXRECENTFILES; ++j)	recentFileActs[j]->setVisible(false);
	separatorAct->setVisible(numRecentFiles > 0);
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

void MainWindow::updateMenus()
{
	bool active = (bool)workspace->activeWindow();
	saveAsAct->setEnabled(active);
	saveSnapshotAct->setEnabled(active);
	filterMenu->setEnabled(active && !filterMenu->actions().isEmpty());
	renderMenu->setEnabled(active);
  windowsMenu->setEnabled(active);
	renderToolBar->setEnabled(active);
	////////////////////////////////////////////////////////////////////
	if(active){
		const RenderMode &rm=GLA()->getCurrentRenderMode();
		switch (rm.drawMode) {
			case GLW::DMBox:
				renderBboxAct->setChecked(true);
			break;
			case GLW::DMPoints:
				renderModePointsAct->setChecked(true);
			break;
			case GLW::DMWire:
				renderModeWireAct->setChecked(true);
			break;
			case GLW::DMFlat:
				renderModeFlatAct->setChecked(true);
			break;
			case GLW::DMSmooth:
				renderModeSmoothAct->setChecked(true);
			break;
			case GLW::DMFlatWire:
				renderModeFlatLinesAct->setChecked(true);
			break;
			case GLW::DMHidden:
				renderModeHiddenLinesAct->setChecked(true);
			break;
		}
		setLightAct->setIcon(rm.Lighting ? QIcon(":/images/lighton.png") : QIcon(":/images/lightoff.png") );
		setLightAct->setChecked(rm.Lighting);

		setFancyLightingAct->setChecked(rm.FancyLighting);
		setDoubleLightingAct->setChecked(rm.DoubleSideLighting);

		foreach (QAction *a,TotalRenderList){a->setChecked(false);}
		if(GLA()->iRendersList){
			pair<QAction *,MeshRenderInterface *> p;
			foreach (p,*GLA()->iRendersList){p.first->setChecked(true);}
		}
	}
}
