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
#include "plugindialog.h"
#include "filterScriptDialog.h"
#include "customDialog.h"
#include "saveSnapshotDialog.h"
#include "ui_aboutDialog.h"
#include "savemaskexporter.h"
#include "alnParser.h"
#include <exception>
#include "xmlgeneratorgui.h"
#include "filterthread.h"
#include "ml_default_decorators.h"

#include <QToolBar>
#include <QToolTip>
#include <QStatusBar>
#include <QMenuBar>
#include <QProgressBar>
#include <QDesktopServices>



#include "../common/scriptinterface.h"
#include "../common/meshlabdocumentxml.h"
#include "../common/meshlabdocumentbundler.h"
#include "../common/mlapplication.h"
#include "../common/filterscript.h"


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

void MainWindow::createXMLStdPluginWnd()
{
    //checks if a MeshlabStdDialog is already open and closes it
    if (xmldialog!=0){
        xmldialog->close();
        delete xmldialog;
    }
    xmldialog = new MeshLabXMLStdDialog(this);
    connect(xmldialog,SIGNAL(filterParametersEvaluated(const QString&,const QMap<QString,QString>&)),meshDoc()->filterHistory,SLOT(addExecutedXMLFilter(const QString&,const QMap<QString,QString>& )));
    //connect(xmldialog,SIGNAL(dialogEvaluateExpression(const Expression&,Value**)),this,SLOT(evaluateExpression(const Expression&,Value**)),Qt::DirectConnection);
    xmldialog->setAllowedAreas (  Qt::NoDockWidgetArea);
    //addDockWidget(Qt::RightDockWidgetArea,xmldialog);
    //stddialog->setAttribute(Qt::WA_DeleteOnClose,true);
    xmldialog->setFloating(true);
    xmldialog->hide();
    connect(GLA(),SIGNAL(glareaClosed()),this,SLOT(updateStdDialog()));
    connect(GLA(),SIGNAL(glareaClosed()),xmldialog,SLOT(closeClick()));
    //connect(GLA(),SIGNAL(glareaClosed()),xmldialog,SLOT(close()));
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

// When we switch the current model (and we change the active window)
// we have to close the stddialog.
// this one is called when user switch current window.
void MainWindow::updateXMLStdDialog()
{
    if(xmldialog!=0){
        if(GLA()!=0){
            if(xmldialog->curModel != meshDoc()->mm()){
                xmldialog->resetPointers(); // invalidate the curgla member that is no more valid.
                xmldialog->close();
            }
        }
    }
}

void MainWindow::updateCustomSettings()
{
    mwsettings.updateGlobalParameterSet(currentGlobalParams);
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

        // View From File act
        windowsMenu->addAction(viewFromFileAct);
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
        GLA()->update();
    }
}

void MainWindow::updateLayerDialog()
{
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
        layerDialog->updateTable(dtf);
        layerDialog->updateLog(meshDoc()->Log);
        layerDialog->updateDecoratorParsView();
        MLRenderingData dt;
        MeshModel* mm = meshDoc()->mm();
        if ((meshDoc()!=NULL) && (meshDoc()->mm() != NULL))
        {
            MLSceneGLSharedDataContext::PerMeshRenderingDataMap::iterator it = dtf.find(meshDoc()->mm()->id());
            layerDialog->updateRenderingParametersTab(meshDoc()->mm()->id(),*it);         
        }
    }
}

