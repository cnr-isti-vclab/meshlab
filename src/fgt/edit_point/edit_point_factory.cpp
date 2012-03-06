/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005-2008                                           \/)\/    *
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

#include "edit_point_factory.h"
#include "edit_point.h"

PointEditFactory::PointEditFactory()
{
        editPoint = new QAction(QIcon(":/images/select_vertex_geodesic.png"),"Select Vertex Clusters", this);
        editPointFittingPlane = new QAction(QIcon(":/images/select_vertex_plane.png"),"Select Vertexes on a Plane", this);
	
        actionList << editPoint;
        actionList << editPointFittingPlane;
	
	foreach(QAction *editAction, actionList)
                editAction->setCheckable(true);
}
	
//gets a list of actions available from this plugin
QList<QAction *> PointEditFactory::actions() const
{
	return actionList;
}

//get the edit tool for the given action
MeshEditInterface* PointEditFactory::getMeshEditInterface(QAction *action)
{
        if(action == editPoint)
            return new EditPointPlugin(EditPointPlugin::SELECT_DEFAULT_MODE);
        else if (action == editPointFittingPlane)
            return new EditPointPlugin(EditPointPlugin::SELECT_FITTING_PLANE_MODE);

        assert(0); //should never be asked for an action that isn't here
}

QString PointEditFactory::getEditToolDescription(QAction *)
{
        return EditPointPlugin::Info();
}

Q_EXPORT_PLUGIN(PointEditFactory)
