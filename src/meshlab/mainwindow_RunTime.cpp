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



#include "mainwindow.h"
#include "savemaskexporter.h"
#include <exception>
#include "ml_default_decorators.h"

#include <QToolBar>
#include <QToolTip>
#include <QStatusBar>
#include <QMenuBar>
#include <QProgressBar>
#include <QDesktopServices>
#include <QSettings>
#include <QSignalMapper>
#include <QMessageBox>
#include <QElapsedTimer>
#include <QMimeData>

#include <common/meshlabdocumentxml.h>
#include <common/meshlabdocumentbundler.h>
#include <common/mlapplication.h>
#include <common/filterscript.h>
#include <common/mlexception.h>
#include <common/globals.h>

#include "rich_parameter_gui/richparameterlistdialog.h"

#include <wrap/io_trimesh/alnParser.h>
#include "dialogs/about_dialog.h"
#include "dialogs/filter_script_dialog.h"
#include "dialogs/options_dialog.h"
#include "dialogs/plugin_info_dialog.h"
#include "dialogs/save_snapshot_dialog.h"

using namespace std;
using namespace vcg;

void MainWindow::updateRecentFileActions()
{
	bool activeDoc = (bool) !mdiarea->subWindowList().empty() && mdiarea->currentSubWindow();
	
	QSettings settings;
	QStringList files = settings.value("recentFileList").toStringList();
	
	int numRecentFiles = qMin(files.size(), (int)MAXRECENTFILES);
	
	for (int i = 0; i < numRecentFiles; ++i)
	{
		QString text = tr("&%1 %2").arg(i + 1).arg(QFileInfo(files[i]).fileName());
		recentFileActs[i]->setText(text);
		recentFileActs[i]->setData(files[i]);
		recentFileActs[i]->setEnabled(activeDoc);
	}
	for (int j = numRecentFiles; j < MAXRECENTFILES; ++j)
		recentFileActs[j]->setVisible(false);
}

void MainWindow::updateRecentProjActions()
{
	//bool activeDoc = (bool) !mdiarea->subWindowList().empty() && mdiarea->currentSubWindow();
	
	QSettings settings;
	QStringList projs = settings.value("recentProjList").toStringList();
	
	int numRecentProjs = qMin(projs.size(), (int)MAXRECENTFILES);
	for (int i = 0; i < numRecentProjs; ++i)
	{
		QString text = tr("&%1 %2").arg(i + 1).arg(QFileInfo(projs[i]).fileName());
		recentProjActs[i]->setText(text);
		recentProjActs[i]->setData(projs[i]);
		recentProjActs[i]->setEnabled(true);
	}
	for (int j = numRecentProjs; j < MAXRECENTFILES; ++j)
		recentProjActs[j]->setVisible(false);
}

// creates the standard plugin window
void MainWindow::createStdPluginWnd()
{
	//checks if a MeshlabStdDialog is already open and closes it
	if (stddialog!=0)
	{
		stddialog->close();
		delete stddialog;
	}
	stddialog = new MeshlabStdDialog(this);
	stddialog->setAllowedAreas (    Qt::NoDockWidgetArea);
	//addDockWidget(Qt::RightDockWidgetArea,stddialog);
	
	//stddialog->setAttribute(Qt::WA_DeleteOnClose,true);
	stddialog->setFloating(true);
	stddialog->hide();
	connect(GLA(),SIGNAL(glareaClosed()),this,SLOT(updateStdDialog()));
	connect(GLA(),SIGNAL(glareaClosed()),stddialog,SLOT(closeClick()));
}

// When we switch the current model (and we change the active window)
// we have to close the stddialog.
// this one is called when user switch current window.
void MainWindow::updateStdDialog()
{
	if(stddialog!=0){
		if(GLA()!=0){
			if(stddialog->curModel != meshDoc()->mm()){
				stddialog->curgla=0; // invalidate the curgla member that is no more valid.
				stddialog->close();
			}
		}
	}
}

void MainWindow::updateCustomSettings()
{
	mwsettings.updateGlobalParameterList(currentGlobalParams);
	emit dispatchCustomSettings(currentGlobalParams);
}

void MainWindow::updateWindowMenu()
{
	windowsMenu->clear();
	windowsMenu->addAction(closeAllAct);
	windowsMenu->addSeparator();
	windowsMenu->addAction(windowsTileAct);
	windowsMenu->addAction(windowsCascadeAct);
	windowsMenu->addAction(windowsNextAct);
	windowsNextAct->setEnabled(mdiarea-> subWindowList().size()>1);
	
	windowsMenu->addSeparator();
	
	
	if((mdiarea-> subWindowList().size()>0)){
		// Split/Unsplit SUBmenu
		splitModeMenu = windowsMenu->addMenu(tr("&Split current view"));
		
		splitModeMenu->addAction(setSplitHAct);
		splitModeMenu->addAction(setSplitVAct);
		
		windowsMenu->addAction(setUnsplitAct);
		
		// Link act
		windowsMenu->addAction(linkViewersAct);
		
		// View From SUBmenu
		viewFromMenu = windowsMenu->addMenu(tr("&View from"));
		foreach(QAction *ac, viewFromGroupAct->actions())
			viewFromMenu->addAction(ac);
		
		// Trackball Step SUBmenu
		trackballStepMenu = windowsMenu->addMenu(tr("Trackball step"));
		foreach(QAction *ac, trackballStepGroupAct->actions())
			trackballStepMenu->addAction(ac);
		
		// View From File act
		windowsMenu->addAction(readViewFromFileAct);
		windowsMenu->addAction(saveViewToFileAct);
		windowsMenu->addAction(viewFromMeshAct);
		windowsMenu->addAction(viewFromRasterAct);
		
		// Copy and paste shot acts
		windowsMenu->addAction(copyShotToClipboardAct);
		windowsMenu->addAction(pasteShotFromClipboardAct);
		
		//Enabling the actions
		MultiViewer_Container *mvc = currentViewContainer();
		if(mvc)
		{
			setUnsplitAct->setEnabled(mvc->viewerCounter()>1);
			GLArea* current = mvc->currentView();
			if(current)
			{
				setSplitHAct->setEnabled(current->size().height()/2 > current->minimumSizeHint().height());
				setSplitVAct->setEnabled(current->size().width()/2 > current->minimumSizeHint().width());
				
				linkViewersAct->setEnabled(currentViewContainer()->viewerCounter()>1);
				if(currentViewContainer()->viewerCounter()==1)
					linkViewersAct->setChecked(false);
				
				windowsMenu->addSeparator();
			}
		}
	}
	
	QList<QMdiSubWindow*> windows = mdiarea->subWindowList();
	
	if(windows.size() > 0)
		windowsMenu->addSeparator();
	
	int i=0;
	foreach(QWidget *w,windows)
	{
		QString text = tr("&%1. %2").arg(i+1).arg(QFileInfo(w->windowTitle()).fileName());
		QAction *action  = windowsMenu->addAction(text);
		action->setCheckable(true);
		action->setChecked(w == mdiarea->currentSubWindow());
		// Connect the signal to activate the selected window
		connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
		windowMapper->setMapping(action, w);
		++i;
	}
}

void MainWindow::enableDocumentSensibleActionsContainer(const bool allowed)
{
	QAction* fileact = fileMenu->menuAction();
	if (fileact != NULL)
		fileact->setEnabled(allowed);
	if (mainToolBar != NULL)
		mainToolBar->setEnabled(allowed);
	if (searchToolBar != NULL)
		searchToolBar->setEnabled(allowed);
	QAction* filtact = filterMenu->menuAction();
	if (filtact != NULL)
		filtact->setEnabled(allowed);
	if (filterToolBar != NULL)
		filterToolBar->setEnabled(allowed);
	QAction* editact = editMenu->menuAction();
	if (editact != NULL)
		editact->setEnabled(allowed);
	if (editToolBar)
		editToolBar->setEnabled(allowed);
}



//menu create is not enabled only in case of not valid/existing meshdocument
void MainWindow::updateSubFiltersMenu( const bool createmenuenabled,const bool validmeshdoc )
{
	showFilterScriptAct->setEnabled(validmeshdoc);
	filterMenuSelect->setEnabled(validmeshdoc);
	updateMenuItems(filterMenuSelect,validmeshdoc);
	filterMenuClean->setEnabled(validmeshdoc);
	updateMenuItems(filterMenuClean,validmeshdoc);
	filterMenuCreate->setEnabled(createmenuenabled || validmeshdoc);
	updateMenuItems(filterMenuCreate,createmenuenabled || validmeshdoc);
	filterMenuRemeshing->setEnabled(validmeshdoc);
	updateMenuItems(filterMenuRemeshing,validmeshdoc);
	filterMenuPolygonal->setEnabled(validmeshdoc);
	updateMenuItems(filterMenuPolygonal,validmeshdoc);
	filterMenuColorize->setEnabled(validmeshdoc);
	updateMenuItems(filterMenuColorize,validmeshdoc);
	filterMenuSmoothing->setEnabled(validmeshdoc);
	updateMenuItems(filterMenuSmoothing,validmeshdoc);
	filterMenuQuality->setEnabled(validmeshdoc);
	updateMenuItems(filterMenuQuality,validmeshdoc);
	filterMenuNormal->setEnabled(validmeshdoc);
	updateMenuItems(filterMenuNormal,validmeshdoc);
	filterMenuMeshLayer->setEnabled(validmeshdoc);
	updateMenuItems(filterMenuMeshLayer,validmeshdoc);
	filterMenuRasterLayer->setEnabled(validmeshdoc);
	updateMenuItems(filterMenuRasterLayer,validmeshdoc);
	filterMenuRangeMap->setEnabled(validmeshdoc);
	updateMenuItems(filterMenuRangeMap,validmeshdoc);
	filterMenuPointSet->setEnabled(validmeshdoc);
	updateMenuItems(filterMenuPointSet,validmeshdoc);
	filterMenuSampling->setEnabled(validmeshdoc);
	updateMenuItems(filterMenuSampling,validmeshdoc);
	filterMenuTexture->setEnabled(validmeshdoc);
	updateMenuItems(filterMenuTexture,validmeshdoc);
	filterMenuCamera->setEnabled(validmeshdoc);
	updateMenuItems(filterMenuCamera,validmeshdoc);
	
}

void MainWindow::updateMenuItems(QMenu* menu,const bool enabled)
{
	foreach(QAction* act,menu->actions())
		act->setEnabled(enabled);
}

void MainWindow::switchOffDecorator(QAction* decorator)
{
	if (GLA() != NULL)
	{
		int res = GLA()->iCurPerMeshDecoratorList().removeAll(decorator);
		if (res == 0)
			GLA()->iPerDocDecoratorlist.removeAll(decorator);
		updateMenus();
		layerDialog->updateDecoratorParsView();
		GLA()->update();
	}
}

void MainWindow::updateLayerDialog()
{
	if ((meshDoc() == NULL) || ((layerDialog != NULL) && !(layerDialog->isVisible())))
		return;
	MultiViewer_Container* mvc = currentViewContainer();
	if (mvc == NULL)
		return;
	MLSceneGLSharedDataContext* shared = mvc->sharedDataContext();
	if (shared == NULL)
		return;
	if(GLA())
	{
		MLSceneGLSharedDataContext::PerMeshRenderingDataMap dtf;
		shared->getRenderInfoPerMeshView(GLA()->context(),dtf);
		/*Add to the table the info for the per view global rendering of the Project*/
		MLRenderingData projdt;
		//GLA()->getPerDocGlobalRenderingData(projdt);
		dtf[-1] = projdt;
		layerDialog->updateTable(dtf);
		layerDialog->updateLog(meshDoc()->Log);
		layerDialog->updateDecoratorParsView();
		MLRenderingData dt;
		if (meshDoc()->mm() != NULL)
		{
			MLSceneGLSharedDataContext::PerMeshRenderingDataMap::iterator it = dtf.find(meshDoc()->mm()->id());
			if (it != dtf.end())
				layerDialog->updateRenderingParametersTab(meshDoc()->mm()->id(),*it);
		}
		if (globrendtoolbar != NULL)
		{
			shared->getRenderInfoPerMeshView(GLA()->context(), dtf);
			globrendtoolbar->statusConsistencyCheck(dtf);
		}
	}
}

void MainWindow::updateMenus()
{
	
	bool activeDoc = !(mdiarea->subWindowList().empty()) && (mdiarea->currentSubWindow() != NULL);
	bool notEmptyActiveDoc = activeDoc && (meshDoc() != NULL) && !(meshDoc()->meshList.empty());
	
	//std::cout << "SubWindowsList empty: " << mdiarea->subWindowList().empty() << " Valid Current Sub Windows: " << (mdiarea->currentSubWindow() != NULL) << " MeshList empty: " << meshDoc()->meshList.empty() << "\n";
	
	importMeshAct->setEnabled(activeDoc);
	
	exportMeshAct->setEnabled(notEmptyActiveDoc);
	exportMeshAsAct->setEnabled(notEmptyActiveDoc);
	reloadMeshAct->setEnabled(notEmptyActiveDoc);
	reloadAllMeshAct->setEnabled(notEmptyActiveDoc);
	importRasterAct->setEnabled(activeDoc);
	
	saveProjectAct->setEnabled(activeDoc);
	closeProjectAct->setEnabled(activeDoc);
	
	saveSnapshotAct->setEnabled(activeDoc);
	
	updateRecentFileActions();
	updateRecentProjActions();
	filterMenu->setEnabled(!filterMenu->actions().isEmpty());
	if (!filterMenu->actions().isEmpty())
		updateSubFiltersMenu(GLA() != NULL,notEmptyActiveDoc);
	lastFilterAct->setEnabled(false);
	lastFilterAct->setText(QString("Apply filter"));
	editMenu->setEnabled(!editMenu->actions().isEmpty());
	updateMenuItems(editMenu,activeDoc);
	renderMenu->setEnabled(!renderMenu->actions().isEmpty());
	updateMenuItems(renderMenu,activeDoc);
	fullScreenAct->setEnabled(activeDoc);
	showLayerDlgAct->setEnabled(activeDoc);
	showTrackBallAct->setEnabled(activeDoc);
	resetTrackBallAct->setEnabled(activeDoc);
	showInfoPaneAct->setEnabled(activeDoc);
	windowsMenu->setEnabled(activeDoc);
	preferencesMenu->setEnabled(activeDoc);
	
	showToolbarStandardAct->setChecked(mainToolBar->isVisible());
	if(activeDoc && GLA())
	{
		if(GLA()->getLastAppliedFilter() != NULL)
		{
			lastFilterAct->setText(QString("Apply filter ") + GLA()->getLastAppliedFilter()->text());
			lastFilterAct->setEnabled(true);
		}
		
		// Management of the editing toolbar
		// when you enter in a editing mode you can toggle between editing
		// and camera moving by esc;
		// you exit from editing mode by pressing again the editing button
		// When you are in a editing mode all the other editing are disabled.
		
		for (EditPluginInterfaceFactory* ep : PM.editPluginFactoryIterator())
			for (QAction* a : ep->actions()) {
				a->setChecked(false);
				a->setEnabled(GLA()->getCurrentEditAction() == nullptr);
		}
		
		suspendEditModeAct->setChecked(GLA()->suspendedEditor);
		suspendEditModeAct->setDisabled(GLA()->getCurrentEditAction() == NULL);
		
		if(GLA()->getCurrentEditAction())
		{
			GLA()->getCurrentEditAction()->setChecked(! GLA()->suspendedEditor);
			GLA()->getCurrentEditAction()->setEnabled(true);
		}
		
		showInfoPaneAct->setChecked(GLA()->infoAreaVisible);
		showTrackBallAct->setChecked(GLA()->isTrackBallVisible());
		
		// Decorator Menu Checking and unChecking
		// First uncheck and disable all the decorators
		for (DecoratePluginInterface* dp : PM.decoratePluginIterator()){
			for (QAction* a : dp->actions()){
				a->setChecked(false);
				a->setEnabled(true);
			}
		}
		// Check the decorator per Document of the current glarea
		foreach (QAction *a,   GLA()->iPerDocDecoratorlist)
		{ a ->setChecked(true); }
		
		// Then check the decorator enabled for the current mesh.
		if(GLA()->mm())
			foreach (QAction *a,   GLA()->iCurPerMeshDecoratorList())
				a ->setChecked(true);
	} // if active
	else
	{
		for (EditPluginInterfaceFactory* ep : PM.editPluginFactoryIterator()) {
			for (QAction* a : ep->actions()) {
				a->setEnabled(false);
			}
		}

		for (DecoratePluginInterface* dp : PM.decoratePluginIterator()){
			for (QAction* a : dp->actions()){
				a->setEnabled(false);
			}
		}
	}
	GLArea* tmp = GLA();
	if(tmp != NULL)
	{
		showLayerDlgAct->setChecked(layerDialog->isVisible());
		showRasterAct->setEnabled((meshDoc() != NULL) && (meshDoc()->rm() != 0));
		showRasterAct->setChecked(tmp->isRaster());
	}
	else
	{
		for (DecoratePluginInterface* dp : PM.decoratePluginIterator()){
			for (QAction* a : dp->actions()){
				a->setChecked(false);
				a->setEnabled(false);
			}
		}
		
		layerDialog->setVisible(false);
	}
	if (searchMenu != NULL)
		searchMenu->searchLineWidth() = longestActionWidthInAllMenus();
	updateWindowMenu();
}

