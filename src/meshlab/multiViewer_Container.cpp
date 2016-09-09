/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
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
#include "multiViewer_Container.h"
#include "glarea.h"
#include <QMouseEvent>
#include "mainwindow.h"
#include <common/mlapplication.h>

using namespace vcg;

Splitter::Splitter ( QWidget * parent):QSplitter(parent){}
Splitter::Splitter(Qt::Orientation orientation, QWidget *parent):QSplitter(orientation,parent){}

QSplitterHandle *Splitter::createHandle()
{
	return new SplitterHandle(orientation(), this);
}

MultiViewer_Container *Splitter::getRootContainer()
{
	Splitter * parentSplitter = this;
	MultiViewer_Container* mvc = qobject_cast<MultiViewer_Container *>(parentSplitter);
	while(!mvc)
	{
		parentSplitter = qobject_cast<Splitter *>(parentSplitter->parent());
		mvc= qobject_cast<MultiViewer_Container *>(parentSplitter);
	}
	return mvc;
}

SplitterHandle::SplitterHandle(Qt::Orientation orientation, QSplitter *parent):QSplitterHandle(orientation, parent){}

void SplitterHandle::mousePressEvent ( QMouseEvent * e )
{
	QSplitterHandle::mousePressEvent(e);

	if(e->button()== Qt::RightButton)
	{
		MainWindow *window = qobject_cast<MainWindow *>(QApplication::activeWindow());
		if (window) window->setHandleMenu(mapToGlobal(e->pos()), orientation(), splitter());
	}
}

MultiViewer_Container::MultiViewer_Container(vcg::QtThreadSafeMemoryInfo& meminfo, bool highprec,size_t perbatchprimitives,QWidget *parent)
    : Splitter(parent),meshDoc()
{
	setChildrenCollapsible(false);
    scenecontext = new MLSceneGLSharedDataContext(meshDoc,meminfo,highprec,perbatchprimitives);
	scenecontext->setHidden(true);
	scenecontext->initializeGL();
	currentId=-1;
}

MultiViewer_Container::~MultiViewer_Container()
{
    /*for(int ii = 0;ii < viewerList.size();++ii)
        delete viewerList[ii];*/
	
    //WARNING!!!! here it's just destroyed the pointer to the MLSceneGLSharedDataContext
    //the data contained in the GPU are deallocated in the closeEvent function
    delete scenecontext;
}

int MultiViewer_Container::getNextViewerId(){
    int newId=-1;

	foreach(GLArea* view, viewerList)
	{
		if(newId < view->getId()) newId = view->getId();
	}

	return ++newId;
}


/*********************************************************************************************************/
/*********************************************************************************************************/
/*WARNING!!!!!!!!!!!! Horizontal and Vertical in QT are the opposite on how we consider them in Meshlab*/
/*********************************************************************************************************/
/*********************************************************************************************************/


