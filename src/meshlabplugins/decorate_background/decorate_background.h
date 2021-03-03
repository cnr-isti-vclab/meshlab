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
sampledecorate.h,v
Revision 1.1  2007/12/13 00:33:55  cignoni
New small samples

****************************************************************************/
#ifndef SAMPLE_DECORATE_PLUGIN_H
#define SAMPLE_DECORATE_PLUGIN_H

#include <QObject>
#include <QAction>
#include <QList>

#include <common/plugins/interfaces/decorate_plugin.h>
#include <common/ml_shared_data_context/ml_scene_gl_shared_data_context.h>
#include "cubemap.h"

class DecorateBackgroundPlugin : public QObject, public DecoratePlugin
{
  Q_OBJECT
  MESHLAB_PLUGIN_IID_EXPORTER(DECORATE_PLUGIN_IID)
  Q_INTERFACES(DecoratePlugin)
  QString decorationName(FilterIDType id) const;
  QString decorationInfo(FilterIDType id) const;
  QString pluginName() const;

  enum {
    DP_SHOW_CUBEMAPPED_ENV,
    DP_SHOW_GRID
        };

private:
vcg::CICubeMap cm;

inline QString CubeMapPathParam() const { return  "MeshLab::Decoration::CubeMapPath" ; }
inline QString GridBackParam() const { return  "MeshLab::Decoration::GridBack" ; }
inline QString GridMajorParam() const { return  "MeshLab::Decoration::GridMajor" ; }
inline QString GridMinorParam() const { return  "MeshLab::Decoration::GridMinor" ; }
inline QString BoxRatioParam() const { return  "MeshLab::Decoration::BoxRatio" ; }
inline QString ShowShadowParam() const { return  "MeshLab::Decoration::ShowShadow" ; }
inline QString GridColorBackParam() const { return  "MeshLab::Decoration::GridColorBack" ; }
inline QString GridColorFrontParam() const { return  "MeshLab::Decoration::GridColorFront" ; }
inline QString GridBaseLineWidthParam() const { return  "MeshLab::Decoration::GridBaseLineWidth" ; }

public:

DecorateBackgroundPlugin()
{
  typeList
      /*<< DP_SHOW_CUBEMAPPED_ENV*/
      << DP_SHOW_GRID;

  for(FilterIDType tt : types()){
    actionList << new QAction(decorationName(tt), this);
    if(tt==DP_SHOW_GRID)
      actionList.last()->setIcon(QIcon(":/images/show_background_grid.png"));
    actionList.last()->setCheckable(true);
  }
}

    QList<QAction *> actions () const {return actionList;}

  QString cubemapFileName;

  bool startDecorate(const QAction* /*mode*/, MeshDocument &/*m*/, const RichParameterList * /*parent*/ par, GLArea * /*parent*/);
  void decorateDoc(const QAction *a, MeshDocument &md, const RichParameterList *, GLArea *gla, QPainter *, GLLogStream &_log);
  void decorateMesh(const QAction *, MeshModel &, const RichParameterList *, GLArea *, QPainter *, GLLogStream &){}
  void initGlobalParameterList(const QAction *, RichParameterList &/*globalparam*/);
  int getDecorationClass(const QAction * /*action*/) const { return DecoratePlugin::PerDocument; }


private:
  void DrawGriddedCube(MLSceneGLSharedDataContext* shared,QGLContext* cont,MeshModel &m, const Box3m &bb, Scalarm majorTick, Scalarm minorTick, bool backCullFlag, bool shadowFlag, vcg::Color4b frontColor, vcg::Color4b backColor, float baseLineWidth);
  Shotm curShot;

signals:
  void askViewerShot(QString);

public slots:
  void  setValue(QString, Shotm val);

};

#endif
