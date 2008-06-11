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
#ifndef CAMERACALIBRATION_H_
#define CAMERACALIBRATION_H_

#include <QObject>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QtXml/QDomNode>

//#include <src/Camera.h>
#include <src/CameraCalibrationData.h> 
#include <meshlab/meshmodel.h>
class CameraCalibration{
public:
	
	static const QString XML_CALIBRATION;
	static const QString XML_TYPE;
	
	//virtual CameraCalibration();
	virtual ~CameraCalibration(){};
	virtual void loadCalibration(QString ){};
	virtual void loadFromXml(QDomElement *xml_cam){
		
	};
	virtual void saveAsXml(QDomDocument* doc,QDomElement *root){
		
	};
	virtual void getUVforPoint(double x, double y, double z, double *u, double *v){
		
	};
	virtual void calibrate(QList<CameraCalibrationData*> &ccd ){
		
	};
	
	virtual void calibrateToTsai(MeshModel *mm){
		
	};
//private:
	//Camera *camera;
	
};

#endif /*CAMERACALIBRATION_H_*/