void MultiViewer_Container::addView(GLArea* viewer,Qt::Orientation orient)
{
	
    MLRenderingData dt;
    MainWindow *window = qobject_cast<MainWindow *>(QApplication::activeWindow());
    if ((window != NULL) && (scenecontext != NULL))
    {
        //window->defaultPerViewRenderingData(dt);
        scenecontext->addView(viewer->context(),dt);
    }
    /* The Viewers are organized like a BSP tree.
	Every new viewer is added within an Horizontal splitter. Its orientation could change according to next insertions.
	  HSplit
	/       \
	View1   VSplit
	        /   \
	      View2  View3

	In the GUI, when a viewer is splitted, the new one appears on its right (the space is split in two equal portions).
	*/
	//CASE 0: only when the first viewer is opened, just add it and return;
	if (viewerCounter()==0)
	{
		viewerList.append(viewer);
		addWidget(viewer);
		updateCurrent(viewer->getId());
		//action for new viewer
		connect(viewer, SIGNAL(currentViewerChanged(int)), this, SLOT(updateCurrent(int)));
		return;
	}

	//CASE 1: happens only at the FIRST split;
	if (viewerCounter()==1)
	{
		viewerList.append(viewer);
		this->setOrientation(orient);
        addWidget(viewer);
		QList<int> sizes;
		if(this->orientation()== Qt::Horizontal){
			sizes.append(this->width()/2);
			sizes.append(this->width()/2);
		}
		else{
			sizes.append(this->height()/2);
			sizes.append(this->height()/2);
		}

		this->setSizes(sizes);
		this->setHandleWidth(2);
		this->setChildrenCollapsible(false);

		updateCurrent(viewer->getId());
		//action for new viewer
		connect(viewer, SIGNAL(currentViewerChanged(int)), this, SLOT(updateCurrent(int)));
		return;
	}

	// Generic Case: Each splitter Has ALWAYS two children.
	viewerList.append(viewer);
	GLArea* currentGLA = this->currentView();
	Splitter* currentSplitter = qobject_cast<Splitter *>(currentGLA->parent());
	QList<int> parentSizes = currentSplitter->sizes();

	int splittedIndex = currentSplitter->indexOf(currentGLA);
	Splitter* newSplitter = new Splitter(orient);
	currentSplitter->insertWidget(splittedIndex,newSplitter);

	newSplitter->addWidget(viewer);
	newSplitter->addWidget(currentGLA);

	QList<int> sizes;
	if(orient== Qt::Horizontal){
		sizes.append(currentSplitter->width()/2);
		sizes.append(currentSplitter->width()/2);
	}
	else{
		sizes.append(currentSplitter->height()/2);
		sizes.append(currentSplitter->height()/2);
	}
	currentSplitter->setSizes(parentSizes);
	newSplitter->setSizes(sizes);
	newSplitter->setHandleWidth(2);
	newSplitter->setChildrenCollapsible(false);

	updateCurrent(viewer->getId());
	//action for new viewer
	connect(viewer, SIGNAL(currentViewerChanged(int)), this, SLOT(updateCurrent(int)));
	return;
}

void MultiViewer_Container::removeView(int viewerId)
{
	GLArea* viewer;
	for (int i=0; i< viewerList.count(); i++)
	{
		if(viewerList.at(i)->getId() == viewerId)
			viewer = viewerList.at(i);
	}
	assert(viewer);
    if (viewer != NULL)
        scenecontext->removeView(viewer->context());
    Splitter* parentSplitter = qobject_cast<Splitter *>(viewer->parent());
	int currentIndex = parentSplitter->indexOf(viewer);

    viewer->deleteLater();
	// Very basic case of just two son of the MultiviewContainer.
	if(viewerList.count()==2)
	{
		viewerList.removeAll(viewer);
        updateCurrent(viewerList.first()->getId());
		return;
	}

	// generic tree with more of two leaves (some splitter involved)
	// two cases
	// 1) the deleted object is a direct child of the root
	// 2) otherwise; e.g. parent->parent exists.


	// First Case: deleting the direct son of the root (the mvc)
	// the sibling content (that is a splitter) surely will be moved up
	if(parentSplitter == this)
	{
		int insertIndex;
		if(currentIndex == 0) insertIndex = 1;
		else insertIndex = 0;

		Splitter *siblingSplitter = qobject_cast<Splitter *>(this->widget(insertIndex));
		assert(siblingSplitter);
        siblingSplitter->hide();
        siblingSplitter->deleteLater();

		QWidget *sonLeft = siblingSplitter->widget(0);
		QWidget *sonRight = siblingSplitter->widget(1);
		this->setOrientation(siblingSplitter->orientation());
        this->insertWidget(0,sonLeft);
		this->insertWidget(1,sonRight);

        patchForCorrectResize(this);
		viewerList.removeAll(viewer);
		//currentId = viewerList.first()->getId();
		updateCurrent(viewerList.first()->getId());
        return;
	}

	// Final case. Very generic, not son of the root.

	Splitter* parentParentSplitter = qobject_cast<Splitter *>(parentSplitter->parent());
	assert(parentParentSplitter);
	int parentIndex= parentParentSplitter->indexOf(parentSplitter);

	int siblingIndex;
	if(currentIndex == 0) siblingIndex = 1;
	else siblingIndex = 0;

	QWidget  *siblingWidget = parentSplitter->widget(siblingIndex);

    parentSplitter->hide();
    parentSplitter->deleteLater();
    parentParentSplitter->insertWidget(parentIndex,siblingWidget);
    
    patchForCorrectResize(parentParentSplitter);
	viewerList.removeAll(viewer);
	updateCurrent(viewerList.first()->getId());
}

