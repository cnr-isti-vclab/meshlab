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

using namespace vcg; 

MultiViewer_Container::MultiViewer_Container(QWidget *parent)
: QSplitter(parent)
{
	viewerList = QVector<Viewer*>();

	setChildrenCollapsible(false);

	idCounter=0;

	layerDialog = new LayerDialog(this);
	layerDialog->setAllowedAreas (Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	
	
}

MultiViewer_Container::~MultiViewer_Container(){
	foreach(Viewer* viewer, viewerList)
		delete viewer;
	delete layerDialog;
}

int MultiViewer_Container::getNextViewerId(){
	return idCounter++;
}

void MultiViewer_Container::addView(Viewer* viewer,Qt::Orientation orient){//bool horiz){
	//The Viewers are organized like in a BSP tree.
	// Every new viewer are added within an Horizontal splitter. Its orientation could change according to next insertions.
	//		HSplit
	//      /   \
	//   View1   VSplit
	//           /   \
	//        View2  HSplit
	//                 /
	//               View3
	//In the GUI, when a viewer is splitted, the new one appears on its right (the space is split in two equal portions). 

	viewerList.append(viewer);
	int count = viewerCounter();

	//CASE 1: only when the first viewer is opened. Here there is no current view.
	if (count==1){ 
		addWidget((GLArea*) viewer);
	}

	else{
		GLArea* current = (GLArea*)currentView();
		QSplitter* parentSplitter = qobject_cast<QSplitter *>(current->parent());
		//CASE 2: Simple insertion inside the parent splitter (right branch).
		//Example: Add View3:
		//
		//			  HSplit					HSplit
		//             /             =>			/    \
		//           View1					 View1   HSplit
		//                                             |
		//                                           View2
		if(parentSplitter->count()==1){ 

			parentSplitter->setOrientation(orient);

			QSplitter* newSplitter = new QSplitter(Qt::Horizontal);

			parentSplitter->addWidget(newSplitter);
			newSplitter->addWidget((GLArea*)viewer);

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
		//CASE 3: The parent splitter has two children. The insertion is on the left branch.
        //Example: Add View3:
		//
		//			  	HSplit					 HSplit
		//        		/    \					 /    \
		//        View1     HSplit      =>   VSplit   HSplit 
		//					  |              /    \      |
		//					View2        View1  HSplit  View2
		//                                         |
		//                                       View3
		else{
			QSplitter* newSplitter;
			newSplitter = new QSplitter(orient);

			QList<int> sizes2 = parentSplitter->sizes();
			parentSplitter->insertWidget(0, newSplitter);
			
			QSplitter* newSplitter2 = new QSplitter(Qt::Horizontal);
			newSplitter2->addWidget((GLArea*)viewer);
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
    connect((GLArea*) viewer, SIGNAL(currentViewerChanged(int)), this, SLOT(updateCurrent(int)));


}

void MultiViewer_Container::removeView(int viewerId){
	for (int i=0; i< viewerList.count(); i++){
		Viewer* viewer = viewerList.at(i);
		if (viewer->getId() == viewerId){
			viewerList.remove(i);
			QSplitter* parentSplitter = qobject_cast<QSplitter *>(((GLArea*)viewer)->parent());
			delete viewer;

			//CASE 1: Simple deletion.
			//Example: Cancel View2:
			//
			//		HSplit					HSplit
			//		 /    \					/
			//	View1   HSplit	=>		View1
			//			  |
			//          View2

			if(parentSplitter->count()==0)
				delete parentSplitter;

			//CASE 2: Complex deletion, adjust the tree.
			// Example: Cancel View1:
			//
			//		 HSplit						   HSplit
			//		 /     \					   /    \
			//	 VSplit    HSplit      =>     HSplit   HSplit 
			//	 /    \        |				| 	     |
			// View1  HSplit  View2			  View3	   View2  
			//          |
			//        View3

			else if(parentSplitter->count()==1){
				QSplitter* parentParentSplitter = qobject_cast<QSplitter *>(parentSplitter->parent());
				if(parentParentSplitter){
					int index = parentParentSplitter->indexOf(parentSplitter);
					parentParentSplitter->insertWidget(index,parentSplitter->widget(0));
					delete parentSplitter;
				}
			}

			currentId = viewerList.at(0)->getId(); //per default il current è il primo viewer della lista (sempre presente)

			updateLayout();
		}
	}
}

void MultiViewer_Container::connectToLayerDialog(Viewer* viewer){
	connect(((GLArea*) viewer)->meshDoc, SIGNAL(currentMeshChanged(int)),layerDialog, SLOT(updateTable()));
}

void MultiViewer_Container::updateCurrent(int current){
	currentId=current;
}

void MultiViewer_Container::updatePressViewers(QMouseEvent *e){
	foreach(Viewer* viewer, viewerList)
		if(viewer->getId() != currentId){
			((GLArea*) viewer)->mousePressEvent2(e);
		}
}

void MultiViewer_Container::updateMoveViewers(QMouseEvent *e){
	foreach(Viewer* viewer, viewerList)
		if(viewer->getId() != currentId){
			((GLArea*) viewer)->mouseMoveEvent(e);
		}
}

void MultiViewer_Container::updateReleaseViewers(QMouseEvent *e){
	foreach(Viewer* viewer, viewerList)
		if(viewer->getId() != currentId){
			((GLArea*) viewer)->mouseReleaseEvent(e);
		}
}

void MultiViewer_Container::updateTrackballInViewers(Shot &shot)
{
	foreach(Viewer* viewer, viewerList)
		if(viewer->getId() != currentId){
			((GLArea*) viewer)->loadShot(shot);
		}
}


void MultiViewer_Container::updateLayout(){
	foreach(Viewer* viewer, viewerList)
		//splitter->removeWidget((GLArea*)viewer);
	for (int i=0; i< viewerList.count(); i++){
		int row = i/2;
		int column = i%2;
		//splitter->addWidget((GLArea*)viewerList.at(i));//, row, column); 
	}
}

Viewer* MultiViewer_Container::currentView(){
	foreach ( Viewer* viewer, viewerList)
		if (viewer->getId() == currentId)
			return viewer;
}

int MultiViewer_Container::viewerCounter(){
	return viewerList.count();
}

//void MultiViewer_Container::resizeEvent ( QResizeEvent * event ) {
//	/*if(data->imageList.size()!=0){
//		imageVis->adjustSize();
//	}*/
//}
/// update all the visual widgets at one time (glw, imageVis, tree, imageMag)
void MultiViewer_Container::updateAll(){
	/*glw->update();
	imageVis->update();
	imageMag->update();
	tree->updatePoints();*/
}
void MultiViewer_Container::keyPressEvent(QKeyEvent *keyEv){
	//keyEv->accept();
	//
	//// if the Delete key is pressed, then remove the selected point
	//if((data->selectedImagePoint.x()!=-1 || data->selectedMeshPoint!=-1) && keyEv->key() == Qt::Key_Delete){
	//	
	//	if(data->selectedImagePoint.x()!=-1){ 
	//		data->deleteImagePoint(data->selectedImagePoint);
	//		imageMag->update();
	//		glw->update();
	//	}
	//	else{ data->deleteMeshPoint(data->selectedMeshPoint);
	//	glw->update();
	//	}
	//	
	//}else  
	//	// if any of WASD keys are pressed, then move the selected image point
	//	if(data->selectedImagePoint.x()!=-1  && (keyEv->key() == Qt::Key_A || keyEv->key() == Qt::Key_S || keyEv->key() == Qt::Key_D || keyEv->key() == Qt::Key_W)){
	//		QPoint currentPoint = data->selectedImagePoint;
	//		
	//		switch(keyEv->key()){
	//			case Qt::Key_A:
	//				currentPoint.setX(currentPoint.x()-1);
	//				data->setSelectedImagePoint(currentPoint);
	//				break;
	//			case Qt::Key_D: 
	//				currentPoint.setX(currentPoint.x()+1);
	//				data->setSelectedImagePoint(currentPoint);
	//				break;
	//			case Qt::Key_W: 
	//				currentPoint.setY(currentPoint.y()-1);
	//				data->setSelectedImagePoint(currentPoint);
	//				break;
	//			case Qt::Key_S: 
	//				currentPoint.setY(currentPoint.y()+1);
	//				data->setSelectedImagePoint(currentPoint);
	//				break;
	//		}
	//	imageMag->update();
	//}
}