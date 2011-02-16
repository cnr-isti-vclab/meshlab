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

  for(QVector<GLArea*>::iterator view=viewerList.begin();view!=viewerList.end();++view)
		if(newId < (*view)->getId()) newId = (*view)->getId();

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
	viewerList.append(viewer);
  int count = viewerCounter();

  //CASE 1: only when the first viewer is opened.
  if (count==1){
		addWidget(viewer);
	}

	else{
    GLArea* current = currentView();
		Splitter* parentSplitter = qobject_cast<Splitter *>(current->parent());
		/*
		CASE 2: Simple insertion inside the parent splitter (right branch). The insertion is on the parent's right branch.
		Example: Add View2:

		HSplit           HSplit
		/       =>      /    \
		View1           View1   HSplit
									|
									View2

		*/
		if(parentSplitter->count()==1){

			parentSplitter->setOrientation(orient);

			Splitter* newSplitter = new Splitter(Qt::Horizontal);

			parentSplitter->addWidget(newSplitter);
			newSplitter->addWidget(viewer);

			//Setting the size of the widgets inside parent splitter.
			QList<int> *sizes = new QList<int>();

			if(parentSplitter->orientation()== Qt::Horizontal){
				sizes->append(parentSplitter->width()/2);
				sizes->append(parentSplitter->width()/2);
			}
			else{
				sizes->append(parentSplitter->height()/2);
				sizes->append(parentSplitter->height()/2);
			}

			parentSplitter->setSizes(*sizes);

			parentSplitter->setHandleWidth(2);

			newSplitter->setChildrenCollapsible(false);

		}
		/*
		CASE 3: The parent splitter has two children. The insertion is on the parent's left branch.
		Example: Add View3:

		HSplit                  HSplit
		/     \                 /     \
		View1 HSplit  =>     VSplit    HSplit
		       |             /    \      |
		      View2       View1  HSplit  View2
									|
									View3
		*/
		else{
			Splitter* newSplitter = new Splitter(orient);

			QList<int> sizes2 = parentSplitter->sizes();
			parentSplitter->insertWidget(0, newSplitter);

			Splitter* newSplitter2 = new Splitter(Qt::Horizontal);
			newSplitter2->addWidget(viewer);
			current->setParent(newSplitter);
			newSplitter->addWidget(newSplitter2);

			//Setting the size of the widgets inside parent splitter
			QList<int> *sizes = new QList<int>();
			if(newSplitter->orientation()== Qt::Horizontal){
				sizes->append(parentSplitter->width()/2);
				sizes->append(parentSplitter->width()/2);
			}
			else{
				sizes->append(parentSplitter->height()/2);
				sizes->append(parentSplitter->height()/2);
			}

			parentSplitter->setSizes(sizes2);
			newSplitter->setSizes(*sizes);
			newSplitter->setHandleWidth(2);

			newSplitter->setChildrenCollapsible(false);
			newSplitter2->setChildrenCollapsible(false);
		}
	}

	currentId = viewer->getId();
	//action for new viewer
	connect(viewer, SIGNAL(currentViewerChanged(int)), this, SLOT(updateCurrent(int)));


}

void MultiViewer_Container::removeView(int viewerId){
	for (int i=0; i< viewerList.count(); i++){
    GLArea* viewer = viewerList.at(i);
		if (viewer->getId() == viewerId){
			viewerList.remove(i);
			Splitter* parentSplitter = qobject_cast<Splitter *>(viewer->parent());
			viewer->deleteLater();

	//viewer has not yet been deleted
/*
    CASE 1: Simple deletion.
    Example: Cancel View2:

        HSplit            HSplit
         /    \             |
      View1   HSplit  =>   View1
               |
              View2
*/
			//Technically the viewer has not yet been cancelled, so parentSplitter has one branch
			if(parentSplitter->count()==1)
				parentSplitter->deleteLater();

/*
    CASE 2: Complex deletion, adjust the tree.
     Example: Cancel View1:

           HSplit                 HSplit
          /      \                /    \
       VSplit    HSplit   =>   HSplit   HSplit
       /    \        |           | 	     |
     View1  HSplit  View2      View3	   View2
              |
            View3
*/
			//Technically the viewer has not yet been cancelled, so parentSplitter has two branches
			else if(parentSplitter->count()==2){
				Splitter* parentParentSplitter = qobject_cast<Splitter *>(parentSplitter->parent());
				if(parentParentSplitter){
					int index = parentParentSplitter->indexOf(parentSplitter);
					parentParentSplitter->insertWidget(index,parentSplitter->widget(1));
					parentSplitter->deleteLater();
				}
			}
		}
	}
	currentId = viewerList.at(0)->getId(); // default: current is the first viewer of the list
}

void MultiViewer_Container::updateCurrent(int current){
	int previousCurrentId = currentId;
	currentId=current;
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