void MainWindow::updateMenus()
{
    bool activeDoc = (bool) !mdiarea->subWindowList().empty() && mdiarea->currentSubWindow();
    bool notEmptyActiveDoc = activeDoc && !meshDoc()->meshList.empty();
    importMeshAct->setEnabled(activeDoc);

    exportMeshAct->setEnabled(notEmptyActiveDoc);
    exportMeshAsAct->setEnabled(notEmptyActiveDoc);
    reloadMeshAct->setEnabled(notEmptyActiveDoc);
    reloadAllMeshAct->setEnabled(notEmptyActiveDoc);
    importRasterAct->setEnabled(activeDoc);

    saveProjectAsAct->setEnabled(activeDoc);
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
    renderMenu->setEnabled(!editMenu->actions().isEmpty());
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

        foreach (QAction *a,PM.editActionList)
        {
            a->setChecked(false);
            a->setEnabled( GLA()->getCurrentEditAction() == NULL );
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

//WARNING!!!! It may be useful
/* RenderMode rendtmp;
if (meshDoc()->meshList.size() > 0)
{
    QMap<int,RenderMode>::iterator it = GLA()->rendermodemap.find(meshDoc()->meshList[0]->id());
    if (it == GLA()->rendermodemap.end())
        throw MeshLabException("Something really bad happened. Mesh id has not been found in rendermodemap.");
    rendtmp = it.value();
}
bool checktext = (rendtmp.textureMode != GLW::TMNone);
int ii = 0;
while(ii < meshDoc()->meshList.size())
{
    if (meshDoc()->meshList[ii] == NULL)
        return;
    QMap<int,RenderMode>::iterator it = GLA()->rendermodemap.find(meshDoc()->meshList[ii]->id());
    if (it == GLA()->rendermodemap.end())
        throw MeshLabException("Something really bad happened. Mesh id has not been found in rendermodemap.");
    RenderMode& rm = it.value();
    if (rendtmp.drawMode != rm.drawMode)
        rendtmp.setDrawMode(vcg::GLW::DMNone);

    if (rendtmp.colorMode != rm.colorMode)
        rendtmp.setColorMode(vcg::GLW::CMNone);

    checktext &= (rm.textureMode != GLW::TMNone);

    rendtmp.setLighting(rendtmp.lighting && rm.lighting);
    rendtmp.setFancyLighting(rendtmp.fancyLighting && rm.fancyLighting);
    rendtmp.setDoubleFaceLighting(rendtmp.doubleSideLighting && rm.doubleSideLighting);
    rendtmp.setBackFaceCull(rendtmp.backFaceCull || rm.backFaceCull);
    ++ii;
}*/
/////////////////////////////

        // Decorator Menu Checking and unChecking
        // First uncheck and disable all the decorators
        foreach (QAction *a, PM.decoratorActionList)
        {
            a->setChecked(false);
            a->setEnabled(true);
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
        foreach (QAction *a,PM.editActionList)
        {
            a->setEnabled(false);
        }
        foreach (QAction *a,PM.decoratorActionList)
            a->setEnabled(false);

    }

    if(GLA())
    {
        showLayerDlgAct->setChecked(layerDialog->isVisible());
        showRasterAct->setEnabled(meshDoc()->rm() != 0);
        showRasterAct->setChecked(GLA()->isRaster());
    }
    else
    {
        foreach (QAction *a,PM.decoratorActionList)
        {
            a->setChecked(false);
            a->setEnabled(false);
        }

        
        layerDialog->setVisible(false);
    }
    if (searchMenu != NULL)
        searchMenu->searchLineWidth() = longestActionWidthInAllMenus();
}

void MainWindow::setSplit(QAction *qa)
{
    MultiViewer_Container *mvc = currentViewContainer();
    if(mvc)
    {
        GLArea *glwClone=new GLArea(this, mvc, &currentGlobalParams);
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

        //the viewer on top can be closed only if the splitter over the handle that orginated the event has one child
        bool unSplittabilityUp = true;
        Splitter * upSplitter = qobject_cast<Splitter *>(origin->widget(0));
        if(upSplitter)
            unSplittabilityUp = !(upSplitter->count()>1);
        unsplitUpAct->setEnabled(unSplittabilityUp);

        //check if the viewer below is splittable according to its size
        pickingId = mvc->getViewerByPicking(QPoint(point.x(), point.y()+epsilon));
        if(pickingId>=0)
            splitDownAct->setEnabled(mvc->getViewer(pickingId)->size().width()/2 > mvc->getViewer(pickingId)->minimumSizeHint().width());

        //the viewer below can be closed only if the splitter ounder the handle that orginated the event has one child
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

        //the viewer on the rigth can be closed only if the splitter on the right the handle that orginated the event has one child
        bool unSplittabilityRight = true;
        Splitter * rightSplitter = qobject_cast<Splitter *>(origin->widget(1));
        if(rightSplitter)
            unSplittabilityRight = !(rightSplitter->count()>1);
        unsplitRightAct->setEnabled(unSplittabilityRight);

        //check if the viewer on the left is splittable according to its size
        pickingId =mvc->getViewerByPicking(QPoint(point.x()-epsilon, point.y()));
        if(pickingId>=0)
            splitLeftAct->setEnabled(mvc->getViewer(pickingId)->size().height()/2 > mvc->getViewer(pickingId)->minimumSizeHint().height());

        //the viewer on the left can be closed only if the splitter on the left of the handle that orginated the event has one child
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

void MainWindow::viewFrom(QAction *qa)
{
    if(GLA()) GLA()->createOrthoView(qa->text());
}

void MainWindow::readViewFromFile()
{
    if(GLA()) GLA()->viewFromFile();
    updateMenus();
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

            if(path.endsWith("mlp",Qt::CaseInsensitive) || path.endsWith("aln",Qt::CaseInsensitive) || path.endsWith("out",Qt::CaseInsensitive) || path.endsWith("nvm",Qt::CaseInsensitive) )
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
    GLA()->endEdit();
}
void MainWindow::applyLastFilter()
{
    if(GLA()==0) return;
    GLA()->getLastAppliedFilter()->activate(QAction::Trigger);
}
void MainWindow::showFilterScript()
{
    if (meshDoc()->filterHistory != NULL)
    {
        FilterScriptDialog dialog(this);

        dialog.setScript(meshDoc()->filterHistory);
        if (dialog.exec()==QDialog::Accepted)
        {
            runFilterScript();
            return ;
        }
    }
}

void MainWindow::runFilterScript()
{
    if ((meshDoc() == NULL) || (meshDoc()->filterHistory == NULL))
        return;
    for(FilterScript::iterator ii= meshDoc()->filterHistory->filtparlist.begin();ii!= meshDoc()->filterHistory->filtparlist.end();++ii)
    {
        QString filtnm = (*ii)->filterName();
        int classes = 0;
        if (!(*ii)->isXMLFilter())
        {
            QAction *action = PM.actionFilterMap[ filtnm];
            MeshFilterInterface *iFilter = qobject_cast<MeshFilterInterface *>(action->parent());

            int req=iFilter->getRequirements(action);
            if (meshDoc()->mm() != NULL)
                meshDoc()->mm()->updateDataMask(req);
            iFilter->setLog(&meshDoc()->Log);
            OldFilterNameParameterValuesPair* old = reinterpret_cast<OldFilterNameParameterValuesPair*>(*ii);
            RichParameterSet &parameterSet = old->pair.second;

            for(int i = 0; i < parameterSet.paramList.size(); i++)
            {
                //get a modifieable reference
                RichParameter* parameter = parameterSet.paramList[i];

                //if this is a mesh paramter and the index is valid
                if(parameter->val->isMesh())
                {
                    RichMesh* md = reinterpret_cast<RichMesh*>(parameter);
                    if(	md->meshindex < meshDoc()->size() &&
                        md->meshindex >= 0  )
                    {
                        RichMesh* rmesh = new RichMesh(parameter->name,md->meshindex,meshDoc());
                        parameterSet.paramList.replace(i,rmesh);
                    } else
                    {
                        printf("Meshes loaded: %i, meshes asked for: %i \n", meshDoc()->size(), md->meshindex );
                        printf("One of the filters in the script needs more meshes than you have loaded.\n");
                        return;
                    }
                    delete parameter;
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

                
                if (iFilter->filterArity(action) == MeshFilterInterface::SINGLE_MESH)
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
            iFilter->applyFilter( action, *meshDoc(), old->pair.second, QCallBack );
            meshDoc()->setBusy(false);
            if (shar != NULL)
                shar->removeView(iFilter->glContext); 
            delete iFilter->glContext;
            classes = int(iFilter->getClass(action));
        }
        else
        {
            MeshLabXMLFilterContainer& cont = PM.stringXMLFilterMap[ filtnm];
            MLXMLPluginInfo* info = cont.xmlInfo;
            MeshLabFilterInterface* cppfilt = cont.filterInterface;
            try
            {
                if (cppfilt != NULL)
                {
                    cppfilt->setLog(&meshDoc()->Log);

                    Env env ;
                    QScriptValue val = env.loadMLScriptEnv(*meshDoc(),PM,this->currentGlobalPars());
                    XMLFilterNameParameterValuesPair* xmlfilt = reinterpret_cast<XMLFilterNameParameterValuesPair*>(*ii);
                    QMap<QString,QString>& parmap = xmlfilt->pair.second;
                    for(QMap<QString,QString>::const_iterator it = parmap.constBegin();it != parmap.constEnd();++it)
                        env.insertExpressionBinding(it.key(),it.value());

                    EnvWrap envwrap(env);
                    MLXMLPluginInfo::XMLMapList params = info->filterParameters(filtnm);
                    for(int i = 0; i < params.size(); ++i)
                    {
                        MLXMLPluginInfo::XMLMap& parinfo = params[i];

                        //if this is a mesh parameter and the index is valid
                        if(parinfo[MLXMLElNames::paramType]  == MLXMLElNames::meshType)
                        {
                            QString& parnm = parinfo[MLXMLElNames::paramName];
                            MeshModel* meshmdl = envwrap.evalMesh(parnm);
                            if( meshmdl == NULL)
                            {
                                //parnm is associated with ,
                                printf("Meshes loaded: %i, meshes asked for: %i \n", meshDoc()->size(), envwrap.evalInt(parnm) );
                                printf("One of the filters in the script needs more meshes than you have loaded.\n");
                                return;
                            }
                        }
                    }
                    disconnect(meshDoc(),SIGNAL(documentUpdated()),GLA(),SLOT(completeUpdateRequested()));
                    MLSceneGLSharedDataContext* shar = NULL;
                    bool created = false;
                    if (currentViewContainer() != NULL)
                    {
                        shar = currentViewContainer()->sharedDataContext();
                        //GLA() is only the parent
                        QGLWidget* filterWidget = new QGLWidget(GLA(),shar);
                        QGLFormat defForm = QGLFormat::defaultFormat();
                        cppfilt->glContext = new MLPluginGLContext(defForm,filterWidget->context()->device(),*shar);
                        created = cppfilt->glContext->create(filterWidget->context());
                        
                        MLRenderingData dt;
                        MLRenderingData::RendAtts atts;
                        atts[MLRenderingData::ATT_NAMES::ATT_VERTPOSITION] = true;
                        atts[MLRenderingData::ATT_NAMES::ATT_VERTNORMAL] = true;


                        if (info->filterAttribute(filtnm,MLXMLElNames::filterArity) == MLXMLElNames::singleMeshArity)
                        {
                            MLRenderingData::PRIMITIVE_MODALITY pm = MLPoliciesStandAloneFunctions::bestPrimitiveModalityAccordingToMesh(meshDoc()->mm());
                            if ((pm != MLRenderingData::PR_ARITY) && (meshDoc()->mm() != NULL))
                            {
                                dt.set(pm,atts);
                                shar->setRenderingDataPerMeshView(meshDoc()->mm()->id(),cppfilt->glContext,dt);
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
                                    shar->setRenderingDataPerMeshView(mm->id(),cppfilt->glContext,dt);
                                }
                            }
                        }
                    }
                    if ((!created) || (!cppfilt->glContext->isValid()))
                        throw MLException("A valid GLContext is required by the filter to work.\n");


                    //WARNING!!!!!!!!!!!!
                    /* IT SHOULD INVOKE executeFilter function. Unfortunately this function create a different thread for each invoked filter, and the MeshLab synchronization mechanisms are quite naive. Better to invoke the filters list in the same thread*/
                    meshDoc()->setBusy(true);
                    cppfilt->applyFilter( filtnm, *meshDoc(), envwrap, QCallBack );
                    meshDoc()->setBusy(false);
                    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    if ((currentViewContainer() != NULL) && (currentViewContainer()->sharedDataContext() != NULL))
                        currentViewContainer()->sharedDataContext()->removeView(cppfilt->glContext); 
                    delete cppfilt->glContext;
                    GLA()->completeUpdateRequested();
                    connect(meshDoc(),SIGNAL(documentUpdated()),GLA(),SLOT(completeUpdateRequested()));
                    /* executeFilter(&cont,*env);*/
                    QStringList filterClassesList = cont.xmlInfo->filterAttribute(filtnm,MLXMLElNames::filterClass).split(QRegExp("\\W+"), QString::SkipEmptyParts);
                    classes = MeshLabFilterInterface::convertStringListToCategoryEnum(filterClassesList);
                }
                else
                    throw MLException("WARNING! The MeshLab Script System is able to manage just the C++ XML filters.");
            }
            catch (MLException& e)
            {
                meshDoc()->Log.Log(GLLogStream::WARNING,e.what());
            }
        }
        if (meshDoc()->mm() != NULL)
        {
            if(classes & MeshFilterInterface::FaceColoring )
            {
//GLA()->setColorMode(vcg::GLW::CMPerFace);
                meshDoc()->mm()->updateDataMask(MeshModel::MM_FACECOLOR);
            }
            if(classes & MeshFilterInterface::VertexColoring ){
//GLA()->setColorMode(vcg::GLW::CMPerVert);
                meshDoc()->mm()->updateDataMask(MeshModel::MM_VERTCOLOR);
            }
            if(classes & MeshModel::MM_COLOR)
            {
//GLA()->setColorMode(vcg::GLW::CMPerMesh);
                meshDoc()->mm()->updateDataMask(MeshModel::MM_COLOR);
            }
            if(classes & MeshModel::MM_CAMERA)
                meshDoc()->mm()->updateDataMask(MeshModel::MM_CAMERA);
        }

        if(classes & MeshFilterInterface::MeshCreation )
            GLA()->resetTrackBall();
        /* to be changed */

        qb->reset();
        GLA()->update();
        GLA()->Logf(GLLogStream::SYSTEM,"Re-Applied filter %s",qPrintable((*ii)->filterName()));
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
// It is splitted in two part
// - startFilter that setup the dialogs and asks for parameters
// - executeFilter callback invoked when the params have been set up.


void MainWindow::startFilter()
{
    if(currentViewContainer() == NULL) return;
    if(GLA() == NULL) return;

    // In order to avoid that a filter changes something assumed by the current editing tool,
    // before actually starting the filter we close the current editing tool (if any).
    if(GLA()) GLA()->endEdit();
    updateMenus();

    QStringList missingPreconditions;
    QAction *action = qobject_cast<QAction *>(sender());
    if (action == NULL)
        throw MLException("Invalid filter action value.");
    MeshFilterInterface *iFilter = qobject_cast<MeshFilterInterface *>(action->parent());
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
            QMessageBox::warning(0, tr("PreConditions' Failure"), QString("Warning the filter <font color=red>'" + iFilter->filterName(action) + "'</font> has not been applied.<br>"
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

        if (xmldialog != NULL)
        {
            xmldialog->close();
            delete xmldialog;
            xmldialog = NULL;
        }

        // (2) Ask for filter parameters and eventally directly invoke the filter
        // showAutoDialog return true if a dialog have been created (and therefore the execution is demanded to the apply event)
        // if no dialog is created the filter must be executed immediately
        if(! stddialog->showAutoDialog(iFilter, meshDoc()->mm(), (meshDoc()), action, this, GLA()) )
        {
            RichParameterSet dummyParSet;
            executeFilter(action, dummyParSet, false);
        }
    }
    else // NEW XML PHILOSOPHY
    {
        try
        {
            MeshLabFilterInterface *iXMLFilter = qobject_cast<MeshLabFilterInterface *>(action->parent());
            QString fname = action->text();
            MeshLabXMLFilterContainer& filt  = PM.stringXMLFilterMap[fname];

            if ((iXMLFilter == NULL) || (filt.xmlInfo == NULL) || (filt.act == NULL))
                throw MLException("An invalid MLXMLPluginInfo handle has been detected in startFilter function.");
            QString filterClasses = filt.xmlInfo->filterAttribute(fname,MLXMLElNames::filterClass);
            QStringList filterClassesList = filterClasses.split(QRegExp("\\W+"), QString::SkipEmptyParts);
            if(filterClassesList.contains("MeshCreation"))
            {
                qDebug("MeshCreation");
                meshDoc()->addNewMesh("","untitled.ply");
            }
            else
            {
                QString preCond = filt.xmlInfo->filterAttribute(fname,MLXMLElNames::filterPreCond);
                QStringList preCondList = preCond.split(QRegExp("\\W+"), QString::SkipEmptyParts);
                int preCondMask = MeshLabFilterInterface::convertStringListToMeshElementEnum(preCondList);
                if (!MeshLabFilterInterface::arePreCondsValid(preCondMask,(*meshDoc()->mm()),missingPreconditions))
                {
                    QString enstr = missingPreconditions.join(",");
                    QMessageBox::warning(0, tr("PreConditions' Failure"), QString("Warning the filter <font color=red>'" + fname + "'</font> has not been applied.<br>"
                        "Current mesh does not have <i>" + enstr + "</i>."));
                    return;
                }
            }
            //INIT PARAMETERS WITH EXPRESSION : Both are defined inside the XML file

            //Inside the MapList there are QMap<QString,QString> containing info about parameters. In particular:
            // "type" - "Boolean","Real" etc
            // "name" - "parameter name"
            // "defaultExpression" - "defExpression"
            // "help" - "parameter help"
            // "typeGui" - "ABSPERC_GUI" "CHECKBOX_GUI" etc
            // "label" - "gui label"
            // Depending to the typeGui could be inside the map other info:
            // for example for ABSPERC_GUI there are also
            // "minExpr" - "minExpr"
            // "maxExpr" - "maxExpr"

            MLXMLPluginInfo::XMLMapList params = filt.xmlInfo->filterParametersExtendedInfo(fname);


            /*****IMPORTANT NOTE******/
            //the popContext will be called:
            //- or in the executeFilter if the filter will be executed
            //- or in the close Event of stdDialog window if the filter will NOT be executed
            //- or in the catch exception if something went wrong during parsing/scanning

            try
            {

                if(currentViewContainer())
                {
                    if (iXMLFilter)
                        iXMLFilter->setLog(currentViewContainer()->LogPtr());
                    currentViewContainer()->LogPtr()->SetBookmark();
                }
                // just to be sure...
                createXMLStdPluginWnd();
                if (stddialog != NULL)
                {
                    stddialog->close();
                    delete stddialog;
                    stddialog = NULL;
                }
                // (2) Ask for filter parameters and eventally directly invoke the filter
                // showAutoDialog return true if a dialog have been created (and therefore the execution is demanded to the apply event)
                // if no dialog is created the filter must be executed immediatel
                if(!xmldialog->showAutoDialog(filt,PM,meshDoc(),  this, GLA()))
                {
                    /*Mock Parameters (there are no ones in the filter indeed) for the filter history.The filters with parameters are inserted by the applyClick of the XMLStdParDialog.
                    That is the only place where I can easily evaluate the parameter values without writing a long, boring and horrible if on the filter type for the correct evaluation of the expressions contained inside the XMLWidgets*/
                    QMap<QString,QString> mock;
                    meshDoc()->filterHistory->addExecutedXMLFilter(fname,mock);

                    executeFilter(&filt, mock, false);
                    meshDoc()->Log.Logf(GLLogStream::SYSTEM,"OUT OF SCOPE\n");
                }
                //delete env;
            }
            catch (MLException& e)
            {
                meshDoc()->Log.Logf(GLLogStream::SYSTEM,e.what());
            }
        }
        catch(ParsingException e)
        {
            meshDoc()->Log.Logf(GLLogStream::SYSTEM,e.what());
        }
    }
}


void MainWindow::updateSharedContextDataAfterFilterExecution(int postcondmask,int fclasses,bool& newmeshcreated)
{
    MultiViewer_Container* mvc = currentViewContainer();
    if (mvc != NULL)
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
                bool currentmeshnewlycreated = false;
                //Just to be sure that the filter author didn't forget to add changing tags to the postCondition field
                if ((mm->hasDataMask(MeshModel::MM_FACECOLOR)) && (fclasses & MeshFilterInterface::FaceColoring ))
                    postcondmask = postcondmask | MeshModel::MM_FACECOLOR;

                if ((mm->hasDataMask(MeshModel::MM_VERTCOLOR)) && (fclasses & MeshFilterInterface::VertexColoring ))
                    postcondmask = postcondmask | MeshModel::MM_VERTCOLOR;

                if ((mm->hasDataMask(MeshModel::MM_COLOR)) && (fclasses & MeshFilterInterface::MeshColoring ))
                    postcondmask = postcondmask | MeshModel::MM_COLOR;

                if ((mm->hasDataMask(MeshModel::MM_FACEQUALITY)) && (fclasses & MeshFilterInterface::Quality ))
                    postcondmask = postcondmask | MeshModel::MM_FACEQUALITY;

                if ((mm->hasDataMask(MeshModel::MM_VERTQUALITY)) && (fclasses & MeshFilterInterface::Quality ))
                    postcondmask = postcondmask | MeshModel::MM_VERTQUALITY;

                MLRenderingData dttoberendered;
                QMap<int,MeshModelTmpData>::Iterator existit = existingmeshesbeforefilterexecution.find(mm->id());
                if (existit != existingmeshesbeforefilterexecution.end())
                {

                    shared->getRenderInfoPerMeshView(mm->id(),GLA()->context(),dttoberendered);
                    bool connectivitychanged = false;
                    int updatemask = MeshModel::MM_NONE;
                    if ((mm->cm.VN() != existit->_nvert) || (mm->cm.FN() != existit->_nface) ||
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
                        if (isworthtobevisualized)
                        {
                            curr.get(pm,rd);
                            MLPoliciesStandAloneFunctions::updatedRendAttsAccordingToPriorities(pm,dttoupdate,rd,rd);
                            MLPoliciesStandAloneFunctions::filterUselessUdpateAccordingToMeshMask(mm,rd);
                        }
                        curr.set(pm,rd);
                        MLPoliciesStandAloneFunctions::setPerViewGLOptionsPriorities(mm,curr);
                    }
                    /*MLPerViewGLOptions opts;
                    curr.get(opts);*/
 
                    shared->setRenderingDataPerMeshView(mm->id(),GLA()->context(),curr);
                    currentmeshnewlycreated = false;
                }
                else 
                {
                    //A new mesh has been created by the filter. I have to add it in the shared context data structure
                    newmeshcreated = true;
                    currentmeshnewlycreated = true;
                    MLPoliciesStandAloneFunctions::suggestedDefaultPerViewRenderingData(mm,dttoberendered);
                    foreach(GLArea* gla,mvc->viewerList)
                    {
                        if (gla != NULL)
                            shared->setRenderingDataPerMeshView(mm->id(),gla->context(),dttoberendered);
                    }
                }
                vcg::tri::Allocator<CMeshO>::CompactEveryVector(mm->cm);
                shared->manageBuffers(mm->id());
                
                //addRenderingSystemLogInfo(mm->id());
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


void MainWindow::executeFilter(QAction *action, RichParameterSet &params, bool isPreview)
{
    MeshFilterInterface         *iFilter    = qobject_cast<        MeshFilterInterface *>(action->parent());
    qb->show();
    iFilter->setLog(&meshDoc()->Log);

    // Ask for filter requirements (eg a filter can need topology, border flags etc)
    // and statisfy them
    qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
    MainWindow::globalStatusBar()->showMessage("Starting Filter...",5000);
    int req=iFilter->getRequirements(action);
    if (!meshDoc()->meshList.isEmpty())
        meshDoc()->mm()->updateDataMask(req);
    qApp->restoreOverrideCursor();

    // (3) save the current filter and its parameters in the history
    if(!isPreview)
    {
        if (meshDoc()->filterHistory != NULL)
        {
            OldFilterNameParameterValuesPair* oldpair = new OldFilterNameParameterValuesPair();
            oldpair->pair = qMakePair(action->text(),params);
            meshDoc()->filterHistory->filtparlist.append(oldpair);
        }
        meshDoc()->Log.ClearBookmark();
    }
    else
        meshDoc()->Log.BackToBookmark();
    // (4) Apply the Filter
    bool ret;
    qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
    QTime tt; tt.start();
    meshDoc()->setBusy(true);
    RichParameterSet mergedenvironment(params);
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

        if (iFilter->filterArity(action) == MeshFilterInterface::SINGLE_MESH)
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
        existingmeshesbeforefilterexecution.clear();
        for(MeshModel* mm = meshDoc()->nextMesh();mm != NULL;mm=meshDoc()->nextMesh(mm))
        {
            int dt = mm->dataMask();
            existingmeshesbeforefilterexecution.insert(mm->id(),MeshModelTmpData(mm->dataMask(),(size_t) mm->cm.VN(),(size_t) mm->cm.FN(),(size_t) mm->cm.EN()));
        }
        ret=iFilter->applyFilter(action, *(meshDoc()), mergedenvironment, QCallBack);
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
            meshDoc()->Log.Logf(GLLogStream::SYSTEM,"Applied filter %s in %i msec",qPrintable(action->text()),tt.elapsed());
            if (meshDoc()->mm() != NULL)
                meshDoc()->mm()->meshModified() = true;
            MainWindow::globalStatusBar()->showMessage("Filter successfully completed...",2000);
            if(GLA())
            {
                GLA()->setWindowModified(true);
                GLA()->setLastAppliedFilter(action);
            }
            lastFilterAct->setText(QString("Apply filter ") + action->text());
            lastFilterAct->setEnabled(true);
        }
        else // filter has failed. show the message error.
        {
            QMessageBox::warning(this, tr("Filter Failure"), QString("Failure of filter <font color=red>: '%1'</font><br><br>").arg(action->text())+iFilter->errorMsg()); // text
            meshDoc()->Log.Logf(GLLogStream::SYSTEM,"Filter failed: %s",qPrintable(iFilter->errorMsg()));
            MainWindow::globalStatusBar()->showMessage("Filter failed...",2000);
        }

        
        MeshFilterInterface::FILTER_ARITY arity = iFilter->filterArity(action);
        QList<MeshModel*> tmp;
        switch(arity)
        {
        case (MeshFilterInterface::SINGLE_MESH):
            {
                tmp.push_back(meshDoc()->mm());
                break;
            }
        case (MeshFilterInterface::FIXED):
            {
                for(int ii = 0;ii < mergedenvironment.paramList.size();++ii)
                {
                    if (mergedenvironment.paramList[ii]->val->isMesh())
                    {
                        MeshModel* mm = mergedenvironment.paramList[ii]->val->getMesh();
                        if (mm != NULL)
                            tmp.push_back(mm);
                    }
                }
                break;
            }
        case (MeshFilterInterface::VARIABLE):
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

        if(iFilter->getClass(action) & MeshFilterInterface::MeshCreation )
            GLA()->resetTrackBall();

        for(int jj = 0;jj < tmp.size();++jj)
        {
            MeshModel* mm = tmp[jj];
            if (mm != NULL)
            {
                // at the end for filters that change the color, or selection set the appropriate rendering mode
                if(iFilter->getClass(action) & MeshFilterInterface::FaceColoring ) 
                    mm->updateDataMask(MeshModel::MM_FACECOLOR);

                if(iFilter->getClass(action) & MeshFilterInterface::VertexColoring )
                    mm->updateDataMask(MeshModel::MM_VERTCOLOR);

                if((iFilter->getClass(action) & MeshFilterInterface::MeshColoring ) || (iFilter->postCondition(action) & MeshModel::MM_COLOR))
                    mm->updateDataMask(MeshModel::MM_COLOR);

                if(iFilter->postCondition(action) & MeshModel::MM_CAMERA)
                    mm->updateDataMask(MeshModel::MM_CAMERA);

                if(iFilter->getClass(action) & MeshFilterInterface::Texture )
                    updateTexture(mm->id());
            }
        }
        
        int fclasses =	iFilter->getClass(action);
        //MLSceneGLSharedDataContext* sharedcont = GLA()->getSceneGLSharedContext();
        int postCondMask = iFilter->postCondition(action);
        updateSharedContextDataAfterFilterExecution(postCondMask,fclasses,newmeshcreated);
        existingmeshesbeforefilterexecution.clear();
    }
    catch (std::bad_alloc& bdall)
    {
        meshDoc()->setBusy(false);
        qApp->restoreOverrideCursor();
        QMessageBox::warning(this, tr("Filter Failure"), QString("Operating system was not able to allocate the requested memory.<br><b>Failure of filter <font color=red>: '%1'</font><br>We warmly suggest you to try a 64-bit version of MeshLab.<br>").arg(action->text())+bdall.what()); // text
        MainWindow::globalStatusBar()->showMessage("Filter failed...",2000);
    }
    qb->reset();
    layerDialog->setVisible(layerDialog->isVisible() || ((newmeshcreated) && (meshDoc()->size() > 0)));
    updateLayerDialog();
    updateMenus();
    MultiViewer_Container* mvc = currentViewContainer();
    if(mvc)
        mvc->updateAllViewer();

}

void MainWindow::initDocumentMeshRenderState(MeshLabXMLFilterContainer* mfc)
{
    /* if (env.isNull())
    throw MeshLabException("Critical error in initDocumentMeshRenderState: Env object inside the QSharedPointer is NULL");*/
    //if (meshDoc() == NULL)
    //    return;

    //QString fname = mfc->act->text();
    //QString ar = mfc->xmlInfo->filterAttribute(fname,MLXMLElNames::filterArity);

    //if ((ar == MLXMLElNames::singleMeshArity)&& (meshDoc()->mm() != NULL))
    //{

    //    QTime tt;
    //    tt.start();
    //    meshDoc()->renderState().add(meshDoc()->mm()->id(),meshDoc()->mm()->cm);
    //    GLA()->Logf(0,"Elapsed time %d\n",tt.elapsed());
    //    return;
    //}

    //if (ar == MLXMLElNames::fixedArity)
    //{
    //    Env env;
    //    QScriptValue val = env.loadMLScriptEnv(*meshDoc(),PM);
    //    EnvWrap envwrap(env);
    //    //I have to check which are the meshes requested as parameters by the filter. It's disgusting but there is not other way.
    //    MLXMLPluginInfo::XMLMapList params = mfc->xmlInfo->filterParameters(fname);
    //    for(int ii = 0;ii < params.size();++ii)
    //    {
    //        if (params[ii][MLXMLElNames::paramType] == MLXMLElNames::meshType)
    //        {
    //            try
    //            {
    //                MeshModel* tmp = envwrap.evalMesh(params[ii][MLXMLElNames::paramName]);
    //                if (tmp != NULL)
    //                    meshDoc()->renderState().add(tmp->id(),tmp->cm);
    //            }
    //            catch (ExpressionHasNotThisTypeException&)
    //            {
    //                QString st = "parameter " + params[ii][MLXMLElNames::paramName] + "declared of type mesh contains a not mesh value.\n";
    //                meshDoc()->Log.Logf(GLLogStream::FILTER,qPrintable(st));
    //            }
    //        }
    //    }
    //    return;
    //}

    ////In this case I can only copy all the meshes in the document!
    //if (ar == MLXMLElNames::variableArity)
    //{
    //    for(int ii = 0;ii<meshDoc()->meshList.size();++ii)
    //        meshDoc()->renderState().add(meshDoc()->meshList[ii]->id(),meshDoc()->meshList[ii]->cm);
    //    return;
    //}
}

void MainWindow::initDocumentRasterRenderState(MeshLabXMLFilterContainer* mfc)
{
    //if (meshDoc() == NULL)
    //    return;
    //QString fname = mfc->act->text();
    //QString ar = mfc->xmlInfo->filterAttribute(fname,MLXMLElNames::filterRasterArity);

    //if ((ar == MLXMLElNames::singleRasterArity)&& (meshDoc()->rm() != NULL))
    //{
    //    meshDoc()->renderState().add(meshDoc()->rm()->id(),*meshDoc()->rm());
    //    return;
    //}

    //if (ar == MLXMLElNames::fixedRasterArity)
    //{
    //    // TO DO!!!!!! I have to add RasterType in order to understand which are the parameters working on Raster!!!

    //    //	//I have to check which are the meshes requested as parameters by the filter. It's disgusting but there is not other way.
    //    //	MLXMLPluginInfo::XMLMapList params = mfc->xmlInfo->filterParameters(fname);
    //    //	for(int ii = 0;ii < params.size();++ii)
    //    //	{
    //    //		if (params[ii][MLXMLElNames::paramType] == MLXMLElNames::meshType)
    //    //		{
    //    //			try
    //    //			{
    //    //				MeshModel* tmp = env.evalMesh(params[ii][MLXMLElNames::paramName]);
    //    //				if (tmp != NULL)
    //    //					meshDoc()->renderState().add(tmp->id(),tmp->cm);
    //    //			}
    //    //			catch (ExpressionHasNotThisTypeException& e)
    //    //			{
    //    //				QString st = "parameter " + params[ii][MLXMLElNames::paramName] + "declared of type mesh contains a not mesh value.\n";
    //    //				meshDoc()->Log.Logf(GLLogStream::FILTER,qPrintable(st));
    //    //			}
    //    //		}
    //    //	}
    //    return;
    //}

    ////In this case I can only copy all the meshes in the document!
    //if (ar == MLXMLElNames::variableRasterArity)
    //{
    //    for(int ii = 0;meshDoc()->rasterList.size();++ii)
    //        if (meshDoc()->rasterList[ii] != NULL)
    //            meshDoc()->renderState().add(meshDoc()->rasterList[ii]->id(),*meshDoc()->rasterList[ii]);
    //    return;
    //}
}

void MainWindow::executeFilter(MeshLabXMLFilterContainer* mfc,const QMap<QString,QString>& parexpval , bool  ispreview)
{
    if (mfc == NULL)
        return;
    MeshLabFilterInterface         *iFilter    = mfc->filterInterface;
    bool jscode = (mfc->xmlInfo->filterScriptCode(mfc->act->text()) != "");
    bool filtercpp = (iFilter != NULL) && (!jscode);

    if ((!filtercpp) && (!jscode))
        throw MLException("A not-C++ and not-JaveScript filter has been invoked.There is something really wrong in MeshLab.");

    QString fname = mfc->act->text();
    QString postCond = mfc->xmlInfo->filterAttribute(fname,MLXMLElNames::filterPostCond);
    QStringList postCondList = postCond.split(QRegExp("\\W+"), QString::SkipEmptyParts);
    int postCondMask = MeshLabFilterInterface::convertStringListToMeshElementEnum(postCondList);
    /*if (postCondMask != MeshModel::MM_NONE)
    initDocumentMeshRenderState(mfc);

    initDocumentRasterRenderState(mfc);
    */
    if(!ispreview)
        meshDoc()->Log.ClearBookmark();
    else
        meshDoc()->Log.BackToBookmark();

    qb->show();
    if (filtercpp)
        iFilter->setLog(&meshDoc()->Log);

    //// Ask for filter requirements (eg a filter can need topology, border flags etc)
    //// and statisfy them
    qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
    MainWindow::globalStatusBar()->showMessage("Starting Filter...",5000);
    //int req=iFilter->getRequirements(action);
    meshDoc()->mm()->updateDataMask(postCondMask);
    qApp->restoreOverrideCursor();

    //// (3) save the current filter and its parameters in the history
    //if(!isPreview)
    //{
    //	meshDoc()->filterHistory.actionList.append(qMakePair(action->text(),params));
    //	meshDoc()->Log.ClearBookmark();
    //}
    //else
    //	meshDoc()->Log.BackToBookmark();
    //// (4) Apply the Filter



    bool ret = true;
    qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
    bool isinter = (mfc->xmlInfo->filterAttribute(fname,MLXMLElNames::filterIsInterruptible) == "true");

    if(!isinter) meshDoc()->setBusy(true);

    //RichParameterSet MergedEnvironment(params);
    //MergedEnvironment.join(currentGlobalParams);

    ////GLA() is only the parent
    xmlfiltertimer.restart();

    /*if (filtercpp)
    {
    QGLWidget* filterWidget = new QGLWidget(GLA());
    QGLFormat defForm = QGLFormat::defaultFormat();
    iFilter->glContext = new QGLContext(defForm,filterWidget->context()->device());
    iFilter->glContext->create(filterWidget->context());
    }*/
    try
    {
        MLXMLPluginInfo::XMLMapList ml = mfc->xmlInfo->filterParametersExtendedInfo(fname);
        QString funcall = "Plugins." + mfc->xmlInfo->pluginAttribute(MLXMLElNames::pluginScriptName) + "." + mfc->xmlInfo->filterAttribute(fname,MLXMLElNames::filterScriptFunctName) + "(";
        if (mfc->xmlInfo->filterAttribute(fname,MLXMLElNames::filterArity) == MLXMLElNames::singleMeshArity && !jscode)
        {
            funcall = funcall + QString::number(meshDoc()->mm()->id());
            if (ml.size() != 0)
                funcall = funcall + ",";
        }
        for(int ii = 0;ii < ml.size();++ii)
        {
            funcall = funcall + parexpval[ml[ii][MLXMLElNames::paramName]];
            if (ii != ml.size() - 1)
                funcall = funcall + ",";
        }
        funcall = funcall + ");";
        if (meshDoc() != NULL)
            meshDoc()->xmlhistory << funcall;
        existingmeshesbeforefilterexecution.clear();
        for(MeshModel* mm = meshDoc()->nextMesh();mm != NULL;mm=meshDoc()->nextMesh(mm))
            existingmeshesbeforefilterexecution.insert(mm->id(),MeshModelTmpData(mm->dataMask(),(size_t) mm->cm.VN(),(size_t) mm->cm.FN(),(size_t) mm->cm.EN()));
        if (filtercpp)
        {
            enableDocumentSensibleActionsContainer(false);
            FilterThread* ft = new FilterThread(fname,parexpval,PM,*(meshDoc()),this);

            connect(ft,SIGNAL(finished()),this,SLOT(postFilterExecution()));
            connect(ft,SIGNAL(threadCB(const int, const QString&)),this,SLOT(updateProgressBar(const int,const QString&)));
            connect(xmldialog,SIGNAL(filterInterrupt(const bool)),PM.stringXMLFilterMap[fname].filterInterface,SLOT(setInterrupt(const bool)));
            
            /*if ((_mw != NULL) && (_mw->currentViewContainer() != NULL))
            {    
                QGLWidget* tmpglwid = new QGLWidget(NULL,currentViewContainer()->sharedDataContext());
                filtercpp->glContext = new MLPluginGLContext(QGLFormat::defaultFormat(),tmpglwid->context()->device(),(*currentViewContainer()->sharedDataContext()));
                bool res = it->filterInterface->glContext->create(tmpglwid->context());
            }*/
            ft->start();
        }
        else
        {
            QTime t;
            t.start();
            Env env;
            env.loadMLScriptEnv(*meshDoc(),PM,currentGlobalPars());
            QScriptValue result = env.evaluate(funcall);
            scriptCodeExecuted(result,t.elapsed(),"");
            postFilterExecution();

        }
    }
    catch(MLException& e)
    {
        meshDoc()->Log.Logf(GLLogStream::SYSTEM,e.what());
        ret = false;
    }

}

void MainWindow::postFilterExecution()
{
    emit filterExecuted();
    //meshDoc()->renderState().clearState();
    qApp->restoreOverrideCursor();
    qb->reset();
    //foreach(QAction* act,filterMenu->actions())
    //    act->setEnabled(true);
    enableDocumentSensibleActionsContainer(true);

    FilterThread* obj = qobject_cast<FilterThread*>(QObject::sender());
    if (obj == NULL)
        return;
    QMap<QString,MeshLabXMLFilterContainer>::const_iterator mfc = PM.stringXMLFilterMap.find(obj->filterName());
    if (mfc == PM.stringXMLFilterMap.constEnd())
        return;
    QString fname = mfc->act->text();
    // at the end for filters that change the color, or selection set the appropriate rendering mode
    QString filterClasses = mfc->xmlInfo->filterAttribute(fname,MLXMLElNames::filterClass);
    QStringList filterClassesList = filterClasses.split(QRegExp("\\W+"), QString::SkipEmptyParts);
    int fclasses =	MeshLabFilterInterface::convertStringListToCategoryEnum(filterClassesList);
    bool newmeshcreated = false;
    if (mfc->filterInterface != NULL)
    {
        mfc->filterInterface->setInterrupt(false);
        QString postCond = mfc->xmlInfo->filterAttribute(fname,MLXMLElNames::filterPostCond);
        QStringList postCondList = postCond.split(QRegExp("\\W+"), QString::SkipEmptyParts);
        int postCondMask = MeshLabFilterInterface::convertStringListToMeshElementEnum(postCondList);
        updateSharedContextDataAfterFilterExecution(postCondMask,fclasses,newmeshcreated);
        MultiViewer_Container* mvc = currentViewContainer();
        if(mvc)
            mvc->updateAllViewer();
    }
    existingmeshesbeforefilterexecution.clear();
    meshDoc()->setBusy(false);



    //// (5) Apply post filter actions (e.g. recompute non updated stuff if needed)

    if(obj->succeed())
    {
        meshDoc()->Log.Logf(GLLogStream::SYSTEM,"Applied filter %s in %i msec\n",qPrintable(fname),xmlfiltertimer.elapsed());
        MainWindow::globalStatusBar()->showMessage("Filter successfully completed...",2000);
        if(GLA())
        {
            GLA()->setLastAppliedFilter(mfc->act);
            GLA()->setWindowModified(true);
        }
        lastFilterAct->setText(QString("Apply filter ") + fname);
        lastFilterAct->setEnabled(true);
    }
    else // filter has failed. show the message error.
    {
        MeshLabFilterInterface         *iFilter = mfc->filterInterface;
        QMessageBox::warning(this, tr("Filter Failure"), QString("Failure of filter: '%1'\n\n").arg(fname)+iFilter->errorMsg()); // text
        meshDoc()->Log.Logf(GLLogStream::SYSTEM,"Filter failed: %s",qPrintable(iFilter->errorMsg()));
        MainWindow::globalStatusBar()->showMessage("Filter failed...",2000);
    }

    if(fclasses & MeshFilterInterface::FaceColoring ) {
//GLA()->setColorMode(vcg::GLW::CMPerFace);
        meshDoc()->mm()->updateDataMask(MeshModel::MM_FACECOLOR);
    }
    if(fclasses & MeshFilterInterface::VertexColoring ){
/*GLA()->setColorMode(vcg::GLW::CMPerVert);*/
        meshDoc()->mm()->updateDataMask(MeshModel::MM_VERTCOLOR);
    }
    if(fclasses & MeshModel::MM_COLOR)
    {
/*GLA()->setColorMode(vcg::GLW::CMPerMesh);*/
        meshDoc()->mm()->updateDataMask(MeshModel::MM_COLOR);
    }


    //if(iFilter->postCondition(action) & MeshModel::MM_CAMERA)
    //	meshDoc()->mm()->updateDataMask(MeshModel::MM_CAMERA);

    if(fclasses & MeshFilterInterface::MeshCreation )
        GLA()->resetTrackBall();

    if(fclasses & MeshFilterInterface::Texture )
    {
//WARNING!!!!! This should be replaced in some way
//WARNING!!!!! HERE IT SHOULD BE A CHECK IF THE FILTER IS FOR MESH OR FOR DOCUMENT (IN THIS CASE I SHOULD ACTIVATE ALL THE TEXTURE MODE FOR EVERYONE...)
//NOW WE HAVE JUST TEXTURE FILTERS WORKING ON SINGLE MESH
//QMap<int,RenderMode>::iterator it = GLA()->rendermodemap.find(meshDoc()->mm()->id());
//if (it != GLA()->rendermodemap.end())
//    it.value().setTextureMode(GLW::TMPerWedgeMulti);
//////////////////////////////////////////////////////////////////////////////

        updateTexture(meshDoc()->mm()->id());
    }
    /* QMap<QThread*,Env*>::iterator it = envtobedeleted.find(obj);
    if (it == envtobedeleted.end())
    {
    QString err;
    err.sprintf("FilterThread with address: %08p already does not exist.",obj);
    throw MeshLabException(err);
    }
    else
    {
    Env* tmpenv = it.value();
    envtobedeleted.erase(it);
    delete tmpenv;
    }*/

    layerDialog->setVisible((layerDialog->isVisible() || ((newmeshcreated) && (meshDoc()->size() > 0))));
    updateLayerDialog();
    updateMenus();
    delete obj;
}

void MainWindow::scriptCodeExecuted( const QScriptValue& val,const int time,const QString& output )
{
    if (val.isError())
    {
        meshDoc()->Log.Logf(GLLogStream::SYSTEM,"Interpreter Error: line %i: %s",val.property("lineNumber").toInt32(),qPrintable(val.toString()));
        layerDialog->updateLog(meshDoc()->Log);
    }
    else
    {
        meshDoc()->Log.Logf(GLLogStream::SYSTEM,"Code executed in %d millisecs.\nOutput:\n%s",time,qPrintable(output));
        GLA()->update();
    }
}

// Edit Mode Managment
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
            GLA()->endEdit();
            updateMenus();
            return;
        }
        assert(0); // it should be impossible to start an action without having ended the previous one.
        return;
    }

    //if this GLArea does not have an instance of this action's MeshEdit tool then give it one
    if(!GLA()->editorExistsForAction(action))
    {
        MeshEditInterfaceFactory *iEditFactory = qobject_cast<MeshEditInterfaceFactory *>(action->parent());
        MeshEditInterface *iEdit = iEditFactory->getMeshEditInterface(action);
        GLA()->addMeshEditor(action, iEdit);
    }
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
    MeshRenderInterface *iRenderTemp = qobject_cast<MeshRenderInterface *>(action->parent());
    bool initsupport = false;
    MultiViewer_Container* mw = this->currentViewContainer();
    if ((iRenderTemp != NULL) && (mw != NULL))
    {
        MLSceneGLSharedDataContext* shared = mw->sharedDataContext();
        if (shared != NULL)
        {
            MLSceneGLSharedDataContext::PerMeshRenderingDataMap dt;
            shared->getRenderInfoPerMeshView(GLA()->context(),dt);
            iRenderTemp->Init(action,*(meshDoc()),dt,GLA());
            initsupport = iRenderTemp->isSupported();
            if (initsupport)
                GLA()->setRenderer(iRenderTemp,action);
            else
            {
                if (!initsupport)
                {
                    QString msg = "The selected shader is not supported by your graphic hardware!";
                    GLA()->Logf(GLLogStream::SYSTEM,qPrintable(msg));
                }
                iRenderTemp->Finalize(action,meshDoc(),GLA());
            }
        }
    }

    /*I clicked None in renderMenu */
    if ((action->parent() == this) || (!initsupport))
    {
        QString msg("No Shader.");
        GLA()->Logf(GLLogStream::SYSTEM,qPrintable(msg));
        GLA()->setRenderer(0,0); //default opengl pipeline or vertex and fragment programs not supported
    }
    GLA()->update();
}


void MainWindow::applyDecorateMode()
{
    if(GLA()->mm() == 0) return;
    QAction *action = qobject_cast<QAction *>(sender());		// find the action which has sent the signal

    MeshDecorateInterface *iDecorateTemp = qobject_cast<MeshDecorateInterface *>(action->parent());

    GLA()->toggleDecorator(iDecorateTemp->decorationName(action));

    layerDialog->updateDecoratorParsView();
    layerDialog->updateLog(meshDoc()->Log);
    layerDialog->update();
    GLA()->update();
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
    QFileDialog* saveDiag = new QFileDialog(this,tr("Save Project File"),lastUsedDirectory.path().append(""), tr("MeshLab Project (*.mlp);;Align Project (*.aln)"));
#if defined(Q_OS_WIN)
    saveDiag->setOption(QFileDialog::DontUseNativeDialog);
#endif
    QCheckBox* saveAllFile = new QCheckBox(QString("Save All Files"),saveDiag);
    saveAllFile->setCheckState(Qt::Unchecked);
    QCheckBox* onlyVisibleLayers = new QCheckBox(QString("Only Visible Layers"),saveDiag);
    onlyVisibleLayers->setCheckState(Qt::Unchecked);
    QGridLayout* layout = qobject_cast<QGridLayout*>(saveDiag->layout());
    if (layout != NULL)
    {
        layout->addWidget(saveAllFile,4,2);
        layout->addWidget(onlyVisibleLayers,4,1);
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
    qDebug("Saving aln file %s\n",qPrintable(fileName));
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
                meshNameVector.push_back(qPrintable(mp->relativePathName()));
                transfVector.push_back(mp->cm.Tr);
            }
        }
        ret= ALNParser::SaveALN(qPrintable(fileName),meshNameVector,transfVector);
    }
    else
        ret = MeshDocumentToXMLFile(*meshDoc(),fileName,onlyVisibleLayers->isChecked());

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
    showLayerDlg(false);
    if (fileName.isEmpty())
        fileName = QFileDialog::getOpenFileName(this,tr("Open Project File"), lastUsedDirectory.path(), "All Project Files (*.mlp *.aln *.out *.nvm);;MeshLab Project (*.mlp);;Align Project (*.aln);;Bundler Output (*.out);;VisualSFM Output (*.nvm)");

    if (fileName.isEmpty()) return false;

    QFileInfo fi(fileName);
    lastUsedDirectory = fi.absoluteDir();

    if((fi.suffix().toLower()!="aln") && (fi.suffix().toLower()!="mlp")  && (fi.suffix().toLower()!="out") && (fi.suffix().toLower()!="nvm"))
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
        int retVal=ALNParser::ParseALN(rmv,qPrintable(fileName));
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
            openRes = loadMeshWithStandardParams(relativeToProj,this->meshDoc()->mm(),ir->trasformation);
            if(!openRes)
                meshDoc()->delMesh(meshDoc()->mm());
        }
    }

    if (QString(fi.suffix()).toLower() == "mlp")
    {
        if (!MeshDocumentFromXML(*meshDoc(),fileName))
        {
            QMessageBox::critical(this, tr("Meshlab Opening Error"), "Unable to open MLP file");
            return false;
        }
        for (int i=0; i<meshDoc()->meshList.size(); i++)
        {
            QString fullPath = meshDoc()->meshList[i]->fullName();
            meshDoc()->setBusy(true);
            Matrix44m trm = this->meshDoc()->meshList[i]->cm.Tr; // save the matrix, because loadMeshClear it...
            if (!loadMeshWithStandardParams(fullPath,this->meshDoc()->meshList[i],trm))
                meshDoc()->delMesh(meshDoc()->meshList[i]);
        }
    }

    ////// BUNDLER
    if (QString(fi.suffix()).toLower() == "out"){

        QString cameras_filename = fileName;
        QString image_list_filename;
        QString model_filename;

        image_list_filename = QFileDialog::getOpenFileName(
            this  ,  tr("Open image list file"),
            QFileInfo(fileName).absolutePath(),
            tr("Bundler images list file (*.txt)")
            );
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
        mvc->resetAllTrackBall();
    qb->reset();
    saveRecentProjectList(fileName);
    showLayerDlg(visiblelayer || (meshDoc()->meshList.size() > 0));
    return true;
}

bool MainWindow::appendProject(QString fileName)
{
    QStringList fileNameList;

    if (fileName.isEmpty())
        fileNameList = QFileDialog::getOpenFileNames(this,tr("Append Project File"), lastUsedDirectory.path(), "All Project Files (*.mlp *.aln);;MeshLab Project (*.mlp);;Align Project (*.aln)");
    else
        fileNameList.append(fileName);

    if (fileNameList.isEmpty()) return false;

    // Common Part: init a Doc if necessary, and
    bool activeDoc = (bool) !mdiarea->subWindowList().empty() && mdiarea->currentSubWindow();
    bool activeEmpty = activeDoc && meshDoc()->meshList.empty();

    if (activeEmpty)  // it is wrong to try appending to an empty project, even if it is possible
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

        if((fi.suffix().toLower()!="aln") && (fi.suffix().toLower()!="mlp"))
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
            int retVal=ALNParser::ParseALN(rmv,qPrintable(fileName));
            if(retVal != ALNParser::NoError)
            {
                QMessageBox::critical(this, tr("Meshlab Opening Error"), "Unable to open ALN file");
                return false;
            }

            for(vector<RangeMap>::iterator ir=rmv.begin();ir!=rmv.end();++ir)
            {
                QString relativeToProj = fi.absoluteDir().absolutePath() + "/" + (*ir).filename.c_str();
                meshDoc()->addNewMesh(relativeToProj,relativeToProj);
                if(!loadMeshWithStandardParams(relativeToProj,this->meshDoc()->mm(),(*ir).trasformation))
                    meshDoc()->delMesh(meshDoc()->mm());
            }
        }

        if (QString(fi.suffix()).toLower() == "mlp")
        {
            if (!MeshDocumentFromXML(*meshDoc(),fileName))
            {
                QMessageBox::critical(this, tr("Meshlab Opening Error"), "Unable to open MLP file");
                return false;
            }
            for (int i=0; i<meshDoc()->meshList.size(); i++)
            {
                QString fullPath = meshDoc()->meshList[i]->fullName();
                meshDoc()->setBusy(true);
                Matrix44m trm = this->meshDoc()->meshList[i]->cm.Tr; // save the matrix, because loadMeshClear it...
                if(!loadMeshWithStandardParams(fullPath,this->meshDoc()->meshList[i],trm))
                    meshDoc()->delMesh(meshDoc()->meshList[i]);
            }
        }
    }

    meshDoc()->setBusy(false);
    if(this->GLA() == 0)  return false;
    this->currentViewContainer()->resetAllTrackBall();
    qb->reset();
    saveRecentProjectList(fileName);
    return true;
}

