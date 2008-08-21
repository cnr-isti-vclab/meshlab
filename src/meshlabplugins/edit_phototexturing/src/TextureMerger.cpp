/*
 * TextureMerger.cpp
 *
 *  Created on: Aug 13, 2008
 *      Author: racoon
 */

#include <cfloat>
#include <src/TextureMerger.h>

ImageFilterContainer::ImageFilterContainer(){
	merged = false;
	image = NULL;
	mergedFilter = NULL;
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

void ImageFilterContainer::addFilter(TextureFilter *filter){
	filterList.push_back(filter);
}
void ImageFilterContainer::mergeFilter(){
	if (image!=NULL){
		mergedFilter = new TextureFilter(image->size().width(),image->size().height(),1);
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


QImage TextureMerger::mergeTextureImagesWinnerTakesAll(int imgWidth, int imgHeight){
	qDebug()<<"mergeTextureImagesWinnerTakesAll";
	QImage image = QImage(imgWidth,imgHeight,QImage::Format_ARGB32);
	int k;


	for(k=0;k<ifcList.size();k++){
		ifcList.at(k)->mergeFilter();
		ifcList.at(k)->applyMergedFilterToImage();
		ifcList.at(k)->mergedFilter->SaveAsImage("mergedfilter",QString::number(k));
		ifcList.at(k)->image->save("texture_"+QString::number(k)+".tiff","TIFF");
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
			image.setPixel(x,y,cpixel.rgba());

		}
	}
	return image;
}