void MultiViewer_Container::updateCurrent(int current)
{
	int previousCurrentId = currentId;
	currentId=current;
	if(getViewer(previousCurrentId))
		update(previousCurrentId);
    emit updateMainWindowMenus();
    if (current != previousCurrentId)
        emit updateDocumentViewer();     
}

GLArea * MultiViewer_Container::getViewer(int id)
{
	foreach ( GLArea* viewer, viewerList)
		if (viewer->getId() == id)
			return viewer;
	return 0;
}

int MultiViewer_Container::getViewerByPicking(QPoint p){
	foreach ( GLArea* viewer, viewerList){
		QPoint pViewer = viewer->mapFromGlobal(p);
		if(viewer->visibleRegion().contains(pViewer))
			return viewer->getId();
	}
	return -1;
}

GLArea* MultiViewer_Container::currentView(){
	return getViewer(currentId);
}

int MultiViewer_Container::viewerCounter(){
	return viewerList.count();
}

void MultiViewer_Container::updateAllViewer(){
	foreach ( GLArea* viewer, viewerList)
		viewer->update();
}

void MultiViewer_Container::resetAllTrackBall(){
	foreach ( GLArea* viewer, viewerList)
		viewer->resetTrackBall();
}

void MultiViewer_Container::update(int id){
	getViewer(id)->update();
}

void MultiViewer_Container::updateTrackballInViewers()
{
	GLArea* glArea = currentView();
	if(glArea)
	{
		QPair<Shotm,float> shotAndScale = glArea->shotFromTrackball();
		foreach(GLArea* viewer, viewerList)
			if(viewer->getId() != currentId){
				((GLArea*) viewer)->loadShot(shotAndScale);
			}
	}
}

void MultiViewer_Container::closeEvent( QCloseEvent *event )
{
	if (meshDoc.hasBeenModified())
	{
		QMessageBox::StandardButton ret=QMessageBox::question(
			this,  tr("MeshLab"), tr("Project '%1' modified.\n\nClose without saving?").arg(meshDoc.docLabel()),
			QMessageBox::Yes|QMessageBox::No,
			QMessageBox::No);

		if(ret==QMessageBox::No)	// don't close please!
		{
			event->ignore();
			return;
		}
	}
	bool close = true;
	int ii = 0;
    scenecontext->deAllocateGPUSharedData();
	while(close && (ii < viewerList.size()))
	{
		close = viewerList.at(ii)->readyToClose();
		++ii;
	}

	if (close)
		event->accept();
	else
		event->ignore();
}

void MultiViewer_Container::patchForCorrectResize( QSplitter* split )
{
    /***************************patch to avoid a qt problem**********************/
    /*in qt it's not possible to remove a widget from a splitter (no comment....).
    it's not sufficient to hide it.
    it looks like that anyway the framework allocates space on the screen also for the hidden splitter. 
    So we have to resize all the visible glareas to half of the size of the new splitter in which they are going to be inserted and set, 
    by hand, to zero the size of the splitter that is going to be deleted */
    /***************************************************************************/

    QSize sz = split->size();
    int newsz = 0;
    if(split->orientation() == Qt::Horizontal)
        newsz = sz.width()/2;
    else
        newsz = sz.height()/2;
    
    QList<int> newwigsizes;
    for(int ii = 0;ii < split->count();++ii)
    {
        QWidget* tmpwid = split->widget(ii);
        if (tmpwid->isVisible())
            newwigsizes.push_back(newsz);
        else
            newwigsizes.push_back(0);
    }

    split->setSizes(newwigsizes);
}
