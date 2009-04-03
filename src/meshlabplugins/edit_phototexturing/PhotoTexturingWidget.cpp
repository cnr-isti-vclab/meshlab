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
	
	lastDirectory = "";
	
	connect(this,SIGNAL(updateGLAreaTextures()),gla,SLOT(updateTexture()));
	connect(this,SIGNAL(setGLAreaTextureMode(vcg::GLW::TextureMode)),gla,SLOT(setTextureMode(vcg::GLW::TextureMode)));
	connect(this,SIGNAL(updateMainWindowMenus()),gla,SIGNAL(updateMainWindowMenus()));

	ptPlugin = plugin;
	photoTexturer = texturer;
	PhotoTexturingWidget::ui.setupUi(this);
	this->setWidget(ui.main_frame);
	//this->setFeatures(QDockWidget::AllDockWidgetFeatures);
	this->setFeatures(QDockWidget::DockWidgetMovable|QDockWidget::DockWidgetClosable);
	this->setAllowedAreas(Qt::NoDockWidgetArea);
	this->setFloating(true);
	mesh = &m;
	glarea = gla;

	//setting up the headers for the tblewidget
	//QStringList headers;
	//headers << "Camera" << "Image";
	//ui.cameraTableWidget->setHorizontalHeaderLabels(headers);

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
	connect(ui.bakeTexturePushButton, SIGNAL(clicked()),this,SLOT(bakeTextures()));

	connect(ui.textureListWidget, SIGNAL(itemClicked(QListWidgetItem* )),this,SLOT(selectCurrentTexture()));

	connect(ui.resetPushButton, SIGNAL(clicked()),this,SLOT(reset()));
	connect(ui.closePushButton, SIGNAL(clicked()),this,SLOT(close()));
	//connect(ui.cancelPushButton, SIGNAL(clicked()),this,SLOT(cancel()));

	photoTexturer->storeOriginalTextureCoordinates(mesh);
	loadDefaultSettings();
	update();

}


PhotoTexturingWidget::~PhotoTexturingWidget(){

}
void PhotoTexturingWidget::loadConfigurationFile(){
	QString dir;
	if (lastDirectory == ""){
		dir=".";
	}else{
		dir = lastDirectory;
	}
	QString filename = QFileDialog::getOpenFileName(this,tr("Select Configuration File"),dir, "*.ptcfg");
	if(!filename.isNull()){
		QFileInfo finfo(filename);
		lastDirectory = finfo.absolutePath();
		ui.calibrationFileLineEdit->setText(filename);
		photoTexturer->loadConfigurationFile(filename);
		update();
	}
}

void PhotoTexturingWidget::saveConfigurationFile(){
	QString dir;
	if (lastDirectory == ""){
		dir=".";
	}else{
		dir = lastDirectory;
	}
	QString filename = QFileDialog::getSaveFileName(this,tr("Select Configuration File"),dir, "*.ptcfg");
	if(!filename.isNull()){
		QFileInfo finfo(filename);
		lastDirectory = finfo.absolutePath();
		
		ui.calibrationFileLineEdit->setText(filename);
		photoTexturer->saveConfigurationFile(filename);
	}
}