void MainWindow::newProject(const QString& projName)
{
    if (gpumeminfo == NULL)
        return;
    MultiViewer_Container *mvcont = new MultiViewer_Container(*gpumeminfo,mwsettings.highprecision,mwsettings.perbatchprimitives,mdiarea);
    connect(&mvcont->meshDoc,SIGNAL(meshAdded(int)),this,SLOT(meshAdded(int)));
    connect(&mvcont->meshDoc,SIGNAL(meshRemoved(int)),this,SLOT(meshRemoved(int)));
    mdiarea->addSubWindow(mvcont);
    connect(mvcont,SIGNAL(updateMainWindowMenus()),this,SLOT(updateMenus()));
    connect(mvcont,SIGNAL(updateDocumentViewer()),this,SLOT(updateLayerDialog()));
    filterMenu->setEnabled(!filterMenu->actions().isEmpty());
    if (!filterMenu->actions().isEmpty())
        updateSubFiltersMenu(true,false);
    GLArea *gla=new GLArea(this, mvcont, &currentGlobalParams);
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
    //if(mdiarea->isVisible())
    updateLayerDialog();
    mvcont->showMaximized();
    connect(mvcont->sharedDataContext(),SIGNAL(currentAllocatedGPUMem(int,int)),this,SLOT(updateGPUMemBar(int,int)));
}

