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
Revision 1.0 2008/02/20 Alessandro Maione, Federico Bellucci
FIRST RELEASE

****************************************************************************/

#ifndef _FILTER_QUALITY_MAPPER_H_
#define _FILTER_QUALITY_MAPPER_H_

#include <QObject>

#include <common/interfaces.h>

#include "../edit_quality/common/transferfunction.h"
#include <vcg/complex/trimesh/stat.h> // for ComputePerVertexQualityMinMax
#include "../edit_quality/common/meshmethods.h"


using namespace vcg;
using namespace std;


  class Frange
  {
  public:
    Frange(){}
    Frange(std::pair<float,float> minmax):minV(minmax.first),maxV(minmax.second){}
    Frange(float _min,float _max):minV(_min),maxV(_max){}

    float minV;
    float maxV;
  };


class QualityMapperFilter : public QObject, public MeshFilterInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshFilterInterface)

private:
	Frange _meshMinMaxQuality;

public:
	enum { FP_QUALITY_MAPPER  } ;

	QualityMapperFilter();
	
	virtual QString filterName(FilterIDType filter) const;
	virtual QString filterInfo(FilterIDType filter) const;
	int getPreConditions(QAction *) const;
	int postCondition( QAction* ) const;
  virtual void initParameterSet(QAction *,MeshModel &/*m*/, RichParameterSet & /*parent*/);
    virtual bool applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & /*parent*/, vcg::CallBackPos * cb) ;
	virtual FilterClass getClass(QAction *);
};

#endif
