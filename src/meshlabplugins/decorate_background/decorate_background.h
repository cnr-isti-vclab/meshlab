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

#include <common/interfaces.h>
#include "cubemap.h"
//#include "../../meshlab/mainwindow.h"

class SampleMeshDecoratePlugin : public QObject, public MeshDecorateInterface
{
  Q_OBJECT
  Q_INTERFACES(MeshDecorateInterface)
  QString decorationName(FilterIDType id) const;
  QString decorationInfo(FilterIDType id) const;

  enum {
    DP_SHOW_CUBEMAPPED_ENV,
    DP_SHOW_GRID
		};

private:
vcg::CICubeMap cm;

inline QString CubeMapPathParam() const { return  "MeshLab::Decoration::CubeMapPath" ; }
inline QString GridSnapParam() const { return  "MeshLab::Decoration::GridSnap" ; }
inline QString GridBackParam() const { return  "MeshLab::Decoration::GridBack" ; }
inline QString GridMajorParam() const { return  "MeshLab::Decoration::GridMajor" ; }
inline QString GridMinorParam() const { return  "MeshLab::Decoration::GridMinor" ; }
inline QString BoxRatioParam() const { return  "MeshLab::Decoration::BoxRatio" ; }
inline QString ShowShadowParam() const { return  "MeshLab::Decoration::ShowShadow" ; }
inline QString GridColorBackParam() const { return  "MeshLab::Decoration::GridColorBack" ; }
inline QString GridColorFrontParam() const { return  "MeshLab::Decoration::GridColorFront" ; }

public:
     
	SampleMeshDecoratePlugin()
	{
    typeList
    /*<< DP_SHOW_CUBEMAPPED_ENV*/
    << DP_SHOW_GRID;

    FilterIDType tt;
    foreach(tt , types()){
        actionList << new QAction(decorationName(tt), this);
    }
    QAction *ap;
    foreach(ap,actionList){
        ap->setCheckable(true);
    }
  }

	QList<QAction *> actions () const {return actionList;}

  QString cubemapFileName;
	
  bool startDecorate(QAction * /*mode*/, MeshDocument &/*m*/, RichParameterSet * /*parent*/ par, GLArea * /*parent*/);
  void decorate(QAction *a, MeshDocument &md, RichParameterSet *, GLArea *gla, QPainter *);
	void initGlobalParameterSet(QAction *, RichParameterSet &/*globalparam*/);	
		

private:
  void DrawGriddedCube(MeshModel &m, const vcg::Box3f &bb, float majorTick, float minorTick, bool snapFlag, bool backCullFlag, bool shadowFlag, vcg::Color4b frontColor, vcg::Color4b backColor, GLArea *gla);
  vcg::Shotf curShot;

signals:
  void askViewerShot(QString);

public slots:
  void  setValue(QString name, vcg::Shotf val);

};

#endif
