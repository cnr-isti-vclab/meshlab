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
#include <QtGui>
#include <QList>
#include <QListWidgetItem>
#include <QListWidget>
#include <PhotoTexturingWidget.h>
#include <meshlab/mainwindow.h>
#include <wrap/gl/trimesh.h>
#include <meshlab/stdpardialog.h>

PhotoTexturingWidget::PhotoTexturingWidget(MeshEditInterface* plugin, PhotoTexturer* texturer,MeshModel &m,GLArea *gla): MeshlabEditDockWidget(gla) {
	
	connect(this,SIGNAL(updateGLAreaTextures()),gla,SLOT(updateTexture()));
	connect(this,SIGNAL(setGLAreaTextureMode(vcg::GLW::TextureMode)),gla,SLOT(setTextureMode(vcg::GLW::TextureMode)));
	connect(this,SIGNAL(updateMainWindowMenus()),gla,SIGNAL(updateMainWindowMenus()));

	ptPlugin = plugin;
	photoTexturer = texturer;
	PhotoTexturingWidget::ui.setupUi(this);
	this->setWidget(ui.main_frame);
	this->setFeatures(QDockWidget::AllDockWidgetFeatures);
	this->setAllowedAreas(Qt::LeftDockWidgetArea);
	this->setFloating(true);
	mesh = &m;
	glarea = gla;

	//setting up the headers for the tblewidget
	QStringList headers;
	headers << "Camera" << "Image";
	ui.cameraTableWidget->setHorizontalHeaderLabels(headers);

	connect(ui.configurationLoadPushButton, SIGNAL(clicked()),this,SLOT(loadConfigurationFile()));
	connect(ui.configurationSavePushButton, SIGNAL(clicked()),this,SLOT(saveConfigurationFile()));
	connect(ui.exportToMaxScriptPushButton, SIGNAL(clicked()),this,SLOT(exportCamerasToMaxScript()));
	connect(ui.convertToTsaiCameraPushButton, SIGNAL(clicked()),this,SLOT(convertToTsaiCamera()));

	connect(ui.addCameraPushButton, SIGNAL(clicked()),this,SLOT(addCamera()));
	connect(ui.removeCameraPushButton, SIGNAL(clicked()),this,SLOT(removeCamera()));

	connect(ui.assignImagePushButton, SIGNAL(clicked()),this,SLOT(assignImage()));
	connect(ui.calculateTexturesPushButton, SIGNAL(clicked()),this,SLOT(calculateTextures()));
	connect(ui.combineTexturesPushButton, SIGNAL(clicked()),this,SLOT(combineTextures()));
	connect(ui.unprojectTexturePushButton, SIGNAL(clicked()),this,SLOT(unprojectTextures()));

	connect(ui.textureListWidget, SIGNAL(itemClicked(QListWidgetItem* )),this,SLOT(selectCurrentTexture()));

	connect(ui.applyPushButton, SIGNAL(clicked()),this,SLOT(apply()));
	connect(ui.closePushButton, SIGNAL(clicked()),this,SLOT(close()));
	connect(ui.cancelPushButton, SIGNAL(clicked()),this,SLOT(cancel()));

	photoTexturer->storeOriginalTextureCoordinates(mesh);

	update();

}


PhotoTexturingWidget::~PhotoTexturingWidget(){

}
void PhotoTexturingWidget::loadConfigurationFile(){
	QString filename = QFileDialog::getOpenFileName(this,tr("Select Configuration File"),".", "*.ptcfg");
	ui.calibrationFileLineEdit->setText(filename);
	photoTexturer->loadConfigurationFile(filename);
	update();
}

void PhotoTexturingWidget::saveConfigurationFile(){
	QString filename = QFileDialog::getSaveFileName(this,tr("Select Configuration File"),".", "*.ptcfg");
	ui.calibrationFileLineEdit->setText(filename);
	photoTexturer->saveConfigurationFile(filename);
}

void PhotoTexturingWidget::addCamera(){
	QString filename = QFileDialog::getOpenFileName(this,tr("Select Calibration File"),".", "Cameras (*.cam)");
	photoTexturer->addCamera(filename);
	update();
	ui.cameraTableWidget->selectRow(ui.cameraTableWidget->rowCount()-1);
}

void PhotoTexturingWidget::removeCamera(){
	//int selectedRow = ui.cameraTableWidget->sel
	QList <QTableWidgetItem*>list = ui.cameraTableWidget->selectedItems();
	if (list.size()>0){
		QTableWidgetItem* item = list.at(0);
		int row = item->row();
		printf("row: %d\n",row);
		photoTexturer->removeCamera(row);
		update();
	}

}

