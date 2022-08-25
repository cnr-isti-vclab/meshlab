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
//
//  CIFXMaterialResource.cpp
//
//	DESCRIPTION
//
//	NOTES
//
//***************************************************************************
#include "IFXSceneGraphPCH.h"
#include "CIFXMaterialResource.h"
#include "IFXCheckX.h"
#include "IFXEncoderX.h"
#include "IFXException.h"


#include "IFXExportingCIDs.h"

/// @todo: Verify correct values for disabled constants.
const IFXVector4 CIFXMaterialResource::mc_vAmbientDisabled = IFXVector4(0.0f,0.0f,0.0f,1.0f);
const IFXVector4 CIFXMaterialResource::mc_vDiffuseDisabled = IFXVector4(0.0f,0.0f,0.0f,1.0f);
const IFXVector4 CIFXMaterialResource::mc_vSpecularDisabled = IFXVector4(0.0f,0.0f,0.0f,1.0f);
const IFXVector4 CIFXMaterialResource::mc_vEmissiveDisabled = IFXVector4(0.0f,0.0f,0.0f,1.0f);
const F32 CIFXMaterialResource::mc_fReflectivityDisabled = 0.0f;
const F32 CIFXMaterialResource::mc_fOpacityDisabled = 1.0f;

CIFXMaterialResource::CIFXMaterialResource()
{
	// IFXUnknown attributes...
	m_uRefCount = 0;

	// IFXMaterialResource attributes...
	m_uAttributes = AMBIENT | DIFFUSE | SPECULAR | EMISSIVE | OPACITY | REFLECTIVITY;

	IFXVector4 color(0.75,0.75,0.75);
	m_vAmbient = color;

	color.Set(0,0,0);
	m_vDiffuse = color;

	color.Set(0,0,0);
	m_vSpecular = color;

	color.Set(0,0,0);
	m_vEmission = color;

	m_fOpacity = 1;
	m_fReflectivity = 0.0f;

	m_bTransparent = TRUE;
}


CIFXMaterialResource::~CIFXMaterialResource()
{
}


IFXRESULT IFXAPI_CALLTYPE CIFXMaterialResource_Factory(IFXREFIID riid, void** ppv)
{
	IFXRESULT result;

	if ( ppv )
	{
		// Create the CIFXClassName component.
		CIFXMaterialResource *pMaterialResource = new CIFXMaterialResource;

		if ( pMaterialResource )
		{
			// Perform a temporary AddRef for our usage of the component.
			pMaterialResource->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			result = pMaterialResource->QueryInterface( riid, ppv );

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the QI fails, this will cause the
			// component to be destroyed.
			pMaterialResource->Release();
		}
		else result = IFX_E_OUT_OF_MEMORY;
	}
	else result = IFX_E_INVALID_POINTER;

	return result;
}


// IFXUnknown interface...
U32 CIFXMaterialResource::AddRef()
{
	return ++m_uRefCount;
}


U32 CIFXMaterialResource::Release()
{
	if (--m_uRefCount) 
		return m_uRefCount;
	delete this;
	return 0;
}


IFXRESULT CIFXMaterialResource::QueryInterface(IFXREFIID riid, void **ppv)
{
	IFXRESULT result = IFX_OK;

	if ( ppv )
	{
		if ( riid == IID_IFXUnknown )
			*ppv = (IFXUnknown*)this;
		else if ( riid == IID_IFXMaterialResource )
			*ppv = (IFXMaterialResource*)this;
		else if ( riid == IID_IFXMarker )
			*ppv = (IFXMarker*)this;
		else if ( riid == IID_IFXMarkerX )
			*ppv = (IFXMarkerX*)this;
		else if ( riid == IID_IFXMetaDataX ) 
			*ppv = ( IFXMetaDataX* ) this;
		else
		{
			*ppv = NULL;
			result = IFX_E_UNSUPPORTED;
		}

		if( IFXSUCCESS(result) )
			AddRef();
	}
	else result = IFX_E_INVALID_POINTER;

	return result;
}

//-----------------------------------------------------------------------------


// IFXMarkerX interface...
void CIFXMaterialResource::GetEncoderX(IFXEncoderX*& rpEncoderX)
{
	CIFXMarker::GetEncoderX(CID_IFXMaterialResourceEncoder, rpEncoderX);
}

