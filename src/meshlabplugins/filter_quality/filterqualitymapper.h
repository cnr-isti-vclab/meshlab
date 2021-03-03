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

#include <common/plugins/interfaces/filter_plugin.h>

#include "../edit_quality/common/transferfunction.h"
#include <vcg/complex/algorithms/stat.h> // for ComputePerVertexQualityMinMax
#include "../edit_quality/common/meshmethods.h"


using namespace vcg;
using namespace std;


class Frange
{
public:
    Frange(){}
    Frange(std::pair<MESHLAB_SCALAR,MESHLAB_SCALAR> minmax):minV(minmax.first),maxV(minmax.second){}
    Frange(MESHLAB_SCALAR _min,MESHLAB_SCALAR _max):minV(_min),maxV(_max){}

    MESHLAB_SCALAR minV;
    MESHLAB_SCALAR maxV;
};


class QualityMapperFilter : public QObject, public FilterPlugin
{
	Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(FILTER_PLUGIN_IID)
	Q_INTERFACES(FilterPlugin)

private:
    Frange _meshMinMaxQuality;

public:
    enum { FP_QUALITY_MAPPER  } ;

    QualityMapperFilter();

    QString pluginName() const;
    virtual QString filterName(ActionIDType filter) const;
    virtual QString filterInfo(ActionIDType filter) const;
    int getPreConditions(const QAction *) const;
    int postCondition(const QAction* ) const;
    virtual void initParameterList(const QAction*, MeshModel &/*m*/, RichParameterList & /*parent*/);
    virtual bool applyFilter(const QAction* filter, MeshDocument &md, std::map<std::string, QVariant>& outputValues, unsigned int& postConditionMask, const RichParameterList & /*parent*/, vcg::CallBackPos * cb) ;
    virtual FilterClass getClass(const QAction*) const;
    FILTER_ARITY filterArity(const QAction *) const {return SINGLE_MESH;}
};

#endif