void MainWindow::setSplit(QAction *qa)
{
	MultiViewer_Container *mvc = currentViewContainer();
	if(mvc)
	{
		GLArea *glwClone=new GLArea(this, mvc, &currentGlobalParams);
		//connect(glwClone, SIGNAL(insertRenderingDataForNewlyGeneratedMesh(int)), this, SLOT(addRenderingDataIfNewlyGeneratedMesh(int)));
		if(qa->text() == tr("&Horizontally"))
			mvc->addView(glwClone,Qt::Vertical);
		else if(qa->text() == tr("&Vertically"))
			mvc->addView(glwClone,Qt::Horizontal);
		
		//The loading of the raster must be here
		if(GLA()->isRaster())
		{
			glwClone->setIsRaster(true);
			if(this->meshDoc()->rm()->id()>=0)
				glwClone->loadRaster(this->meshDoc()->rm()->id());
		}
		
		updateMenus();
		
		glwClone->resetTrackBall();
		glwClone->update();
	}
	
}

void MainWindow::setUnsplit()
{
	MultiViewer_Container *mvc = currentViewContainer();
	if(mvc)
	{
		assert(mvc->viewerCounter() >1);
		
		mvc->removeView(mvc->currentView()->getId());
		
		updateMenus();
	}
}

//set the split/unsplit menu that appears right clicking on a splitter's handle
void MainWindow::setHandleMenu(QPoint point, Qt::Orientation orientation, QSplitter *origin)
{
	MultiViewer_Container *mvc =  currentViewContainer();
	int epsilon =10;
	splitMenu->clear();
	unSplitMenu->clear();
	//the viewer to split/unsplit is chosen through picking
	
	//Vertical handle allows to split horizontally
	if(orientation == Qt::Vertical)
	{
		splitUpAct->setData(point);
		splitDownAct->setData(point);
		
		//check if the viewer on the top is splittable according to its size
		int pickingId = mvc->getViewerByPicking(QPoint(point.x(), point.y()-epsilon));
		if(pickingId>=0)
			splitUpAct->setEnabled(mvc->getViewer(pickingId)->size().width()/2 > mvc->getViewer(pickingId)->minimumSizeHint().width());
		
		//the viewer on top can be closed only if the splitter over the handle that originated the event has one child
		bool unSplittabilityUp = true;
		Splitter * upSplitter = qobject_cast<Splitter *>(origin->widget(0));
		if(upSplitter)
			unSplittabilityUp = !(upSplitter->count()>1);
		unsplitUpAct->setEnabled(unSplittabilityUp);
		
		//check if the viewer below is splittable according to its size
		pickingId = mvc->getViewerByPicking(QPoint(point.x(), point.y()+epsilon));
		if(pickingId>=0)
			splitDownAct->setEnabled(mvc->getViewer(pickingId)->size().width()/2 > mvc->getViewer(pickingId)->minimumSizeHint().width());
		
		//the viewer below can be closed only if the splitter ounder the handle that originated the event has one child
		bool unSplittabilityDown = true;
		Splitter * downSplitter = qobject_cast<Splitter *>(origin->widget(1));
		if(downSplitter)
			unSplittabilityDown = !(downSplitter->count()>1);
		unsplitDownAct->setEnabled(unSplittabilityDown);
		
		splitMenu->addAction(splitUpAct);
		splitMenu->addAction(splitDownAct);
		
		unsplitUpAct->setData(point);
		unsplitDownAct->setData(point);
		
		unSplitMenu->addAction(unsplitUpAct);
		unSplitMenu->addAction(unsplitDownAct);
	}
	//Horizontal handle allows to split vertically
	else if (orientation == Qt::Horizontal)
	{
		splitRightAct->setData(point);
		splitLeftAct->setData(point);
		
		//check if the viewer on the right is splittable according to its size
		int pickingId =mvc->getViewerByPicking(QPoint(point.x()+epsilon, point.y()));
		if(pickingId>=0)
			splitRightAct->setEnabled(mvc->getViewer(pickingId)->size().height()/2 > mvc->getViewer(pickingId)->minimumSizeHint().height());
		
		//the viewer on the right can be closed only if the splitter on the right the handle that originated the event has one child
		bool unSplittabilityRight = true;
		Splitter * rightSplitter = qobject_cast<Splitter *>(origin->widget(1));
		if(rightSplitter)
			unSplittabilityRight = !(rightSplitter->count()>1);
		unsplitRightAct->setEnabled(unSplittabilityRight);
		
		//check if the viewer on the left is splittable according to its size
		pickingId =mvc->getViewerByPicking(QPoint(point.x()-epsilon, point.y()));
		if(pickingId>=0)
			splitLeftAct->setEnabled(mvc->getViewer(pickingId)->size().height()/2 > mvc->getViewer(pickingId)->minimumSizeHint().height());
		
		//the viewer on the left can be closed only if the splitter on the left of the handle that originated the event has one child
		bool unSplittabilityLeft = true;
		Splitter * leftSplitter = qobject_cast<Splitter *>(origin->widget(0));
		if(leftSplitter)
			unSplittabilityLeft = !(leftSplitter->count()>1);
		unsplitLeftAct->setEnabled(unSplittabilityLeft);
		
		splitMenu->addAction(splitRightAct);
		splitMenu->addAction(splitLeftAct);
		
		unsplitRightAct->setData(point);
		unsplitLeftAct->setData(point);
		
		unSplitMenu->addAction(unsplitRightAct);
		unSplitMenu->addAction(unsplitLeftAct);
	}
	handleMenu->popup(point);
}


void MainWindow::splitFromHandle(QAction *qa )
{
	MultiViewer_Container *mvc = currentViewContainer();
	QPoint point = qa->data().toPoint();
	int epsilon =10;
	
	if(qa->text() == tr("&Right"))
		point.setX(point.x()+ epsilon);
	else if(qa->text() == tr("&Left"))
		point.setX(point.x()- epsilon);
	else if(qa->text() == tr("&Up"))
		point.setY(point.y()- epsilon);
	else if(qa->text() == tr("&Down"))
		point.setY(point.y()+ epsilon);
	
	int newCurrent = mvc->getViewerByPicking(point);
	mvc->updateCurrent(newCurrent);
	
	if(qa->text() == tr("&Right") || qa->text() == tr("&Left"))
		setSplit(new QAction(tr("&Horizontally"), this));
	else
		setSplit( new QAction(tr("&Vertically"), this));
}

void MainWindow::unsplitFromHandle(QAction * qa)
{
	MultiViewer_Container *mvc = currentViewContainer();
	
	QPoint point = qa->data().toPoint();
	int epsilon =10;
	
	if(qa->text() == tr("&Right"))
		point.setX(point.x()+ epsilon);
	else if(qa->text() == tr("&Left"))
		point.setX(point.x()- epsilon);
	else if(qa->text() == tr("&Up"))
		point.setY(point.y()- epsilon);
	else if(qa->text() == tr("&Down"))
		point.setY(point.y()+ epsilon);
	
	int newCurrent = mvc->getViewerByPicking(point);
	mvc->updateCurrent(newCurrent);
	
	setUnsplit();
}

void MainWindow::linkViewers()
{
	MultiViewer_Container *mvc = currentViewContainer();
	mvc->updateTrackballInViewers();
}

void MainWindow::toggleOrtho()
{
	if (GLA()) GLA()->toggleOrtho();
}

void MainWindow::viewFrom(QAction *qa)
{
	if(GLA()) GLA()->createOrthoView(qa->text());
}

void MainWindow::trackballStep(QAction *qa)
{
	if (GLA()) GLA()->trackballStep(qa->text());
}

void MainWindow::readViewFromFile()
{
	if(GLA()) GLA()->readViewFromFile();
	updateMenus();
}

void MainWindow::saveViewToFile()
{
	if(GLA()) GLA()->saveViewToFile();
}

void MainWindow::viewFromCurrentMeshShot()
{
	if(GLA()) GLA()->viewFromCurrentShot("Mesh");
	updateMenus();
}

void MainWindow::viewFromCurrentRasterShot()
{
	if(GLA()) GLA()->viewFromCurrentShot("Raster");
	updateMenus();
}

void MainWindow::copyViewToClipBoard()
{
	if(GLA()) GLA()->viewToClipboard();
}

void MainWindow::pasteViewFromClipboard()
{
	if(GLA()) GLA()->viewFromClipboard();
	updateMenus();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
	//qDebug("dragEnterEvent: %s",event->format());
	event->accept();
}

void MainWindow::dropEvent ( QDropEvent * event )
{
	//qDebug("dropEvent: %s",event->format());
	const QMimeData * data = event->mimeData();
	if (data->hasUrls())
	{
		QList< QUrl > url_list = data->urls();
		bool layervis = false;
		if (layerDialog != NULL)
		{
			layervis = layerDialog->isVisible();
			showLayerDlg(false);
		}
		for (int i=0, size=url_list.size(); i<size; i++)
		{
			QString path = url_list.at(i).toLocalFile();
			if( (event->keyboardModifiers () == Qt::ControlModifier ) || ( QApplication::keyboardModifiers () == Qt::ControlModifier ))
			{
				this->newProject();
			}
			
			if(path.endsWith("mlp",Qt::CaseInsensitive) || path.endsWith("mlb", Qt::CaseInsensitive) || path.endsWith("aln",Qt::CaseInsensitive) || path.endsWith("out",Qt::CaseInsensitive) || path.endsWith("nvm",Qt::CaseInsensitive) )
				openProject(path);
			else
			{
				importMesh(path);
			}
		}
		showLayerDlg(layervis || meshDoc()->meshList.size() > 0);
	}
}

void MainWindow::endEdit()
{
	MultiViewer_Container* mvc = currentViewContainer();
	if ((meshDoc() == NULL) || (GLA() == NULL) || (mvc == NULL))
		return;
	
	
	for (int ii = 0; ii < meshDoc()->meshList.size(); ++ii)
	{
		MeshModel* mm = meshDoc()->meshList[ii];
		if (mm != NULL)
			addRenderingDataIfNewlyGeneratedMesh(mm->id());
	}
	meshDoc()->meshDocStateData().clear();
	
	GLA()->endEdit();
	updateLayerDialog();
}

void MainWindow::applyLastFilter()
{
	if(GLA() != nullptr && GLA()->getLastAppliedFilter() != nullptr){
		//TODO
		//GLA()->getLastAppliedFilter()->trigger();
	}
}

void MainWindow::showFilterScript()
{

	FilterScriptDialog dialog(meshDoc()->filterHistory, this);
	
	if (dialog.exec()==QDialog::Accepted)
	{
		runFilterScript();
		return ;
	}
}

void MainWindow::runFilterScript()
{
	if (meshDoc() == nullptr)
		return;
	for (FilterNameParameterValuesPair& pair : meshDoc()->filterHistory)
	{
		QString filtnm = pair.filterName();
		int classes = 0;
		unsigned int postCondMask = MeshModel::MM_UNKNOWN;
		QAction *action = PM.filterAction(filtnm);
		FilterPluginInterface *iFilter = qobject_cast<FilterPluginInterface *>(action->parent());
		
		int req=iFilter->getRequirements(action);
		if (meshDoc()->mm() != NULL)
			meshDoc()->mm()->updateDataMask(req);
		iFilter->setLog(&meshDoc()->Log);
		RichParameterList &parameterSet = pair.second;
		
		for(RichParameter& parameter : parameterSet) {
			//if this is a mesh parameter and the index is valid
			if(parameter.value().isMesh()) {
				RichMesh& md = reinterpret_cast<RichMesh&>(parameter);
				if( md.meshindex < meshDoc()->size() && md.meshindex >= 0  ) {
					parameterSet.setValue(md.name(), MeshValue(meshDoc(), md.meshindex));
				}
				else {
					printf("Meshes loaded: %i, meshes asked for: %i \n", meshDoc()->size(), md.meshindex );
					printf("One of the filters in the script needs more meshes than you have loaded.\n");
					return;
				}
			}
		}
		//iFilter->applyFilter( action, *(meshDoc()->mm()), (*ii).second, QCallBack );
		
		bool created = false;
		MLSceneGLSharedDataContext* shar = NULL;
		if (currentViewContainer() != NULL)
		{
			shar = currentViewContainer()->sharedDataContext();
			//GLA() is only the parent
			QGLWidget* filterWidget = new QGLWidget(GLA(),shar);
			QGLFormat defForm = QGLFormat::defaultFormat();
			iFilter->glContext = new MLPluginGLContext(defForm,filterWidget->context()->device(),*shar);
			created = iFilter->glContext->create(filterWidget->context());
			shar->addView(iFilter->glContext);
			MLRenderingData dt;
			MLRenderingData::RendAtts atts;
			atts[MLRenderingData::ATT_NAMES::ATT_VERTPOSITION] = true;
			atts[MLRenderingData::ATT_NAMES::ATT_VERTNORMAL] = true;
			
			
			if (iFilter->filterArity(action) == FilterPluginInterface::SINGLE_MESH)
			{
				MLRenderingData::PRIMITIVE_MODALITY pm = MLPoliciesStandAloneFunctions::bestPrimitiveModalityAccordingToMesh(meshDoc()->mm());
				if ((pm != MLRenderingData::PR_ARITY) && (meshDoc()->mm() != NULL))
				{
					dt.set(pm,atts);
					shar->setRenderingDataPerMeshView(meshDoc()->mm()->id(),iFilter->glContext,dt);
				}
			}
			else
			{
				for(int ii = 0;ii < meshDoc()->meshList.size();++ii)
				{
					MeshModel* mm = meshDoc()->meshList[ii];
					MLRenderingData::PRIMITIVE_MODALITY pm = MLPoliciesStandAloneFunctions::bestPrimitiveModalityAccordingToMesh(mm);
					if ((pm != MLRenderingData::PR_ARITY) && (mm != NULL))
					{
						dt.set(pm,atts);
						shar->setRenderingDataPerMeshView(mm->id(),iFilter->glContext,dt);
					}
				}
			}
			
		}
		if ((!created) || (!iFilter->glContext->isValid()))
			throw MLException("A valid GLContext is required by the filter to work.\n");
		meshDoc()->setBusy(true);
		//WARNING!!!!!!!!!!!!
		/* to be changed */
		std::map<std::string, QVariant> outputValues;
		iFilter->applyFilter( action, *meshDoc(), outputValues, postCondMask, pair.second, QCallBack);
		if (postCondMask == MeshModel::MM_UNKNOWN)
			postCondMask = iFilter->postCondition(action);
		for (MeshModel* mm = meshDoc()->nextMesh(); mm != NULL; mm = meshDoc()->nextMesh(mm))
			vcg::tri::Allocator<CMeshO>::CompactEveryVector(mm->cm);
		meshDoc()->setBusy(false);
		if (shar != NULL)
			shar->removeView(iFilter->glContext);
		delete iFilter->glContext;
		classes = int(iFilter->getClass(action));
		
		if (meshDoc()->mm() != NULL)
		{
			if(classes & FilterPluginInterface::FaceColoring )
			{
				meshDoc()->mm()->updateDataMask(MeshModel::MM_FACECOLOR);
			}
			if(classes & FilterPluginInterface::VertexColoring )
			{
				meshDoc()->mm()->updateDataMask(MeshModel::MM_VERTCOLOR);
			}
			if(classes & MeshModel::MM_COLOR)
			{
				meshDoc()->mm()->updateDataMask(MeshModel::MM_COLOR);
			}
			if(classes & MeshModel::MM_CAMERA)
				meshDoc()->mm()->updateDataMask(MeshModel::MM_CAMERA);
		}
		
		bool newmeshcreated = false;
		if (classes & FilterPluginInterface::MeshCreation)
			newmeshcreated = true;
		updateSharedContextDataAfterFilterExecution(postCondMask, classes, newmeshcreated);
		meshDoc()->meshDocStateData().clear();
		
		if(classes & FilterPluginInterface::MeshCreation)
			GLA()->resetTrackBall();
		/* to be changed */
		
		qb->reset();
		GLA()->update();
		GLA()->Logf(GLLogStream::SYSTEM,"Re-Applied filter %s",qUtf8Printable(pair.filterName()));
		if (_currviewcontainer != NULL)
			_currviewcontainer->updateAllDecoratorsForAllViewers();
	}
}

