//***************************************************************************
//
//  Copyright (c) 2001 - 2006 Intel Corporation
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//***************************************************************************

/**
	@file	IFXMeshGroup.h
*/

#ifndef IFXMESHGROUP_H
#define IFXMESHGROUP_H

#include "IFXUnknown.h"
#include "IFXRenderable.h"
#include "IFXEnums.h"

class IFXBoundHierarchy;
class IFXMesh;
class IFXInterleavedData;

IFXDEFINE_GUID(IID_IFXMeshGroup,
0x3d825567, 0x276b, 0x4eaa, 0x9c, 0x8d, 0x4f, 0xd, 0x94, 0x8c, 0xab, 0xcf);

/** 
	The IFXMeshGroup class groups multiple IFXMeshes. Typically a MeshGroup represents
	a single object that was partitioned into multiple IFXMeshes.

	The MeshGroup stores only pointers to meshes, so a single mesh may be shared by
	many different MeshGroups.
*/
class IFXMeshGroup : virtual public IFXRenderable
{
public:
	/// Allocates a group of IFXMeshes.
	virtual IFXRESULT IFXAPI Allocate(U32 numMeshes) = 0;

	///	Returns a pointer to the IFXMeshGroup.
	using IFXRenderable::TransferData;
	virtual IFXRESULT IFXAPI TransferData(
									IFXMeshGroup& rMeshGroup, 
									IFXMeshAttributes transferAttr, 
									IFXMeshAttributes copyAttr, 
									BOOL isCopyShaders) = 0;

	///	Gets the number of meshes in the IFXMeshGroup.
	virtual U32 IFXAPI GetNumMeshes() = 0;

	/**
		Returns a pointer to a specified mesh within the mesh group. 
		May return NULL if SetMesh() has not been called previously with a 
		valid pointer.
	*/
	virtual IFXRESULT IFXAPI GetMesh(U32 meshNum, IFXMesh*& pMesh) = 0;

	///	Sets a specified mesh pointer within the mesh group.
	virtual IFXRESULT IFXAPI SetMesh(U32 meshNum, IFXMesh* pMesh) = 0;

	///	Calculates bounding sphere.
	virtual const IFXVector4& IFXAPI CalcBoundingSphere() = 0;

	///	Reverses normals.
	virtual IFXRESULT IFXAPI ReverseNormals() = 0;

	///	Gets the bounding hierarchy.
	virtual IFXRESULT IFXAPI GetBoundHierarchy(
									IFXBoundHierarchy** ppOutBoundHierarchy) = 0;
};
typedef IFXSmartPtr<IFXMeshGroup> IFXMeshGroupPtr;

#endif