void MainWindow::updateGPUMemBar(int allmem,int currentallocated)
{
    if (nvgpumeminfo != NULL)
    {
        nvgpumeminfo->setFormat( "Mem %p% %v/%m MB" );
        int allmb = allmem/1024;
        nvgpumeminfo->setRange(  0 , allmb );
        int remainingmb = (allmem-currentallocated)/1024;
        nvgpumeminfo->setValue( remainingmb);
        nvgpumeminfo->setFixedWidth(300);
    }
}

//WARNING!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//Temporary disgusting inequality between open (slot) - importMesh (function)
//and importRaster (slot). It's not also difficult to remove the problem because
//addNewRaster add a raster on a document and open the file, instead addNewMesh add a new mesh layer
//without loading the model.

bool MainWindow::importRaster(const QString& fileImg)
{
    QStringList filters;
    filters.push_back("Images (*.jpg *.png *.xpm)");
    filters.push_back("*.jpg");
    filters.push_back("*.png");
    filters.push_back("*.xpm");

    QStringList fileNameList;
    if (fileImg.isEmpty())
        fileNameList = QFileDialog::getOpenFileNames(this,tr("Open File"), lastUsedDirectory.path(), filters.join(";;"));
    else
        fileNameList.push_back(fileImg);

    foreach(QString fileName,fileNameList)
    {
        QFileInfo fi(fileName);
        if( fi.suffix().toLower()=="png" || fi.suffix().toLower()=="xpm" || fi.suffix().toLower()=="jpg")
        {
            qb->show();

            if(!fi.exists()) 	{
                QString errorMsgFormat = "Unable to open file:\n\"%1\"\n\nError details: file %1 does not exist.";
                QMessageBox::critical(this, tr("Meshlab Opening Error"), errorMsgFormat.arg(fileName));
                return false;
            }
            if(!fi.isReadable()) 	{
                QString errorMsgFormat = "Unable to open file:\n\"%1\"\n\nError details: file %1 is not readable.";
                QMessageBox::critical(this, tr("Meshlab Opening Error"), errorMsgFormat.arg(fileName));
                return false;
            }

            this->meshDoc()->setBusy(true);
            RasterModel *rm= meshDoc()->addNewRaster();
            rm->setLabel(fileImg);
            rm->addPlane(new Plane(fileName,Plane::RGBA));
            meshDoc()->setBusy(false);
            showLayerDlg(true);

            /// Intrinsics extraction from EXIF
            ///	If no CCD Width value is provided, the intrinsics are extracted using the Equivalent 35mm focal
            /// If no or invalid EXIF info is found, the Intrinsics are initialized as a "plausible" 35mm sensor, with 50mm focal

            ::ResetJpgfile();
            FILE * pFile = fopen(qPrintable(fileName), "rb");

            int ret = ::ReadJpegSections (pFile, READ_METADATA);
            fclose(pFile);
            if (!ret || (ImageInfo.CCDWidth==0.0f && ImageInfo.FocalLength35mmEquiv==0.0f))
            {
                rm->shot.Intrinsics.ViewportPx = vcg::Point2i(rm->currentPlane->image.width(), rm->currentPlane->image.height());
                rm->shot.Intrinsics.CenterPx   = Point2m(float(rm->currentPlane->image.width()/2.0), float(rm->currentPlane->image.width()/2.0));
                rm->shot.Intrinsics.PixelSizeMm[0]=36.0f/(float)rm->currentPlane->image.width();
                rm->shot.Intrinsics.PixelSizeMm[1]=rm->shot.Intrinsics.PixelSizeMm[0];
                rm->shot.Intrinsics.FocalMm = 50.0f;
            }
            else if (ImageInfo.CCDWidth!=0)
            {
                rm->shot.Intrinsics.ViewportPx = vcg::Point2i(ImageInfo.Width, ImageInfo.Height);
                rm->shot.Intrinsics.CenterPx   = Point2m(float(ImageInfo.Width/2.0), float(ImageInfo.Height/2.0));
                float ratio;
                if (ImageInfo.Width>ImageInfo.Height)
                    ratio=(float)ImageInfo.Width/(float)ImageInfo.Height;
                else
                    ratio=(float)ImageInfo.Height/(float)ImageInfo.Width;
                rm->shot.Intrinsics.PixelSizeMm[0]=ImageInfo.CCDWidth/(float)ImageInfo.Width;
                rm->shot.Intrinsics.PixelSizeMm[1]=ImageInfo.CCDWidth/((float)ImageInfo.Height*ratio);
                rm->shot.Intrinsics.FocalMm = ImageInfo.FocalLength;
            }
            else
            {
                rm->shot.Intrinsics.ViewportPx = vcg::Point2i(ImageInfo.Width, ImageInfo.Height);
                rm->shot.Intrinsics.CenterPx   = Point2m(float(ImageInfo.Width/2.0), float(ImageInfo.Height/2.0));
                float ratioFocal=ImageInfo.FocalLength/ImageInfo.FocalLength35mmEquiv;
                rm->shot.Intrinsics.PixelSizeMm[0]=(36.0f*ratioFocal)/(float)ImageInfo.Width;
                rm->shot.Intrinsics.PixelSizeMm[1]=(24.0f*ratioFocal)/(float)ImageInfo.Height;
                rm->shot.Intrinsics.FocalMm = ImageInfo.FocalLength;
            }

            // End of EXIF reading

            //			if(mdiarea->isVisible()) GLA()->mvc->showMaximized();
            updateMenus();
            updateLayerDialog();
        }
        else
            return false;
    }
    return true;
}