// Receives the action that wants to show a tooltip and display it
// on screen at the current mouse position.
// TODO: have the tooltip always display with fixed width at the right
//       hand side of the menu entry (not invasive)
void MainWindow::showTooltip(QAction* q)
{
	QString tip = q->toolTip();
	QToolTip::showText(QCursor::pos(), tip);
}

// /////////////////////////////////////////////////
// The Very Important Procedure of applying a filter
// /////////////////////////////////////////////////
// It is split in two part
// - startFilter that setup the dialogs and asks for parameters
// - executeFilter callback invoked when the params have been set up.


void MainWindow::startFilter()
{
	if(currentViewContainer() == NULL) return;
	if(GLA() == NULL) return;
	
	// In order to avoid that a filter changes something assumed by the current editing tool,
	// before actually starting the filter we close the current editing tool (if any).
	if (GLA()->getCurrentEditAction() != NULL)
		endEdit();
	updateMenus();
	
	QStringList missingPreconditions;
	QAction *action = qobject_cast<QAction *>(sender());
	if (action == NULL)
		throw MLException("Invalid filter action value.");
	FilterPluginInterface *iFilter = qobject_cast<FilterPluginInterface *>(action->parent());
	if (meshDoc() == NULL)
		return;
	//OLD FILTER PHILOSOPHY
	if (iFilter != NULL)
	{
		//if(iFilter->getClass(action) == MeshFilterInterface::MeshCreation)
		//{
		//	qDebug("MeshCreation");
		//	GLA()->meshDoc->addNewMesh("",iFilter->filterName(action) );
		//}
		//else
		if (!iFilter->isFilterApplicable(action,(*meshDoc()->mm()),missingPreconditions))
		{
			QString enstr = missingPreconditions.join(",");
			QMessageBox::warning(this, tr("PreConditions' Failure"), QString("Warning the filter <font color=red>'" + iFilter->filterName(action) + "'</font> has not been applied.<br>"
																																					"Current mesh does not have <i>" + enstr + "</i>."));
			return;
		}
		
		if(currentViewContainer())
		{
			iFilter->setLog(currentViewContainer()->LogPtr());
			currentViewContainer()->LogPtr()->SetBookmark();
		}
		// just to be sure...
		createStdPluginWnd();
		
		// (2) Ask for filter parameters and eventually directly invoke the filter
		// showAutoDialog return true if a dialog have been created (and therefore the execution is demanded to the apply event)
		// if no dialog is created the filter must be executed immediately
		if(! stddialog->showAutoDialog(iFilter, meshDoc()->mm(), (meshDoc()), action, this, GLA()) )
		{
			RichParameterList dummyParSet;
			executeFilter(action, dummyParSet, false);
			
			//Insert the filter to filterHistory
			FilterNameParameterValuesPair tmp;
			tmp.first = action->text(); tmp.second = dummyParSet;
			meshDoc()->filterHistory.append(tmp);
		}
	}
	
}//void MainWindow::startFilter()


void MainWindow::updateSharedContextDataAfterFilterExecution(int postcondmask,int fclasses,bool& newmeshcreated)
{
	MultiViewer_Container* mvc = currentViewContainer();
	if ((meshDoc() != NULL) && (mvc != NULL))
	{
		if (GLA() == NULL)
			return;
		MLSceneGLSharedDataContext* shared = mvc->sharedDataContext();
		if (shared != NULL)
		{
			for(MeshModel* mm = meshDoc()->nextMesh();mm != NULL;mm = meshDoc()->nextMesh(mm))
			{
				if (mm == NULL)
					continue;
				//Just to be sure that the filter author didn't forget to add changing tags to the postCondition field
				if ((mm->hasDataMask(MeshModel::MM_FACECOLOR)) && (fclasses & FilterPluginInterface::FaceColoring ))
					postcondmask = postcondmask | MeshModel::MM_FACECOLOR;
				
				if ((mm->hasDataMask(MeshModel::MM_VERTCOLOR)) && (fclasses & FilterPluginInterface::VertexColoring ))
					postcondmask = postcondmask | MeshModel::MM_VERTCOLOR;
				
				if ((mm->hasDataMask(MeshModel::MM_COLOR)) && (fclasses & FilterPluginInterface::MeshColoring ))
					postcondmask = postcondmask | MeshModel::MM_COLOR;
				
				if ((mm->hasDataMask(MeshModel::MM_FACEQUALITY)) && (fclasses & FilterPluginInterface::Quality ))
					postcondmask = postcondmask | MeshModel::MM_FACEQUALITY;
				
				if ((mm->hasDataMask(MeshModel::MM_VERTQUALITY)) && (fclasses & FilterPluginInterface::Quality ))
					postcondmask = postcondmask | MeshModel::MM_VERTQUALITY;
				
				MLRenderingData dttoberendered;
				QMap<int,MeshModelStateData>::Iterator existit = meshDoc()->meshDocStateData().find(mm->id());
				if (existit != meshDoc()->meshDocStateData().end())
				{
					
					shared->getRenderInfoPerMeshView(mm->id(),GLA()->context(),dttoberendered);
					int updatemask = MeshModel::MM_NONE;
					bool connectivitychanged = false;
					if (((unsigned int)mm->cm.VN() != existit->_nvert) || ((unsigned int)mm->cm.FN() != existit->_nface) ||
							bool(postcondmask & MeshModel::MM_UNKNOWN) || bool(postcondmask & MeshModel::MM_VERTNUMBER) ||
							bool(postcondmask & MeshModel::MM_FACENUMBER) || bool(postcondmask & MeshModel::MM_FACEVERT) ||
							bool(postcondmask & MeshModel::MM_VERTFACETOPO) || bool(postcondmask & MeshModel::MM_FACEFACETOPO))
					{
						updatemask = MeshModel::MM_ALL;
						connectivitychanged = true;
					}
					else
					{
						//masks differences bitwise operator (^) -> remove the attributes that didn't apparently change + the ones that for sure changed according to the postCondition function
						//this operation has been introduced in order to minimize problems with filters that didn't declared properly the postCondition mask
						updatemask = (existit->_mask ^ mm->dataMask()) | postcondmask;
						connectivitychanged = false;
					}
					
					MLRenderingData::RendAtts dttoupdate;
					//1) we convert the meshmodel updating mask to a RendAtts structure
					MLPoliciesStandAloneFunctions::fromMeshModelMaskToMLRenderingAtts(updatemask,dttoupdate);
					//2) The correspondent bos to the updated rendering attributes are set to invalid
					shared->meshAttributesUpdated(mm->id(),connectivitychanged,dttoupdate);
					
					//3) we took the current rendering modality for the mesh in the active gla
					MLRenderingData curr;
					shared->getRenderInfoPerMeshView(mm->id(),GLA()->context(),curr);
					
					//4) we add to the current rendering modality in the current GLArea just the minimum attributes having been updated
					//   WARNING!!!! There are priority policies
					//               ex1) suppose that the current rendering modality is PR_POINTS and ATT_VERTPOSITION, ATT_VERTNORMAL,ATT_VERTCOLOR
					//               if i updated, for instance, just the ATT_FACECOLOR, we switch off in the active GLArea the per ATT_VERTCOLOR attribute
					//               and turn on the ATT_FACECOLOR
					//               ex2) suppose that the current rendering modality is PR_POINTS and ATT_VERTPOSITION, ATT_VERTNORMAL,ATT_VERTCOLOR
					//               if i updated, for instance, both the ATT_FACECOLOR and the ATT_VERTCOLOR, we continue to render the updated value of the ATT_VERTCOLOR
					//               ex3) suppose that in all the GLAreas the current rendering modality is PR_POINTS and we run a surface reconstruction filter
					//               in the current GLA() we switch from the PR_POINTS to PR_SOLID primitive rendering modality. In the other GLArea we maintain the per points visualization
					for(MLRenderingData::PRIMITIVE_MODALITY pm = MLRenderingData::PRIMITIVE_MODALITY(0);pm < MLRenderingData::PR_ARITY;pm = MLRenderingData::next(pm))
					{
						bool wasprimitivemodalitymeaningful = MLPoliciesStandAloneFunctions::isPrimitiveModalityCompatibleWithMeshInfo((existit->_nvert > 0),(existit->_nface > 0),(existit->_nedge > 0),existit->_mask,pm);
						bool isprimitivemodalitymeaningful = MLPoliciesStandAloneFunctions::isPrimitiveModalityCompatibleWithMesh(mm,pm);
						bool isworthtobevisualized = MLPoliciesStandAloneFunctions::isPrimitiveModalityWorthToBeActivated(pm,curr.isPrimitiveActive(pm),wasprimitivemodalitymeaningful,isprimitivemodalitymeaningful);
						
						
						MLRenderingData::RendAtts rd;
						curr.get(pm, rd);
						MLPoliciesStandAloneFunctions::updatedRendAttsAccordingToPriorities(pm, dttoupdate, rd, rd);
						rd[MLRenderingData::ATT_NAMES::ATT_VERTPOSITION] = isworthtobevisualized;
						MLPoliciesStandAloneFunctions::filterUselessUdpateAccordingToMeshMask(mm,rd);
						curr.set(pm,rd);
					}
					MLPerViewGLOptions opts;
					curr.get(opts);
					if (fclasses & FilterPluginInterface::MeshColoring)
					{
						bool hasmeshcolor = mm->hasDataMask(MeshModel::MM_COLOR);
						opts._perpoint_mesh_color_enabled = hasmeshcolor;
						opts._perwire_mesh_color_enabled = hasmeshcolor;
						opts._persolid_mesh_color_enabled = hasmeshcolor;
						
						for (MLRenderingData::PRIMITIVE_MODALITY pm = MLRenderingData::PRIMITIVE_MODALITY(0); pm < MLRenderingData::PR_ARITY; pm = MLRenderingData::next(pm))
						{
							MLRenderingData::RendAtts atts;
							curr.get(pm, atts);
							atts[MLRenderingData::ATT_NAMES::ATT_VERTCOLOR] = false;
							atts[MLRenderingData::ATT_NAMES::ATT_FACECOLOR] = false;
							curr.set(pm, atts);
						}
						curr.set(opts);
					}
					MLPoliciesStandAloneFunctions::setPerViewGLOptionsAccordindToWireModality(mm, curr);
					MLPoliciesStandAloneFunctions::setPerViewGLOptionsPriorities(curr);
					
					if (mm == meshDoc()->mm())
					{
						/*HORRIBLE TRICK IN ORDER TO HAVE VALID ACTIONS ASSOCIATED WITH THE CURRENT WIRE RENDERING MODALITY*/
						MLRenderingFauxEdgeWireAction* fauxaction = new MLRenderingFauxEdgeWireAction(meshDoc()->mm()->id(), NULL);
						fauxaction->setChecked(curr.isPrimitiveActive(MLRenderingData::PR_WIREFRAME_EDGES));
						layerDialog->_tabw->switchWireModality(fauxaction);
						delete fauxaction;
						/****************************************************************************************************/
					}
					
					
					shared->setRenderingDataPerMeshView(mm->id(),GLA()->context(),curr);
				}
				else
				{
					//A new mesh has been created by the filter. I have to add it in the shared context data structure
					newmeshcreated = true;
					MLPoliciesStandAloneFunctions::suggestedDefaultPerViewRenderingData(mm,dttoberendered,mwsettings.minpolygonpersmoothrendering);
					if (mm == meshDoc()->mm())
					{
						/*HORRIBLE TRICK IN ORDER TO HAVE VALID ACTIONS ASSOCIATED WITH THE CURRENT WIRE RENDERING MODALITY*/
						MLRenderingFauxEdgeWireAction* fauxaction = new MLRenderingFauxEdgeWireAction(meshDoc()->mm()->id(), NULL);
						fauxaction->setChecked(dttoberendered.isPrimitiveActive(MLRenderingData::PR_WIREFRAME_EDGES));
						layerDialog->_tabw->switchWireModality(fauxaction);
						delete fauxaction;
						/****************************************************************************************************/
					}
					foreach(GLArea* gla,mvc->viewerList)
					{
						if (gla != NULL)
							shared->setRenderingDataPerMeshView(mm->id(),gla->context(),dttoberendered);
					}
				}
				shared->manageBuffers(mm->id());
			}
			updateLayerDialog();
		}
	}
}

/*
callback function that actually start the chosen filter.
it is called once the parameters have been filled.
It can be called
from the automatic dialog
from the user defined dialog
*/


