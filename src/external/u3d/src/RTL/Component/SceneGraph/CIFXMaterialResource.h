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
//  CIFXMaterialResource.h
//
//	DESCRIPTION
//
//	NOTES
//
//***************************************************************************
#ifndef __CIFXMATERIALRESOURCE_H__
#define __CIFXMATERIALRESOURCE_H__

#include "IFXMaterialResource.h"
#include "IFXVector4.h"
#include "CIFXSubject.h"
#include "CIFXMarker.h"
#include "IFXCoreCIDs.h"

class CIFXMaterialResource : private CIFXSubject,
                             private CIFXMarker,
                     virtual public  IFXMaterialResource
{
	          CIFXMaterialResource();
	virtual  ~CIFXMaterialResource();
	friend 
	IFXRESULT IFXAPI_CALLTYPE CIFXMaterialResource_Factory(IFXREFIID iid, void** ppv);

public:
	// IFXUnknown
	U32 IFXAPI  AddRef ();
	U32 IFXAPI  Release ();
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID riid, void **ppv);


	// IFXMarkerX interface...
	void IFXAPI 		GetEncoderX (IFXEncoderX*& rpEncoderX);

	// IFXMaterialResource
	U32       IFXAPI GetAttributes(void) { return m_uAttributes; };
	void      IFXAPI SetAttributes(U32 uIn) { m_uAttributes = uIn; };

	IFXRESULT IFXAPI GetAmbient(IFXVector4*);
	IFXRESULT IFXAPI SetAmbient(IFXVector4);

	IFXRESULT IFXAPI GetDiffuse(IFXVector4*);
	IFXRESULT IFXAPI SetDiffuse(IFXVector4);

	IFXRESULT IFXAPI GetSpecular(IFXVector4*);
	IFXRESULT IFXAPI SetSpecular(IFXVector4);

	IFXRESULT IFXAPI GetEmission(IFXVector4*);
	IFXRESULT IFXAPI SetEmission(IFXVector4);

	IFXRESULT IFXAPI GetOpacity(F32*);
	IFXRESULT IFXAPI SetOpacity(F32);

	IFXRESULT IFXAPI GetTransparent(BOOL*);
	IFXRESULT IFXAPI SetTransparent(BOOL);

	IFXRESULT IFXAPI GetReflectivity(F32*);
	IFXRESULT IFXAPI SetReflectivity(F32);

private:
	// IFXUnknown
	U32 m_uRefCount;

	// IFXMaterialResource
	U32         m_uAttributes;

	IFXVector4 m_vAmbient;
	IFXVector4 m_vDiffuse;
	IFXVector4 m_vSpecular;
	IFXVector4 m_vEmission;
	F32         m_fOpacity;
	F32         m_fReflectivity;

	BOOL		m_bTransparent;

	static const IFXVector4 mc_vAmbientDisabled;
	static const IFXVector4 mc_vDiffuseDisabled;
	static const IFXVector4 mc_vSpecularDisabled;
	static const IFXVector4 mc_vEmissiveDisabled;
	static const F32 mc_fReflectivityDisabled;
	static const F32 mc_fOpacityDisabled;
};


IFXFORCEINLINE IFXRESULT CIFXMaterialResource::GetAmbient(IFXVector4* pAmbient)
{
	if ( pAmbient )	
	{
		if(IFXMaterialResource::AMBIENT & m_uAttributes) 
		{
			*pAmbient = m_vAmbient;
		} 
		else 
		{
			*pAmbient = mc_vAmbientDisabled;
		}
		return IFX_OK;
	}
	else return IFX_E_INVALID_POINTER;
}

IFXFORCEINLINE IFXRESULT CIFXMaterialResource::GetDiffuse(IFXVector4* pDiffuse)
{
	if ( pDiffuse ) 
	{
		if(IFXMaterialResource::DIFFUSE & m_uAttributes) 
		{
            *pDiffuse = m_vDiffuse;
		} 
		else 
		{
			*pDiffuse = mc_vDiffuseDisabled;
		}
		return IFX_OK;
	}
	else return IFX_E_INVALID_POINTER;
}

