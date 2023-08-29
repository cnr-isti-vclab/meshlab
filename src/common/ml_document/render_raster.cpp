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

#include "render_raster.h"

RasterPlane::RasterPlane(const RasterPlane& pl)
{
    semantic = pl.semantic;
    fullPathFileName = pl.fullPathFileName;
    image = QImage(pl.image);
}

RasterPlane::RasterPlane(const QString& pathName, const int _semantic)
{
    semantic =_semantic;
    fullPathFileName = pathName;

    image = QImage(pathName);
}

RasterPlane::RasterPlane(
        const QImage& img,
        const QString& pathName,
        const int _semantic)
{
    semantic =_semantic;
    fullPathFileName = pathName;
    image = img;
}

MeshLabRenderRaster::MeshLabRenderRaster()
{

}

MeshLabRenderRaster::MeshLabRenderRaster( const MeshLabRenderRaster& rm )
    :shot(rm.shot)
{
    for(QList<RasterPlane*>::const_iterator it = rm.planeList.begin();it != rm.planeList.end();++it)
    {
        planeList.push_back(new RasterPlane(**it));
        if (rm.currentPlane == *it)
            currentPlane = planeList[planeList.size() - 1];
    }
}

void MeshLabRenderRaster::addPlane(RasterPlane *plane)
{
    planeList.append(plane);
    currentPlane = plane;
}

MeshLabRenderRaster::~MeshLabRenderRaster()
{
    currentPlane = NULL;
    for(int ii = 0;ii < planeList.size();++ii)
        delete planeList[ii];
}
