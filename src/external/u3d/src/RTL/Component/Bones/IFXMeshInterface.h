//***************************************************************************
//
//  Copyright (c) 1999 - 2006 Intel Corporation
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
	@file IFXMeshInterface.h
*/

#ifndef IFXMESHINTERFACE_H
#define IFXMESHINTERFACE_H

#include "IFXVertexIndexer.h"

/**
	@note The F32 buffers are assumed to contain 3 values (X,Y,Z).
*/
class IFXMeshInterface
{
public:
	virtual ~IFXMeshInterface() {}
	IFXMeshInterface(void)
	{
		m_meshid = 999999999L;
	}

virtual void    IFXAPI      ChooseMeshIndex(U32 set)    { m_meshid=set; }///<for multi-mesh, choose which we refer to
		U32                 GetMeshIndex(void) const    { return m_meshid; }
virtual U32     IFXAPI      GetNumberMeshes(void) const { return 0; }///<Returns number of meshes in the IFXMeshInterface

		// reading
virtual U32      IFXAPI     GetNumberVertices(void) const =0;
virtual U32      IFXAPI     GetNumberNormals(void) const =0;
virtual U32      IFXAPI     GetNumberTexCoords(void) const { return 0; }///< Always returns 0;
virtual U32      IFXAPI     GetNumberFaces(void) const =0;

					// for CLOD
					/// set current resolution, not total number of vertices
virtual void     IFXAPI     SetNumberVertices(U32 set)  {}///<@note works with current (referred) mesh

virtual U32      IFXAPI     GetMaxNumberVertices(void) const
											{ return GetNumberVertices(); }
virtual U32      IFXAPI     GetMaxNumberNormals(void) const
											{ return GetNumberNormals(); }
virtual U32      IFXAPI     GetMaxNumberTexCoords(void) const
											{ return GetNumberTexCoords(); }
virtual U32      IFXAPI     GetMaxNumberFaces(void) const
											{ return GetNumberFaces(); }

virtual const F32* GetVertexConst(U32 index) const =0;
virtual const F32* GetNormalConst(U32 index) const =0;
virtual const F32* GetTexCoordConst(U32 index) const { return NULL; }///< Always returns NULL;

virtual const U32* GetFaceVerticesConst(U32 index) const =0;
virtual const U32* GetFaceNormalsConst(U32 index) const =0;
virtual const U32* GetFaceTexCoordsConst(U32 index) const   { return NULL; }///< Always returns NULL;

				
virtual BOOL    IFXAPI  CreateMeshes(U32 meshes) { return false; } ///<allocate a given number of empty meshes
virtual BOOL    IFXAPI  CreateMesh(U32 vertices,U32 faces)  { return false; }///<allocate space for mesh (use ChooseMeshIndex() to pick)
virtual BOOL    IFXAPI  ResizeMesh(U32 vertices,U32 faces)  { return false; }///<change space for mesh and move data as necessary

virtual void    IFXAPI  SetVertex(U32 index, const F32 vert[3]) =0;
virtual void    IFXAPI  SetNormal(U32 index, const F32 vert[3]) =0;
virtual void    IFXAPI  SetTexCoord(U32 index, const F32 vert[2]) {}

virtual IFXVertexIndexer* GetVertexIndexer(void) { return NULL; }/**< @returns a pointer to internal IFXVertexIndexer object*/

virtual void     IFXAPI     SetFaceVertices(U32 index, const U32 vertID[3])         {}///< zero function body;
virtual void     IFXAPI     SetFaceNormals(U32 index, const U32 normID[3])          {}///< zero function body;
virtual void     IFXAPI     SetFaceTexCoords(U32 index, const U32 coordID[3])       {}///< zero function body;

protected:
	U32 GetMeshID(void) const { return m_meshid; } ///< Returns object's mesh ID

private:
	U32 m_meshid;
};

#endif
