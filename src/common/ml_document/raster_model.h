/****************************************************************************
* MeshLab                                                           o o     *
* Visual and Computer Graphics Library                            o     o   *
*                                                                _   O  _   *
* Copyright(C) 2004-2020                                           \/)\/    *
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

#ifndef RASTER_MODEL_H
#define RASTER_MODEL_H

#include "render_raster.h"

class MeshDocument;

/*
RasterModel Class
The base class for keeping a set of "registered" images (e.g. images that can be projected onto a 3D space).
Each Raster model is composed by a list of registered images, each image with its own "semantic" (color, depth, normals, quality, masks)
and with all the images sharing the same shot.
*/

class RasterModel : public MeshLabRenderRaster
{
public:
	RasterModel();
	RasterModel(unsigned int id, const QString& _rasterName=QString());
	

	bool isVisible() const {return visible;}
	void setVisible(bool vis = true) {visible = vis;};
	inline unsigned int id() const {return _id;}
	
	void setLabel(QString newLabel) {_label = newLabel;}
	
	const QString label() const {
		if(!_label.isEmpty())  return _label;
		if(!planeList.empty()) return planeList.first()->shortName();
		return "Error!";
	}
	
	enum RasterElement
	{
		RM_NONE		= 0x00000000,
		RM_ALL		= 0xffffffff
	};
	
private:
	bool visible;
	unsigned int _id;
	QString _label;
};// end class RasterModel

#endif // RASTER_MODEL_H
