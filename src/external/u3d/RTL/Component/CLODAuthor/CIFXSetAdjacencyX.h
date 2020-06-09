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
@file CIFXSetAdjacencyX.h
	Declaration of CIFXSetAdjacencyX class.  
	This class is used by the progressive geometry compression and decompression.
*/
//*****************************************************************************
#ifndef CIFXSETADJACENCYX_H__
#define CIFXSETADJACENCYX_H__

#include "IFXSetAdjacencyX.h"
#include "IFXAutoRelease.h"

class  CIFXSetAdjacencyX : public IFXSetAdjacencyX 
{
public:
	// IFXUnknown methods...
	virtual U32 IFXAPI  AddRef( void );
	virtual U32 IFXAPI  Release( void );
	virtual IFXRESULT IFXAPI  QueryInterface( IFXREFIID	interfaceId, void**	ppInterface );

	// IFXSetAdjacencyX
	virtual void IFXAPI  InitializeX(IFXAuthorCLODMesh* pAuthorCLODMesh) ;
	virtual void IFXAPI  AddX(U32 uPositionIndex, U32 uFaceIndex) ;
	virtual void IFXAPI  RemoveX(U32 uPositionIndex, U32 uFaceIndex) ;
	virtual void IFXAPI  GetFaceSetX(U32 uPositionIndex, IFXSetX*& rpFaceSet) ;
	virtual void IFXAPI  GetPositionSetX(IFXSetX& rFaceSet, IFXSetX*& rpPositionSet) ;
	virtual void IFXAPI  GetNormalSetX(U32 uPositionIndex, U32 uNumFaces, IFXSetX*& rpNormalSet) ;
	virtual void IFXAPI  GetDiffuseColorSetX(U32 uPositionIndex, IFXSetX*& rpDiffuseColorSet);
	virtual void IFXAPI  GetSpecularColorSetX(U32 uPositionIndex, IFXSetX*& rpSpecularColorSet);
	virtual void IFXAPI  GetTexCoordSetX(U32 uLayer, U32 uPositionIndex, IFXSetX*& rpTexCoordSet);

	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXSetAdjacencyX_Factory( IFXREFIID	interfaceId, 
											void**		ppInterface );
private:
	CIFXSetAdjacencyX();
	virtual ~CIFXSetAdjacencyX();

	U32 m_uRefCount;

	IFXDECLAREMEMBER(IFXAuthorCLODMesh,m_pAuthorCLODMesh);
	IFXSetX** m_ppPositionSetArray;
	U32 m_uPositionSetArraySize;

};

#endif //#ifndef CIFXSETADJACENCYX_H__
