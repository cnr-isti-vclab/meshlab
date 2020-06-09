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
    @file	IFXAuthorMesh.h

			Declaration of IFXAuthorMesh interface
*/

#ifndef IFXAuthorCLODAccess_H
#define IFXAuthorCLODAccess_H

#include "IFXAuthorCLODMesh.h"

// {C9659981-6249-4915-9F96-97168C41A440}
IFXDEFINE_GUID(IID_IFXAuthorCLODAccess,
0xC9659981, 0x6249, 0x4915, 0x9F, 0x96, 0x97, 0x16, 0x8C, 0x41, 0xA4, 0x40);

class IFXAuthorCLODAccess : public IFXAuthorCLODMesh
{
public:
		virtual IFXRESULT IFXAPI  SetPositionArray(IFXVector3*, U32)		= 0;
		virtual IFXRESULT IFXAPI  SetNormalArray(IFXVector3*, U32)			= 0;
		virtual IFXRESULT IFXAPI  SetDiffuseColorArray(IFXVector4*, U32)	= 0;
		virtual IFXRESULT IFXAPI  SetSpecularColorArray(IFXVector4*, U32)	= 0;
		virtual IFXRESULT IFXAPI  SetTexCoordArray(IFXVector4*, U32)		= 0;
		
		virtual IFXRESULT IFXAPI  SetPositionFaces(IFXAuthorFace*)			= 0;
		virtual IFXRESULT IFXAPI  SetNormalFaces(IFXAuthorFace*)			= 0;
		virtual IFXRESULT IFXAPI  SetDiffuseFaces(IFXAuthorFace*)			= 0;
		virtual IFXRESULT IFXAPI  SetSpecularFaces(IFXAuthorFace*)			= 0;
		virtual IFXRESULT IFXAPI  SetTexFaces(U32, IFXAuthorFace*)			= 0;
		virtual IFXRESULT IFXAPI  SetFaceMaterials(U32*)					= 0;
		virtual IFXRESULT IFXAPI  SetMaxNumFaces(U32)						= 0;
};


#endif