void PhotoTexturingWidget::addCamera(){
	QString dir;
	if (lastDirectory == ""){
		dir=".";
	}else{
		dir = lastDirectory;
	}
	QString filename = QFileDialog::getOpenFileName(this,tr("Select Calibration File"),dir, "Cameras (*.cam)");
	if(!filename.isNull()){
		QFileInfo finfo(filename);
		lastDirectory = finfo.absolutePath();
		photoTexturer->addCamera(filename);
		update();
		ui.cameraTableWidget->selectRow(ui.cameraTableWidget->rowCount()-1);
	}
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
		QTableWidgetItem *camNameItem = new QTableWidgetItem(camname);
		ui.cameraTableWidget->setItem(i, 0, camNameItem);
		QTableWidgetItem *camTypeItem = new QTableWidgetItem(cam->calibration->type);
		ui.cameraTableWidget->setItem(i, 1, camTypeItem);
		QTableWidgetItem *textureImageItem = new QTableWidgetItem(imagename);
		ui.cameraTableWidget->setItem(i, 2, textureImageItem);

	}
	
	QMap<int, QString>::iterator it = photoTexturer->textureList.begin();
	while (it !=  photoTexturer->textureList.end())
	{
	    QMap<int, QString>::iterator prev = it;
	    new QListWidgetItem(it.value(),ui.textureListWidget,it.key());

	    ++it;
	}

	if (currentIdx < ui.textureListWidget->count()){
		ui.textureListWidget->setCurrentRow(currentIdx);
	}
	ui.cameraTableWidget->resizeColumnsToContents();

	if (photoTexturer->cameras.size()>0){
		ui.calculateTexturesPushButton->setDisabled(false);
	}else{
		ui.calculateTexturesPushButton->setDisabled(true);
	}

	if (photoTexturer->textureList.size()>1 && photoTexturer->origTextureID>-1){
		//ui.unprojectTexturePushButton->setDisabled(false); stay disabled until unproject function is fixed
		ui.unprojectTexturePushButton->setDisabled(true);
	}else{
		ui.unprojectTexturePushButton->setDisabled(true);
	}
	
	if (photoTexturer->textureList.size()>1 && photoTexturer->origTextureID>-1){
		ui.bakeTexturePushButton->setDisabled(false);
	}else{
		ui.bakeTexturePushButton->setDisabled(true);
	}
	
	if ((photoTexturer->textureList.size()>1 && photoTexturer->origTextureID>-1)||(photoTexturer->textureList.size()>0 && photoTexturer->origTextureID==-1)){
		ui.combineTexturesPushButton->setDisabled(false);
	}else{
		ui.combineTexturesPushButton->setDisabled(true);
	}
	

}
void PhotoTexturingWidget::assignImage(){
	QString dir;
	if (lastDirectory == ""){
		dir=".";
	}else{
		dir = lastDirectory;
	}
	QString filename = QFileDialog::getOpenFileName(this,tr("Select Image File"),dir, "Images (*.png *.jpg *.bmp)");
	if(!filename.isNull()){
		QFileInfo finfo(filename);
		lastDirectory = finfo.absolutePath();
		QList <QTableWidgetItem*>list = ui.cameraTableWidget->selectedItems();
		if (list.size()>0){
			QTableWidgetItem* item = list.at(0);
			int row = item->row();
			Camera *cam = photoTexturer->cameras.at(row);
			cam->textureImage = filename;
			update();
		}
	}

}
void PhotoTexturingWidget::calculateTextures(){
	bool calcZBuffer =ui.checkBoxzBuffer->isChecked();
	bool selectedCamOnly = ui.selectedCameraOnlyCheckBox->isChecked();
	if (!selectedCamOnly){
		photoTexturer->calculateMeshTextureForAllCameras(mesh,calcZBuffer);
	}else{
		QList<QTableWidgetSelectionRange> ranges = ui.cameraTableWidget->selectedRanges();
		for (int i=0;i<ranges.size();i++){
			QTableWidgetSelectionRange range = ranges.at(i);
			for(int j=range.topRow();j<=range.bottomRow();j++){
				Camera *cam = photoTexturer->cameras.at(j);
				photoTexturer->calculateMeshTextureForCamera(mesh,cam,calcZBuffer);
			}
		}
	}
	glarea->update();
	update();

	updateGLAreaTextures();

	updateMainWindowMenus();
}

void PhotoTexturingWidget::selectCurrentTexture(){
	//int icam = ui.textureListWidget->currentRow();
	QListWidgetItem* lwis = ui.textureListWidget->currentItem();

	int textureID = lwis[0].type();
	photoTexturer->applyTextureToMesh(mesh,textureID);
	glarea->setWindowModified(true);
	setGLAreaTextureMode(vcg::GLW::TMPerWedgeMulti);
	updateMainWindowMenus();
	glarea->update();
}

void PhotoTexturingWidget::combineTextures(){
	photoTexturer->combineTextures(mesh);
	
	//QListWidgetItem *item = new QListWidgetItem("combined",ui.textureListWidget,textureId);
	//item->setData(QVariant(textureId));

	setGLAreaTextureMode(vcg::GLW::TMPerWedgeMulti);
	updateMainWindowMenus();
	glarea->update();
	update();


}

