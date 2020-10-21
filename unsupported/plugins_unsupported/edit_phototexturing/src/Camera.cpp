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
#include "Camera.h"
#include <src/Tsai/TsaiCameraCalibration.h>
//#include <src/Poly/PolyCameraCalibration.h>

const QString Camera::XML_CAMERADOCUMENT = "CameraDocument";
const QString Camera::XML_CAMERA = "Camera";
const QString Camera::XML_NAME = "name";
const QString Camera::XML_IMAGE = "textureImage";
const QString Camera::XML_RESOLUTION_X = "resolutionX";
const QString Camera::XML_RESOLUTION_Y = "resolutionY";


Camera::Camera(){
	textureId = -1;
	zBuffer = 0;
	textureImage = QString("");
	calculatedTextures = false;
}
Camera::~Camera(){

}
void Camera::loadCameraCalibration(QString calibfile){

}
void Camera::calibrateCamera(){

}

void Camera::assignTextureImage(QString image){
	textureImage = image;
}
void Camera::saveAsXml(QDomDocument* doc,QDomElement *root){
	QDomElement xml_cam = doc->createElement(XML_CAMERA);
	xml_cam.setAttribute(XML_NAME,name);
	xml_cam.setAttribute(XML_IMAGE,textureImage);
	xml_cam.setAttribute(XML_RESOLUTION_X,resolution[0]);
	xml_cam.setAttribute(XML_RESOLUTION_Y,resolution[1]);

	calibration->saveAsXml(doc,&xml_cam);

	if (root == NULL){
		doc->appendChild(xml_cam);
	}else{
		root->appendChild(xml_cam);
	}

}

void Camera::loadFromXml(QDomElement *xml_cam){

	name = xml_cam->attribute(XML_NAME);
	textureImage  = xml_cam->attribute(XML_IMAGE);
	resolution[0] = xml_cam->attribute(XML_RESOLUTION_X).toInt();
	resolution[1] = xml_cam->attribute(XML_RESOLUTION_Y).toInt();

	QDomElement xml_calib = xml_cam->firstChildElement(CameraCalibration::XML_CALIBRATION);
	if (!xml_calib.isNull()){
		QString ctype = xml_calib.attribute(CameraCalibration::XML_TYPE);

		if (!ctype.compare("TSAI")){
			//qDebug("TSAI\n");
			calibration = new TsaiCameraCalibration();
			calibration->loadFromXml(&xml_calib);
		}/*else if (!ctype.compare("POLY")){
			//qDebug("POLY\n");
			calibration = new PolyCameraCalibration();
			calibration->loadFromXml(&xml_calib);

		}*/else{

		}
		if(calibration!=NULL){
			calibration->resolution[0] = resolution[0];
			calibration->resolution[1] = resolution[1];
		}
	}else{
		//qDebug("no calibration child in xmlfile \n");
	}

}