bool MainWindow::loadMesh(const QString& fileName, MeshIOInterface *pCurrentIOPlugin, MeshModel* mm, int& mask,RichParameterSet* prePar, const Matrix44m &mtr)
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

    // this change of dir is needed for subsequent textures/materials loading
    QDir::setCurrent(fi.absoluteDir().absolutePath());

    // retrieving corresponding IO plugin
    if (pCurrentIOPlugin == 0)
    {
        QString errorMsgFormat = "Error encountered while opening file:\n\"%1\"\n\nError details: The \"%2\" file extension does not correspond to any supported format.";
        QMessageBox::critical(this, tr("Opening Error"), errorMsgFormat.arg(fileName, extension));
        return false;
    }
    meshDoc()->setBusy(true);
    pCurrentIOPlugin->setLog(&meshDoc()->Log);
    if (!pCurrentIOPlugin->open(extension, fileName, *mm ,mask,*prePar,QCallBack,this /*gla*/))
    {
        QMessageBox::warning(this, tr("Opening Failure"), QString("While opening: '%1'\n\n").arg(fileName)+pCurrentIOPlugin->errorMsg()); // text+
        pCurrentIOPlugin->clearErrorString();
        meshDoc()->setBusy(false);
        return false;
    }
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


    if(!meshDoc()->mm()->cm.textures.empty())
        updateTexture(meshDoc()->mm()->id());

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

    if(delVertNum>0 || delFaceNum>0 )
        QMessageBox::warning(this, "MeshLab Warning", QString("Warning mesh contains %1 vertices with NAN coords and %2 degenerated faces.\nCorrected.").arg(delVertNum).arg(delFaceNum) );
    mm->cm.Tr = mtr;

    MultiViewer_Container* mv = currentViewContainer();
    if (mv != NULL)
    {
        MLSceneGLSharedDataContext* shared = mv->sharedDataContext();
        if (shared != NULL)
        {
            MLRenderingData defdt;
            MLPoliciesStandAloneFunctions::suggestedDefaultPerViewRenderingData(mm,defdt);
            for(int glarid = 0;glarid < mv->viewerCounter();++glarid)
            {
                GLArea* ar = mv->getViewer(glarid);
                if (ar != NULL)
                    shared->setRenderingDataPerMeshView(mm->id(),ar->context(),defdt);
            }
            shared->manageBuffers(mm->id());
            updateLayerDialog();
        }
    }

    meshDoc()->setBusy(false);

    return true;
}