IFXFORCEINLINE IFXRESULT CIFXMaterialResource::GetEmission(IFXVector4* pEmission)
{
	if ( pEmission ) 
	{
		if(IFXMaterialResource::EMISSIVE & m_uAttributes) 
		{
            *pEmission = m_vEmission;
		} 
		else 
		{
			*pEmission = mc_vEmissiveDisabled;
		}
		return IFX_OK;
	}
	else return IFX_E_INVALID_POINTER;
}


IFXFORCEINLINE IFXRESULT CIFXMaterialResource::GetOpacity(F32* pOpacity)
{
	if ( pOpacity )	
	{
		if(IFXMaterialResource::OPACITY & m_uAttributes) 
		{
            *pOpacity = m_fOpacity;
		} 
		else 
		{
			*pOpacity = mc_fOpacityDisabled;
		}
		return IFX_OK;
	}
	else return IFX_E_INVALID_POINTER;
}


IFXFORCEINLINE IFXRESULT CIFXMaterialResource::GetReflectivity(F32* pReflect)
{
	if ( pReflect )	
	{
		if( IFXMaterialResource::REFLECTIVITY & m_uAttributes) 
		{
            *pReflect = m_fReflectivity;
		} 
		else 
		{
			*pReflect = mc_fReflectivityDisabled;
		}
		return IFX_OK;
	}
	else return IFX_E_INVALID_POINTER;
}

IFXFORCEINLINE IFXRESULT CIFXMaterialResource::GetTransparent(BOOL* pTransparent)
{
	if ( pTransparent )
	{
		*pTransparent = m_bTransparent;
		return IFX_OK;
	}
	else return IFX_E_INVALID_POINTER;
}


IFXFORCEINLINE IFXRESULT CIFXMaterialResource::GetSpecular(IFXVector4* pSpecular)
{
	if ( pSpecular ) 
	{
		if(IFXMaterialResource::SPECULAR & m_uAttributes) 
		{
            *pSpecular = m_vSpecular;
		} 
		else 
		{
			*pSpecular = mc_vSpecularDisabled;
		}
		return IFX_OK;
	}
	else return IFX_E_INVALID_POINTER;
}


IFXFORCEINLINE IFXRESULT CIFXMaterialResource::SetAmbient(IFXVector4 vAmbient)
{
	m_vAmbient = vAmbient;
	return IFX_OK;
}


IFXFORCEINLINE IFXRESULT CIFXMaterialResource::SetDiffuse(IFXVector4 vDiffuse)
{
	m_vDiffuse = vDiffuse;
	return IFX_OK;
}


IFXFORCEINLINE IFXRESULT CIFXMaterialResource::SetEmission(IFXVector4 vEmission)
{
	m_vEmission = vEmission;
	return IFX_OK;
}

IFXFORCEINLINE IFXRESULT CIFXMaterialResource::SetOpacity(F32 fOpacity)
{
	if (( fOpacity < 0 )||( fOpacity > 1 ))
		return IFX_E_INVALID_RANGE;

	m_fOpacity = fOpacity;
	return IFX_OK;
}

IFXFORCEINLINE IFXRESULT CIFXMaterialResource::SetTransparent(BOOL bTransparent)
{
	m_bTransparent = bTransparent;
	return IFX_OK;
}

IFXFORCEINLINE IFXRESULT CIFXMaterialResource::SetReflectivity(F32 fReflect)
{
	if (( fReflect < 0 )||( fReflect > 1 ))
		return IFX_E_INVALID_RANGE;

	m_fReflectivity = fReflect;
	return IFX_OK;
}

IFXFORCEINLINE IFXRESULT CIFXMaterialResource::SetSpecular(IFXVector4 vSpecular)
{
	m_vSpecular = vSpecular;
	return IFX_OK;
}

#endif
