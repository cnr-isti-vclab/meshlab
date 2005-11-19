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


#include "interfaces.h"
#include "mainwindow.h"
#include "glarea.h"
#include "plugindialog.h"
				
MainWindow::MainWindow()
{
	workspace = new QWorkspace(this);
	
	setCentralWidget(workspace);
	windowMapper = new QSignalMapper(this);
	connect(windowMapper, SIGNAL(mapped(QWidget *)),workspace, SLOT(setActiveWindow(QWidget *)));
	createActions();
	createMenus();
	createToolBars();

	addToolBar(mainToolBar);
	addToolBar(renderToolBar);

	setWindowTitle(tr("MeshLab v0.1"));

	loadPlugins();


	

	//QTimer::singleShot(500, this, SLOT(aboutPlugins()));

	if(QCoreApplication::instance ()->argc()>1)
		open(QCoreApplication::instance ()->argv()[1]);
	else 
		QTimer::singleShot(500, this, SLOT(open()));
}

void MainWindow::open(QString fileName)
{

	if (fileName.isEmpty()) 
	{
		/*QStringList types << 
		<< "Text files (*.txt)"
		<< "Any files (*)";
		QFileDialog fd = new QFileDialog( this );
		fd->setFilters( types );
		fd->show();*/

		fileName = QFileDialog::getOpenFileName(this,tr("Open File"),"../sample","Mesh files (*.ply *.off *.stl)");
	}
	if (!fileName.isEmpty()) {
		MeshModel *nm= new MeshModel();
		if(!nm->Open(fileName.toAscii())){
			QMessageBox::information(this, tr("Plug & Paint"),
				tr("Cannot load %1.").arg(fileName));

			delete nm;
			return;
		}
		else
		{
			//QMessageBox::information(this, tr("MeshLab"), tr("Opened Mesh of %1. triangles").arg(nm->cm.fn));
			VM.push_back(nm);
			gla=new GLArea(workspace);
			gla->mm=nm;
			gla->setWindowTitle(QFileInfo(fileName).fileName());   
			workspace->addWindow(gla);
			gla->showMaximized();
			return;
		}
	}
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
	
	exitAct = new QAction(tr("E&xit"), this);
	exitAct->setShortcut(tr("Ctrl+Q"));
	connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

	//////////////Action Menu View /////////////////////////////////////////////////////////////
	viewToolbarStandardAct = new QAction (tr("&Standard"), this);
	viewToolbarStandardAct->setCheckable(true);
	viewToolbarStandardAct->setChecked(true);
	connect(viewToolbarStandardAct, SIGNAL(triggered()), this, SLOT(viewToolbarFile()));

	viewToolbarRenderAct = new QAction (tr("&Render"), this);
	viewToolbarRenderAct->setCheckable(true);
	viewToolbarRenderAct->setChecked(true);
	connect(viewToolbarRenderAct, SIGNAL(triggered()), this, SLOT(viewToolbarRender()));


	//////////////Action Menu About ////////////////////////////////////////////////////////////
	aboutAct = new QAction(tr("&About"), this);
	connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

	aboutQtAct = new QAction(tr("About &Qt"), this);
	connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

	aboutPluginsAct = new QAction(tr("About &Plugins"), this);
	connect(aboutPluginsAct, SIGNAL(triggered()), this, SLOT(aboutPlugins()));

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

	//////////////Action Toolbar Render//////////////////////////////////////////////////////////
	viewModePoints	  = new QAction(QIcon(":/images/points.png"),tr("&Points"), this);
	connect(viewModePoints, SIGNAL(triggered()), this, SLOT(RenderPoint()));

	viewModeWire		  = new QAction(QIcon(":/images/wire.png"),tr("&Wireframe"), this);
	connect(viewModeWire, SIGNAL(triggered()), this, SLOT(RenderWire()));

	viewModeLines		  = new QAction(QIcon(":/images/backlines.png"),tr("&Hidden Lines"), this);
	connect(viewModeLines, SIGNAL(triggered()), this, SLOT(RenderHiddenLines()));
	viewModeLines->setDisabled(true);

	viewModeFlatLines = new QAction(QIcon(":/images/flatlines.png"),tr("Flat &Lines"), this);
	connect(viewModeFlatLines, SIGNAL(triggered()), this, SLOT(RenderFlatLine()));

	viewModeFlat		  = new QAction(QIcon(":/images/flat.png"),tr("&Flat"), this);
	connect(viewModeFlat, SIGNAL(triggered()), this, SLOT(RenderFlat()));

	viewModeSmooth	  = new QAction(QIcon(":/images/smooth.png"),tr("&Smooth"), this);
	connect(viewModeSmooth, SIGNAL(triggered()), this, SLOT(RenderSmooth()));
}

