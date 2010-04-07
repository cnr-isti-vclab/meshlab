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
: QWidget(parent)
{
	viewerList = QVector<Viewer*>();
	
	mainLayout = new QGridLayout;

	idCounter=0;

	layerDialog = new LayerDialog(this);
	layerDialog->setAllowedAreas (    Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	
	setLayout(mainLayout);
}

MultiViewer_Container::~MultiViewer_Container(){
	for each(Viewer* viewer in viewerList)
		delete viewer;
	delete mainLayout;
	delete layerDialog;
}

int MultiViewer_Container::getNextViewerId(){
	return idCounter++;
}

void MultiViewer_Container::addView(Viewer* viewer){
	currentId = viewer->getId();
	viewerList.append(viewer);
	int count = mainLayout->count();
	int row = count/2;
	int column = count%2;
	mainLayout->addWidget((GLArea*)viewer, row, column); 

	//action for new viewer
    connect((GLArea*) viewer, SIGNAL(currentViewerChanged(int)), this, SLOT(updateCurrent(int)));
}

void MultiViewer_Container::removeView(int viewerId){
	for (int i=0; i< viewerList.count(); i++){
		Viewer* viewer = viewerList.at(i);
		if (viewer->getId() == viewerId){
			mainLayout->removeWidget((GLArea*)viewer);
			viewerList.remove(i);
			delete viewer;
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

void MultiViewer_Container::updateLayout(){
	for each(Viewer* viewer in viewerList)
		mainLayout->removeWidget((GLArea*)viewer);
	for (int i=0; i< viewerList.count(); i++){
		int row = i/2;
		int column = i%2;
		mainLayout->addWidget((GLArea*)viewerList.at(i), row, column); 
	}
}

Viewer* MultiViewer_Container::currentView(){
	for each ( Viewer* viewer in viewerList)
		if (viewer->getId() == currentId)
			return viewer;
}

int MultiViewer_Container::viewerCounter(){
	return viewerList.count();
}

void MultiViewer_Container::resizeEvent ( QResizeEvent * event ) {
	/*if(data->imageList.size()!=0){
		imageVis->adjustSize();
	}*/
}
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