void MainWindow::executeFilter(const QAction* action, RichParameterList &params, bool isPreview)
{
	FilterPluginInterface *iFilter = qobject_cast<FilterPluginInterface *>(action->parent());
	qb->show();
	iFilter->setLog(&meshDoc()->Log);
	
	// Ask for filter requirements (eg a filter can need topology, border flags etc)
	// and satisfy them
	qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
	MainWindow::globalStatusBar()->showMessage("Starting Filter...",5000);
	int req=iFilter->getRequirements(action);
	if (!meshDoc()->meshList.isEmpty())
		meshDoc()->mm()->updateDataMask(req);
	qApp->restoreOverrideCursor();
	
	// (3) save the current filter and its parameters in the history
	if(!isPreview)
		meshDoc()->Log.ClearBookmark();
	else
		meshDoc()->Log.BackToBookmark();
	// (4) Apply the Filter
	bool ret;
	qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
	QElapsedTimer tt; tt.start();
	meshDoc()->setBusy(true);
	RichParameterList mergedenvironment(params);
	mergedenvironment.join(currentGlobalParams);
	
	MLSceneGLSharedDataContext* shar = NULL;
	QGLWidget* filterWidget = NULL;
	if (currentViewContainer() != NULL)
	{
		shar = currentViewContainer()->sharedDataContext();
		//GLA() is only the parent
		filterWidget = new QGLWidget(NULL,shar);
		QGLFormat defForm = QGLFormat::defaultFormat();
		iFilter->glContext = new MLPluginGLContext(defForm,filterWidget->context()->device(),*shar);
		iFilter->glContext->create(filterWidget->context());
		
		MLRenderingData dt;
		MLRenderingData::RendAtts atts;
		atts[MLRenderingData::ATT_NAMES::ATT_VERTPOSITION] = true;
		atts[MLRenderingData::ATT_NAMES::ATT_VERTNORMAL] = true;
		
		if (iFilter->filterArity(action) == FilterPluginInterface::SINGLE_MESH)
		{
			MLRenderingData::PRIMITIVE_MODALITY pm = MLPoliciesStandAloneFunctions::bestPrimitiveModalityAccordingToMesh(meshDoc()->mm());
			if ((pm != MLRenderingData::PR_ARITY) && (meshDoc()->mm() != NULL))
			{
				dt.set(pm,atts);
				iFilter->glContext->initPerViewRenderingData(meshDoc()->mm()->id(),dt);
			}
		}
		else
		{
			for(int ii = 0;ii < meshDoc()->meshList.size();++ii)
			{
				MeshModel* mm = meshDoc()->meshList[ii];
				MLRenderingData::PRIMITIVE_MODALITY pm = MLPoliciesStandAloneFunctions::bestPrimitiveModalityAccordingToMesh(mm);
				if ((pm != MLRenderingData::PR_ARITY) && (mm != NULL))
				{
					dt.set(pm,atts);
					iFilter->glContext->initPerViewRenderingData(mm->id(),dt);
				}
			}
		}
	}
	bool newmeshcreated = false;
	try
	{
		meshDoc()->meshDocStateData().clear();
		meshDoc()->meshDocStateData().create(*meshDoc());
		unsigned int postCondMask = MeshModel::MM_UNKNOWN;
		std::map<std::string, QVariant> outputValues;
		ret=iFilter->applyFilter(action, *(meshDoc()), outputValues, postCondMask,  mergedenvironment, QCallBack);
		if (postCondMask == MeshModel::MM_UNKNOWN)
			postCondMask = iFilter->postCondition(action);
		for (MeshModel* mm = meshDoc()->nextMesh(); mm != NULL; mm = meshDoc()->nextMesh(mm))
			vcg::tri::Allocator<CMeshO>::CompactEveryVector(mm->cm);
		
		if (shar != NULL)
		{
			shar->removeView(iFilter->glContext);
			delete filterWidget;
		}
		
		meshDoc()->setBusy(false);
		
		qApp->restoreOverrideCursor();
		
		// (5) Apply post filter actions (e.g. recompute non updated stuff if needed)
		
		if(ret)
		{
			meshDoc()->Log.Logf(GLLogStream::SYSTEM,"Applied filter %s in %i msec",qUtf8Printable(action->text()),tt.elapsed());
			if (meshDoc()->mm() != NULL)
				meshDoc()->mm()->setMeshModified();
			MainWindow::globalStatusBar()->showMessage("Filter successfully completed...",2000);
			if(GLA())
			{
				GLA()->setLastAppliedFilter(action);
			}
			lastFilterAct->setText(QString("Apply filter ") + action->text());
			lastFilterAct->setEnabled(true);
		}
		else // filter has failed. show the message error.
		{
			QMessageBox::warning(this, tr("Filter Failure"), QString("Failure of filter <font color=red>: '%1'</font><br><br>").arg(action->text())+iFilter->errorMsg()); // text
			meshDoc()->Log.Logf(GLLogStream::SYSTEM,"Filter failed: %s",qUtf8Printable(iFilter->errorMsg()));
			MainWindow::globalStatusBar()->showMessage("Filter failed...",2000);
		}
		
		
		FilterPluginInterface::FILTER_ARITY arity = iFilter->filterArity(action);
		QList<MeshModel*> tmp;
		switch(arity)
		{
		case (FilterPluginInterface::SINGLE_MESH):
		{
			tmp.push_back(meshDoc()->mm());
			break;
		}
		case (FilterPluginInterface::FIXED):
		{
			for(const RichParameter& p : mergedenvironment)
			{
				if (p.value().isMesh())
				{
					MeshModel* mm = p.value().getMesh();
					if (mm != NULL)
						tmp.push_back(mm);
				}
			}
			break;
		}
		case (FilterPluginInterface::VARIABLE):
		{
			for(MeshModel* mm = meshDoc()->nextMesh();mm != NULL;mm=meshDoc()->nextMesh(mm))
			{
				if (mm->isVisible())
					tmp.push_back(mm);
			}
			break;
		}
		default:
			break;
		}
		
		if(iFilter->getClass(action) & FilterPluginInterface::MeshCreation )
			GLA()->resetTrackBall();
		
		for(int jj = 0;jj < tmp.size();++jj)
		{
			MeshModel* mm = tmp[jj];
			if (mm != NULL)
			{
				// at the end for filters that change the color, or selection set the appropriate rendering mode
				if(iFilter->getClass(action) & FilterPluginInterface::FaceColoring )
					mm->updateDataMask(MeshModel::MM_FACECOLOR);
				
				if(iFilter->getClass(action) & FilterPluginInterface::VertexColoring )
					mm->updateDataMask(MeshModel::MM_VERTCOLOR);
				
				if(iFilter->getClass(action) & FilterPluginInterface::MeshColoring )
					mm->updateDataMask(MeshModel::MM_COLOR);
				
				if(postCondMask & MeshModel::MM_CAMERA)
					mm->updateDataMask(MeshModel::MM_CAMERA);
				
				if(iFilter->getClass(action) & FilterPluginInterface::Texture )
					updateTexture(mm->id());
			}
		}
		
		int fclasses =	iFilter->getClass(action);
		//MLSceneGLSharedDataContext* sharedcont = GLA()->getSceneGLSharedContext();
		
		updateSharedContextDataAfterFilterExecution(postCondMask,fclasses,newmeshcreated);
		meshDoc()->meshDocStateData().clear();
	}
	catch (const std::bad_alloc& bdall)
	{
		meshDoc()->setBusy(false);
		qApp->restoreOverrideCursor();
		QMessageBox::warning(
					this, tr("Filter Failure"),
					QString("Operating system was not able to allocate the requested memory.<br><b>"
					"Failure of filter <font color=red>: '%1'</font><br>").arg(action->text())+bdall.what()); // text
		MainWindow::globalStatusBar()->showMessage("Filter failed...",2000);
	}
	qb->reset();
	layerDialog->setVisible(layerDialog->isVisible() || ((newmeshcreated) && (meshDoc()->size() > 0)));
	updateLayerDialog();
	updateMenus();
	MultiViewer_Container* mvc = currentViewContainer();
	if (mvc)
	{
		mvc->updateAllDecoratorsForAllViewers();
		mvc->updateAllViewers();
	}
}

// Edit Mode Management
// At any point there can be a single editing plugin active.
// When a plugin is active it intercept the mouse actions.
// Each active editing tools
//
//


void MainWindow::suspendEditMode()
{
	// return if no window is open
	if(!GLA()) return;
	
	// return if no editing action is currently ongoing
	if(!GLA()->getCurrentEditAction()) return;
	
	GLA()->suspendEditToggle();
	updateMenus();
	GLA()->update();
}
void MainWindow::applyEditMode()
{
	if(!GLA()) { //prevents crash without mesh
		QAction *action = qobject_cast<QAction *>(sender());
		action->setChecked(false);
		return;
	}
	
	QAction *action = qobject_cast<QAction *>(sender());
	
	if(GLA()->getCurrentEditAction()) //prevents multiple buttons pushed
	{
		if(action==GLA()->getCurrentEditAction()) // We have double pressed the same action and that means disable that actioon
		{
			if(GLA()->suspendedEditor)
			{
				suspendEditMode();
				return;
			}
			endEdit();
			updateMenus();
			return;
		}
		assert(0); // it should be impossible to start an action without having ended the previous one.
		return;
	}
	
	//if this GLArea does not have an instance of this action's MeshEdit tool then give it one
	if(!GLA()->editorExistsForAction(action))
	{
		EditPluginInterfaceFactory *iEditFactory = qobject_cast<EditPluginInterfaceFactory *>(action->parent());
		EditPluginInterface *iEdit = iEditFactory->getMeshEditInterface(action);
		GLA()->addMeshEditor(action, iEdit);
	}
	meshDoc()->meshDocStateData().create(*meshDoc());
	GLA()->setCurrentEditAction(action);
	updateMenus();
	GLA()->update();
}

void MainWindow::applyRenderMode()
{
	QAction *action = qobject_cast<QAction *>(sender());		// find the action which has sent the signal
	if ((GLA()!= NULL) && (GLA()->getRenderer() != NULL))
	{
		GLA()->getRenderer()->Finalize(GLA()->getCurrentShaderAction(),meshDoc(),GLA());
		GLA()->setRenderer(NULL,NULL);
	}
	// Make the call to the plugin core
	RenderPluginInterface *iRenderTemp = qobject_cast<RenderPluginInterface *>(action->parent());
	bool initsupport = false;
	
	if (currentViewContainer() == NULL)
		return;
	
	MLSceneGLSharedDataContext* shared = currentViewContainer()->sharedDataContext();
	
	if ((shared != NULL) && (iRenderTemp != NULL))
	{
		MLSceneGLSharedDataContext::PerMeshRenderingDataMap rdmap;
		shared->getRenderInfoPerMeshView(GLA()->context(), rdmap);
		iRenderTemp->Init(action,*(meshDoc()),rdmap, GLA());
		initsupport = iRenderTemp->isSupported();
		if (initsupport)
			GLA()->setRenderer(iRenderTemp,action);
		else
		{
			if (!initsupport)
			{
				QString msg = "The selected shader is not supported by your graphic hardware!";
				GLA()->Log(GLLogStream::SYSTEM,qUtf8Printable(msg));
			}
			iRenderTemp->Finalize(action,meshDoc(),GLA());
		}
	}
	
	/*I clicked None in renderMenu */
	if ((action->parent() == this) || (!initsupport))
	{
		QString msg("No Shader.");
		GLA()->Log(GLLogStream::SYSTEM,qUtf8Printable(msg));
		GLA()->setRenderer(0,0); //default opengl pipeline or vertex and fragment programs not supported
	}
	GLA()->update();
}


void MainWindow::applyDecorateMode()
{
	if(GLA()->mm() == 0) return;
	QAction *action = qobject_cast<QAction *>(sender());		// find the action which has sent the signal
	
	DecoratePluginInterface *iDecorateTemp = qobject_cast<DecoratePluginInterface *>(action->parent());
	
	GLA()->toggleDecorator(iDecorateTemp->decorationName(action));
	
	updateMenus();
	layerDialog->updateDecoratorParsView();
	layerDialog->updateLog(meshDoc()->Log);
	layerDialog->update();
	GLA()->update();
}

void MainWindow::addShaders()
{
	QStringList fileList = QFileDialog::getOpenFileNames(this, "Load Shaders", "", "*GDP Shader File (*.gdp)");
	for (const QString& fileName : fileList){
		QFileInfo finfo(fileName);

		/** ToDo check vert and frag files **/
		/** ToDo check if shader already exists **/

		QString newGdpFileName = MeshLabApplication::extraShadersLocation() + "/" +finfo.fileName();

		
		QFile::copy(fileName, newGdpFileName);
	}
}


/*
Save project. It saves the info of all the layers and the layer themselves. So
*/
void MainWindow::saveProject()
{
	if (meshDoc() == NULL)
		return;
	//if a mesh has been created by a create filter we must before to save it. Otherwise the project will refer to a mesh without file name path.
	foreach(MeshModel * mp, meshDoc()->meshList)
	{
		if ((mp != NULL) && (mp->fullName().isEmpty()))
		{
			bool saved = exportMesh(tr(""),mp,false);
			if (!saved)
			{
				QString msg = "Mesh layer " + mp->label() + " cannot be saved on a file.\nProject \"" + meshDoc()->docLabel() + "\" saving has been aborted.";
				QMessageBox::warning(this,tr("Project Saving Aborted"),msg);
				return;
			}
		}
	}
	QFileDialog* saveDiag = new QFileDialog(this,tr("Save Project File"),lastUsedDirectory.path().append(""), tr("MeshLab Project (*.mlp);;MeshLab Binary Project (*.mlb);;Align Project (*.aln)"));
#if defined(Q_OS_WIN)
	saveDiag->setOption(QFileDialog::DontUseNativeDialog);
#endif
	QCheckBox* saveAllFile = new QCheckBox(QString("Save All Files"),saveDiag);
	saveAllFile->setCheckState(Qt::Unchecked);
	QCheckBox* onlyVisibleLayers = new QCheckBox(QString("Only Visible Layers"),saveDiag);
	onlyVisibleLayers->setCheckState(Qt::Unchecked);
	QCheckBox* saveViewState = new QCheckBox(QString("Save View State"), saveDiag);
	saveViewState->setCheckState(Qt::Checked);
	QGridLayout* layout = qobject_cast<QGridLayout*>(saveDiag->layout());
	if (layout != NULL)
	{
		layout->addWidget(onlyVisibleLayers, 4, 0);
		layout->addWidget(saveViewState, 4, 1);
		layout->addWidget(saveAllFile, 4, 2);
	}
	saveDiag->setAcceptMode(QFileDialog::AcceptSave);
	saveDiag->exec();
	QStringList files = saveDiag->selectedFiles();
	if (files.size() != 1)
		return;
	QString fileName = files[0];
	// this change of dir is needed for subsequent textures/materials loading
	QFileInfo fi(fileName);
	if (fi.isDir())
		return;
	if (fi.suffix().isEmpty())
	{
		QRegExp reg("\\.\\w+");
		saveDiag->selectedNameFilter().indexOf(reg);
		QString ext = reg.cap();
		fileName.append(ext);
		fi.setFile(fileName);
	}
	QDir::setCurrent(fi.absoluteDir().absolutePath());
	
	/*********WARNING!!!!!! CHANGE IT!!! ALSO IN THE OPENPROJECT FUNCTION********/
	meshDoc()->setDocLabel(fileName);
	QMdiSubWindow* sub = mdiarea->currentSubWindow();
	if (sub != NULL)
	{
		sub->setWindowTitle(meshDoc()->docLabel());
		layerDialog->setWindowTitle(meshDoc()->docLabel());
	}
	/****************************************************************************/
	
	
	bool ret;
	qDebug("Saving aln file %s\n", qUtf8Printable(fileName));
	if (fileName.isEmpty()) return;
	else
	{
		//save path away so we can use it again
		QString path = fileName;
		path.truncate(path.lastIndexOf("/"));
		lastUsedDirectory.setPath(path);
	}
	if (QString(fi.suffix()).toLower() == "aln")
	{
		vector<string> meshNameVector;
		vector<Matrix44m> transfVector;
		
		foreach(MeshModel * mp, meshDoc()->meshList)
		{
			if((!onlyVisibleLayers->isChecked()) || (mp->visible))
			{
				meshNameVector.push_back(qUtf8Printable(mp->relativePathName()));
				transfVector.push_back(mp->cm.Tr);
			}
		}
		ret = ALNParser::SaveALN(qUtf8Printable(fileName), meshNameVector, transfVector);
	}
	else
	{
		std::map<int, MLRenderingData> rendOpt;
		foreach(MeshModel * mp, meshDoc()->meshList)
		{
			MLRenderingData ml;
			getRenderingData(mp->id(), ml);
			rendOpt.insert(std::pair<int, MLRenderingData>(mp->id(), ml));
		}
		ret = MeshDocumentToXMLFile(*meshDoc(), fileName, onlyVisibleLayers->isChecked(), saveViewState->isChecked(), QString(fi.suffix()).toLower() == "mlb", rendOpt);
	}
	
	if (saveAllFile->isChecked())
	{
		for(int ii = 0; ii < meshDoc()->meshList.size();++ii)
		{
			MeshModel* mp = meshDoc()->meshList[ii];
			if((!onlyVisibleLayers->isChecked()) || (mp->visible))
			{
				ret |= exportMesh(mp->fullName(),mp,true);
			}
		}
	}
	if(!ret)
		QMessageBox::critical(this, tr("Meshlab Saving Error"), QString("Unable to save project file %1\n").arg(fileName));
}

