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

#ifndef FILTER_MUTUALINFO_H
#define FILTER_MUTUALINFO_H

#include <QObject>

#include <common/interfaces.h>
#include "alignset.h"



class QScriptEngine;

class FilterMutualInfoPlugin : public QObject, public MeshFilterInterface
{
	Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(MESH_FILTER_INTERFACE_IID)
	Q_INTERFACES(MeshFilterInterface)

public:

	enum { FP_IMAGE_GLOBALIGN} ;

	FilterMutualInfoPlugin();

	virtual QString pluginName() const;

	QString filterName(FilterIDType filter) const;
	QString filterInfo(FilterIDType filter) const;
	FilterClass getClass(QAction *a);
	FILTER_ARITY filterArity(QAction *) const;
	void initParameterSet(QAction *,MeshDocument & md, RichParameterSet & /*parent*/);
	bool applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & /*parent*/, vcg::CallBackPos * cb) ;
	int postCondition(QAction*) const;
	QString filterScriptFunctionName(FilterIDType filterID);

private:
	bool imageGlobalAlagin(
			MeshDocument &md,
			float thresDiff,
			bool preAlign,
			int maxRefinementSteps,
			bool estimateFocal,
			bool fine,
			int rendmode);

	bool preAlignment(
			MeshDocument &md,
			bool estimateFocal,
			bool fine,
			int rendmode);

	std::vector<SubGraph> buildGraph(MeshDocument &md, bool globalign=true);
	std::vector<AlignPair> calcPairs(MeshDocument &md, bool globalign=true);
	std::vector<SubGraph> createGraphs(MeshDocument &md, std::vector<AlignPair> arcs);
	bool alignGlobal(MeshDocument &md, std::vector<SubGraph> graphs);
	int getTheRightNode(SubGraph graph);
	bool alignNode(MeshDocument &md, Node node);
	bool allActive(SubGraph graph);
	bool updateGraph(MeshDocument &md, SubGraph graph, int n);
	float calcShotsDifference(MeshDocument &md, std::vector<vcg::Shotf> oldShots, std::vector<vcg::Point3f> points);
	void initGL();
};


#endif
