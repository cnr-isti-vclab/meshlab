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
#ifndef CAMERA_H_
#define CAMERA_H_

#include <QObject> 
#include <QList>

#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QtXml/QDomNode>

#include "CameraSensor.h"
#include "CameraCalibration.h"

#include "TextureFilter.h"
/*
 * 
 */
class Camera{
public: 
	
	static const QString XML_CAMERADOCUMENT;
	static const QString XML_CAMERA;
	static const QString XML_NAME;
	static const QString XML_IMAGE;
	static const QString XML_RESOLUTION_X;
	static const QString XML_RESOLUTION_Y;
	
	
	bool calculatedTextures;
	QString name;
	int textureId;
	//QString type;
	QString textureImage;
	CameraSensor sensor;
	CameraCalibration *calibration;
	int resolution[2];
	
	TextureFilterZB *zBuffer;
	
	Camera();
	~Camera();
	void loadCameraCalibration(QString);
	void calibrateCamera();
	void assignTextureImage(QString);
	
	/*
	 * Saves the camera model and its calibration into a
	 * 
	 */
	void saveAsXml(QDomDocument* doc,QDomElement *root);
	
	/*
	 * Loads a camera model from a xml file. 
	 */
	void loadFromXml(QDomElement *xml_cam);
};

#endif /*CAMERA_H_*/