bool MainWindow::openProject(QString fileName)
{
	bool visiblelayer = layerDialog->isVisible();
	//showLayerDlg(false);
	globrendtoolbar->setEnabled(false);
	if (fileName.isEmpty())
		fileName = QFileDialog::getOpenFileName(this,tr("Open Project File"), lastUsedDirectory.path(), tr("All Project Files (*.mlp *.mlb *.aln *.out *.nvm);;MeshLab Project (*.mlp);;MeshLab Binary Project (*.mlb);;Align Project (*.aln);;Bundler Output (*.out);;VisualSFM Output (*.nvm)"));
	
	if (fileName.isEmpty()) return false;
	
	QFileInfo fi(fileName);
	lastUsedDirectory = fi.absoluteDir();
	if((fi.suffix().toLower()!="aln") && (fi.suffix().toLower()!="mlp")  && (fi.suffix().toLower() != "mlb") && (fi.suffix().toLower()!="out") && (fi.suffix().toLower()!="nvm"))
	{
		QMessageBox::critical(this, tr("Meshlab Opening Error"), "Unknown project file extension");
		return false;
	}
	
	// Common Part: init a Doc if necessary, and
	bool activeDoc = (bool) !mdiarea->subWindowList().empty() && mdiarea->currentSubWindow();
	bool activeEmpty = activeDoc && meshDoc()->meshList.empty();
	
	if (!activeEmpty)  newProject(fileName);
	
	meshDoc()->setFileName(fileName);
	mdiarea->currentSubWindow()->setWindowTitle(fileName);
	meshDoc()->setDocLabel(fileName);
	
	meshDoc()->setBusy(true);
	
	// this change of dir is needed for subsequent textures/materials loading
	QDir::setCurrent(fi.absoluteDir().absolutePath());
	qb->show();
	
	if (QString(fi.suffix()).toLower() == "aln")
	{
		vector<RangeMap> rmv;
		int retVal = ALNParser::ParseALN(rmv, qUtf8Printable(fileName));
		if(retVal != ALNParser::NoError)
		{
			QMessageBox::critical(this, tr("Meshlab Opening Error"), "Unable to open ALN file");
			return false;
		}
		
		bool openRes=true;
		vector<RangeMap>::iterator ir;
		for(ir=rmv.begin();ir!=rmv.end() && openRes;++ir)
		{
			QString relativeToProj = fi.absoluteDir().absolutePath() + "/" + (*ir).filename.c_str();
			meshDoc()->addNewMesh(relativeToProj,relativeToProj);
			openRes = loadMeshWithStandardParams(relativeToProj,this->meshDoc()->mm(),ir->transformation);
			if(!openRes)
				meshDoc()->delMesh(meshDoc()->mm());
		}
	}
	
	if (QString(fi.suffix()).toLower() == "mlp" || QString(fi.suffix()).toLower() == "mlb")
	{
		std::map<int, MLRenderingData> rendOpt;
		if (!MeshDocumentFromXML(*meshDoc(), fileName, (QString(fi.suffix()).toLower() == "mlb"), rendOpt))
		{
			QMessageBox::critical(this, tr("Meshlab Opening Error"), "Unable to open MeshLab Project file");
			return false;
		}
		GLA()->updateMeshSetVisibilities();
		for (int i=0; i<meshDoc()->meshList.size(); i++)
		{
			QString fullPath = meshDoc()->meshList[i]->fullName();
			//meshDoc()->setBusy(true);
			Matrix44m trm = this->meshDoc()->meshList[i]->cm.Tr; // save the matrix, because loadMeshClear it...
			MLRenderingData* ptr = NULL;
			if (rendOpt.find(meshDoc()->meshList[i]->id()) != rendOpt.end())
				ptr = &rendOpt[meshDoc()->meshList[i]->id()];
			if (!loadMeshWithStandardParams(fullPath, this->meshDoc()->meshList[i], trm, false, ptr))
				meshDoc()->delMesh(meshDoc()->meshList[i]);
		}
	}
	
	////// BUNDLER
	if (QString(fi.suffix()).toLower() == "out"){
		
		QString cameras_filename = fileName;
		QString image_list_filename;
		QString model_filename;
		
		image_list_filename = QFileDialog::getOpenFileName(
				this,
				tr("Open image list file"),
				QFileInfo(fileName).absolutePath(),
				tr("Bundler images list file (*.txt)"));
		if(image_list_filename.isEmpty())
			return false;
		
		if(!MeshDocumentFromBundler(*meshDoc(),cameras_filename,image_list_filename,model_filename)){
			QMessageBox::critical(this, tr("Meshlab Opening Error"), "Unable to open OUTs file");
			return false;
		}
		
		
		//WARNING!!!!! i suppose it's not useful anymore but.......
		/*GLA()->setColorMode(GLW::CMPerVert);
GLA()->setDrawMode(GLW::DMPoints);*/
		/////////////////////////////////////////////////////////
	}
	
	//////NVM
	if (QString(fi.suffix()).toLower() == "nvm"){
		
		QString cameras_filename = fileName;
		QString model_filename;
		
		if(!MeshDocumentFromNvm(*meshDoc(),cameras_filename,model_filename)){
			QMessageBox::critical(this, tr("Meshlab Opening Error"), "Unable to open NVMs file");
			return false;
		}
		//WARNING!!!!! i suppose it's not useful anymore but.......
		/*GLA()->setColorMode(GLW::CMPerVert);
GLA()->setDrawMode(GLW::DMPoints);*/
		/////////////////////////////////////////////////////////
	}
	
	meshDoc()->setBusy(false);
	if(this->GLA() == 0)  return false;
	
	MultiViewer_Container* mvc = currentViewContainer();
	if (mvc != NULL)
	{
		mvc->resetAllTrackBall();
		mvc->updateAllDecoratorsForAllViewers();
	}
	
	setCurrentMeshBestTab();
	qb->reset();
	saveRecentProjectList(fileName);
	globrendtoolbar->setEnabled(true);
	showLayerDlg(visiblelayer || (meshDoc()->meshList.size() > 0));
	
	return true;
}

bool MainWindow::appendProject(QString fileName)
{
	QStringList fileNameList;
	globrendtoolbar->setEnabled(false);
	if (fileName.isEmpty())
		fileNameList = QFileDialog::getOpenFileNames(this, tr("Append Project File"), lastUsedDirectory.path(), "All Project Files (*.mlp *.mlb *.aln *.out *.nvm);;MeshLab Project (*.mlp);;MeshLab Binary Project (*.mlb);;Align Project (*.aln);;Bundler Output (*.out);;VisualSFM Output (*.nvm)");
	else
		fileNameList.append(fileName);
	
	if (fileNameList.isEmpty()) return false;
	
	// Ccheck if we have a doc and if it is empty
	bool activeDoc = (bool) !mdiarea->subWindowList().empty() && mdiarea->currentSubWindow();
	if (!activeDoc || meshDoc()->meshList.empty())  // it is wrong to try appending to an empty project, even if it is possible
	{
		QMessageBox::critical(this, tr("Meshlab Opening Error"), "Current project is empty, cannot append");
		return false;
	}
	
	meshDoc()->setBusy(true);
	
	// load all projects
	foreach(fileName,fileNameList)
	{
		QFileInfo fi(fileName);
		lastUsedDirectory = fi.absoluteDir();
		
		if((fi.suffix().toLower()!="aln") && (fi.suffix().toLower()!="mlp") && (fi.suffix().toLower() != "mlb") && (fi.suffix().toLower() != "out") && (fi.suffix().toLower() != "nvm"))
		{
			QMessageBox::critical(this, tr("Meshlab Opening Error"), "Unknown project file extension");
			return false;
		}
		
		// this change of dir is needed for subsequent textures/materials loading
		QDir::setCurrent(fi.absoluteDir().absolutePath());
		qb->show();
		
		if (QString(fi.suffix()).toLower() == "aln")
		{
			vector<RangeMap> rmv;
			int retVal = ALNParser::ParseALN(rmv, qUtf8Printable(fileName));
			if(retVal != ALNParser::NoError)
			{
				QMessageBox::critical(this, tr("Meshlab Opening Error"), "Unable to open ALN file");
				return false;
			}
			
			for(vector<RangeMap>::iterator ir=rmv.begin();ir!=rmv.end();++ir)
			{
				QString relativeToProj = fi.absoluteDir().absolutePath() + "/" + (*ir).filename.c_str();
				meshDoc()->addNewMesh(relativeToProj,relativeToProj);
				if(!loadMeshWithStandardParams(relativeToProj,this->meshDoc()->mm(),(*ir).transformation))
					meshDoc()->delMesh(meshDoc()->mm());
			}
		}
		
		if (QString(fi.suffix()).toLower() == "mlp" || QString(fi.suffix()).toLower() == "mlb")
		{
			int alreadyLoadedNum = meshDoc()->meshList.size();
			std::map<int, MLRenderingData> rendOpt;
			if (!MeshDocumentFromXML(*meshDoc(),fileName, QString(fi.suffix()).toLower() == "mlb", rendOpt))
			{
				QMessageBox::critical(this, tr("Meshlab Opening Error"), "Unable to open MeshLab Project file");
				return false;
			}
			GLA()->updateMeshSetVisibilities();
			for (int i = alreadyLoadedNum; i<meshDoc()->meshList.size(); i++)
			{
				QString fullPath = meshDoc()->meshList[i]->fullName();
				meshDoc()->setBusy(true);
				Matrix44m trm = this->meshDoc()->meshList[i]->cm.Tr; // save the matrix, because loadMeshClear it...
				MLRenderingData* ptr = NULL;
				if (rendOpt.find(meshDoc()->meshList[i]->id()) != rendOpt.end())
					ptr = &rendOpt[meshDoc()->meshList[i]->id()];
				if(!loadMeshWithStandardParams(fullPath,this->meshDoc()->meshList[i],trm, false, ptr))
					meshDoc()->delMesh(meshDoc()->meshList[i]);
			}
		}
		
		if (QString(fi.suffix()).toLower() == "out") {
			
			QString cameras_filename = fileName;
			QString image_list_filename;
			QString model_filename;
			
			image_list_filename = QFileDialog::getOpenFileName(
					this, 
					tr("Open image list file"),
					QFileInfo(fileName).absolutePath(),
					tr("Bundler images list file (*.txt)"));
			if (image_list_filename.isEmpty())
				return false;
			
			if (!MeshDocumentFromBundler(*meshDoc(), cameras_filename, image_list_filename, model_filename)) {
				QMessageBox::critical(this, tr("Meshlab Opening Error"), "Unable to open OUTs file");
				return false;
			}
		}
		
		if (QString(fi.suffix()).toLower() == "nvm") {
			
			QString cameras_filename = fileName;
			QString model_filename;
			
			if (!MeshDocumentFromNvm(*meshDoc(), cameras_filename, model_filename)) {
				QMessageBox::critical(this, tr("Meshlab Opening Error"), "Unable to open NVMs file");
				return false;
			}
		}
	}
	
	globrendtoolbar->setEnabled(true);
	meshDoc()->setBusy(false);
	if(this->GLA() == 0)  return false;
	MultiViewer_Container* mvc = currentViewContainer();
	if (mvc != NULL)
	{
		mvc->updateAllDecoratorsForAllViewers();
		mvc->resetAllTrackBall();
	}
	
	setCurrentMeshBestTab();
	qb->reset();
	saveRecentProjectList(fileName);
	return true;
}

void MainWindow::setCurrentMeshBestTab()
{
	if (layerDialog == NULL)
		return;
	
	MultiViewer_Container* mvc = currentViewContainer();
	if (mvc != NULL)
	{
		MLSceneGLSharedDataContext* cont = mvc->sharedDataContext();
		if ((GLA() != NULL) && (meshDoc() != NULL) && (meshDoc()->mm() != NULL))
		{
			MLRenderingData dt;
			cont->getRenderInfoPerMeshView(meshDoc()->mm()->id(), GLA()->context(), dt);
			layerDialog->setCurrentTab(dt);
		}
	}
}

void MainWindow::newProject(const QString& projName)
{
	if (gpumeminfo == NULL)
		return;
	MultiViewer_Container *mvcont = new MultiViewer_Container(*gpumeminfo,mwsettings.highprecision,mwsettings.perbatchprimitives,mwsettings.minpolygonpersmoothrendering,mdiarea);
	connect(&mvcont->meshDoc,SIGNAL(meshAdded(int)),this,SLOT(meshAdded(int)));
	connect(&mvcont->meshDoc,SIGNAL(meshRemoved(int)),this,SLOT(meshRemoved(int)));
	connect(&mvcont->meshDoc, SIGNAL(documentUpdated()), this, SLOT(documentUpdateRequested()));
	connect(mvcont, SIGNAL(closingMultiViewerContainer()), this, SLOT(closeCurrentDocument()));
	mdiarea->addSubWindow(mvcont);
	connect(mvcont,SIGNAL(updateMainWindowMenus()),this,SLOT(updateMenus()));
	connect(mvcont,SIGNAL(updateDocumentViewer()),this,SLOT(updateLayerDialog()));
	connect(&mvcont->meshDoc.Log, SIGNAL(logUpdated()), this, SLOT(updateLog()));
	filterMenu->setEnabled(!filterMenu->actions().isEmpty());
	if (!filterMenu->actions().isEmpty())
		updateSubFiltersMenu(true,false);
	GLArea *gla=new GLArea(this, mvcont, &currentGlobalParams);
	//connect(gla, SIGNAL(insertRenderingDataForNewlyGeneratedMesh(int)), this, SLOT(addRenderingDataIfNewlyGeneratedMesh(int)));
	mvcont->addView(gla, Qt::Horizontal);
	
	if (projName.isEmpty())
	{
		static int docCounter = 1;
		mvcont->meshDoc.setDocLabel(QString("Project_") + QString::number(docCounter));
		++docCounter;
	}
	else
		mvcont->meshDoc.setDocLabel(projName);
	mvcont->setWindowTitle(mvcont->meshDoc.docLabel());
	if (layerDialog != NULL)
		layerDialog->reset();
	//if(mdiarea->isVisible())
	updateLayerDialog();
	mvcont->showMaximized();
	connect(mvcont->sharedDataContext(),SIGNAL(currentAllocatedGPUMem(int,int,int,int)),this,SLOT(updateGPUMemBar(int,int,int,int)));
}

void MainWindow::documentUpdateRequested()
{
	if (meshDoc() == NULL)
		return;
	for (int ii = 0; ii < meshDoc()->meshList.size(); ++ii)
	{
		MeshModel* mm = meshDoc()->meshList[ii];
		if (mm != NULL)
		{
			addRenderingDataIfNewlyGeneratedMesh(mm->id());
			updateLayerDialog();
			if (currentViewContainer() != NULL)
			{
				currentViewContainer()->resetAllTrackBall();
				currentViewContainer()->updateAllViewers();
			}
		}
	}
}

void MainWindow::updateFilterToolBar()
{
	filterToolBar->clear();
	
	for(FilterPluginInterface *iFilter: PM.filterPluginIterator()) {
		for(QAction* filterAction: iFilter->actions()) {
			if (!filterAction->icon().isNull()) {
				// tooltip = iFilter->filterInfo(filterAction) + "<br>" + getDecoratedFileName(filterAction->data().toString());
				if (filterAction->priority() != QAction::LowPriority)
					filterToolBar->addAction(filterAction);
			} //else qDebug() << "action was null";
		}
	}
}

void MainWindow::updateGPUMemBar(int nv_allmem, int nv_currentallocated, int ati_free_tex, int ati_free_vbo)
{
#ifdef Q_OS_WIN
	if (nvgpumeminfo != NULL)
	{
		if (nv_allmem + nv_currentallocated > 0)
		{
			nvgpumeminfo->setFormat("Mem %p% %v/%m MB");
			int allmb = nv_allmem / 1024;
			nvgpumeminfo->setRange(0, allmb);
			int remainingmb = (nv_allmem - nv_currentallocated) / 1024;
			nvgpumeminfo->setValue(remainingmb);
			nvgpumeminfo->setFixedWidth(300);
		}
		else if (ati_free_tex + ati_free_vbo > 0)
		{
			int texmb = ati_free_tex / 1024;
			int vbomb = ati_free_vbo / 1024;
			nvgpumeminfo->setFormat(QString("Free: " + QString::number(vbomb) + "MB vbo - " + QString::number(texmb) + "MB tex"));
			nvgpumeminfo->setRange(0, 100);
			nvgpumeminfo->setValue(100);
			nvgpumeminfo->setFixedWidth(300);
		}
		else
		{
			nvgpumeminfo->setFormat("UNRECOGNIZED CARD");
			nvgpumeminfo->setRange(0, 100);
			nvgpumeminfo->setValue(0);
			nvgpumeminfo->setFixedWidth(300);
		}
	}
#else
	//avoid unused parameter warning
	(void) nv_allmem;
	(void) nv_currentallocated;
	(void) ati_free_tex;
	(void) ati_free_vbo;
	nvgpumeminfo->hide();
#endif
}
//WARNING!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//Temporary disgusting inequality between open (slot) - importMesh (function)
//and importRaster (slot). It's not also difficult to remove the problem because
//addNewRaster add a raster on a document and open the file, instead addNewMesh add a new mesh layer
//without loading the model.

