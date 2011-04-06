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

/*
 * This class will will hold useful things for doing alignment of meshes
 *
 * @author Oscar Barney
 *
 */

#ifndef ALIGN_TOOLS_H
#define ALIGN_TOOLS_H

#include <QObject>

#include <common/meshmodel.h>
#include <meshlab/glarea.h>

#include <meshlabplugins/edit_pickpoints/pickedPoints.h>
#include <meshlabplugins/edit_align/align/AlignPair.h>

class AlignTools : public QObject
{
	Q_OBJECT

public:
	static const QString FilterName;
	static const QString UseMarkers;
	static const QString AllowScaling;
	static const QString UseICP;
	static const QString StuckMesh;
	static const QString MeshToMove;
	
	//make the default parameter set for this filter
	static void buildParameterSet(MeshDocument &md,RichParameterSet & parlst);

	//setup all the parameters and then call align
	static bool setupThenAlign(MeshModel &mm, RichParameterSet & par);

	/*
	 * stuckModel - the mesh one that stays put
	 * modelToMove - the mesh to be moved into place
	 * modelToMoveGLArea - so we can update the position of the model
	 * parentWidget - the widget that should be the parent of the confirm dialog window
	 * confirm - whether to ask the user if they want the alignment to stay put
	 *
	 * return value - true if align was accepted or successful
	*/
	static bool align(MeshModel *stuckModel, PickedPoints *stuckPickedPoints,
				MeshModel *modelToMove, PickedPoints *modelToMovePickedPoints,
				GLArea *modelToMoveGLArea,
				RichParameterSet &parameters,
				QWidget *parentWidget = 0, bool confirm = false);

	//returns the key applied if this transform is stored to perMeshAttribute
	static const std::string getKey() { return "TransformAppliedKey"; }
	
protected:
	AlignTools();

};

#endif
