/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2009                                          \/)\/    *
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

#ifndef _EPOCH_RECONSTRUCTION_H
#define _EPOCH_RECONSTRUCTION_H

#include <QtXml>

#include <common/meshmodel.h>

#include "radial_distortion.h"
#include "epoch_camera.h"
#include "scalar_image.h"


class EpochModel
{
public:
  int index;
  QString cameraName;
  QString maskName;
  QString depthName;
  QString textureName;
  QString countName;
  vcg::EpochCamera cam;
  bool Init(QDomNode &node);
  static QString ThumbName(QString &imageName);

	bool BuildMesh(CMeshO &m, int subsampleFactor, int minCount, float minAngleCos, int smoothSteps,
		bool dilation, int dilationPasses, int dilationSize, bool erosion, int erosionPasses, int erosionSize,float scalingFactor);
  void SmartSubSample(int subsampleFactor, FloatImage &fli, CharImage &chi, FloatImage &subD,FloatImage &subQ, int minCount);
  void AddCameraIcon(CMeshO &m);
  bool CombineHandMadeMaskAndCount(CharImage &qualityImg, QString maskName );
  void GenerateCountImage();
  void GenerateGradientSmoothingMask(int subsampleFactor, QImage &OriginalTexture, CharImage &mask);
  void Laplacian2(FloatImage &depth, FloatImage &Q, int minCount, CharImage &mask, float depthThr);
  float ComputeDepthJumpThr(FloatImage &depthImgf, float percentile);
	void depthFilter(FloatImage &depthImgf, FloatImage &countImgf, float depthJumpThr, 
		bool dilation, int dilationNumPasses, int dilationWinsize, bool erosion, int erosionNumPasses, int erosionWinsize);

  QIcon *getIcon();
};

class EpochReconstruction
{
 public:
  QString name, author, created;
  QList<EpochModel> modelList;
};

#endif