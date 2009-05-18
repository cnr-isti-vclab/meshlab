/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2009                                          \/)\/    *
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

#ifndef DIALOGSLICE_H
#define DIALOGSLICE_H

#include <QWidget>
#include <QtGui/QDockWidget>
#include "ui_slicedialog.h"

class dialogslice : public QDockWidget
{
	Q_OBJECT

public:
	dialogslice(QWidget *parent = 0);
	~dialogslice();
	
	inline int getPlaneNumber() {return numPlane;}
	inline float getDistance() {return (distance*distanceRange);}
	inline bool getDefaultTrackball(){return defaultTrackball;}
	inline bool getdistanceDefault(){return distanceDefault;}
	inline bool getRestoreDefalut(){return restoreDefalut;}
	inline void setRestoreDefalut(bool in){ 
		restoreDefalut=in;}
	inline bool getExportOption(){
	    return multi_sigleExportFile;
	}
	inline void setDefaultDistance(float f){
	defaultdistance=f;
	}
		
	inline void setDistanceRange(float dRange){
		this->distanceRange=dRange/100;  // Scale of values in slider rappresent the value in px of any tick 
		ui.labelDistanceRange->setText(QString("Distance range from 0 to "+ QString::number(dRange)));
		
	}
	
Q_SIGNALS:
	void exportMesh(); 
	void Update_glArea();
     void RestoreDefault();
private:
	Ui::dialogsliceClass ui;
	int numPlane; //numeber of plane
	float distance; 
	float distanceRange;
	float defaultdistance;
    QWidget* parent;
	bool distanceDefault; // enable/disable distance 
	bool multi_sigleExportFile; 
	bool  defaultTrackball;
	bool restoreDefalut;
private slots:
	
	void on_spinBox_valueChanged(int);
	void on_DistanceEdit_returnPressed();
	void on_Exporter_MultiFile_clicked();
	void on_Exporter_singleFile_clicked();
    void on_SliderPlaneDistance_sliderMoved(int);

	void on_ExportButton_clicked();
	void on_DefultButton_clicked();
	
	
};

#endif

