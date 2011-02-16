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

using namespace vcg; 


Splitter::Splitter ( QWidget * parent):QSplitter(parent){}

Splitter::Splitter(Qt::Orientation orientation, QWidget *parent):QSplitter(orientation,parent){}

QSplitterHandle *Splitter::createHandle()
 {
     return new SplitterHandle(orientation(), this);
 }

MultiViewer_Container *Splitter::getRootContainer()
{
	Splitter * parentSplitter = qobject_cast<Splitter *>(this);
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

MultiViewer_Container::MultiViewer_Container(QWidget *parent)
: Splitter(parent)
{
	setChildrenCollapsible(false);

	currentId=-1;

}

MultiViewer_Container::~MultiViewer_Container()
{
  //foreach(GLArea* viewer, viewerList)
  //{
		//delete viewer;
  //}
}

int MultiViewer_Container::getNextViewerId(){
	int newId=-1;

  foreach(GLArea* view, viewerList)
  {
    if(newId < view->getId()) newId = view->getId();
  }

	return ++newId;
}


void MultiViewer_Container::addView(GLArea* viewer,Qt::Orientation orient){
	/* The Viewers are organized like a BSP tree.
	Every new viewer is added within an Horizontal splitter. Its orientation could change according to next insertions.
	HSplit
	/   \
	View1   VSplit
	/   \
	View2  HSplit
	/
	View3
	In the GUI, when a viewer is splitted, the new one appears on its right (the space is split in two equal portions).
	*/

  //CASE 0: only when the first viewer is opened, just add it and return;
  if (viewerCounter()==0)
  {
    viewerList.append(viewer);
    addWidget(viewer);
    currentId = viewer->getId();
    //action for new viewer
    connect(viewer, SIGNAL(currentViewerChanged(int)), this, SLOT(updateCurrent(int)));
    return;
	}

  //CASE 1: happens only at the FIRST split;
  if (viewerCounter()==1)
  {
    viewerList.append(viewer);
    addWidget(viewer);
    this->setOrientation(orient);
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

    currentId = viewer->getId();
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

  currentId = viewer->getId();
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

  Splitter* parentSplitter = qobject_cast<Splitter *>(viewer->parent());
  int currentIndex = parentSplitter->indexOf(viewer);

  viewer->deleteLater();
  // Very basic case of just two son of the MultiviewContainer.
  if(viewerList.count()==2)
  {
    viewerList.removeAll(viewer);
    currentId = viewerList.first()->getId();
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

    QWidget *sonLeft = siblingSplitter->widget(0);
    QWidget *sonRight = siblingSplitter->widget(1);
    this->setOrientation(siblingSplitter->orientation());
    this->insertWidget(0,sonLeft);
    this->insertWidget(1,sonRight);
    siblingSplitter->deleteLater();
    viewerList.removeAll(viewer);
    currentId = viewerList.first()->getId();
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
assert(siblingWidget);
parentParentSplitter->insertWidget(parentIndex,siblingWidget);
parentSplitter->deleteLater();
viewerList.removeAll(viewer);
currentId = viewerList.first()->getId();
}

void MultiViewer_Container::updateCurrent(int current){
	int previousCurrentId = currentId;
	currentId=current;
  if(getViewer(previousCurrentId))
        update(previousCurrentId);
	emit updateMainWindowMenus(); 
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
	GLArea* glArea = qobject_cast<GLArea*>(currentView());
	if(glArea)
	{
    QPair<Shotf,float> shotAndScale = glArea->shotFromTrackball();
    foreach(GLArea* viewer, viewerList)
			if(viewer->getId() != currentId){
				((GLArea*) viewer)->loadShot(shotAndScale);
			}
	}
}

void MultiViewer_Container::closeEvent( QCloseEvent *event )
{
	bool close = true;
	int ii = 0;
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
