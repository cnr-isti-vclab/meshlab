/*
 * TextureMerger.h
 *
 *  Created on: Aug 13, 2008
 *      Author: racoon
 */

#ifndef TEXTUREMERGER_H_
#define TEXTUREMERGER_H_

#include <QtGui>
#include <QImage>
#include <src/TextureFilter.h>

class ImageFilterContainer{
public:
	QImage *image;
	QList<TextureFilter*> filterList;
	bool merged;
	TextureFilter *mergedFilter;
	ImageFilterContainer();
	~ImageFilterContainer();

	void setImage(QImage* img);

	void addFilter(TextureFilter *filter);
	void mergeFilter();

	void applyMergedFilterToImage();
};


class TextureMerger{

public:
	bool normalized;
	QList<ImageFilterContainer*> ifcList;
	TextureMerger();
	~TextureMerger();

	void normalizeFilterContainerList();
	QImage mergeTextureImagesWinnerTakesAll(int imgWidth, int imgHeight);

};

#endif /* TEXTUREMERGER_H_ */
