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

#include <common/mlapplication.h>
#include <common/filterscript.h>
#include <common/mlexception.h>
#include <common/globals.h>
#include <common/utilities/load_save.h>

#include "rich_parameter_gui/richparameterlistdialog.h"

#include <wrap/io_trimesh/alnParser.h>
#include "dialogs/about_dialog.h"
#include "dialogs/filter_script_dialog.h"
#include "dialogs/options_dialog.h"
#include "dialogs/plugin_info_dialog.h"
#include "dialogs/save_mesh_attributes_dialog.h"
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
	bool notEmptyActiveDoc = activeDoc && (meshDoc() != NULL) && !(meshDoc()->meshNumber() == 0);
	
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
		
		for (EditPlugin* ep : PM.editPluginFactoryIterator())
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
		for (DecoratePlugin* dp : PM.decoratePluginIterator()){
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
		for (EditPlugin* ep : PM.editPluginFactoryIterator()) {
			for (QAction* a : ep->actions()) {
				a->setEnabled(false);
			}
		}

		for (DecoratePlugin* dp : PM.decoratePluginIterator()){
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
		for (DecoratePlugin* dp : PM.decoratePluginIterator()){
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
		if (layerDialog != NULL) {
			layervis = layerDialog->isVisible();
			showLayerDlg(false);
		}
		for (int i=0, size=url_list.size(); i<size; i++) {
			QString path = url_list.at(i).toLocalFile();
			if( (event->keyboardModifiers () == Qt::ControlModifier ) || ( QApplication::keyboardModifiers () == Qt::ControlModifier )) {
				this->newProject();
			}

			QString extension = QFileInfo(path).suffix();
			if (PM.isInputProjectFormatSupported(extension)){
				openProject(path);
			}
			else if (PM.isInputMeshFormatSupported(extension)){
				importMesh(path);
			}
			else if (PM.isInputImageFormatSupported(extension)){
				importRaster(path);
			}
			else {
				QMessageBox::warning(
						this, "File format not supported",
						extension + " file format is not supported by MeshLab.");
			}
			

		}
		showLayerDlg(layervis || meshDoc()->meshNumber() > 0);
	}
}

void MainWindow::endEdit()
{
	MultiViewer_Container* mvc = currentViewContainer();
	if ((meshDoc() == NULL) || (GLA() == NULL) || (mvc == NULL))
		return;
	
	
	for (const MeshModel& mm : meshDoc()->meshIterator()) {
		addRenderingDataIfNewlyGeneratedMesh(mm.id());
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
	QString filterName;
	try {
		for (FilterNameParameterValuesPair& pair : meshDoc()->filterHistory)
		{
			filterName = pair.filterName();
			int classes = 0;
			unsigned int postCondMask = MeshModel::MM_UNKNOWN;
			QAction *action = PM.filterAction(filterName);
			FilterPlugin *iFilter = qobject_cast<FilterPlugin *>(action->parent());

			int req=iFilter->getRequirements(action);
			if (meshDoc()->mm() != NULL)
				meshDoc()->mm()->updateDataMask(req);
			iFilter->setLog(&meshDoc()->Log);

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


				if (iFilter->filterArity(action) == FilterPlugin::SINGLE_MESH) {
					MLRenderingData::PRIMITIVE_MODALITY pm = MLPoliciesStandAloneFunctions::bestPrimitiveModalityAccordingToMesh(meshDoc()->mm());
					if ((pm != MLRenderingData::PR_ARITY) && (meshDoc()->mm() != nullptr)) {
						dt.set(pm,atts);
						shar->setRenderingDataPerMeshView(meshDoc()->mm()->id(),iFilter->glContext,dt);
					}
				}
				else {
					for(const MeshModel& mm : meshDoc()->meshIterator()) {
						MLRenderingData::PRIMITIVE_MODALITY pm = MLPoliciesStandAloneFunctions::bestPrimitiveModalityAccordingToMesh(&mm);
						if ((pm != MLRenderingData::PR_ARITY)) {
							dt.set(pm,atts);
							shar->setRenderingDataPerMeshView(mm.id(),iFilter->glContext,dt);
						}
					}
				}

			}
			if ((!created) || (!iFilter->glContext->isValid()))
				throw MLException("A valid GLContext is required by the filter to work.\n");
			meshDoc()->setBusy(true);
			iFilter->applyFilter(action, pair.second, *meshDoc(), postCondMask, QCallBack);
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
				if(classes & FilterPlugin::FaceColoring )
				{
					meshDoc()->mm()->updateDataMask(MeshModel::MM_FACECOLOR);
				}
				if(classes & FilterPlugin::VertexColoring )
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
			if (classes & FilterPlugin::MeshCreation)
				newmeshcreated = true;
			updateSharedContextDataAfterFilterExecution(postCondMask, classes, newmeshcreated);
			meshDoc()->meshDocStateData().clear();

			if(classes & FilterPlugin::MeshCreation)
				GLA()->resetTrackBall();
			/* to be changed */

			qb->reset();
			GLA()->update();
			GLA()->Logf(GLLogStream::SYSTEM,"Re-Applied filter %s",qUtf8Printable(pair.filterName()));
			if (_currviewcontainer != NULL)
				_currviewcontainer->updateAllDecoratorsForAllViewers();
		}
	}
	catch(const MLException& exc){
		QMessageBox::warning(
				this,
				tr("Filter Failure"),
				"Failure of filter <font color=red>: '" + filterName + "'</font><br><br>" + exc.what());
		meshDoc()->Log.log(GLLogStream::SYSTEM, filterName + " failed: " + exc.what());
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
	FilterPlugin *iFilter = qobject_cast<FilterPlugin *>(action->parent());
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
			currentViewContainer()->LogPtr()->setBookmark();
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
				if ((mm->hasDataMask(MeshModel::MM_FACECOLOR)) && (fclasses & FilterPlugin::FaceColoring ))
					postcondmask = postcondmask | MeshModel::MM_FACECOLOR;

				if ((mm->hasDataMask(MeshModel::MM_VERTCOLOR)) && (fclasses & FilterPlugin::VertexColoring ))
					postcondmask = postcondmask | MeshModel::MM_VERTCOLOR;

				if ((mm->hasDataMask(MeshModel::MM_COLOR)) && (fclasses & FilterPlugin::MeshColoring ))
					postcondmask = postcondmask | MeshModel::MM_COLOR;

				if ((mm->hasDataMask(MeshModel::MM_FACEQUALITY)) && (fclasses & FilterPlugin::Quality ))
					postcondmask = postcondmask | MeshModel::MM_FACEQUALITY;

				if ((mm->hasDataMask(MeshModel::MM_VERTQUALITY)) && (fclasses & FilterPlugin::Quality ))
					postcondmask = postcondmask | MeshModel::MM_VERTQUALITY;

				MLRenderingData dttoberendered;
				QMap<int,MeshModelStateData>::Iterator existit = meshDoc()->meshDocStateData().find(mm->id());
				if (existit != meshDoc()->meshDocStateData().end())
				{
					shared->getRenderInfoPerMeshView(mm->id(),GLA()->context(),dttoberendered);

					//masks differences bitwise operator (^) -> remove the attributes that didn't apparently change + the ones that for sure changed according to the postCondition function
					//this operation has been introduced in order to minimize problems with filters that didn't declared properly the postCondition mask
					int updatemask = (existit->_mask ^ mm->dataMask()) | postcondmask;
					bool connectivitychanged = false;
					if (((unsigned int)mm->cm.VN() != existit->_nvert) || ((unsigned int)mm->cm.FN() != existit->_nface) ||
							bool(postcondmask & MeshModel::MM_UNKNOWN) || bool(postcondmask & MeshModel::MM_VERTNUMBER) ||
							bool(postcondmask & MeshModel::MM_FACENUMBER) || bool(postcondmask & MeshModel::MM_FACEVERT) ||
							bool(postcondmask & MeshModel::MM_VERTFACETOPO) || bool(postcondmask & MeshModel::MM_FACEFACETOPO))
					{
						connectivitychanged = true;
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
					if (fclasses & FilterPlugin::MeshColoring)
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
					if (!mm->cm.textures.empty())
						updateTexture(mm->id());
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
	FilterPlugin *iFilter = qobject_cast<FilterPlugin *>(action->parent());
	qb->show();
	iFilter->setLog(&meshDoc()->Log);
	
	// Ask for filter requirements (eg a filter can need topology, border flags etc)
	// and satisfy them
	qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
	MainWindow::globalStatusBar()->showMessage("Starting Filter...",5000);
	int req=iFilter->getRequirements(action);
	if (!(meshDoc()->meshNumber() == 0))
		meshDoc()->mm()->updateDataMask(req);
	qApp->restoreOverrideCursor();
	
	// (3) save the current filter and its parameters in the history
	if(!isPreview)
		meshDoc()->Log.clearBookmark();
	else
		meshDoc()->Log.backToBookmark();
	// (4) Apply the Filter
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
		
		if (iFilter->filterArity(action) == FilterPlugin::SINGLE_MESH) {
			MLRenderingData::PRIMITIVE_MODALITY pm = MLPoliciesStandAloneFunctions::bestPrimitiveModalityAccordingToMesh(meshDoc()->mm());
			if ((pm != MLRenderingData::PR_ARITY) && (meshDoc()->mm() != NULL)) {
				dt.set(pm,atts);
				iFilter->glContext->initPerViewRenderingData(meshDoc()->mm()->id(),dt);
			}
		}
		else {
			for(const MeshModel& mm : meshDoc()->meshIterator()) {
				MLRenderingData::PRIMITIVE_MODALITY pm = MLPoliciesStandAloneFunctions::bestPrimitiveModalityAccordingToMesh(&mm);
				if (pm != MLRenderingData::PR_ARITY) {
					dt.set(pm,atts);
					iFilter->glContext->initPerViewRenderingData(mm.id(),dt);
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
		iFilter->applyFilter(action, mergedenvironment, *(meshDoc()), postCondMask, QCallBack);
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
		
		meshDoc()->Log.logf(GLLogStream::SYSTEM,"Applied filter %s in %i msec",qUtf8Printable(action->text()),tt.elapsed());
		if (meshDoc()->mm() != NULL)
			meshDoc()->mm()->setMeshModified();
		MainWindow::globalStatusBar()->showMessage("Filter successfully completed...",2000);
		if(GLA()) {
			GLA()->setLastAppliedFilter(action);
		}
		lastFilterAct->setText(QString("Apply filter ") + action->text());
		lastFilterAct->setEnabled(true);

		
		
		FilterPlugin::FilterArity arity = iFilter->filterArity(action);
		QList<MeshModel*> tmp;
		switch(arity)
		{
		case (FilterPlugin::SINGLE_MESH):
		{
			tmp.push_back(meshDoc()->mm());
			break;
		}
		case (FilterPlugin::FIXED):
		{
			for(const RichParameter& p : mergedenvironment)
			{
				if (p.isOfType<RichMesh>())
				{
					MeshModel* mm = meshDoc()->getMesh(p.value().getInt());
					if (mm != NULL)
						tmp.push_back(mm);
				}
			}
			break;
		}
		case (FilterPlugin::VARIABLE):
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
		
		if(iFilter->getClass(action) & FilterPlugin::MeshCreation )
			GLA()->resetTrackBall();
		
		for(int jj = 0;jj < tmp.size();++jj) {
			MeshModel* mm = tmp[jj];
			if (mm != NULL) {
				// at the end for filters that change the color, or selection set the appropriate rendering mode
				if(iFilter->getClass(action) & FilterPlugin::FaceColoring )
					mm->updateDataMask(MeshModel::MM_FACECOLOR);
				
				if(iFilter->getClass(action) & FilterPlugin::VertexColoring )
					mm->updateDataMask(MeshModel::MM_VERTCOLOR);
				
				if(iFilter->getClass(action) & FilterPlugin::MeshColoring )
					mm->updateDataMask(MeshModel::MM_COLOR);
				
				if(postCondMask & MeshModel::MM_CAMERA)
					mm->updateDataMask(MeshModel::MM_CAMERA);
				
				if(iFilter->getClass(action) & FilterPlugin::Texture )
					updateTexture(mm->id());
			}
		}
		
		int fclasses =	iFilter->getClass(action);
		//MLSceneGLSharedDataContext* sharedcont = GLA()->getSceneGLSharedContext();
		
		updateSharedContextDataAfterFilterExecution(postCondMask,fclasses,newmeshcreated);
		meshDoc()->meshDocStateData().clear();
	}
	catch (const std::bad_alloc& bdall) {
		meshDoc()->setBusy(false);
		qApp->restoreOverrideCursor();
		QMessageBox::warning(
					this, tr("Filter Failure"),
					QString("Operating system was not able to allocate the requested memory.<br><b>"
					"Failure of filter <font color=red>: '%1'</font><br>").arg(action->text())+bdall.what()); // text
		MainWindow::globalStatusBar()->showMessage("Filter failed...",2000);
	}
	catch(const MLException& exc){
		meshDoc()->setBusy(false);
		qApp->restoreOverrideCursor();
		QMessageBox::warning(
				this,
				tr("Filter Failure"),
				"Failure of filter <font color=red>: '" + iFilter->filterName(action) + "'</font><br><br>" + exc.what());
		meshDoc()->Log.log(GLLogStream::SYSTEM, iFilter->filterName(action) + " failed: " + exc.what());
		MainWindow::globalStatusBar()->showMessage("Filter failed...",2000);
	}

	qb->reset();
	layerDialog->setVisible(layerDialog->isVisible() || ((newmeshcreated) && (meshDoc()->meshNumber() > 0)));
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
		EditPlugin *iEditFactory = qobject_cast<EditPlugin *>(action->parent());
		EditTool *iEdit = iEditFactory->getEditTool(action);
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
		GLA()->getRenderer()->finalize(GLA()->getCurrentShaderAction(),meshDoc(),GLA());
		GLA()->setRenderer(NULL,NULL);
	}
	// Make the call to the plugin core
	RenderPlugin *iRenderTemp = qobject_cast<RenderPlugin *>(action->parent());
	bool initsupport = false;
	
	if (currentViewContainer() == NULL)
		return;
	
	MLSceneGLSharedDataContext* shared = currentViewContainer()->sharedDataContext();
	
	if ((shared != NULL) && (iRenderTemp != NULL))
	{
		MLSceneGLSharedDataContext::PerMeshRenderingDataMap rdmap;
		shared->getRenderInfoPerMeshView(GLA()->context(), rdmap);
		iRenderTemp->init(action,*(meshDoc()),rdmap, GLA());
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
			iRenderTemp->finalize(action,meshDoc(),GLA());
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
	
	DecoratePlugin *iDecorateTemp = qobject_cast<DecoratePlugin *>(action->parent());
	
	GLA()->toggleDecorator(iDecorateTemp->decorationName(action));
	
	updateMenus();
	layerDialog->updateDecoratorParsView();
	layerDialog->updateLog(meshDoc()->Log);
	layerDialog->update();
	GLA()->update();
}

std::pair<QString, QString> MainWindow::extractVertFragFileNames(const QDomElement& root)
{
	std::pair<QString, QString> fileNames;
	if (root.nodeName() == tr("GLSLang")) {
		QDomElement elem;

		//Vertex program filename
		elem = root.firstChildElement("VPCount");
		if (!elem.isNull()) {
			//first child of VPCount is "Filenames"
			QDomNode child = elem.firstChild();
			if (!child.isNull()) {
				//first child of "Filenames" is "Filename0"
				child = child.firstChild();
				fileNames.first = (child.toElement()).attribute("VertexProgram", "");
			}
		}

		//Fragment program filename
		elem = root.firstChildElement("FPCount");
		if (!elem.isNull()) {
			//first child of FPCount is "Filenames"
			QDomNode child = elem.firstChild();
			if (!child.isNull()) {
				//first child of "Filenames" is "Filename0"
				child = child.firstChild();
				fileNames.second = (child.toElement()).attribute("FragmentProgram", "");
			}
		}
	}
	return fileNames;
}

/**
 * @brief this function opens a dialog that allows to open gdp files.
 * All the selected files will be copied (along their vert/frag files)
 * inside the extraShadersLocation stored in the local system default app
 * location.
 * This location will be automatically checked by the renderGDP plugin.
 */
void MainWindow::addShaders()
{
	QStringList fileList = QFileDialog::getOpenFileNames(this, "Load Shaders", "", "*GDP Shader File (*.gdp)");
	QString errors;
	for (const QString& fileName : fileList){
		try {
			QFileInfo finfo(fileName);
			QString newGdpFileName = MeshLabApplication::extraShadersLocation() + "/" + finfo.fileName();
			//check if shader already exists
			if (QFile::exists(newGdpFileName)){
				throw MLException(finfo.fileName() + " already exists in " + MeshLabApplication::extraShadersLocation());
			}

			//check vert and frag files
			QFile file(fileName);
			bool openOk = file.open(QIODevice::ReadOnly);
			if (!openOk){
				throw MLException(finfo.fileName() + ": impossible to open file.");
			}
			QDomDocument doc;
			doc.setContent(&file);
			file.close();
			QDomElement root = doc.documentElement();
			std::pair<QString, QString> shaderFiles = extractVertFragFileNames(root);
			if (shaderFiles.first.isEmpty() || shaderFiles.second.isEmpty()){
				throw MLException(finfo.fileName() + ": malformed file: missing VertexProgram and/or FragmentProgram.");
			}
			QString vFilePath = QDir(finfo.absolutePath()).filePath(shaderFiles.first);
			QFileInfo vfinfo(vFilePath);
			if (!vfinfo.exists()){
				throw MLException(finfo.fileName() + ": cannot find VertexProgram " + vfinfo.fileName());
			}
			QString fFilePath = QDir(finfo.absolutePath()).filePath(shaderFiles.second);
			QFileInfo ffinfo(fFilePath);
			if (!ffinfo.exists()){
				throw MLException(finfo.fileName() + ": cannot find FragmentProgram " + ffinfo.fileName());
			}
			QString newVertFileName = MeshLabApplication::extraShadersLocation() + "/" + vfinfo.fileName();
			QString newFragFileName = MeshLabApplication::extraShadersLocation() + "/" + ffinfo.fileName();

			//copy gdp, vert and frag to the extraShadersLocation
			QFile::copy(fileName, newGdpFileName);
			QFile::copy(vFilePath, newVertFileName);
			QFile::copy(fFilePath, newFragFileName);
		}
		catch (const MLException& e){
			errors += QString(e.what()) + "\n";
		}
	}
	if (!errors.isEmpty()){
		QMessageBox::warning(this, "Error while loading GDP", "Error while loading the following GDP files: \n" + errors);
	}

	//refresh actions of render plugins -> needed to update the shaders menu
	for (RenderPlugin* renderPlugin : PM.renderPluginIterator()){
		 renderPlugin->refreshActions();
	}
	fillRenderMenu(); //clean and refill menu
}


/*
Save project. It saves the info of all the layers and the layer themselves. So
*/
void MainWindow::saveProject()
{
	if (meshDoc() == NULL)
		return;

	QFileDialog* saveDiag = new QFileDialog(
				this,
				tr("Save Project File"),
				lastUsedDirectory.path().append(""));
	saveDiag->setNameFilters(PM.outputProjectFormatListDialog());
	saveDiag->setOption(QFileDialog::DontUseNativeDialog);
	QCheckBox* saveAllFilesCheckBox = new QCheckBox(QString("Save All Files"),saveDiag);
	saveAllFilesCheckBox->setCheckState(Qt::Unchecked);
	QCheckBox* onlyVisibleLayersCheckBox = new QCheckBox(QString("Only Visible Layers"),saveDiag);
	onlyVisibleLayersCheckBox->setCheckState(Qt::Unchecked);
	QCheckBox* saveViewStateCheckBox = new QCheckBox(QString("Save View State"), saveDiag);
	saveViewStateCheckBox->setCheckState(Qt::Checked);
	QGridLayout* layout = qobject_cast<QGridLayout*>(saveDiag->layout());
	if (layout != NULL) {
		layout->addWidget(onlyVisibleLayersCheckBox, 4, 0);
		layout->addWidget(saveViewStateCheckBox, 4, 1);
		layout->addWidget(saveAllFilesCheckBox, 4, 2);
	}
	saveDiag->setAcceptMode(QFileDialog::AcceptSave);
	int res = saveDiag->exec();

	if (res == QFileDialog::AcceptSave){
		if (!saveAllFilesCheckBox->isChecked()){
			bool firstNotSaved = true;
			//if a mesh has been created by a create filter we must before to save it.
			//Otherwise the project will refer to a mesh without file name path.
			for(MeshModel& mp : meshDoc()->meshIterator()) {
				if (mp.fullName().isEmpty()) {
					if (firstNotSaved) {
						QMessageBox::information(this, "Layer(s) not saved",
								"Layers must be saved into files before saving the project.\n"
								"Opening save dialog(s) to save the layer(s)...");
						firstNotSaved = false;
					}
					bool saved = exportMesh(tr(""), &mp, false);
					if (!saved) {
						QString msg = "Mesh layer " + mp.label() + " cannot be saved on a file.\nProject \"" + meshDoc()->docLabel() + "\" saving has been aborted.";
						QMessageBox::warning(this,tr("Project Saving Aborted"),msg);
						return;
					}
				}
			}
		}

		QString fileName = saveDiag->selectedFiles().first();
		// this change of dir is needed for subsequent textures/materials loading
		QFileInfo fi(fileName);
		if (fi.suffix().isEmpty()) {
			QRegExp reg("\\.\\w+");
			saveDiag->selectedNameFilter().indexOf(reg);
			QString ext = reg.cap();
			fileName.append(ext);
			fi.setFile(fileName);
		}
		QDir::setCurrent(fi.absoluteDir().absolutePath());

		//save path away so we can use it again
		QString path = fileName;
		path.truncate(path.lastIndexOf("/"));
		lastUsedDirectory.setPath(path);

		std::vector<MLRenderingData> rendData;
		for(const MeshModel& mp : meshDoc()->meshIterator()) {
			MLRenderingData ml;
			getRenderingData(mp.id(), ml);
			rendData.push_back(ml);
		}

		try {
			if (saveAllFilesCheckBox->isChecked()) {
				meshlab::saveAllMeshes(path, *meshDoc(), onlyVisibleLayersCheckBox->isChecked());
			}
			meshlab::saveProject(fileName, *meshDoc(), onlyVisibleLayersCheckBox->isChecked(), rendData);
			/*********WARNING!!!!!! CHANGE IT!!! ALSO IN THE OPENPROJECT FUNCTION********/
			meshDoc()->setDocLabel(fileName);
			QMdiSubWindow* sub = mdiarea->currentSubWindow();
			if (sub != nullptr) {
				sub->setWindowTitle(meshDoc()->docLabel());
				layerDialog->setWindowTitle(meshDoc()->docLabel());
			}
			/****************************************************************************/
		}
		catch (const MLException& e) {
			QMessageBox::critical(
					this, "Meshlab Saving Error",
					"Unable to save project file " + fileName + "\nDetails:\n" + e.what());
		}
	}
}

bool MainWindow::openProject(QString fileName, bool append)
{
	bool visiblelayer = layerDialog->isVisible();
	globrendtoolbar->setEnabled(false);

	if (fileName.isEmpty())
		fileName =
			QFileDialog::getOpenFileName(
					this, tr("Open Project File"),
					lastUsedDirectory.path(), PM.inputProjectFormatListDialog().join(";;"));

	if (fileName.isEmpty())
		return false;

	QFileInfo fi(fileName);
	lastUsedDirectory = fi.absoluteDir();

	QString extension = fi.suffix();
	PluginManager& pm = meshlab::pluginManagerInstance();
	IOPlugin *ioPlugin = pm.inputProjectPlugin(extension);

	if (ioPlugin == nullptr) {
		QMessageBox::critical(this, tr("Meshlab Opening Error"),
				"Project " + fileName + " cannot be loaded. Your MeshLab version "
				"has not plugin to load " + extension + " file format.");
		return false;
	}

	std::list<FileFormat> additionalFilesFormats =
			ioPlugin->projectFileRequiresAdditionalFiles(
				extension, fileName);

	QStringList filenames;
	filenames.push_front(fileName);
	for (const FileFormat& ff : additionalFilesFormats){
		QString currentFilterEntry = ff.description + " (";
		for (QString currentExtension : ff.extensions) {
			currentExtension = currentExtension.toLower();
			currentFilterEntry.append(QObject::tr(" *."));
			currentFilterEntry.append(currentExtension);
		}
		currentFilterEntry.append(')');
		QString additionalFile =
			QFileDialog::getOpenFileName(
					this, tr("Open Additional Project File"),
					lastUsedDirectory.path(), currentFilterEntry);
		if (!additionalFile.isEmpty())
			filenames.push_back(additionalFile);
		else
			return false;
	}

	// Common Part: init a Doc if necessary, and
	bool activeDoc = (bool) !mdiarea->subWindowList().empty() && mdiarea->currentSubWindow();
	bool activeEmpty = activeDoc && ((meshDoc()->meshNumber() == 0));

	if (!activeEmpty && !append)
		newProject(fileName);

	mdiarea->currentSubWindow()->setWindowTitle(fileName);

	meshDoc()->setFileName(fileName);
	meshDoc()->setDocLabel(fileName);

	meshDoc()->setBusy(true);

	// this change of dir is needed for subsequent textures/materials loading
	QDir::setCurrent(fi.absoluteDir().absolutePath());
	qb->show();

	std::vector<MeshModel*> meshList;
	std::vector<MLRenderingData> rendOptions;
	try {
		meshList = meshlab::loadProject(filenames, ioPlugin, *meshDoc(), rendOptions, &meshDoc()->Log, QCallBack);
	}
	catch (const MLException& e) {
		QMessageBox::critical(this, tr("Meshlab Opening Error"), e.what());
		return false;
	}
	QString warningString = ioPlugin->warningMessageString();
	if (!warningString.isEmpty()){
		QMessageBox::warning(this, "Warning", warningString);
	}

	for (unsigned int i = 0; i < meshList.size(); i++){
		MLRenderingData* ptr = nullptr;
		if (rendOptions.size() == meshList.size())
			ptr = &rendOptions[i];
		computeRenderingDataOnLoading(meshList[i], false, ptr);
		if (!(meshList[i]->cm.textures.empty()))
			updateTexture(meshList[i]->id());
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
	showLayerDlg(visiblelayer || (meshDoc()->meshNumber() > 0));
	
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
	
	// Check if we have a doc and if it is empty
	bool activeDoc = (bool) !mdiarea->subWindowList().empty() && mdiarea->currentSubWindow();
	if (!activeDoc || (meshDoc()->meshNumber() == 0))  // it is wrong to try appending to an empty project, even if it is possible
	{
		QMessageBox::critical(this, tr("Meshlab Opening Error"), "Current project is empty, cannot append");
		return false;
	}
	
	meshDoc()->setBusy(true);
	
	// load all projects
	for(QString fileName: fileNameList) {
		openProject(fileName, true);
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
	for (const MeshModel& mm : meshDoc()->meshIterator()) {
		addRenderingDataIfNewlyGeneratedMesh(mm.id());
		updateLayerDialog();
		if (currentViewContainer() != NULL) {
			currentViewContainer()->resetAllTrackBall();
			currentViewContainer()->updateAllViewers();
		}
	}
}

void MainWindow::updateFilterToolBar()
{
	filterToolBar->clear();
	
	for(FilterPlugin *iFilter: PM.filterPluginIterator()) {
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
	if (!GLA()) {
		this->newProject();
		if (!GLA())
			return false;
	}

	QStringList fileNameList;
	if (fileImg.isEmpty())
		fileNameList = QFileDialog::getOpenFileNames(this,tr("Import Mesh"), lastUsedDirectory.path(), PM.inputImageFormatListDialog().join(";;"));
	else
		fileNameList.push_back(fileImg);

	if (fileNameList.isEmpty())	return false;
	else {
		//save path away so we can use it again
		QString path = fileNameList.first();
		path.truncate(path.lastIndexOf("/"));
		lastUsedDirectory.setPath(path);
	}

	QElapsedTimer allFileTime;
	allFileTime.start();

	for(const QString& fileName : fileNameList) {
		RasterModel *rm = nullptr;
		try {
			QElapsedTimer t;
			t.start();
			rm = meshDoc()->addNewRaster();
			meshlab::loadRaster(fileName, *rm, &meshDoc()->Log, QCallBack);
			GLA()->Logf(0, "Opened raster %s in %i msec", qUtf8Printable(fileName), t.elapsed());
			GLA()->resetTrackBall();
			GLA()->fov = meshDoc()->rm()->shot.GetFovFromFocal();
			meshDoc()->rm()->shot = GLA()->shotFromTrackball().first;
			GLA()->resetTrackBall(); // and then we reset the trackball again, to have the standard view
			if (!layerDialog->isVisible())
				layerDialog->setVisible(true);
			GLA()->Logf(0,"All files opened in %i msec",allFileTime.elapsed());
		}
		catch(const MLException& e){
			meshDoc()->delRaster(rm->id());
			QMessageBox::warning(
						this,
						tr("Opening Failure"),
						"While opening: " + fileName + "\n\n" + e.what());
			GLA()->Logf(0, "Warning: Raster %s has not been opened", qUtf8Printable(fileName));
		}
	}// end foreach file of the input list

	if (_currviewcontainer != NULL)
		_currviewcontainer->updateAllDecoratorsForAllViewers();

	updateMenus();
	updateLayerDialog();

	qb->reset();
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
			//count the number of faces contained in the whole meshdocument before the newly inserted mm
			unsigned int totalSumFaces = 0;
			bool found = false;
			for (const MeshModel& m : meshDoc()->meshIterator()){
				if (!found){
					if (&m != mm){
						totalSumFaces += mm->cm.FN();
					}
					else { //found mm: stop counting
						found = true;
					}
				}
			}
			unsigned int minNumberPolFlatShading = mwsettings.minpolygonpersmoothrendering;
			//if the total number of faces is greater than 5*minpolynumber,
			//the newly mesh will be set with flat shading
			if (totalSumFaces > 5 * minNumberPolFlatShading)
				minNumberPolFlatShading = 0;
			MLRenderingData defdt;
			MLPoliciesStandAloneFunctions::suggestedDefaultPerViewRenderingData(mm, defdt, minNumberPolFlatShading);
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
	bool res = importMesh(fileName);
	globrendtoolbar->setEnabled(true);
	if (layerDialog != NULL)
		showLayerDlg(layervisible || meshDoc()->meshNumber());
	setCurrentMeshBestTab();
	return res;
}

// Opening files in a transparent form (IO plugins contribution is hidden to user)
bool MainWindow::importMesh(QString fileName)
{
	if (!GLA()) {
		this->newProject();
		if(!GLA())
			return false;
	}
	
	QStringList fileNameList;
	if (fileName.isEmpty()) {
		fileNameList = QFileDialog::getOpenFileNames(
					this, tr("Import Mesh"),
					lastUsedDirectory.path(),
					PM.inputMeshFormatListDialog().join(";;"));
	}
	else {
		fileNameList.push_back(fileName);
	}
	
	if (fileNameList.isEmpty()) {
		return false;
	}
	else {
		//save path away so we can use it again
		QString path = fileNameList.first();
		path.truncate(path.lastIndexOf("/"));
		lastUsedDirectory.setPath(path);
	}
	
	QElapsedTimer allFileTime;
	allFileTime.start();
	for(const QString& fileName : fileNameList) {
		QFileInfo fi(fileName);
		if(!fi.exists()) {
			QString errorMsgFormat = "Unable to open file:\n\"%1\"\n\nError details: file %1 does not exist.";
			QMessageBox::critical(this, tr("Meshlab Opening Error"), errorMsgFormat.arg(fileName));
			return false;
		}
		if(!fi.isReadable()) {
			QString errorMsgFormat = "Unable to open file:\n\"%1\"\n\nError details: file %1 is not readable.";
			QMessageBox::critical(this, tr("Meshlab Opening Error"), errorMsgFormat.arg(fileName));
			return false;
		}

		QString extension = fi.suffix();
		IOPlugin *pCurrentIOPlugin = PM.inputMeshPlugin(extension);

		if (pCurrentIOPlugin == nullptr) {
			QString errorMsgFormat("Unable to open file:\n\"%1\"\n\nError details: file format " + extension + " not supported.");
			QMessageBox::critical(this, tr("Meshlab Opening Error"), errorMsgFormat.arg(fileName));
			return false;
		}
		
		pCurrentIOPlugin->setLog(&meshDoc()->Log);
		RichParameterList prePar = pCurrentIOPlugin->initPreOpenParameter(extension);
		if(!prePar.isEmpty()) {
			// take the default values of the plugin and overwrite to the settings
			// default values
			for (RichParameter& p : prePar){
				QString prefixName = "MeshLab::IO::" + extension.toUpper() + "::";
				if (currentGlobalParams.hasParameter(prefixName + p.name())){
					const RichParameter& cp = currentGlobalParams.getParameterByName(prefixName + p.name());
					p.setValue(cp.value());
				}
			}

			bool showPreOpenParameterDialog = true;
			QString showDialogParamName = "MeshLab::IO::" + extension.toUpper() + "::showPreOpenParameterDialog";
			if (currentGlobalParams.hasParameter(showDialogParamName)){
				showPreOpenParameterDialog = currentGlobalParams.getBool(showDialogParamName);
			}
			// the user wants to see each time the pre parameters dialog:
			if (showPreOpenParameterDialog){
				RichParameterListDialog preOpenDialog(this, prePar, tr("Pre-Open Options"));
				preOpenDialog.addVerticalSpacer();

				QString cbDoNotShow = "Do not show this dialog next time";
				preOpenDialog.addCheckBox(cbDoNotShow, false);
				QString cbRememberOptions = "Remember these values for the next time";
				preOpenDialog.addCheckBox(cbRememberOptions, true);
				preOpenDialog.setFocus();
				int res = preOpenDialog.exec();
				if (res == QDialog::Accepted){
					if (preOpenDialog.isCheckBoxChecked(cbDoNotShow)){
						RichBool rp(showDialogParamName, false, "", "");
						if (currentGlobalParams.hasParameter(showDialogParamName))
							currentGlobalParams.setValue(rp.name(), BoolValue(false));
						else
							currentGlobalParams.addParam(rp);
						QSettings settings;
						QDomDocument doc("MeshLabSettings");
						doc.appendChild(rp.fillToXMLDocument(doc));
						QString docstring =  doc.toString();
						settings.setValue(rp.name(), QVariant(docstring));
					}
					if (preOpenDialog.isCheckBoxChecked(cbRememberOptions)){
						QSettings settings;
						for (const RichParameter& p : prePar){

							QString prefixName = "MeshLab::IO::" + extension.toUpper() + "::";
							RichParameter& cp = currentGlobalParams.getParameterByName(prefixName + p.name());
							cp.setValue(p.value());
							RichParameter* pp = p.clone();
							pp->setName(prefixName + p.name());
							QDomDocument doc("MeshLabSettings");
							doc.appendChild(pp->fillToXMLDocument(doc));
							QString docstring =  doc.toString();
							settings.setValue(pp->name(), QVariant(docstring));
							delete pp;
						}
					}
				}
				else {
					return false;
				}
			}
		}

		meshDoc()->setBusy(true);
		qApp->setOverrideCursor(QCursor(Qt::WaitCursor));

		//check how many meshes are going to be loaded from the file
		unsigned int nMeshes = pCurrentIOPlugin->numberMeshesContainedInFile(extension, fileName, prePar);

		QFileInfo info(fileName);
		std::list<MeshModel*> meshList;
		for (unsigned int i = 0; i < nMeshes; i++) {
			MeshModel *mm = meshDoc()->addNewMesh(fileName, info.fileName());
			if (nMeshes != 1) {
				// if the file contains more than one mesh, this id will be
				// != -1
				mm->setIdInFile(i);
			}
			meshList.push_back(mm);
		}
		qb->show();
		std::list<int> masks;
		try {
			QElapsedTimer t;
			t.start();
			std::list<std::string> unloadedTextures =
					meshlab::loadMesh(fileName, pCurrentIOPlugin, prePar, meshList, masks, QCallBack);
			saveRecentFileList(fileName);
			updateLayerDialog();
			for (MeshModel* mm : meshList) {
				computeRenderingDataOnLoading(mm, false, nullptr);
				if (! (mm->cm.textures.empty()))
					updateTexture(mm->id());
			}
			QString warningString = pCurrentIOPlugin->warningMessageString();
			if (unloadedTextures.size() > 0){
				warningString += "\n\nThe following textures have not been loaded: \n";
				for (const std::string& txt : unloadedTextures)
					warningString += QString::fromStdString(txt) + "\n";
			}
			if (!warningString.isEmpty()){
				QMessageBox::warning(this, "Meshlab Opening Warning", warningString);
			}
			GLA()->Logf(0, "Opened mesh %s in %i msec", qUtf8Printable(fileName), t.elapsed());
		}
		catch (const MLException& e){
			for (const MeshModel* mm : meshList)
				meshDoc()->delMesh(mm->id());
			GLA()->Logf(0, "Error: File %s has not been loaded", qUtf8Printable(fileName));
			QMessageBox::critical(this, "Meshlab Opening Error", e.what());
		}
		meshDoc()->setBusy(false);
		qApp->restoreOverrideCursor();
	}// end foreach file of the input list
	GLA()->Logf(0,"All files opened in %i msec",allFileTime.elapsed());
	
	if (_currviewcontainer != NULL) {
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

void MainWindow::reloadAllMesh()
{
	// Discards changes and reloads current file
	// save current file name
	qb->show();
	QElapsedTimer t;
	t.start();
	MeshDocument* md = meshDoc();
	md->setBusy(true);
	qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
	for(MeshModel& mmm : md->meshIterator()) {
		if (mmm.idInFile() <= 0){
			QString fileName = mmm.fullName();
			if (!fileName.isEmpty()){
				std::list<MeshModel* > meshList = meshDoc()->getMeshesLoadedFromSameFile(mmm);
				std::vector<bool> isReload(meshList.size(), true);
				unsigned int i = 0;
				for (MeshModel* m : meshList) {
					if (m->cm.VN() == 0)
						isReload[i] = false;
					i++;
				}
				try {
					meshlab::reloadMesh(fileName, meshList, &meshDoc()->Log, QCallBack);
					for (MeshModel* m : meshList){
						computeRenderingDataOnLoading(m, true, nullptr);
					}
				}
				catch (const MLException& e) {
					QMessageBox::critical(this, "Reload Error", e.what());
				}
			}
		}
	}
	md->setBusy(false);
	qApp->restoreOverrideCursor();
	GLA()->Log(0, ("All meshes reloaded in " + std::to_string(t.elapsed()) + " msec.").c_str());
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
	if (fileName.isEmpty()) {
		QMessageBox::critical(this, "Reload Error", "Impossible to reload an unsaved mesh model!!");
		return;
	}

	meshDoc()->setBusy(true);
	qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
	std::list<MeshModel*> meshList = meshDoc()->getMeshesLoadedFromSameFile(*meshDoc()->mm());
	std::vector<bool> isReload(meshList.size(), true);
	unsigned int i = 0;
	for (MeshModel* m : meshList){
		if (m->cm.VN() == 0)
			isReload[i] = false;
		i++;
	}

	try {
		QElapsedTimer t;
		t.start();
		meshlab::reloadMesh(fileName, meshList, &meshDoc()->Log, QCallBack);
		for (MeshModel* m : meshList){
			computeRenderingDataOnLoading(m, true, nullptr);
		}
		GLA()->Log(0, ("File reloaded in " + std::to_string(t.elapsed()) + " msec.").c_str());
	}
	catch (const MLException& e) {
		QMessageBox::critical(this, "Reload Error", e.what());
	}
	meshDoc()->setBusy(false);
	qApp->restoreOverrideCursor();
	qb->reset();
	if (_currviewcontainer != NULL) {
		_currviewcontainer->updateAllDecoratorsForAllViewers();
		_currviewcontainer->updateAllViewers();
	}
}

bool MainWindow::exportMesh(QString fileName,MeshModel* mod,const bool saveAllPossibleAttributes)
{
	const QStringList& suffixList = PM.outputMeshFormatListDialog();
	if (fileName.isEmpty()) {
		//QHash<QString, MeshIOInterface*> allKnownFormats;
		//PM.LoadFormats( suffixList, allKnownFormats,PluginManager::EXPORT);
		//QString defaultExt = "*." + mod->suffixName().toLower();
		QString defaultExt = "*.ply";
		if (mod == NULL)
			return false;
		mod->setMeshModified(false);
		QString laylabel = "Save \"" + mod->label() + "\" Layer";
		QFileDialog* saveDialog = new QFileDialog(this,laylabel, lastUsedDirectory.path());
		//saveDialog->setOption(QFileDialog::DontUseNativeDialog);
		saveDialog->setNameFilters(suffixList);
		saveDialog->setAcceptMode(QFileDialog::AcceptSave);
		saveDialog->setFileMode(QFileDialog::AnyFile);
		saveDialog->selectFile(fileName);
		QStringList matchingExtensions=suffixList.filter(defaultExt);
		if(!matchingExtensions.isEmpty())
			saveDialog->selectNameFilter(matchingExtensions.last());
		//connect(saveDialog,SIGNAL(filterSelected(const QString&)),this,SLOT(changeFileExtension(const QString&)));

		saveDialog->selectFile(meshDoc()->mm()->fullName());
		int dialogRet = saveDialog->exec();
		if(dialogRet==QDialog::Rejected)
			return false;
		fileName=saveDialog->selectedFiles().at(0);
		QFileInfo fni(fileName);
		if(fni.suffix().isEmpty()) {
			QString ext = saveDialog->selectedNameFilter();
			ext.chop(1); ext = ext.right(4);
			fileName = fileName + ext;
			qDebug("File without extension adding it by hand '%s'", qUtf8Printable(fileName));
		}
	}
	QFileInfo fi(fileName);

	QStringList fs = fileName.split(".");

	if(!fileName.isEmpty() && fs.size() < 2) {
		QMessageBox::warning(this,"Save Error","You must specify file extension!!");
		return false;
	}

	bool saved = true;
	if (!fileName.isEmpty()) {
		//save path away so we can use it again
		QString path = fileName;
		path.truncate(path.lastIndexOf("/"));
		lastUsedDirectory.setPath(path);

		QString extension = fileName;
		extension.remove(0, fileName.lastIndexOf('.')+1);

		QStringListIterator itFilter(suffixList);

		IOPlugin *pCurrentIOPlugin = PM.outputMeshPlugin(extension);
		if (pCurrentIOPlugin == 0) {
			QMessageBox::warning(this, "Unknown type", "File extension not supported!");
			return false;
		}
		pCurrentIOPlugin->setLog(&meshDoc()->Log);

		int capability=0,defaultBits=0;
		pCurrentIOPlugin->exportMaskCapability(extension,capability,defaultBits);

		// optional saving parameters (like ascii/binary encoding)
		RichParameterList savePar = pCurrentIOPlugin->initSaveParameter(extension,*(mod));

		SaveMeshAttributesDialog maskDialog(this, mod, capability, defaultBits, savePar, this->GLA());
		int quality = -1;
		bool saveTextures = true;
		if (!saveAllPossibleAttributes) {
			maskDialog.exec();
		}
		else {
			//this is horrible: creating a dialog object but then not showing the
			//dialog.. And using it just to select all the possible options..
			//to be removed soon
			maskDialog.selectAllPossibleBits();
		}
		int mask = maskDialog.getNewMask();
		savePar = maskDialog.getNewAdditionalSaveParameters();
		quality = maskDialog.getTextureQuality();
		saveTextures = maskDialog.saveTextures();

		if (!saveTextures){
			std::vector<std::string> textureNames = maskDialog.getTextureNames();

			for (unsigned int i = 0; i < mod->cm.textures.size(); ++i){
				if (textureNames[i].find('.') == std::string::npos){
					textureNames[i] += ".png";
				}
				mod->changeTextureName(mod->cm.textures[i], textureNames[i]);
			}
		}
		if (!saveAllPossibleAttributes) {
			maskDialog.close();
			if(maskDialog.result() == QDialog::Rejected)
				return false;
		}
		if(mask == -1)
			return false;

		qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
		qb->show();
		QElapsedTimer tt; tt.start();
		meshDoc()->setBusy(true);
		try {
			if (mask & vcg::tri::io::Mask::IOM_BITPOLYGONAL)
				mod->updateDataMask(MeshModel::MM_FACEFACETOPO);
			pCurrentIOPlugin->save(extension, fileName, *mod ,mask,savePar,QCallBack);
			QFileInfo finfo(fileName);
			if (saveTextures)
				mod->saveTextures(finfo.absolutePath(), quality, &meshDoc()->Log, QCallBack);
			GLA()->Logf(GLLogStream::SYSTEM, "Saved Mesh %s in %i msec", qUtf8Printable(fileName), tt.elapsed());
			mod->setFileName(fileName);
			QSettings settings;
			int savedMeshCounter = settings.value("savedMeshCounter", 0).toInt();
			settings.setValue("savedMeshCounter", savedMeshCounter + 1);
		}
		catch(const MLException& e) {
			GLA()->Logf(GLLogStream::SYSTEM, "Error Saving Mesh %s", qUtf8Printable(fileName));
			QMessageBox::critical(this, tr("Meshlab Saving Error"),  e.what());
			saved = false;
		}
		qApp->restoreOverrideCursor();
		updateLayerDialog();
		meshDoc()->setBusy(false);
		qb->reset();

		if (saved)
			QDir::setCurrent(fi.absoluteDir().absolutePath()); //set current dir
	}
	return saved;
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
	for (MeshLabPlugin* pf : PM.pluginIterator(true)){
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

void MainWindow::showInfoPane()
{
	if(GLA() != 0) {
		GLA()->infoAreaVisible = !(GLA()->infoAreaVisible);
		GLA()->update();
	}
}

void MainWindow::showTrackBall()
{
	if(GLA() != 0)
		GLA()->showTrackBall(!GLA()->isTrackBallVisible());
}

void MainWindow::resetTrackBall()
{
	if(GLA() != 0)
		GLA()->resetTrackBall();
}

void MainWindow::showRaster()
{
	if(GLA() != 0)
		GLA()->showRaster((QApplication::keyboardModifiers () & Qt::ShiftModifier));
}

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

	QString cwd = QDir::currentPath();
	QDir::setCurrent(mymesh->pathName());

	shared->deAllocateTexturesPerMesh(mymesh->id());
	
	int textmemMB = int(mwsettings.maxTextureMemory / ((float) 1024 * 1024));
	
	size_t totalTextureNum = 0;
	for (const MeshModel& mp : meshDoc()->meshIterator())
		totalTextureNum+=mp.cm.textures.size();
	
	int singleMaxTextureSizeMpx = int(textmemMB/((totalTextureNum != 0)? totalTextureNum : 1));

	bool sometextnotfound = false;
	for(const std::string& textname : mymesh->cm.textures)
	{
		QImage img = mymesh->getTexture(textname);

		if (img.isNull()){
			img.load(":/images/dummy.png");
		}
		GLuint textid = shared->allocateTexturePerMesh(meshid,img,singleMaxTextureSizeMpx);
		
		for(int tt = 0;tt < mvc->viewerCounter();++tt)
		{
			GLArea* ar = mvc->getViewer(tt);
			if (ar != NULL)
				ar->setupTextureEnv(textid);
		}
	}
	if (sometextnotfound)
		QMessageBox::warning(this,"Texture error", "Some texture has not been found. Using dummy texture.");

	QDir::setCurrent(cwd);
}

void MainWindow::updateProgressBar( const int pos,const QString& text )
{
	this->QCallBack(pos,qUtf8Printable(text));
}

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
				meshDoc()->Log.log(GLLogStream::SYSTEM, data);
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
	for (const MeshModel& mm : meshDoc()->meshIterator()) {
		if (mm.isVisible()) {
			updateRenderingDataAccordingToActionsCommonCode(mm.id(), acts);
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
	
	for (const MeshModel& mm : meshDoc()->meshIterator()) {
		updateRenderingDataAccordingToActionsCommonCode(mm.id(), tmpacts);
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
	
	for (const MeshModel& mm : meshDoc()->meshIterator()) {
		if (mm.isVisible()) {
			updateRenderingDataAccordingToActionCommonCode(mm.id(), act);
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
	
	for (const MeshModel& mm : meshDoc()->meshIterator())
	{
		foreach(MLRenderingGlobalAction* act, actlist) {
			foreach(MLRenderingAction* ract, act->mainActions())
				updateRenderingDataAccordingToActionCommonCode(mm.id(), ract);

			foreach(MLRenderingAction* ract, act->relatedActions())
				updateRenderingDataAccordingToActionCommonCode(mm.id(), ract);
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
	for(const MeshModel& mm : meshDoc()->meshIterator()) {
		updateRenderingDataAccordingToActionCommonCode(mm.id(), sisteract);
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
