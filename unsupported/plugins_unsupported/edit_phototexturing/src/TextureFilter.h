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


#ifndef TEXTUREFILTER_H_
#define TEXTUREFILTER_H_

#include <QtGui>
#include <QImage>
#include <cfloat>
#include <limits>



template <typename T>
class TextureFilter{

private:
	T** value_matrix;

public:
	int weight;
	int type;
	int vm_width;
	int vm_height;

	bool normalized;

	T max;
	T min;

	TextureFilter(int width, int height, int newWeight);
	~TextureFilter();

	void normalize();
	void normalize(T min, T max);

	void SaveAsImage(QString imgFilename, QString tag);

	void setValue(int x, int y, T value);
	T getValue(int x, int y);

	T normalizeValue(T value);
};

typedef TextureFilter<double> TextureFilterZB;
typedef TextureFilter<float> TextureFilterSTD;

template <typename T>
TextureFilter<T>::TextureFilter(int width, int height, int newWeight){

	min =  std::numeric_limits<T>::max();
	max =  std::numeric_limits<T>::min();
	int x;
	int y;

	vm_height = height;
	vm_width = width;
	weight = newWeight;

	value_matrix=(T**)malloc(width*sizeof(T*));
	for(x=0;x<width;x++){
		value_matrix[x]=(T*)malloc(height*sizeof(T));
		for(y=0;y<height;y++){
			value_matrix[x][y]= std::numeric_limits<T>::min();
		}
	}

	normalized = false;
}

template <typename T>
TextureFilter<T>::~TextureFilter(){
	qDebug()<<"TextureFilter<T>::~TextureFilter()";
	int x;

	for(x=0;x<vm_width;x++){
		free(value_matrix[x]);

	}
	free(value_matrix);
}

template <typename T>
void TextureFilter<T>::normalize(){
	normalize(min,max);

}

template <typename T>
void TextureFilter<T>::normalize(T min, T max){
	qDebug()<< "normalize: " << min << max;
	if(!normalized){
		int x;
		int y;
		for(x=0;x<vm_width;x++){
			for(y=0;y<vm_height;y++){
				value_matrix[x][y] = normalizeValue(value_matrix[x][y]);
			}
		}
		normalized = true;
	}
}

template <typename T>
void TextureFilter<T>::SaveAsImage(QString imgFilename,QString tag){

	QImage image = QImage(vm_width,vm_height,QImage::Format_ARGB32);
	if(!normalized){

		normalize();
	}
	int x;
	int y;
	for(x=0;x<vm_width;x++){
		for(y=0;y<vm_height;y++){
			QColor color = QColor::fromRgb(0,0,0);
			color.setAlpha((int)255.0*value_matrix[x][y]);
			image.setPixel(x,y,color.rgba());
		}
	}

	image.save(imgFilename+"_"+tag+".png","PNG");

}

template <typename T>
void TextureFilter<T>::setValue(int x, int y, T value){
	if (value<min){
		min =value;
	}
	if (value>max){
		max = value;
	}

	value_matrix[x][y]=value;
}

template <typename T>
T TextureFilter<T>::getValue(int x, int y){

	return value_matrix[x][y];
}


template <typename T>
T TextureFilter<T>::normalizeValue(T value){
	if (value ==  std::numeric_limits<T>::min()){
			value=1.0;
	}else{
		value = (value-min)/(max-min);
	}
	return value;
}


#endif /* TEXTUREFILTER_H_ */