bool MainWindow::importRaster(const QString& fileImg)
{
	if (!GLA())
	{
		this->newProject();
		if (!GLA())
			return false;
	}
	
	QStringList fileNameList;
	if (fileImg.isEmpty())
		fileNameList = QFileDialog::getOpenFileNames(this,tr("Import Mesh"), lastUsedDirectory.path(), PM.inputRasterFormatListDialog().join(";;"));
	else
		fileNameList.push_back(fileImg);
	
	if (fileNameList.isEmpty())	return false;
	else
	{
		//save path away so we can use it again
		QString path = fileNameList.first();
		path.truncate(path.lastIndexOf("/"));
		lastUsedDirectory.setPath(path);
	}
	
	QElapsedTimer allFileTime;
	allFileTime.start();
	
	for(const QString& fileName : fileNameList) {
		QFileInfo fi(fileName);
		QString extension = fi.suffix();
		IORasterPluginInterface *pCurrentIOPlugin = PM.inputRasterPlugin(extension);
		//pCurrentIOPlugin->setLog(gla->log);
		if (pCurrentIOPlugin == NULL)
		{
			QString errorMsgFormat("Unable to open file:\n\"%1\"\n\nError details: file format " + extension + " not supported.");
			QMessageBox::critical(this, tr("Meshlab Opening Error"), errorMsgFormat.arg(fileName));
			return false;
		}
		
		
		QFileInfo info(fileName);
		RasterModel *rm = meshDoc()->addNewRaster();
		qb->show();
		QElapsedTimer t;
		t.start();
		bool open = pCurrentIOPlugin->open(extension, fileName, *rm, QCallBack);
		if(open) {
			GLA()->Logf(0, "Opened raster %s in %i msec", qUtf8Printable(fileName), t.elapsed());
			GLA()->resetTrackBall();
			GLA()->fov = rm->shot.GetFovFromFocal();
			rm->shot = GLA()->shotFromTrackball().first;
			GLA()->resetTrackBall(); // and then we reset the trackball again, to have the standard view
			if (!layerDialog->isVisible())
				layerDialog->setVisible(true);
		}
		else {
			meshDoc()->delRaster(rm);
			GLA()->Logf(0, "Warning: Raster %s has not been opened", qUtf8Printable(fileName));
		}
	}// end foreach file of the input list
	GLA()->Logf(0,"All files opened in %i msec",allFileTime.elapsed());
	
	if (_currviewcontainer != NULL)
		_currviewcontainer->updateAllDecoratorsForAllViewers();
	
	updateMenus();
	updateLayerDialog();

	qb->reset();
	return true;
}

bool MainWindow::loadMesh(const QString& fileName, IOMeshPluginInterface *pCurrentIOPlugin, MeshModel* mm, int& mask,RichParameterList* prePar, const Matrix44m &mtr, bool isareload, MLRenderingData* rendOpt)
{
	if ((GLA() == NULL) || (mm == NULL))
		return false;
	
	QFileInfo fi(fileName);
	QString extension = fi.suffix();
	if(!fi.exists())
	{
		QString errorMsgFormat = "Unable to open file:\n\"%1\"\n\nError details: file %1 does not exist.";
		QMessageBox::critical(this, tr("Meshlab Opening Error"), errorMsgFormat.arg(fileName));
		return false;
	}
	if(!fi.isReadable())
	{
		QString errorMsgFormat = "Unable to open file:\n\"%1\"\n\nError details: file %1 is not readable.";
		QMessageBox::critical(this, tr("Meshlab Opening Error"), errorMsgFormat.arg(fileName));
		return false;
	}
	
	// the original directory path before we switch it
	QString origDir = QDir::current().path();
	
	// this change of dir is needed for subsequent textures/materials loading
	QDir::setCurrent(fi.absoluteDir().absolutePath());
	
	// Adjust the file name after changing the directory
	QString fileNameSansDir = fi.fileName();
	
	// retrieving corresponding IO plugin
	if (pCurrentIOPlugin == 0)
	{
		QString errorMsgFormat = "Error encountered while opening file:\n\"%1\"\n\nError details: The \"%2\" file extension does not correspond to any supported format.";
		QMessageBox::critical(this, tr("Opening Error"), errorMsgFormat.arg(fileName, extension));
		QDir::setCurrent(origDir); // undo the change of directory before leaving
		return false;
	}
	meshDoc()->setBusy(true);
	pCurrentIOPlugin->setLog(&meshDoc()->Log);
	
	if (!pCurrentIOPlugin->open(extension, fileNameSansDir, *mm ,mask,*prePar,QCallBack,this /*gla*/))
	{
		QMessageBox::warning(this, tr("Opening Failure"), QString("While opening: '%1'\n\n").arg(fileName)+pCurrentIOPlugin->errorMsg()); // text+
		pCurrentIOPlugin->clearErrorString();
		meshDoc()->setBusy(false);
		QDir::setCurrent(origDir); // undo the change of directory before leaving
		return false;
	}
	
	
	//std::cout << "Opened mesh: in " << tm.elapsed() << " secs\n";
	// After opening the mesh lets ask to the io plugin if this format
	// requires some optional, or userdriven post-opening processing.
	// and in that case ask for the required parameters and then
	// ask to the plugin to perform that processing
	//RichParameterSet par;
	//pCurrentIOPlugin->initOpenParameter(extension, *mm, par);
	//pCurrentIOPlugin->applyOpenParameter(extension, *mm, par);
	
	QString err = pCurrentIOPlugin->errorMsg();
	if (!err.isEmpty())
	{
		QMessageBox::warning(this, tr("Opening Problems"), QString("While opening: '%1'\n\n").arg(fileName)+pCurrentIOPlugin->errorMsg());
		pCurrentIOPlugin->clearErrorString();
	}
	
	saveRecentFileList(fileName);
	
	if (!(mm->cm.textures.empty()))
		updateTexture(mm->id());
	
	// In case of polygonal meshes the normal should be updated accordingly
	if( mask & vcg::tri::io::Mask::IOM_BITPOLYGONAL)
	{
		mm->updateDataMask(MeshModel::MM_POLYGONAL); // just to be sure. Hopefully it should be done in the plugin...
		int degNum = tri::Clean<CMeshO>::RemoveDegenerateFace(mm->cm);
		if(degNum)
			GLA()->Logf(0,"Warning model contains %i degenerate faces. Removed them.",degNum);
		mm->updateDataMask(MeshModel::MM_FACEFACETOPO);
		vcg::tri::UpdateNormal<CMeshO>::PerBitQuadFaceNormalized(mm->cm);
		vcg::tri::UpdateNormal<CMeshO>::PerVertexFromCurrentFaceNormal(mm->cm);
	} // standard case
	else
	{
		vcg::tri::UpdateNormal<CMeshO>::PerFaceNormalized(mm->cm);
		if(!( mask & vcg::tri::io::Mask::IOM_VERTNORMAL) )
			vcg::tri::UpdateNormal<CMeshO>::PerVertexAngleWeighted(mm->cm);
	}
	
	vcg::tri::UpdateBounding<CMeshO>::Box(mm->cm);					// updates bounding box
	if(mm->cm.fn==0 && mm->cm.en==0)
	{
		if(mask & vcg::tri::io::Mask::IOM_VERTNORMAL)
			mm->updateDataMask(MeshModel::MM_VERTNORMAL);
	}
	
	if(mm->cm.fn==0 && mm->cm.en>0)
	{
		if (mask & vcg::tri::io::Mask::IOM_VERTNORMAL)
			mm->updateDataMask(MeshModel::MM_VERTNORMAL);
	}
	
	updateMenus();
	int delVertNum = vcg::tri::Clean<CMeshO>::RemoveDegenerateVertex(mm->cm);
	int delFaceNum = vcg::tri::Clean<CMeshO>::RemoveDegenerateFace(mm->cm);
	tri::Allocator<CMeshO>::CompactEveryVector(mm->cm);
	if(delVertNum>0 || delFaceNum>0 )
		QMessageBox::warning(this, "MeshLab Warning", QString("Warning mesh contains %1 vertices with NAN coords and %2 degenerated faces.\nCorrected.").arg(delVertNum).arg(delFaceNum) );
	mm->cm.Tr = mtr;
	
	computeRenderingDataOnLoading(mm,isareload, rendOpt);
	updateLayerDialog();
	
	
	meshDoc()->setBusy(false);
	
	QDir::setCurrent(origDir); // undo the change of directory before leaving
	
	return true;
}

void MainWindow::computeRenderingDataOnLoading(MeshModel* mm,bool isareload, MLRenderingData* rendOpt)
{
	MultiViewer_Container* mv = currentViewContainer();
	if (mv != NULL)
	{
		MLSceneGLSharedDataContext* shared = mv->sharedDataContext();
		if ((shared != NULL) && (mm != NULL))
		{
			MLRenderingData defdt;
			MLPoliciesStandAloneFunctions::suggestedDefaultPerViewRenderingData(mm, defdt,mwsettings.minpolygonpersmoothrendering);
			if (rendOpt != NULL)
				defdt = *rendOpt;
			for (int glarid = 0; glarid < mv->viewerCounter(); ++glarid)
			{
				GLArea* ar = mv->getViewer(glarid);
				if (ar != NULL)
				{
					
					if (isareload)
					{
						MLRenderingData currentdt;
						shared->getRenderInfoPerMeshView(mm->id(), ar->context(), currentdt);
						MLRenderingData newdt;
						MLPoliciesStandAloneFunctions::computeRequestedRenderingDataCompatibleWithMeshSameGLOpts(mm, currentdt, newdt);
						MLPoliciesStandAloneFunctions::setPerViewGLOptionsAccordindToWireModality(mm,newdt);
						MLPoliciesStandAloneFunctions::setBestWireModality(mm, newdt);
						shared->setRenderingDataPerMeshView(mm->id(), ar->context(), newdt);
						shared->meshAttributesUpdated(mm->id(), true, MLRenderingData::RendAtts(true));
					}
					else
						shared->setRenderingDataPerMeshView(mm->id(), ar->context(), defdt);
				}
			}
			shared->manageBuffers(mm->id());
		}
	}
}

bool MainWindow::importMeshWithLayerManagement(QString fileName)
{
	bool layervisible = false;
	if (layerDialog != NULL)
	{
		layervisible = layerDialog->isVisible();
		//showLayerDlg(false);
	}
	globrendtoolbar->setEnabled(false);
	bool res = importMesh(fileName,false);
	globrendtoolbar->setEnabled(true);
	if (layerDialog != NULL)
		showLayerDlg(layervisible || meshDoc()->meshList.size());
	setCurrentMeshBestTab();
	return res;
}

// Opening files in a transparent form (IO plugins contribution is hidden to user)
bool MainWindow::importMesh(QString fileName,bool isareload)
{
	if (!GLA())
	{
		this->newProject();
		if(!GLA())
			return false;
	}
	
	
	//QStringList suffixList;
	// HashTable storing all supported formats together with
	// the (1-based) index  of first plugin which is able to open it
	//QHash<QString, MeshIOInterface*> allKnownFormats;
	//PM.LoadFormats(suffixList, allKnownFormats,PluginManager::IMPORT);
	QStringList fileNameList;
	if (fileName.isEmpty())
		fileNameList = QFileDialog::getOpenFileNames(this,tr("Import Mesh"), lastUsedDirectory.path(), PM.inputMeshFormatListDialog().join(";;"));
	else
		fileNameList.push_back(fileName);
	
	if (fileNameList.isEmpty())	return false;
	else
	{
		//save path away so we can use it again
		QString path = fileNameList.first();
		path.truncate(path.lastIndexOf("/"));
		lastUsedDirectory.setPath(path);
	}
	
	QElapsedTimer allFileTime;
	allFileTime.start();
	for(const QString& fileName : fileNameList)
	{
		QFileInfo fi(fileName);
		QString extension = fi.suffix();
		IOMeshPluginInterface *pCurrentIOPlugin = PM.inputMeshPlugin(extension);
		//pCurrentIOPlugin->setLog(gla->log);
		if (pCurrentIOPlugin == nullptr)
		{
			QString errorMsgFormat("Unable to open file:\n\"%1\"\n\nError details: file format " + extension + " not supported.");
			QMessageBox::critical(this, tr("Meshlab Opening Error"), errorMsgFormat.arg(fileName));
			return false;
		}
		
		RichParameterList prePar;
		pCurrentIOPlugin->initPreOpenParameter(extension, fileName,prePar);
		if(!prePar.isEmpty())
		{
			RichParameterListDialog preOpenDialog(this, prePar, tr("Pre-Open Options"));
			preOpenDialog.setFocus();
			preOpenDialog.exec();
		}
		prePar.join(currentGlobalParams);
		int mask = 0;
		//MeshModel *mm= new MeshModel(gla->meshDoc);
		QFileInfo info(fileName);
		MeshModel *mm = meshDoc()->addNewMesh(fileName, info.fileName());
		qb->show();
		QElapsedTimer t;
		t.start();
		Matrix44m mtr;
		mtr.SetIdentity();
		bool open = loadMesh(fileName,pCurrentIOPlugin,mm,mask,&prePar,mtr,isareload);
		if(open)
		{
			GLA()->Logf(0, "Opened mesh %s in %i msec", qUtf8Printable(fileName), t.elapsed());
			RichParameterList par;
			pCurrentIOPlugin->initOpenParameter(extension, *mm, par);
			if(!par.isEmpty())
			{
				RichParameterListDialog postOpenDialog(this, par, tr("Post-Open Processing"));
				postOpenDialog.setFocus();
				postOpenDialog.exec();
				pCurrentIOPlugin->applyOpenParameter(extension, *mm, par);
			}
			/*MultiViewer_Container* mv = GLA()->mvc();
			if (mv != NULL)
			{
			for(int glarid = 0;glarid < mv->viewerCounter();++glarid)
			{
			GLArea* ar = mv->getViewer(glarid);
			if (ar != NULL)
			MLSceneRenderModeAdapter::setupRequestedAttributesAccordingToRenderMode(mm->id(),*ar);
			}
			}*/
		}
		else
		{
			meshDoc()->delMesh(mm);
			GLA()->Logf(0, "Warning: Mesh %s has not been opened", qUtf8Printable(fileName));
		}
	}// end foreach file of the input list
	GLA()->Logf(0,"All files opened in %i msec",allFileTime.elapsed());
	
	if (_currviewcontainer != NULL)
	{
		_currviewcontainer->resetAllTrackBall();
		_currviewcontainer->updateAllDecoratorsForAllViewers();
	}
	qb->reset();
	return true;
}

void MainWindow::openRecentMesh()
{
	if(!GLA()) return;
	if(meshDoc()->isBusy()) return;
	QAction *action = qobject_cast<QAction *>(sender());
	if (action)	importMeshWithLayerManagement(action->data().toString());
}

void MainWindow::openRecentProj()
{
	QAction *action = qobject_cast<QAction *>(sender());
	if (action)	openProject(action->data().toString());
}

bool MainWindow::loadMeshWithStandardParams(QString& fullPath, MeshModel* mm, const Matrix44m &mtr, bool isreload, MLRenderingData* rendOpt)
{
	if ((meshDoc() == NULL) || (mm == NULL))
		return false;
	bool ret = false;
	if (!mm->isVisible())
	{
		mm->Clear();
		mm->visible = false;
	}
	else
		mm->Clear();
	QFileInfo fi(fullPath);
	QString extension = fi.suffix();
	IOMeshPluginInterface *pCurrentIOPlugin = PM.inputMeshPlugin(extension);
	
	if(pCurrentIOPlugin != NULL)
	{
		RichParameterList prePar;
		pCurrentIOPlugin->initPreOpenParameter(extension, fullPath,prePar);
		prePar.join(currentGlobalParams);
		int mask = 0;
		QElapsedTimer t;t.start();
		bool open = loadMesh(fullPath,pCurrentIOPlugin,mm,mask,&prePar,mtr,isreload, rendOpt);
		if(open)
		{
			GLA()->Logf(0, "Opened mesh %s in %i msec", qUtf8Printable(fullPath), t.elapsed());
			RichParameterList par;
			pCurrentIOPlugin->initOpenParameter(extension, *mm, par);
			pCurrentIOPlugin->applyOpenParameter(extension,*mm,par);
			ret = true;
		}
		else
			GLA()->Logf(0, "Warning: Mesh %s has not been opened", qUtf8Printable(fullPath));
	}
	else
		GLA()->Logf(0, "Warning: Mesh %s cannot be opened. Your MeshLab version has not plugin to read %s file format", qUtf8Printable(fullPath), qUtf8Printable(extension));
	return ret;
}

