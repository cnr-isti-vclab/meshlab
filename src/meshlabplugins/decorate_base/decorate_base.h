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

#ifndef EXTRADECORATEPLUGIN_H
#define EXTRADECORATEPLUGIN_H

#include <common/interfaces.h>
#include <wrap/gui/coordinateframe.h>
#include "colorhistogram.h"
typedef vcg::ColorHistogram<float>  CHist;


class ExtraMeshDecoratePlugin : public QObject, public MeshDecorateInterface
{
  Q_OBJECT
  Q_INTERFACES(MeshDecorateInterface)
  QString decorationName(FilterIDType filter) const;
  QString decorationInfo(FilterIDType filter) const;

  enum {
      DP_SHOW_FACE_NORMALS,
      DP_SHOW_VERT_NORMALS,
      DP_SHOW_VERT,
      DP_SHOW_EDGE,
      DP_SHOW_NON_FAUX_EDGE,
      DP_SHOW_BOUNDARY,
      DP_SHOW_NON_MANIF_EDGE,
      DP_SHOW_NON_MANIF_VERT,
      DP_SHOW_VERT_PRINC_CURV_DIR,
      DP_SHOW_BOX_CORNERS,
      DP_SHOW_BOX_CORNERS_ABS,
      DP_SHOW_AXIS,
      DP_SHOW_QUOTED_BOX,
      DP_SHOW_VERT_LABEL,
      DP_SHOW_EDGE_LABEL,
      DP_SHOW_VERT_QUALITY_HISTOGRAM,
      DP_SHOW_FACE_QUALITY_HISTOGRAM,
      DP_SHOW_FACE_LABEL,
      DP_SHOW_CAMERA,
      DP_SHOW_TEXPARAM,
      DP_SHOW_BOUNDARY_TEX
  };


private:
	float niceRound2(float value,float base);
	float niceRound(float value);

  void	drawQuotedLine(const vcg::Point3d &a,const vcg::Point3d &b,float aVal, float bVal,float tickDist,QPainter *painter, QFont qf,float angle =0,bool rightAlign=false);

	void	chooseX(vcg::Box3f &box,double *modelview,double *projection,GLint *viewport,vcg::Point3d &x1,vcg::Point3d &x2);
	void	chooseY(vcg::Box3f &box,double *modelview,double *projection,GLint *viewport,vcg::Point3d &y1,vcg::Point3d &y2);
	void	chooseZ(vcg::Box3f &box,double *modelview,double *projection,GLint *viewport,vcg::Point3d &z1,vcg::Point3d &z2);
  void drawHistogram(QGLWidget *gla, CHist &ch);
public:
     
  ExtraMeshDecoratePlugin()
  {
      typeList <<
                  DP_SHOW_VERT <<
                  DP_SHOW_NON_FAUX_EDGE <<
                  DP_SHOW_BOUNDARY <<
                  DP_SHOW_NON_MANIF_EDGE <<
                  DP_SHOW_NON_MANIF_VERT <<
                  DP_SHOW_FACE_NORMALS <<
                  DP_SHOW_VERT_NORMALS <<
                  DP_SHOW_VERT_QUALITY_HISTOGRAM <<
                  DP_SHOW_FACE_QUALITY_HISTOGRAM <<
                  DP_SHOW_VERT_PRINC_CURV_DIR <<
                  DP_SHOW_BOX_CORNERS <<
                  DP_SHOW_BOX_CORNERS_ABS <<
                  DP_SHOW_AXIS <<
                  DP_SHOW_QUOTED_BOX <<
                  DP_SHOW_VERT_LABEL <<
                  DP_SHOW_EDGE_LABEL <<
                  DP_SHOW_FACE_LABEL <<
                  DP_SHOW_CAMERA <<
                  DP_SHOW_TEXPARAM <<
                  DP_SHOW_BOUNDARY_TEX;

      FilterIDType tt;
      foreach(tt , types()){
          actionList << new QAction(decorationName(tt), this);
      }
      QAction *ap;
    foreach(ap,actionList){
        ap->setCheckable(true);
    }
  }