bool MainWindow::importMeshWithLayerManagement(QString fileName)
{
    bool layervisible = false;
    if (layerDialog != NULL)
    {
        layervisible = layerDialog->isVisible();
        showLayerDlg(false);
    }
    bool res = importMesh(fileName);
    if (layerDialog != NULL)
        showLayerDlg(layervisible || meshDoc()->meshList.size());
    return res;
}

// Opening files in a transparent form (IO plugins contribution is hidden to user)
bool MainWindow::importMesh(QString fileName)
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
        fileNameList = QFileDialog::getOpenFileNames(this,tr("Import Mesh"), lastUsedDirectory.path(), PM.inpFilters.join(";;"));
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

    QTime allFileTime;
    allFileTime.start();
    foreach(fileName,fileNameList)
    {
        QFileInfo fi(fileName);
        QString extension = fi.suffix();
        MeshIOInterface *pCurrentIOPlugin = PM.allKnowInputFormats[extension.toLower()];
        //pCurrentIOPlugin->setLog(gla->log);
        if (pCurrentIOPlugin == NULL)
        {
            QString errorMsgFormat("Unable to open file:\n\"%1\"\n\nError details: file format " + extension + " not supported.");
            QMessageBox::critical(this, tr("Meshlab Opening Error"), errorMsgFormat.arg(fileName));
            return false;
        }

        RichParameterSet prePar;
        pCurrentIOPlugin->initPreOpenParameter(extension, fileName,prePar);
        if(!prePar.isEmpty())
        {
            GenericParamDialog preOpenDialog(this, &prePar, tr("Pre-Open Options"));
            preOpenDialog.setFocus();
            preOpenDialog.exec();
        }
        int mask = 0;
        //MeshModel *mm= new MeshModel(gla->meshDoc);
        QFileInfo info(fileName);
        MeshModel *mm=meshDoc()->addNewMesh(qPrintable(fileName),info.fileName());
        qb->show();
        QTime t;t.start();
        bool open = loadMesh(fileName,pCurrentIOPlugin,mm,mask,&prePar);
        if(open)
        {
            GLA()->Logf(0,"Opened mesh %s in %i msec",qPrintable(fileName),t.elapsed());
            RichParameterSet par;
            pCurrentIOPlugin->initOpenParameter(extension, *mm, par);
            if(!par.isEmpty())
            {
                GenericParamDialog postOpenDialog(this, &par, tr("Post-Open Processing"));
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
            GLA()->Logf(0,"Warning: Mesh %s has not been opened",qPrintable(fileName));
        }
    }// end foreach file of the input list
    GLA()->Logf(0,"All files opened in %i msec",allFileTime.elapsed());

    this->currentViewContainer()->resetAllTrackBall();

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

bool MainWindow::loadMeshWithStandardParams(QString& fullPath, MeshModel* mm, const Matrix44m &mtr)
{
    if ((meshDoc() == NULL) || (mm == NULL))
        return false;
    bool ret = false;
    mm->Clear();
    QFileInfo fi(fullPath);
    QString extension = fi.suffix();
    MeshIOInterface *pCurrentIOPlugin = PM.allKnowInputFormats[extension.toLower()];
    if(pCurrentIOPlugin != NULL)
    {
        RichParameterSet prePar;
        pCurrentIOPlugin->initPreOpenParameter(extension, fullPath,prePar);
        int mask = 0;
        QTime t;t.start();
        bool open = loadMesh(fullPath,pCurrentIOPlugin,mm,mask,&prePar,mtr);
        if(open)
        {
            GLA()->Logf(0,"Opened mesh %s in %i msec",qPrintable(fullPath),t.elapsed());
            RichParameterSet par;
            pCurrentIOPlugin->initOpenParameter(extension, *mm, par);
            pCurrentIOPlugin->applyOpenParameter(extension,*mm,par);
            ret = true;
        }
        else
            GLA()->Logf(0,"Warning: Mesh %s has not been opened",qPrintable(fullPath));
    }
    else
        GLA()->Logf(0,"Warning: Mesh %s cannot be opened. Your MeshLab version has not plugin to read %s file format",qPrintable(fullPath),qPrintable(extension));
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
        loadMeshWithStandardParams(fileName,mmm);
    }
    qb->reset();
    update();
}

