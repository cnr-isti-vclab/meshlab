/*
 * TextureFilter.h
 *
 *  Created on: Aug 12, 2008
 *      Author: racoon
 */

#ifndef TEXTUREFILTER_H_
#define TEXTUREFILTER_H_

#include <QtGui>
#include <QImage>

class TextureFilter{

private:
	float** value_matrix;

public:
	int weight;
	int type;
	int vm_width;
	int vm_height;

	bool normalized;

	float max;
	float min;

	TextureFilter(int width, int height, int newWeight);
	~TextureFilter();

	void normalize();
	void normalize(float min, float max);

	void SaveAsImage(QString imgFilename, QString tag);

	void setValue(int x, int y, float value);
	float getValue(int x, int y);

};

#endif /* TEXTUREFILTER_H_ */