void MainWindow::createToolBars()
{
	mainToolBar = addToolBar(tr("Standard"));
	mainToolBar->setIconSize(QSize(32,32));
	mainToolBar->addAction(openAct);
	mainToolBar->addAction(saveAsAct);

	renderToolBar = addToolBar(tr("Render"));
	renderToolBar->setIconSize(QSize(32,32));
	renderToolBar->addAction(viewModePoints);
	renderToolBar->addAction(viewModeWire);
	renderToolBar->addAction(viewModeLines);
	renderToolBar->addAction(viewModeFlatLines);
	renderToolBar->addAction(viewModeFlat);
	renderToolBar->addAction(viewModeSmooth);
}

void MainWindow::createMenus()
{
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(openAct);
	fileMenu->addAction(saveAsAct);
	fileMenu->addSeparator();
	fileMenu->addAction(exitAct);

	filterMenu = menuBar()->addMenu(tr("&Filter"));

	viewMenu		= menuBar()->addMenu(tr("&View"));
	toolBarMenu	= viewMenu->addMenu(tr("&ToolBars"));
	toolBarMenu->addAction(viewToolbarStandardAct);
	toolBarMenu->addAction(viewToolbarRenderAct);

	windowsMenu = menuBar()->addMenu(tr("&Windows"));
	connect(windowsMenu, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));

	menuBar()->addSeparator();

	helpMenu = menuBar()->addMenu(tr("&Help"));
	helpMenu->addAction(aboutAct);
	helpMenu->addAction(aboutQtAct);
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
			MeshFilterInterface *iFilter = qobject_cast<MeshFilterInterface *>(plugin);
			if (iFilter)
				addToMenu(plugin, iFilter->filters(), filterMenu, SLOT(applyFilter()));                

			pluginFileNames += fileName;
		}
	}

	// brushMenu->setEnabled(!brushActionGroup->actions().isEmpty());
	// shapesMenu->setEnabled(!shapesMenu->actions().isEmpty());
	filterMenu->setEnabled(!filterMenu->actions().isEmpty());
}

void MainWindow::addToMenu(QObject *plugin, const QStringList &texts,QMenu *menu, const char *member,
													 QActionGroup *actionGroup)
{
	foreach (QString text, texts) {
		QAction *action = new QAction(text, plugin);
		connect(action, SIGNAL(triggered()), this, member);
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
	iFilter->applyFilter(action->text(), *(((GLArea *)(workspace->activeWindow()))->mm ), this);
}


void MainWindow::windowsTile(){
	workspace->tile();
}

void MainWindow::windowsCascade(){
	workspace->cascade();
}
void MainWindow::viewToolbarFile(){
	if(mainToolBar->isVisible()){
		mainToolBar->hide();
		viewToolbarStandardAct->setChecked(false);
	}else{
		mainToolBar->show();
		viewToolbarStandardAct->setChecked(true);
	}
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


void MainWindow::RenderPoint()
{
	// Set render type just on active window!
	qobject_cast<GLArea *>(workspace->activeWindow())->setDrawMode(GLW::DMPoints);
}

void MainWindow::RenderWire()
{
	// Set render type just on active window!
	qobject_cast<GLArea *>(workspace->activeWindow())->setDrawMode(GLW::DMWire);
}

void MainWindow::RenderFlat()
{
	// Set render type just on active window!
	qobject_cast<GLArea *>(workspace->activeWindow())->setDrawMode(GLW::DMFlat);
}

void MainWindow::RenderSmooth()
{
	// Set render type just on active window!
	qobject_cast<GLArea *>(workspace->activeWindow())->setDrawMode(GLW::DMSmooth);
}

void MainWindow::RenderFlatLine()
{
	// Set render type just on active window!
	qobject_cast<GLArea *>(workspace->activeWindow())->setDrawMode(GLW::DMFlatWire);
}

void MainWindow::RenderHiddenLines()
{
	// Set render type just on active window!
	qobject_cast<GLArea *>(workspace->activeWindow())->setDrawMode(GLW::DMHidden);
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