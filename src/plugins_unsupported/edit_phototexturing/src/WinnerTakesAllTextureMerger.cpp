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


#include "WinnerTakesAllTextureMerger.h"

WinnerTakesAllTextureMerger::WinnerTakesAllTextureMerger(){
	normalized = false;
}
WinnerTakesAllTextureMerger::~WinnerTakesAllTextureMerger(){
	qDebug()<<"TextureMerger::~TextureMerger()";

	while(!ifcList.empty()){
		delete ifcList.takeFirst();
	}

}

QImage *WinnerTakesAllTextureMerger::merge(int imgWidth, int imgHeight){
	qDebug()<<"mergeTextureImagesWinnerTakesAll";
	QImage *image = new QImage(imgWidth,imgHeight,QImage::Format_ARGB32);
	int k;


	for(k=0;k<ifcList.size();k++){
		ifcList.at(k)->mergeFilter();
		ifcList.at(k)->applyMergedFilterToImage();
		ifcList.at(k)->mergedFilter->SaveAsImage("mergedfilter",QString::number(k));
		ifcList.at(k)->image->save("texture_"+QString::number(k)+".png","PNG");
	}

	int x;
	int y;
	for(x=0; x<imgWidth;x++){
		for(y=0;y<imgHeight;y++){
			QColor cpixel = QColor(0, 0, 0, 255);
			double min = 1.0;
			int i;
			for(i=0;i<ifcList.size();i++){
				QImage *tmpImg = ifcList.at(i)->image;


				if(ifcList.at(i)->mergedFilter->getValue(x,y)<min){
					min =ifcList.at(i)->mergedFilter->getValue(x,y);
					cpixel= QColor::fromRgba(tmpImg->pixel(x,y));
					//int alpha = (int)(255.0 * (1.0-min));
					//cpixel.setAlpha(alpha);
					cpixel.setAlpha(255);
				}
			}
			image->setPixel(x,y,cpixel.rgba());

		}
	}
	return image;
}
