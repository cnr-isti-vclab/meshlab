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
#include "viewer.h"

using namespace vcg; 

MultiViewer_Container::MultiViewer_Container(QWidget *parent)
: QSplitter(parent)
{
	setChildrenCollapsible(false);

	currentId=-1;

}

MultiViewer_Container::~MultiViewer_Container(){
	foreach(Viewer* viewer, viewerList)
		delete viewer;
}

int MultiViewer_Container::getNextViewerId(){
	int newId=-1;

	for(QVector<Viewer*>::iterator view=viewerList.begin();view!=viewerList.end();++view)
		if(newId < (*view)->getId()) newId = (*view)->getId();

	return ++newId;
}

void MultiViewer_Container::addView(Viewer* viewer,Qt::Orientation orient){
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
		Viewer* current = currentView();
		QSplitter* parentSplitter = qobject_cast<QSplitter *>(current->parent());
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

			QSplitter* newSplitter = new QSplitter(Qt::Horizontal);

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
              /    \                 /     \
          View1     HSplit  =>   VSplit    HSplit
            |                    /    \      |
          View2              View1  HSplit  View2
                                             |
                                            View3
  */
  else{
      QSplitter* newSplitter = new QSplitter(orient);

			QList<int> sizes2 = parentSplitter->sizes();
			parentSplitter->insertWidget(0, newSplitter);
			
			QSplitter* newSplitter2 = new QSplitter(Qt::Horizontal);
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
		Viewer* viewer = viewerList.at(i);
		if (viewer->getId() == viewerId){
			viewerList.remove(i);
			QSplitter* parentSplitter = qobject_cast<QSplitter *>(viewer->parent());
			delete viewer;

/*
    CASE 1: Simple deletion.
    Example: Cancel View2:

        HSplit            HSplit
         /    \             |
      View1   HSplit  =>   View1
               |
              View2
*/
			if(parentSplitter->count()==0)
				delete parentSplitter;

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
			else if(parentSplitter->count()==1){
				QSplitter* parentParentSplitter = qobject_cast<QSplitter *>(parentSplitter->parent());
				if(parentParentSplitter){
					int index = parentParentSplitter->indexOf(parentSplitter);
					parentParentSplitter->insertWidget(index,parentSplitter->widget(0));
					delete parentSplitter;
				}
			}

			currentId = viewerList.at(0)->getId(); // default: current is the first viewer of the list
		}
	}
}

void MultiViewer_Container::updateCurrent(int current){
	int previousCurrentId = currentId;
	currentId=current;
	update(previousCurrentId);
	emit updateMainWindowMenus(); 
}

Viewer* MultiViewer_Container::currentView(){
	foreach ( Viewer* viewer, viewerList)
		if (viewer->getId() == currentId)
			return viewer;
  assert(0);
  return 0;
}

int MultiViewer_Container::viewerCounter(){
	return viewerList.count();
}

void MultiViewer_Container::updateAll(){
	foreach ( Viewer* viewer, viewerList)
		viewer->update();
}

void MultiViewer_Container::update(int id){
	foreach ( Viewer* viewer, viewerList)
		if (viewer->getId() == id)
			viewer->update();
}