void MainWindow::reload()
{
    if ((meshDoc() == NULL) || (meshDoc()->mm() == NULL))
        return;
    // Discards changes and reloads current file
    // save current file name
    qb->show();
    QString fileName = meshDoc()->mm()->fullName();
    loadMeshWithStandardParams(fileName,meshDoc()->mm());
    qb->reset();
    update();
}

bool MainWindow::exportMesh(QString fileName,MeshModel* mod,const bool saveAllPossibleAttributes)
{
    QStringList& suffixList = PM.outFilters;

    //QHash<QString, MeshIOInterface*> allKnownFormats;
    QFileInfo fi(fileName);
    //PM.LoadFormats( suffixList, allKnownFormats,PluginManager::EXPORT);
    //QString defaultExt = "*." + mod->suffixName().toLower();
    QString defaultExt = "*." + fi.suffix().toLower();
    if(defaultExt == "*.")
        defaultExt = "*.ply";
    if (mod == NULL)
        return false;
    mod->meshModified() = false;
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
            qDebug("File without extension adding it by hand '%s'", qPrintable(fileName));
        }
    }


    bool ret = false;

    QStringList fs = fileName.split(".");

    if(!fileName.isEmpty() && fs.size() < 2)
    {
        QMessageBox::warning(new QWidget(),"Save Error","You must specify file extension!!");
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

        MeshIOInterface *pCurrentIOPlugin = PM.allKnowOutputFormats[extension.toLower()];
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
        RichParameterSet savePar;

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
        QTime tt; tt.start();
        ret = pCurrentIOPlugin->save(extension, fileName, *mod ,mask,savePar,QCallBack,this);
        qb->reset();
        GLA()->Logf(GLLogStream::SYSTEM,"Saved Mesh %s in %i msec",qPrintable(fileName),tt.elapsed());

        qApp->restoreOverrideCursor();
        mod->setFileName(fileName);
        QSettings settings;
        int savedMeshCounter=settings.value("savedMeshCounter",0).toInt();
        settings.setValue("savedMeshCounter",savedMeshCounter+1);
        GLA()->setWindowModified(false);

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

bool MainWindow::saveSnapshot()
{
    SaveSnapshotDialog dialog(this);

    dialog.setValues(GLA()->ss);

    if (dialog.exec()==QDialog::Accepted)
    {
        GLA()->ss=dialog.getValues();
        GLA()->saveSnapshot();

        // if user ask to add the snapshot to raster layers
        /*
        if(dialog.addToRasters())
        {
        QString savedfile = QString("%1/%2%3.png")
        .arg(GLA()->ss.outdir).arg(GLA()->ss.basename)
        .arg(GLA()->ss.counter,2,10,QChar('0'));

        importRaster(savedfile);
        }
        */
        return true;
    }

    return false;
}
void MainWindow::about()
{
    QDialog *about_dialog = new QDialog();
    Ui::aboutDialog temp;
    temp.setupUi(about_dialog);
    temp.labelMLName->setText(MeshLabApplication::completeName(MeshLabApplication::HW_ARCHITECTURE(QSysInfo::WordSize))+"   ("+__DATE__+")");
    //about_dialog->setFixedSize(566,580);
    about_dialog->show();
}

void MainWindow::aboutPlugins()
{
    qDebug( "aboutPlugins(): Current Plugins Dir: %s ",qPrintable(pluginManager().getDefaultPluginDirPath()));
    PluginDialog dialog(pluginManager().getDefaultPluginDirPath(), pluginManager().pluginsLoaded, this);
    dialog.exec();
}

void MainWindow::helpOnscreen()
{
    if(GLA()) GLA()->toggleHelpVisible();
}

void MainWindow::helpOnline()
{
    checkForUpdates(false);
    QDesktopServices::openUrl(QUrl("http://sourceforge.net/apps/mediawiki/meshlab"));
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
void MainWindow::showXMLPluginEditorGui()
{
    if(GLA() != 0)
        plugingui->setVisible( !plugingui->isVisible() );
}


void MainWindow::setCustomize()
{
    CustomDialog dialog(currentGlobalParams,defaultGlobalParams, this);
    connect(&dialog,SIGNAL(applyCustomSetting()),this,SLOT(updateCustomSettings()));
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

bool MainWindow::QCallBack(const int pos, const char * str)
{
    int static lastPos=-1;
    if(pos==lastPos) return true;
    lastPos=pos;

    static QTime currTime = QTime::currentTime();
    if(currTime.elapsed()< 100) return true;
    currTime.start();
    MainWindow::globalStatusBar()->showMessage(str,5000);
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
                meshDoc()->Log.Log(GLLogStream::WARNING,qPrintable(errmsg));
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
            QMessageBox::warning(this,"Texture files has not been correctly loaded",unexistingtext);

        for(int tt = 0;tt < mvc->viewerCounter();++tt)
        {
            GLArea* ar = mvc->getViewer(tt);
            if (ar != NULL)
                ar->setupTextureEnv(textid);
        }
    }
    if (sometextfailed)
        QMessageBox::warning(this,"Texture files has not been correctly loaded",unexistingtext);
}

void MainWindow::updateProgressBar( const int pos,const QString& text )
{
    this->QCallBack(pos,qPrintable(text));
}

//void MainWindow::evaluateExpression(const Expression& exp,Value** res )
//{
//	try
//	{
//		PM.env.pushContext();
//		*res = exp.eval(&PM.env);
//		PM.env.popContext();
//	}
//	catch (ParsingException& e)
//	{
//		GLA()->Logf(GLLogStream::WARNING,e.what());
//	}
//}
void MainWindow::updateDocumentScriptBindings()
{
    if(currentViewContainer())
    {
        plugingui->setDocument(meshDoc());
        //PM.updateDocumentScriptBindings(*meshDoc());
    }
}

void MainWindow::loadAndInsertXMLPlugin(const QString& xmlpath,const QString& scriptname)
{
    if ((xmldialog != NULL) && (xmldialog->isVisible()))
        this->xmldialog->close();
    PM.deleteXMLPlugin(scriptname);
    try
    {
        PM.loadXMLPlugin(xmlpath);
    }
    catch (MeshLabXMLParsingException& e)
    {
        qDebug() << e.what();
    }
    fillFilterMenu();
    initSearchEngine();
}

void MainWindow::sendHistory()
{
    plugingui->getHistory(meshDoc()->xmlhistory);
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
            }
        }
    }
    updateLayerDialog();
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
    MultiViewer_Container* cont = currentViewContainer();
    if (cont != NULL)
    {
        MLSceneGLSharedDataContext* share = cont->sharedDataContext();
        if ((share != NULL) && (GLA() != NULL))
            share->getRenderInfoPerMeshView(mid,GLA()->context(),dt);
    }
}

