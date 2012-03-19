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

#ifndef FILTER_MEASURE_H
#define FILTER_MEASURE_H

#include <QObject>

#include <common/interfaces.h>

class FilterMeasurePlugin : public MeshLabFilterInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshLabFilterInterface)

public:

	FilterMeasurePlugin():MeshLabFilterInterface(){}
	bool applyFilter( const QString& filterName,MeshDocument& md,EnvWrap& env, vcg::CallBackPos * cb );
	QTreeWidgetItem *tagDump(TagBase * /*tag*/, MeshDocument &/*md*/, MeshModel *mm=0);
};

class MeasureTopoTag: public TagBase
{
public:
	int edges;
	int boundaryEdges;
	int connectComp;
	bool manifoldness;
	int genus;
	int holes;
	int edgeManifNum;
	int vertManifNum;
	int faceEdgeManif;
	int faceVertManif;

	MeasureTopoTag(MeshDocument &parent, MeshModel *mm, QString name);
};

#endif
