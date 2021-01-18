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

#ifndef FILTER_IMG_PATCH_PARAM_PLUGIN__PATCH_H
#define FILTER_IMG_PATCH_PARAM_PLUGIN__PATCH_H




#include <common/ml_document/raster_model.h>


struct TriangleUV
{
    vcg::TexCoord2f v[3];
};

struct Patch
{
    RasterModel             *ref;
    std::vector<CFaceO*>    faces;
    std::vector<CFaceO*>    boundary;
    std::vector<TriangleUV> boundaryUV;
    vcg::Box2f              bbox;
    vcg::Matrix44f          img2tex;
    bool                    valid;
};

typedef QVector<Patch>              PatchVec;
typedef QMap<RasterModel*,PatchVec> RasterPatchMap;




#endif // FILTER_IMG_PATCH_PARAM_PLUGIN__PATCH_H