void MainWindow::setRenderingData(int mid,const MLRenderingData& dt)
{
    MultiViewer_Container* cont = currentViewContainer();
    if (cont != NULL)
    {
        MLSceneGLSharedDataContext* share = cont->sharedDataContext();
        if ((share != NULL) && (GLA() != NULL))
        {
            share->setRenderingDataPerMeshView(mid,GLA()->context(),dt);
            share->manageBuffers(mid);
            //addRenderingSystemLogInfo(mid);
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
                meshDoc()->Log.Logf(0,data.toAscii());
            }
        }
    }
}

void MainWindow::updateRenderingDataAccordingToActions(int meshid,const QList<MLRenderingAction*>& acts)
{
    MLRenderingData olddt;
    getRenderingData(meshid,olddt);
    MLRenderingData dt(olddt);
    foreach(MLRenderingAction* act,acts)
    {
        if (act != NULL)
            act->updateRenderingData(dt);
    }
    setRenderingData(meshid,dt);
    MeshModel* mm = meshDoc()->getMesh(meshid);
    if (mm != NULL) 
    {
        MLDefaultMeshDecorators dec;
        dec.updateMeshDecorationData(*mm,olddt,dt);
    }
    GLA()->update();
}

void MainWindow::updateRenderingDataAccordingToAction( int meshid,MLRenderingAction* act)
{
    MLRenderingData olddt;
    getRenderingData(meshid,olddt);
    MLRenderingData dt(olddt);

    if (act != NULL)
        act->updateRenderingData(dt);

    setRenderingData(meshid,dt);
    MeshModel* mm = meshDoc()->getMesh(meshid);
    if (mm != NULL)
    {
        MLDefaultMeshDecorators dec;
        dec.updateMeshDecorationData(*mm,olddt,dt);
    }
    GLA()->update();
}