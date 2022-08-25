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


#include <cfloat>
#include <src/TextureMerger.h>

ImageFilterContainer::ImageFilterContainer(){
	merged = false;
	image = NULL;
	mergedFilter = NULL;
	tag="";
	//filterList = QList<TextureFilter>();
}
ImageFilterContainer::~ImageFilterContainer(){
	qDebug()<< "ImageFilterContainer::~ImageFilterContainer()";
	if (mergedFilter!=NULL){
		delete mergedFilter;
	}
	if (image!=NULL){
		delete image;
	}
	while(!filterList.empty()){
		delete filterList.takeFirst();
	}

}

void ImageFilterContainer::addFilter(TextureFilterSTD *filter){
	filterList.push_back(filter);
}
void ImageFilterContainer::mergeFilter(){
	if (image!=NULL){
		mergedFilter = new TextureFilterSTD(image->size().width(),image->size().height(),1);
		mergedFilter->normalized = true;
		int size;
		size = filterList.size();
		int x,y;
		for (x=0;x<mergedFilter->vm_height;x++){
			for (y=0;y<mergedFilter->vm_width;y++){
				int i;
				float value = 0.0;
				bool disqualified = false;
				int weight = 0;
				for(i=0;i<size;i++){
					value+=filterList.at(i)->weight*filterList.at(i)->getValue(x,y);
					weight+=filterList.at(i)->weight;
					if(filterList.at(i)->getValue(x,y)==1.0){
						disqualified = true;
					}
				}
				if (!disqualified){
					value/=(float)weight;
				}else{
					value = 1.0;
				}
				//value = value * value;
				mergedFilter->setValue(x,y,value);
			}
		}
		merged = true;
	}
}

void ImageFilterContainer::setImage(QImage* img){
	image= img;
}

void ImageFilterContainer::applyMergedFilterToImage(){
	if (image!=NULL && merged){

		int x,y;
		for (x=0;x<image->width();x++){
			for (y=0;y<image->height();y++){
				QColor color = QColor::fromRgb(image->pixel(x,y));
				color.setAlpha((int)(255.0*(1.0-mergedFilter->getValue(x,y))));
				image->setPixel(x,y,color.rgba());
			}
		}
	}
}

TextureMerger::TextureMerger(){
	normalized = false;
}
TextureMerger::~TextureMerger(){
	qDebug()<<"TextureMerger::~TextureMerger()";

	while(!ifcList.empty()){
		delete ifcList.takeFirst();
	}

}

void TextureMerger::normalizeFilterContainerList(){
	qDebug()<<"normalizeFilterContainerList";
	int i;
	qDebug() << "ifcList.size:" << ifcList.size();
	if (ifcList.size()>0){
		for(i=0;i<ifcList.at(0)->filterList.size();i++){
			int j;
			float max = FLT_MIN;
			float min = FLT_MAX;
			qDebug()<< "calculate min max for filter: "<< i;
			for(j=0;j<ifcList.size();j++){
				if(ifcList.at(j)->filterList.at(i)->min< min){
					min = ifcList.at(j)->filterList.at(i)->min;
				}
				if(ifcList.at(j)->filterList.at(i)->max> max){
					max = ifcList.at(j)->filterList.at(i)->max;
				}
			}
			qDebug()<< "normalizing filter: "<< i;
			for(j=0;j<ifcList.size();j++){
				ifcList.at(j)->filterList.at(i)->normalize(min,max);
			}
		}
		normalized = true;
	}
}