void PhotoTexturingWidget::unprojectTextures(){
	QSettings ptSettings;
	QVariant setSmartBlend = ptSettings.value(PhotoTexturer::BAKE_SMARTBLEND,"smartblend.exe");
	
	FilterParameterSet combineParamSet;
	combineParamSet.addInt(PhotoTexturer::TEXTURE_SIZE_WIDTH,1024,"Image width:","");
	combineParamSet.addInt(PhotoTexturer::TEXTURE_SIZE_HEIGHT,1024,"Image height:","");
	
	combineParamSet.addBool(PhotoTexturer::UNPROJECT_ENABLE_ANGLE,true,"Enable angle map:","");
	combineParamSet.addInt(PhotoTexturer::UNPROJECT_ANGLE_WEIGHT,1,"Angle map weight:","");
	combineParamSet.addFloat(PhotoTexturer::UNPROJECT_ANGLE,85,"Min angle:","");
	combineParamSet.addInt(PhotoTexturer::UNPROJECT_ANGLE_SHARPNESS,1,"Angle map sharpness:","");
	
	combineParamSet.addBool(PhotoTexturer::UNPROJECT_ENABLE_DISTANCE,false,"Enable distance map:","");
	combineParamSet.addInt(PhotoTexturer::UNPROJECT_DISTANCE_WEIGHT,1,"Distance map weight:","");

	combineParamSet.addBool(PhotoTexturer::BAKE_SAVE_UNPROJECT,true,"Save unprojected textures:","");
	combineParamSet.addBool(PhotoTexturer::UNPROJECT_ENABLE_EDGE_STRETCHING,false,"Enable Texture edge Stretching:","");
	combineParamSet.addInt(PhotoTexturer::UNPROJECT_EDGE_STRETCHING_PASS,2,"Edge Stretching Passes:","");
	
	combineParamSet.addBool(PhotoTexturer::BAKE_MERGE_TEXTURES,true,"Merge unprojected textures:","");
	combineParamSet.addEnum(PhotoTexturer::BAKE_MERGE_TYPE,0,QStringList() <<"Merge Faces by Angle"<<"Smartblend","Merge Mode:","");
	combineParamSet.addString(PhotoTexturer::BAKE_MERGED_TEXTURE,"","Merged Texture Filename:","");
	combineParamSet.addString(PhotoTexturer::BAKE_SMARTBLEND,setSmartBlend.toString(),"smartblend:","");

	QListWidgetItem* lwis = ui.textureListWidget->currentItem();
	int textureID = lwis[0].type();

	GenericParamDialog ad(this,&combineParamSet,"Texture Baking Parameters");
	int result=ad.exec();
	ptSettings.setValue(PhotoTexturer::BAKE_SMARTBLEND,combineParamSet.getString(PhotoTexturer::BAKE_SMARTBLEND));
	

	if (result == 1){
		photoTexturer->unprojectTextures(mesh,textureID,&combineParamSet);
	}
	update();

	updateGLAreaTextures();
}

void PhotoTexturingWidget::bakeTextures(){

	FilterParameterSet combineParamSet = loadDefaultBakeSettings();
	
	//creating template name for baked texture file
	QFileInfo fi = QFileInfo(mesh->fileName.c_str());
	QString bTextureFile = fi.baseName()+"_baked.png";
	if (QFile(bTextureFile).exists()){
		int count = 1;
		while(QFile(bTextureFile).exists()){
			bTextureFile = fi.baseName()+"_baked_";
			int max = 1000;
			while(count<max){
				bTextureFile+="0";
				max/=10;
			}
			bTextureFile+=QString::number(count)+".png";
			count++;
		}
		
	}
	combineParamSet.addString(PhotoTexturer::BAKE_MERGED_TEXTURE,bTextureFile,"Merged Texture Filename:","");
	
	GenericParamDialog ad(this,&combineParamSet,"Texture Baking Parameters");
	int result=ad.exec();
	
	int textureId =0;
	if (result == 1){
		saveDefaultBakeSettings(combineParamSet);
		textureId = photoTexturer->bakeTextures(mesh,&combineParamSet);
	}
	update();

	updateGLAreaTextures();
}

void PhotoTexturingWidget::reset(){
	photoTexturer->reset(mesh);
	update();
	glarea->update();
}
void PhotoTexturingWidget::close(){
	saveDefaultSettings();
	this->closeEvent(NULL);
}
void PhotoTexturingWidget::cancel(){
	photoTexturer->restoreOriginalTextureCoordinates(mesh);
	glarea->update();
}

void PhotoTexturingWidget::exportCamerasToMaxScript(){
	QString dir;
	if (lastDirectory == ""){
		dir=".";
	}else{
		dir = lastDirectory;
	}
	QString filename = QFileDialog::getSaveFileName(this,tr("Select MaxScript File"),dir, "*.ms");
	if(!filename.isNull()){
		QFileInfo finfo(filename);
		lastDirectory = finfo.absolutePath();
		photoTexturer->exportMaxScript(filename,mesh);
	}
}

