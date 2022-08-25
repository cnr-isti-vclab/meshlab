//***************************************************************************
//
//  Copyright (c) 2002 - 2006 Intel Corporation
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
@file IFXSetAdjacencyX.h
	Declaration of IFXSetAdjacencyX interface.
	This interface is used by the progressive geometry compression and decompression.
*/
//*****************************************************************************
#ifndef IFXSETADJACENCYX_H__
#define IFXSETADJACENCYX_H__

#include "IFXUnknown.h"
#include "IFXSetX.h"
#include "IFXAuthorCLODMesh.h"

// {D0BB8355-1F73-4ed4-9E81-B02797BF066F}
IFXDEFINE_GUID(IID_IFXSetAdjacencyX,
0xd0bb8355, 0x1f73, 0x4ed4, 0x9e, 0x81, 0xb0, 0x27, 0x97, 0xbf, 0x6, 0x6f);

/**This interface is used by progressive geometry compression and decompression.*/
class  IFXSetAdjacencyX : public IFXUnknown
{
public:
	virtual void IFXAPI  InitializeX(IFXAuthorCLODMesh* pAuthorCLODMesh) = 0;

	virtual void IFXAPI  AddX(U32 uPositionIndex, U32 uFaceIndex) = 0;
	virtual void IFXAPI  RemoveX(U32 uPositionIndex, U32 uFaceIndex) = 0;

	virtual void IFXAPI  GetFaceSetX(U32 uPositionIndex, IFXSetX*& rpFaceSet) = 0;
	virtual void IFXAPI  GetPositionSetX(IFXSetX& rFaceSet, IFXSetX*& rpPositionSet) = 0;
	virtual void IFXAPI  GetNormalSetX(U32 uPositionIndex, U32 uNumFaces, IFXSetX*& rpNormalSet) = 0;
	virtual void IFXAPI  GetDiffuseColorSetX(U32 uPositionIndex, IFXSetX*& rpDiffuseColorSet) = 0;
	virtual void IFXAPI  GetSpecularColorSetX(U32 uPositionIndex, IFXSetX*& rpSpecularColorSet) = 0;
	virtual void IFXAPI  GetTexCoordSetX(U32 uLayer, U32 uPositionIndex, IFXSetX*& rpTexCoordSet) = 0;
};

#endif //#ifndef IFXSETADJACENCYX_H__