  void DrawBBoxCorner(MeshModel &m, bool absBBoxFlag=true);
  void DrawQuotedBox(MeshModel &m,QPainter *gla, QFont qf);
  void DrawVertLabel(MeshModel &m, QPainter *gla);
  void DrawEdgeLabel(MeshModel &m, QPainter *gla);
  void DrawFaceLabel(MeshModel &m, QPainter *gla);
  void DisplayCamera(MeshModel *m, vcg::Shotf &ls, int cameraSourceId, QPainter *painter, QFont qf);
  void DrawCamera(MeshModel *m, vcg::Shotf &ls, vcg::Color4b camcolor, vcg::Matrix44f &currtr, RichParameterSet *rm, QPainter *painter, QFont qf);
  void PlaceTexParam(int TexInd, int TexNum);
  void DrawTexParam(MeshModel &m, GLArea *gla, QPainter *painter, RichParameterSet *, QFont qf);
  void DrawColorHistogram(CHist &ch, GLArea *gla, QPainter *painter, RichParameterSet *, QFont qf);

  void decorate(QAction *a, MeshDocument &md, RichParameterSet *, GLArea *gla, QPainter *painter);
  bool startDecorate(QAction * /*mode*/, MeshDocument &/*m*/, RichParameterSet *, GLArea * /*parent*/);
  bool isDecorationApplicable(QAction *action, const MeshModel& m, QString &ErrorMessage) const;

  void initGlobalParameterSet(QAction *, RichParameterSet &/*globalparam*/);
  
  inline QString CameraScaleParam() const    { return  "MeshLab::Decoration::CameraRenderScaleType" ; }
  inline QString FixedScaleParam() const     { return  "MeshLab::Decoration::CameraFixedScaleParam" ; }

  inline QString ShowCameraDetails() const     { return  "MeshLab::Decoration::CameraShowCameraDetails" ; }

  inline QString ShowMeshCameras() const     { return  "MeshLab::Decoration::ShowMeshCameras" ; }
  inline QString ShowRasterCameras() const     { return  "MeshLab::Decoration::ShowRasterCameras" ; }

  inline QString ShowNonRegular() const     { return  "MeshLab::Decoration::ShowNonRegular" ; }
  inline QString ShowSeparatrix() const     { return  "MeshLab::Decoration::ShowSeparatrix" ; }
  inline QString ShowBorderFlag() const     { return  "MeshLab::Decoration::ShowBorderFlag" ; }

  inline QString TextureStyleParam() const   { return  "MeshLab::Decoration::TextureStyle" ; }
  inline QString TextureFaceColorParam() const   { return  "MeshLab::Decoration::TextureFaceColor" ; }
  inline QString VertDotSizeParam() const    { return  "MeshLab::Decoration::VertDotSize" ; }
  inline QString HistBinNumParam() const     { return  "MeshLab::Decoration::HistBinNumParam" ; }
  inline QString UseFixedHistParam() const   { return  "MeshLab::Decoration::UseFixedHistParam" ; }
  inline QString FixedHistMinParam() const   { return  "MeshLab::Decoration::FixedHistMinParam" ; }
  inline QString FixedHistMaxParam() const   { return  "MeshLab::Decoration::FixedHistMaxParam" ; }
  inline QString FixedHistWidthParam() const { return  "MeshLab::Decoration::FixedHistWidthParam" ; }
  inline QString AreaHistParam() const { return  "MeshLab::Decoration::AreaHistParam" ; }
  inline QString NormalLength() const { return  "MeshLab::Decoration::NormalLength" ; }

signals:
  void askViewerShot(QString);

public slots:
  void  setValue(QString name, vcg::Shotf val);

private:
  vcg::Shotf curShot;
};

#endif