void PhotoTexturingWidget::convertToTsaiCamera(){
	QString dir;
	if (lastDirectory == ""){
		dir=".";
	}else{
		dir = lastDirectory;
	}
	QString filename = QFileDialog::getSaveFileName(this,tr("Select Tsai Calibration File"),".", "*.cam");
	if(!filename.isNull()){
		QFileInfo finfo(filename);
		lastDirectory = finfo.absolutePath();
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
	
	
}

void PhotoTexturingWidget::loadDefaultSettings(){
	QSettings settings;
	QVariant calcZBuffer = settings.value("pt_calczbuffer",QVariant(false));
	QVariant onlySelectedCamera = settings.value("pt_selectedcameraonly",QVariant(false));
	ui.checkBoxzBuffer->setChecked(calcZBuffer.toBool());
	ui.selectedCameraOnlyCheckBox->setChecked(onlySelectedCamera.toBool());
}
void PhotoTexturingWidget::saveDefaultSettings(){
	bool onlySelectedCamera;
	bool calcZBuffer;
	calcZBuffer = ui.checkBoxzBuffer->isChecked();
	onlySelectedCamera = ui.selectedCameraOnlyCheckBox->isChecked();
	QSettings settings;
	settings.setValue("pt_calczbuffer",calcZBuffer);
	settings.setValue("pt_selectedcameraonly",onlySelectedCamera);
}


FilterParameterSet PhotoTexturingWidget::loadDefaultBakeSettings(){
	QSettings ptSettings;
	QVariant tmpValue;
	FilterParameterSet combineParamSet;
	tmpValue= ptSettings.value(PhotoTexturer::TEXTURE_SIZE_WIDTH,1024);
	combineParamSet.addInt(PhotoTexturer::TEXTURE_SIZE_WIDTH,tmpValue.toInt(),"Image width:","");
	
	tmpValue = ptSettings.value(PhotoTexturer::TEXTURE_SIZE_HEIGHT,1024);
	combineParamSet.addInt(PhotoTexturer::TEXTURE_SIZE_HEIGHT,tmpValue.toInt(),"Image height:","");
	
	tmpValue = ptSettings.value(PhotoTexturer::UNPROJECT_ENABLE_ANGLE,true);
	combineParamSet.addBool(PhotoTexturer::UNPROJECT_ENABLE_ANGLE,tmpValue.toBool(),"Enable angle map:","");
	
	tmpValue = ptSettings.value(PhotoTexturer::UNPROJECT_ANGLE_WEIGHT,1);
	combineParamSet.addInt(PhotoTexturer::UNPROJECT_ANGLE_WEIGHT,tmpValue.toInt(),"Angle map weight:","");
	
	tmpValue = ptSettings.value(PhotoTexturer::UNPROJECT_ANGLE,85.0);
	combineParamSet.addFloat(PhotoTexturer::UNPROJECT_ANGLE,tmpValue.toDouble(),"Max angle (degrees):","");
	
	tmpValue = ptSettings.value(PhotoTexturer::UNPROJECT_ANGLE_SHARPNESS,1);
	combineParamSet.addInt(PhotoTexturer::UNPROJECT_ANGLE_SHARPNESS,tmpValue.toInt(),"Angle map sharpness:","");
	
	tmpValue = ptSettings.value(PhotoTexturer::UNPROJECT_ENABLE_DISTANCE,false);
	combineParamSet.addBool(PhotoTexturer::UNPROJECT_ENABLE_DISTANCE,tmpValue.toBool(),"Enable distance map:","");
	
	tmpValue = ptSettings.value(PhotoTexturer::UNPROJECT_DISTANCE_WEIGHT,1);
	combineParamSet.addInt(PhotoTexturer::UNPROJECT_DISTANCE_WEIGHT,tmpValue.toInt(),"Distance map weight:","");

	tmpValue = ptSettings.value(PhotoTexturer::BAKE_SAVE_UNPROJECT,true);
	combineParamSet.addBool(PhotoTexturer::BAKE_SAVE_UNPROJECT,tmpValue.toBool(),"Save unprojected textures:","");
	
	tmpValue = ptSettings.value(PhotoTexturer::UNPROJECT_ENABLE_EDGE_STRETCHING,false);
	combineParamSet.addBool(PhotoTexturer::UNPROJECT_ENABLE_EDGE_STRETCHING,tmpValue.toBool(),"Enable Texture edge Stretching:","");
	
	tmpValue = ptSettings.value(PhotoTexturer::UNPROJECT_EDGE_STRETCHING_PASS,2);
	combineParamSet.addInt(PhotoTexturer::UNPROJECT_EDGE_STRETCHING_PASS,tmpValue.toInt(),"Edge Stretching Passes:","");
	
	tmpValue = ptSettings.value(PhotoTexturer::BAKE_MERGE_TEXTURES,true);
	combineParamSet.addBool(PhotoTexturer::BAKE_MERGE_TEXTURES,tmpValue.toBool(),"Merge unprojected textures:","");
	
	tmpValue = ptSettings.value(PhotoTexturer::BAKE_MERGE_TYPE,0);
	combineParamSet.addEnum(PhotoTexturer::BAKE_MERGE_TYPE,tmpValue.toInt(),QStringList() <<"Merge Faces by Angle"<<"Smartblend","Merge Mode:","");
	
	tmpValue = ptSettings.value(PhotoTexturer::BAKE_SMARTBLEND,"smartblend.exe");
	//combineParamSet.addOpenFileName(PhotoTexturer::BAKE_SMARTBLEND,tmpValue.toString(),".exe","Smartblend:","");
	combineParamSet.addString(PhotoTexturer::BAKE_SMARTBLEND,tmpValue.toString(),"Smartblend:","");

	return combineParamSet;
	
}
void PhotoTexturingWidget::saveDefaultBakeSettings(FilterParameterSet pset){
	QSettings ptSettings;
	
	ptSettings.setValue(PhotoTexturer::TEXTURE_SIZE_WIDTH,pset.getInt(PhotoTexturer::TEXTURE_SIZE_WIDTH));
	ptSettings.setValue(PhotoTexturer::TEXTURE_SIZE_HEIGHT,pset.getInt(PhotoTexturer::TEXTURE_SIZE_HEIGHT));
	ptSettings.setValue(PhotoTexturer::UNPROJECT_ENABLE_ANGLE,pset.getBool(PhotoTexturer::UNPROJECT_ENABLE_ANGLE));
	ptSettings.setValue(PhotoTexturer::UNPROJECT_ANGLE_WEIGHT,pset.getInt(PhotoTexturer::UNPROJECT_ANGLE_WEIGHT));
	ptSettings.setValue(PhotoTexturer::UNPROJECT_ANGLE,pset.getFloat(PhotoTexturer::UNPROJECT_ANGLE));
	ptSettings.setValue(PhotoTexturer::UNPROJECT_ANGLE_SHARPNESS,pset.getInt(PhotoTexturer::UNPROJECT_ANGLE_SHARPNESS));
	ptSettings.setValue(PhotoTexturer::UNPROJECT_ENABLE_DISTANCE,pset.getBool(PhotoTexturer::UNPROJECT_ENABLE_DISTANCE));
	ptSettings.setValue(PhotoTexturer::UNPROJECT_DISTANCE_WEIGHT,pset.getInt(PhotoTexturer::UNPROJECT_DISTANCE_WEIGHT));
	ptSettings.setValue(PhotoTexturer::BAKE_SAVE_UNPROJECT,pset.getBool(PhotoTexturer::BAKE_SAVE_UNPROJECT));
	ptSettings.setValue(PhotoTexturer::UNPROJECT_ENABLE_EDGE_STRETCHING,pset.getBool(PhotoTexturer::UNPROJECT_ENABLE_EDGE_STRETCHING));
	ptSettings.setValue(PhotoTexturer::UNPROJECT_EDGE_STRETCHING_PASS,pset.getInt(PhotoTexturer::UNPROJECT_EDGE_STRETCHING_PASS));
	ptSettings.setValue(PhotoTexturer::BAKE_MERGE_TEXTURES,pset.getBool(PhotoTexturer::BAKE_MERGE_TEXTURES));
	ptSettings.setValue(PhotoTexturer::BAKE_MERGE_TYPE,pset.getEnum(PhotoTexturer::BAKE_MERGE_TYPE));
	ptSettings.setValue(PhotoTexturer::BAKE_SMARTBLEND,pset.getString(PhotoTexturer::BAKE_SMARTBLEND));
	
}
