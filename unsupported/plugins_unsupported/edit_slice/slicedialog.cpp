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

#include "slicedialog.h"

dialogslice::dialogslice(QWidget *parent)
	: QDockWidget(parent)
{
	
	ui.setupUi(this);
	this->setWidget(ui.frame);
	numPlane=1;  //number of plans
	distance=1;  
	
	//defaultTrackball=false;
	restoreDefalut=false;
	QPoint p=parent->mapToGlobal(QPoint(0,0));
	this->setGeometry(p.x()+(parent->width()-width()),p.y()+40,width(),height() );
	this->setFloating(true);
	multi_sigleExportFile=false; //Multi/Single file choose
    
}

dialogslice::~dialogslice(){
  
}
void dialogslice::on_DefultButton_clicked()
{
 emit Update_glArea();
 emit  RestoreDefault();
 
 numPlane=1;
 distance=1;
 ui.SliderPlaneDistance->setValue(1);
 ui.spinBox->setValue(1);
 

}



void dialogslice::on_ExportButton_clicked()
{
emit exportMesh();
}




void dialogslice::on_SliderPlaneDistance_sliderMoved(int x)
{
distanceDefault=false;
distance=x;  
QString c;
ui.DistanceEdit->setText(c.setNum(distance*distanceRange));

emit Update_glArea();
}
void dialogslice::on_Exporter_singleFile_clicked()
{
  this->multi_sigleExportFile=true;
}

void dialogslice::on_Exporter_MultiFile_clicked()
{
  this->multi_sigleExportFile=false;
}

void dialogslice::on_DistanceEdit_returnPressed()
{
distanceDefault=false;
bool ok;
float val=this->ui.DistanceEdit->text().toFloat(&ok);
 if (ok)
 {
	 this->ui.SliderPlaneDistance->setValue(val/distanceRange); 
	 distance=val/distanceRange;
	 QString c;
	
 }
 else
 this->ui.SliderPlaneDistance->setValue(1); 
 emit Update_glArea();
}

void dialogslice::on_spinBox_valueChanged(int x)
{numPlane=x;
	if(x>1){
		ui.DistanceEdit->setEnabled(true);
		ui.SliderPlaneDistance->setEnabled(true);
		ui.SliderPlaneDistance->setValue(defaultdistance/distanceRange);
	}
	else
	{
		ui.DistanceEdit->setEnabled(false);
		ui.SliderPlaneDistance->setEnabled(false);
	}
	
	emit Update_glArea();
}

