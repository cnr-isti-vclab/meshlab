/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005-2021                                           \/)\/    *
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

#ifndef GLTF_LOADER_H
#define GLTF_LOADER_H

#include "tinygltf_include.h"
#include "callback_progress.h"

#include <common/ml_document/mesh_model.h>

namespace gltf {

unsigned int getNumberMeshes(
		const tinygltf::Model& model);

void loadMeshes(
		const std::list<MeshModel*>& meshModelList,
		std::list<int>& maskList,
		const tinygltf::Model& model,
		vcg::CallBackPos* cb = nullptr);

namespace internal {

enum GLTF_ATTR_TYPE {POSITION, NORMAL, COLOR_0, TEXCOORD_0, INDICES};
const std::array<std::string, 4> GLTF_ATTR_STR {"POSITION", "NORMAL", "COLOR_0", "TEXCOORD_0"};

unsigned int getNumberMeshes(
		const tinygltf::Model& model,
		unsigned int node);

void loadMeshesWhileTraversingNodes(
		const tinygltf::Model& model,
		std::list<MeshModel*>::const_iterator& currentMesh,
		std::list<int>::iterator& currentMask,
		Matrix44m currentMatrix,
		unsigned int currentNode,
		vcg::CallBackPos* cb,
		CallBackProgress& progress);

Matrix44m getCurrentNodeTrMatrix(
		const tinygltf::Model& model,
		unsigned int currentNode);

void loadMesh(
		MeshModel& m,
		int& mask,
		const tinygltf::Mesh& tm,
		const tinygltf::Model& model,
		vcg::CallBackPos* cb,
		CallBackProgress& progress);

void loadMeshPrimitive(
		MeshModel& m,
		int& mask,
		const tinygltf::Model& model,
		const tinygltf::Primitive& p,
		vcg::CallBackPos* cb,
		CallBackProgress& progress);

bool loadAttribute(
		MeshModel& m,
		std::vector<CMeshO::VertexPointer>& ivp,
		const tinygltf::Model& model,
		const tinygltf::Primitive& p,
		GLTF_ATTR_TYPE attr,
		int textID = -1);

template <typename Scalar>
void populateAttr(
		GLTF_ATTR_TYPE attr,
		MeshModel&m,
		std::vector<CMeshO::VertexPointer>& ivp,
		const Scalar* array,
		unsigned int number,
		int textID = -1);

template <typename Scalar>
void populateVertices(
		MeshModel& m,
		std::vector<CMeshO::VertexPointer>& ivp,
		const Scalar* posArray,
		unsigned int vertNumber);

template <typename Scalar>
void populateVNormals(
		const std::vector<CMeshO::VertexPointer>& ivp,
		const Scalar* normArray,
		unsigned int vertNumber);

template <typename Scalar>
void populateVColors(
		const std::vector<CMeshO::VertexPointer>& ivp,
		const Scalar* colorArray,
		unsigned int vertNumber,
		int nElemns);

template <typename Scalar>
void populateVTextCoords(
		const std::vector<CMeshO::VertexPointer>& ivp,
		const Scalar* textCoordArray,
		unsigned int vertNumber,
		int textID);

template <typename Scalar>
void populateTriangles(
		MeshModel&m,
		const std::vector<CMeshO::VertexPointer>& ivp,
		const Scalar* triArray,
		unsigned int triNumber);
}

}


#endif // GLTF_LOADER_H