void MainWindow::reloadAllMesh()
{
	// Discards changes and reloads current file
	// save current file name
	qb->show();
	foreach(MeshModel *mmm,meshDoc()->meshList)
	{
		QString fileName = mmm->fullName();
		Matrix44m mat;
		mat.SetIdentity();
		loadMeshWithStandardParams(fileName,mmm,mat,true);
	}
	qb->reset();
	
	if (_currviewcontainer != NULL)
	{
		_currviewcontainer->updateAllDecoratorsForAllViewers();
		_currviewcontainer->updateAllViewers();
	}
}

void MainWindow::reload()
{
	if ((meshDoc() == NULL) || (meshDoc()->mm() == NULL))
		return;
	// Discards changes and reloads current file
	// save current file name
	qb->show();
	QString fileName = meshDoc()->mm()->fullName();
	if (fileName.isEmpty())
	{
		QMessageBox::critical(this, "Reload Error", "Impossible to reload an unsaved mesh model!!");
		return;
	}
	Matrix44m mat;
	mat.SetIdentity();
	loadMeshWithStandardParams(fileName,meshDoc()->mm(),mat,true);
	qb->reset();
	if (_currviewcontainer != NULL)
	{
		_currviewcontainer->updateAllDecoratorsForAllViewers();
		_currviewcontainer->updateAllViewers();
	}
}

bool MainWindow::exportMesh(QString fileName,MeshModel* mod,const bool saveAllPossibleAttributes)
{
	const QStringList& suffixList = PM.outputMeshFormatListDialog();
	
	//QHash<QString, MeshIOInterface*> allKnownFormats;
	QFileInfo fi(fileName);
	//PM.LoadFormats( suffixList, allKnownFormats,PluginManager::EXPORT);
	//QString defaultExt = "*." + mod->suffixName().toLower();
	QString defaultExt = "*." + fi.suffix().toLower();
	if(defaultExt == "*.")
		defaultExt = "*.ply";
	if (mod == NULL)
		return false;
	mod->setMeshModified(false);
	QString laylabel = "Save \"" + mod->label() + "\" Layer";
	QString ss = fi.absoluteFilePath();
	QFileDialog* saveDialog = new QFileDialog(this,laylabel, fi.absolutePath());
#if defined(Q_OS_WIN)
	saveDialog->setOption(QFileDialog::DontUseNativeDialog);
#endif
	saveDialog->setNameFilters(suffixList);
	saveDialog->setAcceptMode(QFileDialog::AcceptSave);
	saveDialog->setFileMode(QFileDialog::AnyFile);
	saveDialog->selectFile(fileName);
	QStringList matchingExtensions=suffixList.filter(defaultExt);
	if(!matchingExtensions.isEmpty())
		saveDialog->selectNameFilter(matchingExtensions.last());
	connect(saveDialog,SIGNAL(filterSelected(const QString&)),this,SLOT(changeFileExtension(const QString&)));
	
	if (fileName.isEmpty()){
		saveDialog->selectFile(meshDoc()->mm()->fullName());
		int dialogRet = saveDialog->exec();
		if(dialogRet==QDialog::Rejected	)
			return false;
		fileName=saveDialog->selectedFiles ().first();
		QFileInfo fni(fileName);
		if(fni.suffix().isEmpty())
		{
			QString ext = saveDialog->selectedNameFilter();
			ext.chop(1); ext = ext.right(4);
			fileName = fileName + ext;
			qDebug("File without extension adding it by hand '%s'", qUtf8Printable(fileName));
		}
	}
	
	
	bool ret = false;
	
	QStringList fs = fileName.split(".");
	
	if(!fileName.isEmpty() && fs.size() < 2)
	{
		QMessageBox::warning(this,"Save Error","You must specify file extension!!");
		return ret;
	}
	
	if (!fileName.isEmpty())
	{
		//save path away so we can use it again
		QString path = fileName;
		path.truncate(path.lastIndexOf("/"));
		lastUsedDirectory.setPath(path);
		
		QString extension = fileName;
		extension.remove(0, fileName.lastIndexOf('.')+1);
		
		QStringListIterator itFilter(suffixList);
		
		IOMeshPluginInterface *pCurrentIOPlugin = PM.outputMeshPlugin(extension);
		if (pCurrentIOPlugin == 0)
		{
			QMessageBox::warning(this, "Unknown type", "File extension not supported!");
			return false;
		}
		//MeshIOInterface* pCurrentIOPlugin = meshIOPlugins[idx-1];
		pCurrentIOPlugin->setLog(&meshDoc()->Log);
		
		int capability=0,defaultBits=0;
		pCurrentIOPlugin->GetExportMaskCapability(extension,capability,defaultBits);
		
		// optional saving parameters (like ascii/binary encoding)
		RichParameterList savePar;
		
		pCurrentIOPlugin->initSaveParameter(extension,*(mod),savePar);
		
		SaveMaskExporterDialog maskDialog(new QWidget(),mod,capability,defaultBits,&savePar,this->GLA());
		if (!saveAllPossibleAttributes)
			maskDialog.exec();
		else
		{
			maskDialog.SlotSelectionAllButton();
			maskDialog.updateMask();
		}
		int mask = maskDialog.GetNewMask();
		if (!saveAllPossibleAttributes)
		{
			maskDialog.close();
			if(maskDialog.result() == QDialog::Rejected)
				return false;
		}
		if(mask == -1)
			return false;
		
		qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
		qb->show();
		QElapsedTimer tt; tt.start();
		ret = pCurrentIOPlugin->save(extension, fileName, *mod ,mask,savePar,QCallBack,this);
		qb->reset();
		if (ret)
		{
			GLA()->Logf(GLLogStream::SYSTEM, "Saved Mesh %s in %i msec", qUtf8Printable(fileName), tt.elapsed());
			mod->setFileName(fileName);
			QSettings settings;
			int savedMeshCounter = settings.value("savedMeshCounter", 0).toInt();
			settings.setValue("savedMeshCounter", savedMeshCounter + 1);
		}
		else
		{
			GLA()->Logf(GLLogStream::SYSTEM, "Error Saving Mesh %s", qUtf8Printable(fileName));
			QMessageBox::critical(this, tr("Meshlab Saving Error"),  pCurrentIOPlugin->errorMsg());
		}
		qApp->restoreOverrideCursor();
		updateLayerDialog();
		
		if (ret)
			QDir::setCurrent(fi.absoluteDir().absolutePath()); //set current dir
	}
	return ret;
}

void MainWindow::changeFileExtension(const QString& st)
{
	QFileDialog* fd = qobject_cast<QFileDialog*>(sender());
	if (fd == NULL)
		return;
	QRegExp extlist("\\*.\\w+");
	int start = st.indexOf(extlist);
	(void)start;
	QString ext = extlist.cap().remove("*");
	QStringList stlst = fd->selectedFiles();
	if (!stlst.isEmpty())
	{
		QFileInfo fi(stlst[0]);
		fd->selectFile(fi.baseName() + ext);
	}
}

bool MainWindow::save(const bool saveAllPossibleAttributes)
{
	return exportMesh(meshDoc()->mm()->fullName(),meshDoc()->mm(),saveAllPossibleAttributes);
}

bool MainWindow::saveAs(QString fileName,const bool saveAllPossibleAttributes)
{
	return exportMesh(fileName,meshDoc()->mm(),saveAllPossibleAttributes);
}

void MainWindow::readViewFromFile(QString const& filename){
	if(GLA() != 0)
		GLA()->readViewFromFile(filename);
}

bool MainWindow::saveSnapshot()
{
	if (!GLA()) return false;
	if (meshDoc()->isBusy()) return false;
	
	SaveSnapshotDialog* dialog = new SaveSnapshotDialog(this);
	//dialog->setModal(true);
	dialog->setValues(GLA()->ss);
	int res = dialog->exec();
	
	if (res == QDialog::Accepted) {
		GLA()->ss=dialog->getValues();
		GLA()->saveSnapshot();
		return true;
	}
	
	return false;
}
void MainWindow::about()
{
	AboutDialog* aboutDialog = new AboutDialog(this);
	aboutDialog->show();
}

void MainWindow::aboutPlugins()
{
	qDebug( "aboutPlugins(): Current Plugins Dir: %s ", qUtf8Printable(meshlab::defaultPluginPath()));
	PluginInfoDialog dialog(this);
	dialog.exec();
	updateAllPluginsActions();
	QSettings settings;
	QStringList disabledPlugins;
	for (PluginFileInterface* pf : PM.pluginIterator(true)){
		if (!pf->isEnabled()){
			disabledPlugins.append(pf->pluginName());
		}
	}
	settings.setValue("DisabledPlugins", QVariant::fromValue(disabledPlugins));
}

void MainWindow::helpOnscreen()
{
	if(GLA()) GLA()->toggleHelpVisible();
}

void MainWindow::helpOnline()
{
	checkForUpdates(false);
	QDesktopServices::openUrl(QUrl("http://www.meshlab.net/#support"));
}

void MainWindow::showToolbarFile(){
	mainToolBar->setVisible(!mainToolBar->isVisible());
}

void MainWindow::showInfoPane()  {if(GLA() != 0)	GLA()->infoAreaVisible =!GLA()->infoAreaVisible;}
void MainWindow::showTrackBall() {if(GLA() != 0) 	GLA()->showTrackBall(!GLA()->isTrackBallVisible());}
void MainWindow::resetTrackBall(){if(GLA() != 0)	GLA()->resetTrackBall();}
void MainWindow::showRaster()    {if(GLA() != 0)	GLA()->showRaster((QApplication::keyboardModifiers () & Qt::ShiftModifier));}
void MainWindow::showLayerDlg(bool visible)
{
	if ((GLA() != 0) && (layerDialog != NULL))
	{
		layerDialog->setVisible( visible);
		showLayerDlgAct->setChecked(visible);
	}
}

void MainWindow::setCustomize()
{
	MeshLabOptionsDialog dialog(currentGlobalParams,defaultGlobalParams, this);
	connect(&dialog, SIGNAL(applyCustomSetting()), this, SLOT(updateCustomSettings()));
	dialog.exec();
}

void MainWindow::fullScreen(){
	if(!isFullScreen())
	{
		toolbarState = saveState();
		menuBar()->hide();
		mainToolBar->hide();
		globalStatusBar()->hide();
		setWindowState(windowState()^Qt::WindowFullScreen);
		bool found=true;
		//Caso di piu' finestre aperte in tile:
		if((mdiarea->subWindowList()).size()>1){
			foreach(QWidget *w,mdiarea->subWindowList()){if(w->isMaximized()) found=false;}
			if (found)mdiarea->tileSubWindows();
		}
	}
	else
	{
		menuBar()->show();
		restoreState(toolbarState);
		globalStatusBar()->show();
		
		setWindowState(windowState()^ Qt::WindowFullScreen);
		bool found=true;
		//Caso di piu' finestre aperte in tile:
		if((mdiarea->subWindowList()).size()>1){
			foreach(QWidget *w,mdiarea->subWindowList()){if(w->isMaximized()) found=false;}
			if (found){mdiarea->tileSubWindows();}
		}
		fullScreenAct->setChecked(false);
	}
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
	if(e->key()==Qt::Key_Return && e->modifiers()==Qt::AltModifier)
	{
		fullScreen();
		e->accept();
	}
	else e->ignore();
}

/**
 * @brief static function that updates the progress bar
 * @param pos: an int value between 0 and 100
 * @param str
 * @return
 */
bool MainWindow::QCallBack(const int pos, const char * str)
{
	int static lastPos = -1;
	if (pos == lastPos) return true;
	lastPos = pos;
	
	static QElapsedTimer currTime;
	if (currTime.isValid() && currTime.elapsed() < 100)
		return true;
	currTime.start();
	MainWindow::globalStatusBar()->showMessage(str, 5000);
	qb->show();
	qb->setEnabled(true);
	qb->setValue(pos);
	MainWindow::globalStatusBar()->update();
	qApp->processEvents();
	return true;
}

void MainWindow::updateTexture(int meshid)
{
	MultiViewer_Container* mvc = currentViewContainer();
	if ((mvc == NULL) || (meshDoc() == NULL))
		return;
	
	MLSceneGLSharedDataContext* shared = mvc->sharedDataContext();
	if (shared == NULL)
		return;
	
	MeshModel* mymesh = meshDoc()->getMesh(meshid);
	if (mymesh  == NULL)
		return;
	
	shared->deAllocateTexturesPerMesh(mymesh->id());
	
	int textmemMB = int(mwsettings.maxTextureMemory / ((float) 1024 * 1024));
	
	size_t totalTextureNum = 0;
	foreach (MeshModel *mp, meshDoc()->meshList)
		totalTextureNum+=mp->cm.textures.size();
	
	int singleMaxTextureSizeMpx = int(textmemMB/((totalTextureNum != 0)? totalTextureNum : 1));
	bool sometextfailed = false;
	QString unexistingtext = "In mesh file <i>" + mymesh->fullName() + "</i> : Failure loading textures:<br>";
	for(size_t i =0; i< mymesh->cm.textures.size();++i)
	{
		QImage img;
		QFileInfo fi(mymesh->cm.textures[i].c_str());
		QString filename = fi.absoluteFilePath();
		bool res = img.load(filename);
		sometextfailed = sometextfailed || !res;
		if(!res)
		{
			res = img.load(filename);
			if(!res)
			{
				QString errmsg = QString("Failure of loading texture %1").arg(fi.fileName());
				meshDoc()->Log.Log(GLLogStream::WARNING,qUtf8Printable(errmsg));
				unexistingtext += "<font color=red>" + filename + "</font><br>";
			}
		}
		
		/*PLEASE EXPLAIN ME!*********************************************************************************************************************************************************************************/
		//if(!res && filename.endsWith("dds",Qt::CaseInsensitive))
		//{
		//    qDebug("DDS binding!");
		//    int newTexId = shared->bindTexture(filename);
		//    shared->txtcont.push_back(newTexId);
		//}
		/*PLEASE EXPLAIN ME!*********************************************************************************************************************************************************************************/
		
		if (!res)
			res = img.load(":/images/dummy.png");
		GLuint textid = shared->allocateTexturePerMesh(meshid,img,singleMaxTextureSizeMpx);
		
		if (sometextfailed)
			QMessageBox::warning(this,"Texture file has not been correctly loaded",unexistingtext);
		
		for(int tt = 0;tt < mvc->viewerCounter();++tt)
		{
			GLArea* ar = mvc->getViewer(tt);
			if (ar != NULL)
				ar->setupTextureEnv(textid);
		}
	}
	if (sometextfailed)
		QMessageBox::warning(this,"Texture file has not been correctly loaded",unexistingtext);
}

void MainWindow::updateProgressBar( const int pos,const QString& text )
{
	this->QCallBack(pos,qUtf8Printable(text));
}

//WARNING!!!! Probably it's useless
//void MainWindow::updateRenderMode( )
//{
//    if ((GLA() == NULL) || (meshDoc() == NULL))
//        return;
//    QMap<int,RenderMode>& rmode = GLA()->rendermodemap;
//
//    RenderModeAction* act = qobject_cast<RenderModeAction*>(sender());
//    RenderModeTexturePerWedgeAction* textact = qobject_cast<RenderModeTexturePerWedgeAction*>(act);
//
//    //act->data contains the meshid to which the action is referred.
//    //if the meshid is -1 the action is intended to be per-document and not per mesh
//    bool isvalidid = true;
//    int meshid = act->data().toInt(&isvalidid);
//    if (!isvalidid)
//        throw MeshLabException("A RenderModeAction contains a non-integer data id.");
//
//    if (meshid == -1)
//    {
//        for(QMap<int,RenderMode>::iterator it =	rmode.begin();it != rmode.end();++it)
//        {
//            RenderMode& rm = it.value();
//            RenderMode old = rm;
//
//            act->updateRenderMode(rm);
//            //horrible trick caused by MeshLab GUI. In MeshLab exists just a button turning on/off the texture visualization.
//            //Unfortunately the RenderMode::textureMode member field is not just a boolean value but and enum one.
//            //The enum-value depends from the enabled attributes of input mesh.
//            if (textact != NULL)
//                setBestTextureModePerMesh(textact,it.key(),rm);
//
//            MeshModel* mmod = meshDoc()->getMesh(it.key());
//            if (mmod != NULL)
//            {
//                throw MeshLabException("A RenderModeAction referred to a non-existent mesh.");
//                act->updateRenderMode(rm);
//                if (textact != NULL)
//                    setBestTextureModePerMesh(textact,it.key(),rm);
//
////                deallocateReqAttsConsideringAllOtherGLArea(GLA(),it.key(),old,rm;)
//                
//                GLA()->setupRequestedAttributesPerMesh(it.key());
//            }
//            else throw MeshLabException("A RenderModeAction referred to a non-existent mesh.");
//
//			GLA()->setupRequestedAttributesPerMesh(it.key());
//        }
//    }
//    else
//    {
//        QMap<int,RenderMode>::iterator it = rmode.find(meshid);
//        RenderMode& rm = it.value();
//        RenderMode old = rm;
//        if (it == rmode.end())
//            throw MeshLabException("A RenderModeAction contains a non-valid data meshid.");
//        MeshModel* mmod = meshDoc()->getMesh(it.key());
//        if (mmod == NULL)
//            throw MeshLabException("A RenderModeAction referred to a non-existent mesh.");
//        act->updateRenderMode(rm);
//        updateMenus();
//        //horrible trick caused by MeshLab GUI. In MeshLab exists just a button turning on/off the texture visualization.
//        //Unfortunately the RenderMode::textureMode member field is not just a boolean value but and enum one.
//        //The enum-value depends from the enabled attributes of input mesh.
//        if (textact != NULL)
///*setBestTextureModePerMesh(textact,meshid,rm)*/;
//
////        deallocateReqAttsConsideringAllOtherGLArea(GLA(),it.key(),old,rm);
//        GLA()->setupRequestedAttributesPerMesh(it.key());
//    }
//    GLA()->update();
//}

