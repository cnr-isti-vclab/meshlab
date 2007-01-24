#ifndef _EPOCH_RECONSTRUCTION_H
#define _EPOCH_RECONSTRUCTION_H

#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QtXml/QDomNode>

#include <meshlab/meshmodel.h>

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
  EpochCamera cam;
  bool Init(QDomNode &node);
  static QString ThumbName(QString &imageName);

  bool BuildMesh(CMeshO &m, int subsample, int count, float minQuality,int smoothSteps);
  void SmartSubSample(int subsampleFactor, FloatImage &fli, CharImage &chi, FloatImage &subD,FloatImage &subQ, int minCount);
  void AddCameraIcon(CMeshO &m);
  bool CombineHandMadeMaskAndCount(CharImage &qualityImg, QString maskName );
  void GenerateCountImage();
  void GenerateGradientSmoothingMask(int subsampleFactor, QImage &OriginalTexture, CharImage &mask);
  void Laplacian2(FloatImage &depth, FloatImage &Q, int minCount, CharImage &mask, float depthThr);
  float ComputeDepthJumpThr(FloatImage &depthImgf, float percentile);
	void depthFilter(FloatImage &depthImgf, FloatImage &countImgf,float depthThr);

  QIcon *getIcon();
};

class EpochReconstruction
{
 public:
  QString name, author, created;
  QList<EpochModel> modelList;
};

#endif