void PhotoTexturingWidget::update(){
	int rowcount = photoTexturer->cameras.size();
	ui.cameraTableWidget->setRowCount((rowcount));
	int i;
	int currentIdx = ui.textureListWidget->currentRow();
	ui.textureListWidget->clear();
	for (i=0;i<rowcount;i++){
		QString camname;
		QString imagename;
		Camera *cam = photoTexturer->cameras.at(i);
		camname =cam->name;
		imagename = cam->textureImage;
		QTableWidgetItem *camTypeItem = new QTableWidgetItem(camname);
		ui.cameraTableWidget->setItem(i, 0, camTypeItem);
		QTableWidgetItem *textureImageItem = new QTableWidgetItem(imagename);
		ui.cameraTableWidget->setItem(i, 1, textureImageItem);
		if (cam->calculatedTextures){
			QListWidgetItem *newItem = new QListWidgetItem;
			newItem->setText(camname);
			//newItem->setData(QVariant(i));
			ui.textureListWidget->addItem(newItem);
		}
	}
	if (currentIdx < ui.textureListWidget->count()){
		ui.textureListWidget->setCurrentRow(currentIdx);
	}
	ui.cameraTableWidget->resizeColumnsToContents();


	if (vcg::tri::HasPerFaceAttribute(mesh->cm,PhotoTexturer::ORIGINALUVTEXTURECOORDS) && vcg::tri::HasPerFaceAttribute(mesh->cm,PhotoTexturer::CAMERAUVTEXTURECOORDS)){
		ui.unprojectTexturePushButton->setDisabled(false);
	}else{
		ui.unprojectTexturePushButton->setDisabled(true);
	}
	
	if (vcg::tri::HasPerFaceAttribute(mesh->cm,PhotoTexturer::CAMERAUVTEXTURECOORDS)){
		ui.combineTexturesPushButton->setDisabled(false);
	}else{
		ui.combineTexturesPushButton->setDisabled(true);
	}
	

}
void PhotoTexturingWidget::assignImage(){
	QString filename = QFileDialog::getOpenFileName(this,tr("Select Image File"),".", "Images (*.png *.jpg *.bmp)");
	QList <QTableWidgetItem*>list = ui.cameraTableWidget->selectedItems();
	if (list.size()>0){
		QTableWidgetItem* item = list.at(0);
		int row = item->row();
		Camera *cam = photoTexturer->cameras.at(row);
		cam->textureImage = filename;
		update();
	}

}
void PhotoTexturingWidget::calculateTextures(){
	photoTexturer->calculateMeshTextureForAllCameras(mesh);
	glarea->update();
	update();

	updateGLAreaTextures();

	updateMainWindowMenus();
}

void PhotoTexturingWidget::selectCurrentTexture(){
	int icam = ui.textureListWidget->currentRow();

	photoTexturer->applyTextureToMesh(mesh,icam);
	setGLAreaTextureMode(vcg::GLW::TMPerWedgeMulti);
	updateMainWindowMenus();
	glarea->update();
}

void PhotoTexturingWidget::combineTextures(){
	photoTexturer->combineTextures(mesh);
	setGLAreaTextureMode(vcg::GLW::TMPerWedgeMulti);
	updateMainWindowMenus();
	glarea->update();


}

void PhotoTexturingWidget::unprojectTextures(){
	FilterParameterSet combineParamSet;
	combineParamSet.addInt("width",1024,"Image width:","");
	combineParamSet.addInt("height",1024,"Image height:","");
	combineParamSet.addInt("edgeStretchingPasses",2,"Edge Stretching Passes:","");
	combineParamSet.addBool("enable_angle_map",true,"Enable angle map:","");
	combineParamSet.addInt("angle_map_weight",2,"Angle map weight:","");
	combineParamSet.addFloat("min_angle",80,"Min angle:","");

	combineParamSet.addInt("angle_map_sharpness",2,"Angle map sharpness:","");
	combineParamSet.addBool("enable_distance_map",true,"Enable distance map:","");
	combineParamSet.addInt("distance_map_weight",1,"Distance map weight:","");

	//combineParamSet.addBool("saveImages",true,"Save all images","");
	//combineParamSet.addBool("sameFolder",true,"Same folder as mesh","");
	//buildParameterSet(alignParamSet, defaultAP);

	GenericParamDialog ad(this,&combineParamSet,"Texture Baking Parameters");
	int result=ad.exec();
	if (result == 1){
		photoTexturer->unprojectTextures(mesh,combineParamSet.getInt("width"),combineParamSet.getInt("height"),combineParamSet.getInt("edgeStretchingPasses"),combineParamSet.getBool("enable_angle_map"),combineParamSet.getInt("angle_map_weight"),combineParamSet.getInt("angle_map_sharpness"),combineParamSet.getFloat("min_angle"),combineParamSet.getBool("enable_distance_map"),combineParamSet.getInt("distance_map_weight"));
	}
	update();
}

void PhotoTexturingWidget::apply(){

}
void PhotoTexturingWidget::close(){
	//glarea->endEdit();
	//ptPlugin->EndEdit(NULL,NULL,NULL);
}
void PhotoTexturingWidget::cancel(){
	photoTexturer->restoreOriginalTextureCoordinates(mesh);
	glarea->update();
}

void PhotoTexturingWidget::exportCamerasToMaxScript(){
	QString filename = QFileDialog::getSaveFileName(this,tr("Select MaxScript File"),".", "*.ms");
	photoTexturer->exportMaxScript(filename,mesh);
}

void PhotoTexturingWidget::convertToTsaiCamera(){
	QString filename = QFileDialog::getSaveFileName(this,tr("Select Tsai Calibration File"),".", "*.cam");
	QList <QTableWidgetItem*>list = ui.cameraTableWidget->selectedItems();

	bool optimize;
	QMessageBox messageBox(QMessageBox::Question, "Convert to Tsai Camera", "Use optimization mode for Tsai calibration?",
			QMessageBox::Yes|QMessageBox::No , this);
	messageBox.setWindowModality(Qt::WindowModal);
	int returnValue = messageBox.exec();

	if(returnValue == QMessageBox::No)
	{
		optimize = false;
	} else{
		optimize = true;
	}

	if (list.size()>0){
		QTableWidgetItem* item = list.at(0);
		int row = item->row();
		photoTexturer->convertToTsaiCamera(row,optimize,filename,mesh);
	}


}
