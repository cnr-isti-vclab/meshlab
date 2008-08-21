#include <cfloat>
#include "TextureFilter.h"

TextureFilter::TextureFilter(int width, int height, int newWeight){

	min = DBL_MAX;
	max = DBL_MIN;
	int x;
	int y;

	vm_height = height;
	vm_width = width;
	weight = newWeight;

	value_matrix=(float**)malloc(width*sizeof(float*));
	for(x=0;x<width;x++){
		value_matrix[x]=(float*)malloc(height*sizeof(float));
		for(y=0;y<height;y++){
			value_matrix[x][y]=FLT_MIN;
		}
	}

	normalized = false;
}
TextureFilter::~TextureFilter(){
	qDebug()<<"TextureFilter::~TextureFilter()";
	int x;

	for(x=0;x<vm_width;x++){
		free(value_matrix[x]);

	}
	free(value_matrix);
}

void TextureFilter::normalize(){
	normalize(min,max);

}

void TextureFilter::normalize(float min, float max){
	qDebug()<< "normalize: " << min << max;
	if(!normalized){
		int x;
		int y;
		for(x=0;x<vm_width;x++){
			for(y=0;y<vm_height;y++){
				if (value_matrix[x][y] == FLT_MIN){
					value_matrix[x][y]=1.0;
				}else{
					value_matrix[x][y] = (value_matrix[x][y]-min)/(max-min);
				}
			}
		}
		normalized = true;
	}
}

void TextureFilter::SaveAsImage(QString imgFilename,QString tag){

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

void TextureFilter::setValue(int x, int y, float value){
	if (value<min){
		min =value;
	}
	if (value>max){
		max = value;
	}

	value_matrix[x][y]=value;
}

float TextureFilter::getValue(int x, int y){

	return value_matrix[x][y];
}