//WARNING!!!!!! I suppose it should not be useful anymore, but....
//void MainWindow::setBestTextureModePerMesh(RenderModeAction* textact,const int meshid, RenderMode& rm)
//{
//    MeshModel* mesh = NULL;
//    if ((meshDoc() == NULL) || ((mesh  = meshDoc()->getMesh(meshid)) == NULL))
//    {
//        bool clicked = (textact != NULL) && (textact->isChecked());
//        MLPoliciesStandAloneFunctions::computeRequestedRenderingAttributesCompatibleWithMesh(mesh,rm.pmmask,rm.atts,rm.pmmask,rm.atts);
//        rm.atts[MLRenderingData::ATT_NAMES::ATT_VERTTEXTURE] = rm.atts[MLRenderingData::ATT_NAMES::ATT_VERTTEXTURE] && clicked;
//        rm.atts[MLRenderingData::ATT_NAMES::ATT_WEDGETEXTURE] = rm.atts[MLRenderingData::ATT_NAMES::ATT_WEDGETEXTURE] && clicked;
//    }
//}


void MainWindow::showEvent(QShowEvent * event)
{
	QWidget::showEvent(event);
	QSettings settings;
	QSettings::setDefaultFormat(QSettings::NativeFormat);
	const QString versioncheckeddatestring("lastTimeMeshLabVersionCheckedOnStart");
	QDate today = QDate::currentDate();
	QString todayStr = today.toString();
	if (settings.contains(versioncheckeddatestring))
	{
		QDate lasttimechecked = QDate::fromString(settings.value(versioncheckeddatestring).toString());
		if (lasttimechecked < today)
		{
			checkForUpdates(false);
			settings.setValue(versioncheckeddatestring, todayStr);
		}
	}
	else
	{
		checkForUpdates(false);
		settings.setValue(versioncheckeddatestring, todayStr);
	}
	sendUsAMail();
}

void MainWindow::meshAdded(int mid)
{
	MultiViewer_Container* mvc = currentViewContainer();
	if (mvc != NULL)
	{
		MLSceneGLSharedDataContext* shared = mvc->sharedDataContext();
		if (shared != NULL)
		{
			shared->meshInserted(mid);
			QList<QGLContext*> contlist;
			for(int glarid = 0;glarid < mvc->viewerCounter();++glarid)
			{
				GLArea* ar = mvc->getViewer(glarid);
				if (ar != NULL)
					contlist.push_back(ar->context());
			}
			MLRenderingData defdt;
			if (meshDoc() != NULL)
			{
				MeshModel* mm = meshDoc()->getMesh(mid);
				if (mm != NULL)
				{
					for(int glarid = 0;glarid < mvc->viewerCounter();++glarid)
					{
						GLArea* ar = mvc->getViewer(glarid);
						if (ar != NULL)
							shared->setRenderingDataPerMeshView(mid,ar->context(),defdt);
					}
					shared->manageBuffers(mid);
				}
				//layerDialog->setVisible(meshDoc()->meshList.size() > 0);
				updateLayerDialog();
			}
		}
		
	}
}

void MainWindow::meshRemoved(int mid)
{
	MultiViewer_Container* mvc = currentViewContainer();
	if (mvc != NULL)
	{
		MLSceneGLSharedDataContext* shared = mvc->sharedDataContext();
		if (shared != NULL)
			shared->meshRemoved(mid);
	}
	updateLayerDialog();
}

void MainWindow::getRenderingData( int mid,MLRenderingData& dt) const
{
	if (mid == -1)
	{
		//if (GLA() != NULL)
		//GLA()->getPerDocGlobalRenderingData(dt);
	}
	else
	{
		MultiViewer_Container* cont = currentViewContainer();
		if (cont != NULL)
		{
			MLSceneGLSharedDataContext* share = cont->sharedDataContext();
			if ((share != NULL) && (GLA() != NULL))
				share->getRenderInfoPerMeshView(mid, GLA()->context(), dt);
		}
	}
}

void MainWindow::setRenderingData(int mid,const MLRenderingData& dt)
{
	if (mid == -1)
	{
		/*if (GLA() != NULL)
			GLA()->setPerDocGlobalRenderingData(dt);*/
	}
	else
	{
		MultiViewer_Container* cont = currentViewContainer();
		if (cont != NULL)
		{
			MLSceneGLSharedDataContext* share = cont->sharedDataContext();
			if ((share != NULL) && (GLA() != NULL))
			{
				share->setRenderingDataPerMeshView(mid, GLA()->context(), dt);
				share->manageBuffers(mid);
				//addRenderingSystemLogInfo(mid);
				if (globrendtoolbar != NULL)
				{
					MLSceneGLSharedDataContext::PerMeshRenderingDataMap mp;
					share->getRenderInfoPerMeshView(GLA()->context(), mp);
					globrendtoolbar->statusConsistencyCheck(mp);
				}
			}
		}
	}
}


void MainWindow::addRenderingSystemLogInfo(unsigned mmid)
{
	MultiViewer_Container* cont = currentViewContainer();
	if (cont != NULL)
	{
		MLRenderingData::DebugInfo deb;
		MLSceneGLSharedDataContext* share = cont->sharedDataContext();
		if ((share != NULL) && (GLA() != NULL))
		{
			share->getLog(mmid,deb);
			MeshModel* mm = meshDoc()->getMesh(mmid);
			if (mm != NULL)
			{
				QString data = QString(deb._currentlyallocated.c_str()) + "\n" + QString(deb._tobedeallocated.c_str()) + "\n" + QString(deb._tobeallocated.c_str()) + "\n" + QString(deb._tobeupdated.c_str()) + "\n";
				for(std::vector<std::string>::iterator it = deb._perviewdata.begin();it != deb._perviewdata.end();++it)
					data += QString((*it).c_str()) + "<br>";
				meshDoc()->Log.Log(GLLogStream::SYSTEM, data);
			}
		}
	}
}

void MainWindow::updateRenderingDataAccordingToActionsCommonCode(int meshid, const QList<MLRenderingAction*>& acts)
{
	if (meshDoc() == NULL)
		return;
	
	MLRenderingData olddt;
	getRenderingData(meshid, olddt);
	MLRenderingData dt(olddt);
	foreach(MLRenderingAction* act, acts)
	{
		if (act != NULL)
			act->updateRenderingData(dt);
	}
	MeshModel* mm = meshDoc()->getMesh(meshid);
	if (mm != NULL)
	{
		MLPoliciesStandAloneFunctions::setBestWireModality(mm, dt);
		MLPoliciesStandAloneFunctions::computeRequestedRenderingDataCompatibleWithMeshSameGLOpts(mm, dt, dt);
	}
	setRenderingData(meshid, dt);
	
	/*if (meshid == -1)
	{
		foreach(MeshModel* mm, meshDoc()->meshList)
		{
			if (mm != NULL)
			{
				MLDefaultMeshDecorators dec(this);
				dec.updateMeshDecorationData(*mm, olddt, dt);
			}
		}
	}
	else
	{*/
	if (mm != NULL)
	{
		MLDefaultMeshDecorators dec(this);
		dec.updateMeshDecorationData(*mm, olddt, dt);
	}
	/*}*/
	
}


void MainWindow::updateRenderingDataAccordingToActions(int meshid,const QList<MLRenderingAction*>& acts)
{
	updateRenderingDataAccordingToActionsCommonCode(meshid, acts);
	if (GLA() != NULL)
		GLA()->update();
}

void MainWindow::updateRenderingDataAccordingToActionsToAllVisibleLayers(const QList<MLRenderingAction*>& acts)
{
	if (meshDoc() == NULL)
		return;
	for (int ii = 0; ii < meshDoc()->meshList.size(); ++ii)
	{
		MeshModel* mm = meshDoc()->meshList[ii];
		if ((mm != NULL) && (mm->isVisible()))
		{
			updateRenderingDataAccordingToActionsCommonCode(mm->id(), acts);
		}
	}
	//updateLayerDialog();
	if (GLA() != NULL)
		GLA()->update();
}

void MainWindow::updateRenderingDataAccordingToActions(int /*meshid*/, MLRenderingAction* act, QList<MLRenderingAction*>& acts)
{
	if ((meshDoc() == NULL) || (act == NULL))
		return;
	
	QList<MLRenderingAction*> tmpacts;
	for (int ii = 0; ii < acts.size(); ++ii)
	{
		if (acts[ii] != NULL)
		{
			MLRenderingAction* sisteract = NULL;
			acts[ii]->createSisterAction(sisteract, NULL);
			sisteract->setChecked(acts[ii] == act);
			tmpacts.push_back(sisteract);
		}
	}
	
	for (int hh = 0; hh < meshDoc()->meshList.size(); ++hh)
	{
		if (meshDoc()->meshList[hh] != NULL)
			updateRenderingDataAccordingToActionsCommonCode(meshDoc()->meshList[hh]->id(), tmpacts);
	}
	
	for (int ii = 0; ii < tmpacts.size(); ++ii)
		delete tmpacts[ii];
	tmpacts.clear();
	
	if (GLA() != NULL)
		GLA()->update();
	
	updateLayerDialog();
}


void MainWindow::updateRenderingDataAccordingToActionCommonCode(int meshid, MLRenderingAction* act)
{
	if ((meshDoc() == NULL) || (act == NULL))
		return;
	
	if (meshid != -1)
	{
		MLRenderingData olddt;
		getRenderingData(meshid, olddt);
		MLRenderingData dt(olddt);
		act->updateRenderingData(dt);
		MeshModel* mm = meshDoc()->getMesh(meshid);
		if (mm != NULL)
		{
			MLPoliciesStandAloneFunctions::setBestWireModality(mm, dt);
			MLPoliciesStandAloneFunctions::computeRequestedRenderingDataCompatibleWithMeshSameGLOpts(mm, dt, dt);
		}
		setRenderingData(meshid, dt);
		if (mm != NULL)
		{
			MLDefaultMeshDecorators dec(this);
			dec.updateMeshDecorationData(*mm, olddt, dt);
		}
	}
}

void MainWindow::updateRenderingDataAccordingToAction( int meshid,MLRenderingAction* act)
{
	updateRenderingDataAccordingToActionCommonCode(meshid, act);
	if (GLA() != NULL)
		GLA()->update();
}

void MainWindow::updateRenderingDataAccordingToActionToAllVisibleLayers(MLRenderingAction* act)
{
	if (meshDoc() == NULL)
		return;
	
	for (int ii = 0; ii < meshDoc()->meshList.size(); ++ii)
	{
		MeshModel* mm = meshDoc()->meshList[ii];
		if ((mm != NULL) && (mm->isVisible()))
		{
			updateRenderingDataAccordingToActionCommonCode(mm->id(), act);
		}
	}
	updateLayerDialog();
	if (GLA() != NULL)
		GLA()->update();
}

void  MainWindow::updateRenderingDataAccordingToActions(QList<MLRenderingGlobalAction*> actlist)
{
	if (meshDoc() == NULL)
		return;
	
	for (int ii = 0; ii < meshDoc()->meshList.size(); ++ii)
	{
		MeshModel* mm = meshDoc()->meshList[ii];
		if (mm != NULL)
		{
			foreach(MLRenderingGlobalAction* act, actlist)
			{
				foreach(MLRenderingAction* ract, act->mainActions())
					updateRenderingDataAccordingToActionCommonCode(mm->id(), ract);
				
				foreach(MLRenderingAction* ract, act->relatedActions())
					updateRenderingDataAccordingToActionCommonCode(mm->id(), ract);
			}
		}
	}
	updateLayerDialog();
	if (GLA() != NULL)
		GLA()->update();
}

void MainWindow::updateRenderingDataAccordingToAction(int /*meshid*/, MLRenderingAction* act, bool check)
{
	MLRenderingAction* sisteract = NULL;
	act->createSisterAction(sisteract, NULL);
	sisteract->setChecked(check);
	foreach(MeshModel* mm, meshDoc()->meshList)
	{
		if (mm != NULL)
			updateRenderingDataAccordingToActionCommonCode(mm->id(), sisteract);
	}
	delete sisteract;
	if (GLA() != NULL)
		GLA()->update();
	updateLayerDialog();
}

bool MainWindow::addRenderingDataIfNewlyGeneratedMesh(int meshid)
{
	MultiViewer_Container* mvc = currentViewContainer();
	if (mvc == NULL)
		return false;
	MLSceneGLSharedDataContext* shared = mvc->sharedDataContext();
	if (shared != NULL)
	{
		MeshModel* mm = meshDoc()->getMesh(meshid);
		if ((meshDoc()->meshDocStateData().find(meshid) == meshDoc()->meshDocStateData().end()) && (mm != NULL))
		{
			MLRenderingData dttoberendered;
			MLPoliciesStandAloneFunctions::suggestedDefaultPerViewRenderingData(mm, dttoberendered,mwsettings.minpolygonpersmoothrendering);
			foreach(GLArea* gla, mvc->viewerList)
			{
				if (gla != NULL)
					shared->setRenderingDataPerMeshView(meshid, gla->context(), dttoberendered);
			}
			shared->manageBuffers(meshid);
			return true;
		}
	}
	return false;
}

unsigned int MainWindow::viewsRequiringRenderingActions(int meshid, MLRenderingAction* act)
{
	unsigned int res = 0;
	MultiViewer_Container* cont = currentViewContainer();
	if (cont != NULL)
	{
		MLSceneGLSharedDataContext* share = cont->sharedDataContext();
		if (share != NULL)
		{
			foreach(GLArea* area,cont->viewerList)
			{
				MLRenderingData dt;
				share->getRenderInfoPerMeshView(meshid, area->context(), dt);
				if (act->isRenderingDataEnabled(dt))
					++res;
			}
		}
	}
	return res;
}

void MainWindow::updateLog()
{
	GLLogStream* senderlog = qobject_cast<GLLogStream*>(sender());
	if ((senderlog != NULL) && (layerDialog != NULL))
		layerDialog->updateLog(*senderlog);
}

void MainWindow::switchCurrentContainer(QMdiSubWindow * subwin)
{
	if (subwin == NULL)
	{
		if (globrendtoolbar != NULL)
			globrendtoolbar->reset();
		return;
	}
	if (mdiarea->currentSubWindow() != 0)
	{
		MultiViewer_Container* split = qobject_cast<MultiViewer_Container*>(mdiarea->currentSubWindow()->widget());
		if (split != NULL)
			_currviewcontainer = split;
	}
	if (_currviewcontainer != NULL)
	{
		updateLayerDialog();
		updateMenus();
		updateStdDialog();
	}
}

void MainWindow::closeCurrentDocument()
{
	_currviewcontainer = NULL;
	layerDialog->setVisible(false);
	if (mdiarea != NULL)
		mdiarea->closeActiveSubWindow();
	updateMenus();
}
