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

#ifndef RENDER_RASTER_H
#define RENDER_RASTER_H

#include <QString>
#include <QImage>
#include <QFileInfo>
#include "cmesh.h"

/*
RasterPlane Class
the base class for a registered image that contains the path, the semantic and the data of the image
*/

class RasterPlane
{
public:

    enum PlaneSemantic
    {
      NONE        = 0x0000,
      RGBA        = 0x0001,
      MASK_UB     = 0x0002,
      MASK_F      = 0x0004,
      DEPTH_F     = 0x0008,
      EXTRA00_F        = 0x0100,
      EXTRA01_F        = 0x0200,
      EXTRA02_F        = 0x0400,
      EXTRA03_F        = 0x0800,
      EXTRA00_RGBA     = 0x1000,
      EXTRA01_RGBA     = 0x2000,
      EXTRA02_RGBA     = 0x4000,
      EXTRA03_RGBA     = 0x8000
    };

    int semantic;
    QString fullPathFileName;
    QImage image;
    QImage thumb;
    float *buf;

    bool IsInCore() { return !image.isNull(); }
    void Load();
    void Discard(); //discard  the loaded image freeing the mem.

    /// The whole full path name of the mesh
    const QString fullName() const {return fullPathFileName;}
    /// just the name of the file
    const QString shortName() const { return QFileInfo(fullPathFileName).fileName(); }

    RasterPlane(const RasterPlane& pl);
    RasterPlane(const QString& pathName, const int _semantic);

}; //end class Plane

class MeshLabRenderRaster
{
public:
    MeshLabRenderRaster();
    MeshLabRenderRaster(const MeshLabRenderRaster& rm);
    ~MeshLabRenderRaster();

    Shotm shot;

    ///The list of the registered images
    QList<RasterPlane *> planeList;
    RasterPlane *currentPlane;

    void addPlane(RasterPlane * plane);
};


#endif // RENDER_RASTER_H
