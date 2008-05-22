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
/****************************************************************************
  History
$Log: sampleplugins.h,v $

****************************************************************************/

#ifndef _FILTER_TEXTURE_PLUGIN_H
#define _FILTER_TEXTURE_PLUGIN_H

#include <QObject>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>
#include "rect_packer.h"

class FilterTexturePlugin : public QObject, public MeshFilterInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshFilterInterface)

public:
	enum { FP_MAKE_ATLAS  } ;

	FilterTexturePlugin();
	
	virtual const QString filterName(FilterIDType filter);
	virtual const QString filterInfo(FilterIDType filter);
	virtual const PluginInfo &pluginInfo();

	virtual const FilterClass getClass(QAction *);
	virtual const int getRequirements(QAction *);

	virtual bool autoDialog(QAction *) {return true;}
	virtual void initParameterSet(QAction *,MeshModel &m, FilterParameterSet &parlst);
	virtual bool applyFilter(QAction *filter, MeshModel &m, FilterParameterSet &par, vcg::CallBackPos * cb);
	virtual void maxFaceSpan(float maxdiffUV[][2], MeshModel &m, int &mat, CMeshO::FaceIterator &fit);
	virtual void copyTiles(QPixmap images[], QImage tiledimages[], int &numTextures, int &c, float maxdiffUV[][2], MeshModel &m, int &algo, std::vector<Point2i>& sizes, Point2i &size);
	virtual void positionTextures(bool &made, int &numAtlases, int &sizesIndeces, int &length, std::map<int, int> &whichAtlas, std::vector< std::vector<Point2i> > &splitSizes, Point2i max_size, int &c, std::vector<Point2i> splitsize, std::vector< std::vector<Point2i> > splitPosiz, std::vector<Point2i> splitglobalSizes, std::vector<Point2i> sizes);
	virtual void createAtlas(QImage &atlas, int &index, int &c, QString &filename, bool &result, int &numAtlases, MeshModel &m, int &length, QPixmap images[], std::vector< std::vector<Point2i> > splitPosiz, std::vector<Point2i> splitglobalSizes);
	virtual void adjustUVCoords(int &mat, int &c, int &length, CMeshO::FaceIterator &fit, std::vector< std::vector<Point2i> > splitPosiz, std::vector<Point2i> splitglobalSizes, MeshModel &m, float maxdiffUV[][2], std::map<int, int> &whichAtlas, QPixmap images[]);
};

#endif
