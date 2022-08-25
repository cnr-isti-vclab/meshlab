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
	@file	IFXMeshCompiler.h

			The mesh compiler converts author mesh into ifx mesh.
			This compiler handles static or streaming CLOD meshes.
			The compiler also handles meshes that a have a static low 
			resolution portion which is followed streaming high resolution
			portion. This is controlled by settings on the AuthorMesh which
			must be made before calling the compiler.
*/

#ifndef IFXMeshCompiler_H
#define IFXMeshCompiler_H

#include "IFXAuthorCLODMesh.h"
#include "IFXAuthorMeshMap.h"
#include "IFXUpdatesGroup.h"
#include "IFXMesh.h"

class IFXMeshMap;

// {5D733AC1-0F9A-11d4-BE4C-00A0C9D6AB25}
IFXDEFINE_GUID(IID_IFXMeshCompiler,
0x5d733ac1, 0xf9a, 0x11d4, 0xbe, 0x4c, 0x0, 0xa0, 0xc9, 0xd6, 0xab, 0x25);

/**
	An array of these (one for each material in the AuthorMesh)
	are passed to the run-time mesh compiler so that it can allocate it's 
	outputs to the right size before the mesh has finished streaming. 
	These structures are allocated and filled in at author time by calling 
	PreviewCompile.
*/
struct IFXMeshSize
{
  IFXMeshSize();
  IFXVertexAttributes VertexAttributes;
  U32 NumVertices;
  U32 NumFaces;
  U32 NumResolutionChanges;
  U32 NumFaceUpdates;
};

/**
	The mesh compiler converts AuthorMesh into IFXMesh. This compiler handles static
	or streaming CLOD meshes. The compiler also handles meshes that a have a static low
	resolution portion which is followed streaming high resolution portion.  This is
	controlled by settings on the AuthorMesh which must be made before calling the 
	compiler.
*/
class IFXMeshCompiler : public IFXUnknown
{
public:

    /** This is called at export time to determine the size of the output IFXMesh
    and to compute the author to ifx mesh mappings.  It also produces the IFXMesh.
    To specify a fully static compile the min resolution of the AuthorMesh should be
    set the same as the max resolution.*/
    virtual IFXRESULT IFXAPI PreviewCompile(IFXAuthorCLODMesh* pInputMesh) = 0;

    /** This is called at playback time on the client to initialize
    the compiler for an incremental streaming style compilation
    or a static compilation. The ouput size information should come from the export time
    call to GetIFXMeshSizes after doing the PreviewCompile. */
    virtual IFXRESULT IFXAPI InitCompile(
								IFXAuthorCLODMesh*,
								U32 numOutputSizes,
								IFXMeshSize* OutputSizeArray) = 0;

    /** 
	This is typically called many times to incrementally convert the mesh to IFX format.
    This should only be called after calling InitCompile.
    Converts available AuthorMesh data to IFXMesh.
    The available amount is indicated by calling SetMaxResolution on the AuthorMesh
    that was passed in during InitStreamCompile.*/
    virtual IFXRESULT IFXAPI Compile() = 0;

    /** This method should be called after or during compilation.
    It always returns information relating to the current or most recent compilation.
    It returns NULL if preview or init have never been called.*/
    virtual IFXMeshGroup* IFXAPI GetMeshGroup() = 0;

    /** This method should be called after or during compilation.
    It always returns information relating to the current or most recent compilation.
    It returns NULL if preview or init have never been called.*/
    virtual IFXUpdatesGroup* IFXAPI GetUpdatesGroup() = 0;

    /** Gets the Author-to-IFXMesh mapping. */
    virtual IFXMeshMap* IFXAPI GetMeshMap() = 0;

    /** Gets the IFXMesh sizes. */
    virtual IFXRESULT IFXAPI GetIFXMeshSizes(
									U32* pNumSizes,
									IFXMeshSize** ppMeshSizes)  = 0;

    virtual void IFXAPI SetCosMaxNormalError(F32 fCosMaxNormalError) = 0;

    virtual F32 IFXAPI GetCosMaxNormalError() = 0;
};

IFXINLINE IFXMeshSize::IFXMeshSize()
{
//  VertexAttributes = 0;
	NumVertices = 0;
	NumFaces = 0;
	NumFaceUpdates = 0;
	NumResolutionChanges = 0;
}

#endif
