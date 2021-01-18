/****************************************************************************
* MeshLab                                                           o o     *
* Visual and Computer Graphics Library                            o     o   *
*                                                                _   O  _   *
* Copyright(C) 2004-2020                                           \/)\/    *
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

#ifndef MESHLAB_MESH_MODEL_STATE_H
#define MESHLAB_MESH_MODEL_STATE_H

#include <vector>
#include "cmesh.h"

class MeshModel;

/*
A class designed to save partial aspects of the state of a mesh, such as vertex colors, current selections, vertex positions
and then be able to restore them later.
This is a fundamental part for the dynamic filters framework.

Note: not all the MeshElements are supported!!
*/
class MeshModelState
{
public:
	// This function save the <mask> portion of a mesh into the private members of the MeshModelState class;
	void create(int _mask, MeshModel* _m);
	bool apply(MeshModel *_m);
	//bool isValid(MeshModel *m);
	int maskChangedAtts() const;
	
private:
	int changeMask; // a bit mask indicating what have been changed. Composed of MeshModel::MeshElement (e.g. stuff like MeshModel::MM_VERTCOLOR)
	MeshModel *m; // the mesh which the changes refers to.
	std::vector<float> vertQuality;
	std::vector<vcg::Color4b> vertColor;
	std::vector<vcg::Color4b> faceColor;
	std::vector<Point3m> vertCoord;
	std::vector<Point3m> vertNormal;
	std::vector<Point3m> faceNormal;
	std::vector<bool> faceSelection;
	std::vector<bool> vertSelection;
	Matrix44m Tr;
	Shotm shot;
};

#endif // MESHLAB_MESH_MODEL_STATE_H
