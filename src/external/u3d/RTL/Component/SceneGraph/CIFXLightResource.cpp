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
	@file	CIFXLightResource.cpp

			Implementation of IFXLightResource interface
*/


#include "IFXSceneGraphPCH.h"
#include "CIFXLightResource.h"
#include "IFXExportingCIDs.h"


#define _HALF_ANGLE		2.0f


CIFXLightResource::CIFXLightResource()
{
	// Setup the default light resource to match the specification which 
	// is an ambient light that is enabled, no specularity, and color values 
	// rgb(0.75, 0.75, 0.75).  Note:  For an ambient light the color is 
	// stored in the diffuse variable.
	m_u8Attributes     = ENABLED;
	m_Light.SetType(IFX_AMBIENT);
	m_Light.SetDiffuse(IFXVector4(0.75f, 0.75f, 0.75f, 1.0f));
	m_Light.SetSpecular(IFXVector4(0.75f, 0.75f, 0.75f, 1.0f));

	// Setup the ambient color ahead of time so that when this light resource 
	// is later possibly changed to a non-AMBIENT type, that it does not 
	// influence the global ambient color.
	m_Light.SetAmbient(IFXVector4(0.0f, 0.0f, 0.0f, 1.0f));

	m_uRefCount = 0;
}


CIFXLightResource::~CIFXLightResource()
{
}


IFXRESULT IFXAPI_CALLTYPE CIFXLightResource_Factory(IFXREFIID riid, void **ppv)
{
	IFXRESULT result;

	if ( ppv )
	{
		// Create the CIFXClassName component.
		CIFXLightResource *pLight = new CIFXLightResource;

		if ( pLight )
		{
			// Perform a temporary AddRef for our usage of the component.
			pLight->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			result = pLight->QueryInterface( riid, ppv );

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the QI fails, this will cause the
			// component to be destroyed.
			pLight->Release();
		}
		else result = IFX_E_OUT_OF_MEMORY;
	}
	else result = IFX_E_INVALID_POINTER;

	return result;
}


// IFXUnknown
U32 CIFXLightResource::AddRef()
{
	return ++m_uRefCount;
}


U32 CIFXLightResource::Release()
{
	if (m_uRefCount == 1)
	{
		delete this ;
		return 0 ;
	}
	else return (--m_uRefCount);
}


IFXRESULT CIFXLightResource::QueryInterface(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT result = IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXLightResource )
			*ppInterface = ( IFXLightResource* ) this;			
		else if ( interfaceId == IID_IFXUnknown )
			*ppInterface = ( IFXUnknown* ) this;
		else if ( interfaceId == IID_IFXMarker )
			*ppInterface = ( IFXMarker* ) this;	
		else if ( interfaceId == IID_IFXMarkerX )
			*ppInterface = ( IFXMarkerX* ) this;	
		else if ( interfaceId == IID_IFXMetaDataX ) 
			*ppInterface = ( IFXMetaDataX* ) this;
		else
		{
			*ppInterface = NULL;
			result = IFX_E_UNSUPPORTED;
		}

		if ( IFXSUCCESS( result ) )
			AddRef();
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}


// IFXMarkerX
void CIFXLightResource::GetEncoderX(IFXEncoderX*& rpEncoderX)
{
	CIFXMarker::GetEncoderX(CID_IFXLightResourceEncoder, rpEncoderX);
}

void CIFXLightResource::SetColor( IFXVector4 vInColor ) 
{ 
	m_Light.SetDiffuse(vInColor);
	m_Light.SetSpecular(vInColor);
	CalculateRange();
}

// Attenuation = 1 / (qd^2 + ld + c)
IFXRESULT CIFXLightResource::SetAttenuation( F32* pAttenuation )
{
	IFXRESULT result = IFX_OK;

	if ( pAttenuation )
	{
		F32 fConstant  = *pAttenuation++;
		F32 fLinear    = *pAttenuation++;
		F32 fQuadradic = *pAttenuation;

		if ( ( fConstant >= 0 )  &&
			 ( fLinear >= 0 )    &&
			 ( fQuadradic >= 0 ) &&
			 ( (fConstant+fLinear+fQuadradic) > 0 ) )
		{
			m_Light.SetAttenuation(IFXVector3(fConstant, fLinear, fQuadradic));
			CalculateRange();
		}
		else
			result = IFX_E_INVALID_RANGE;
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}


IFXRESULT CIFXLightResource::GetAttenuation( F32* pAttenuation )
{
	IFXRESULT result = IFX_OK;

	if ( pAttenuation )
	{
		IFXVector3 vAtten = m_Light.GetAttenuation();
		*pAttenuation++ = vAtten[CONSTANT];
		*pAttenuation++ = vAtten[LINEAR];
		*pAttenuation   = vAtten[QUADRADIC];
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}


IFXRESULT CIFXLightResource::SetIntensity( F32 fIntensity )
{
	m_Light.SetIntensity(fIntensity);
	CalculateRange();
	return IFX_OK;
}


F32 CIFXLightResource::GetIntensity( void )
{
	return m_Light.GetIntensity();
}


F32 CIFXLightResource::GetSpotAngle()
{
	// Return the full spot light angle from cone edge-to-edge.  It's stored 
	// in the IFXRenderLight as the angle between the spot center line and a 
	// cone edge so multiple it by two.
	return m_Light.GetSpotOuterAngle() * _HALF_ANGLE;
}


void CIFXLightResource::SetSpotAngle( F32 fAngle )
{
	// Given a full spot light angle from cone edge-to-edge, setup the 
	// IFXRenderLight which stores only the angle between the spot center 
	// line and a cone edge.  So, divide the supplied angle by two.
	m_Light.SetSpotOuterAngle( fAngle / _HALF_ANGLE );
}


const F32 CIFXLightResource::m_scfMaxExtent = 1e9;
const F32 CIFXLightResource::m_scfAttenuationEpsilon = 0.1f;

void CIFXLightResource::CalculateRange( void )
{
	IFXVector3 atten = m_Light.GetAttenuation();
	F32 fExtent = 0;
	if ( atten[QUADRADIC] > 0.0 )
	{
		fExtent = -m_scfAttenuationEpsilon * atten[LINEAR]; 
		fExtent += (float)sqrt( fExtent * fExtent - 
			4.0f * m_scfAttenuationEpsilon * atten[QUADRADIC] *
			(m_scfAttenuationEpsilon * atten[CONSTANT] - 1.0f) );
		fExtent /= ( 2 * m_scfAttenuationEpsilon * atten[QUADRADIC] );
	}
	else if ( atten[LINEAR] > 0.0 )
		fExtent = (1.0f - m_scfAttenuationEpsilon * atten[CONSTANT]) /
						   (m_scfAttenuationEpsilon * atten[LINEAR]);
	else
		fExtent = m_scfMaxExtent;

	if ( fExtent > m_scfMaxExtent )
		fExtent = m_scfMaxExtent;

	F32 maxIntensity = ((m_Light.GetDiffuse().R()) > (m_Light.GetDiffuse().G()) ? (m_Light.GetDiffuse().R()) : (m_Light.GetDiffuse().G()));
	maxIntensity = (maxIntensity > (m_Light.GetDiffuse().B()) ? maxIntensity : (m_Light.GetDiffuse().B()));
	fExtent *= maxIntensity;
	fExtent *= m_Light.GetIntensity();

	m_Light.SetRange(fExtent);
}
