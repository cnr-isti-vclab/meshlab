/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005-2008                                           \/)\/    *
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


#include "SmartBlendTextureMerger.h"

SmartBlendTextureMerger::SmartBlendTextureMerger(QString command){
	normalized = false;
	
	
	
#ifdef _WIN32
	if (command.indexOf("\"")==0 && command.lastIndexOf("\"")== command.size()){
		cmd = command;
	}else{
		cmd = "\"" + command + "\"";
	}
	
	

#else
	command = command.replace(" ","\\ ");
	cmd ="wine "+command;
#endif
	
	qDebug()<<"cmd: "<<cmd;
}
SmartBlendTextureMerger::~SmartBlendTextureMerger(){
	qDebug()<<"TextureMerger::~TextureMerger()";

	while(!ifcList.empty()){
		delete ifcList.takeFirst();
	}

}

QImage *SmartBlendTextureMerger::merge(int imgWidth, int imgHeight){
	qDebug()<<"mergeTextureImagesWinnerTakesAll";
	QImage *image;
	QDir tmpDir = QDir::tempPath();
	qDebug()<<"Temp Dir:"<<tmpDir.absolutePath();
	QList<QTemporaryFile *> tmpFileNames;
	

	
	for(int i = 0;i<ifcList.size();i++){
		QImage* tmpImg = ifcList.at(i)->image;
		QTemporaryFile *file = new QTemporaryFile(tmpDir.absolutePath()+"/pt_smartblend");
		file->open();
		qDebug()<<"file:"<<file->fileName();
		tmpFileNames.append(file);
		tmpImg->save(file,"PNG");

		
	}
	
	QString sbCommand = cmd+" -o "+tmpDir.absolutePath()+"/pt_smartblend_merged.png"; 
	for(int i= 0; i< tmpFileNames.size();i++){
		sbCommand+=" " +tmpFileNames.at(i)->fileName();
	}
	
	qDebug()<<"sbCommand: "<<sbCommand;
	//char *c_command = sbCommand.toStdString().c_str();
	int r= system(sbCommand.toStdString().c_str());
	
	if(r !=0){
		QErrorMessage errormsg(NULL);
		errormsg.showMessage("An error occurred when trying to run SmartBlend. Please check the Smartblend path.");
		errormsg.setWindowModality(Qt::ApplicationModal);
		errormsg.exec();
		return NULL;
	}
	qDebug()<< "system:"<<r;
	
	image = new QImage(tmpDir.absolutePath()+"/pt_smartblend_merged.png");
	for (int i=0;i<tmpFileNames.size();i++){
		tmpFileNames.at(i)->remove();
	}
	QFile tmp_sb_file(tmpDir.absolutePath()+"/pt_smartblend_merged.png");
	tmp_sb_file.remove();
	return image;
}
