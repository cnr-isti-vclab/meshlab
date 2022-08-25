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

#include "IFXSceneGraphPCH.h"
#include "CIFXMarker.h"
#include "IFXCheckX.h"

CIFXMarker::CIFXMarker()
{
	m_pSceneGraph	= NULL;
	m_bExternalFlag	= FALSE;
	m_uMark			= 0;
	m_uPriority		= 256;
	m_uDefaultQualityFactor = 1000;
	m_uPositionQualityFactor = m_uDefaultQualityFactor;
	m_uTexCoordQualityFactor = m_uDefaultQualityFactor;
	m_uNormalQualityFactor = m_uDefaultQualityFactor;
	m_uDiffuseQualityFactor = m_uDefaultQualityFactor;
	m_uSpecularQualityFactor = m_uDefaultQualityFactor;

	m_pMetaData = NULL;
	IFXCHECKX(IFXCreateComponent(CID_IFXMetaDataX, IID_IFXMetaDataX, (void**)&m_pMetaData));
}


CIFXMarker::~CIFXMarker()
{
	m_pSceneGraph = NULL; // m_pSceneGraph is not addref'd.

	IFXRELEASE(m_pMetaData);
	m_pMetaData = NULL;
}


IFXRESULT CIFXMarker::SetSceneGraph( IFXSceneGraph* pInSceneGraph )
{
	if (m_pSceneGraph) 
	{
		if ( m_pSceneGraph == pInSceneGraph )
			return IFX_E_ALREADY_INITIALIZED;
		else
			IFXASSERT( !pInSceneGraph );
	}

	m_pSceneGraph = pInSceneGraph;

	if( m_pSceneGraph )
		return InitializeObject();
	else
		return IFX_OK;
}


IFXRESULT CIFXMarker::InitializeObject()
{
	return IFX_OK;
}


IFXRESULT CIFXMarker::GetSceneGraph( IFXSceneGraph** ppOutSceneGraph )
{
	IFXRESULT result = IFX_E_UNDEFINED;

	if ( ppOutSceneGraph )
	{
		// Callers get their own reference to the scene graph;
		// Caller is responsible for releasing the returned interface.
		if ( m_pSceneGraph )
		{
			m_pSceneGraph->AddRef();
			*ppOutSceneGraph = m_pSceneGraph;
			result = IFX_OK;
		}
		else
			result = IFX_E_NOT_INITIALIZED;
	}
	else result = IFX_E_INVALID_POINTER;

	return result;
}


IFXRESULT CIFXMarker::Mark()
{
	IFXRESULT rc = IFX_OK ;

	if (m_pSceneGraph != NULL)
		m_uMark = m_pSceneGraph->CurrentMark();
	else 
		rc = IFX_E_NOT_INITIALIZED;

	return rc;
}


IFXRESULT CIFXMarker::Marked(BOOL* pbOutMarked)
{
	IFXRESULT rc = IFX_OK;

	if (m_pSceneGraph != NULL)
	{
		if (pbOutMarked != NULL)
			*pbOutMarked = (m_uMark == m_pSceneGraph->CurrentMark());
		else 
			rc = IFX_E_INVALID_POINTER;
	} 
	else 
		rc = IFX_E_NOT_INITIALIZED;

	return rc;
}


void CIFXMarker::SetPriority( U32 uInPriority,
							  BOOL bRecursive,
							  BOOL bPromotionOnly )
{
	// take the priority if it's a higher priority and we're only promoting:
	if (bPromotionOnly == IFX_TRUE)
	{
		if (uInPriority < m_uPriority)
			m_uPriority = uInPriority ;
	}
	else
		m_uPriority = uInPriority ;
}

void CIFXMarker::SetQualityFactorX(U32 uQualityFactor, U32 uQualityFactorMask )
{
	if(uQualityFactorMask & IFXMarkerX::POSITION_QUALITY) 
	{
		m_uPositionQualityFactor = uQualityFactor;
	}

	if(uQualityFactorMask & IFXMarkerX::TEXCOORD_QUALITY) 
	{
		m_uTexCoordQualityFactor = uQualityFactor;
	}

	if(uQualityFactorMask & IFXMarkerX::NORMAL_QUALITY) 
	{
		m_uNormalQualityFactor = uQualityFactor;
	}

	if(uQualityFactorMask & IFXMarkerX::DIFFUSE_QUALITY) 
	{
		m_uDiffuseQualityFactor = uQualityFactor;
	}

	if(uQualityFactorMask & IFXMarkerX::SPECULAR_QUALITY) 
	{
		m_uSpecularQualityFactor = uQualityFactor;
	}

	if(uQualityFactorMask & (U32)IFXMarkerX::ALL) 
	{
		m_uDefaultQualityFactor = uQualityFactor;
	}
}

void CIFXMarker::GetQualityFactorX(U32& ruQualityFactor, U32 uQualityFactorMask )
{
	if(uQualityFactorMask & IFXMarkerX::SPECULAR_QUALITY) 
	{
		ruQualityFactor = m_uSpecularQualityFactor;
	}

	if(uQualityFactorMask & IFXMarkerX::DIFFUSE_QUALITY) 
	{
		ruQualityFactor = m_uDiffuseQualityFactor;
	}

	if(uQualityFactorMask & IFXMarkerX::NORMAL_QUALITY) 
	{
		ruQualityFactor = m_uNormalQualityFactor;
	}

	if(uQualityFactorMask & IFXMarkerX::TEXCOORD_QUALITY) 
	{
		ruQualityFactor = m_uTexCoordQualityFactor;
	}

	if(uQualityFactorMask & IFXMarkerX::POSITION_QUALITY) 
	{
		ruQualityFactor = m_uPositionQualityFactor;
	}

	if(uQualityFactorMask & (U32)IFXMarkerX::ALL) 
	{
		ruQualityFactor = m_uDefaultQualityFactor;
	}
}


void CIFXMarker::GetEncoderX(const IFXCID& in_CID, IFXEncoderX*& rpEncoderX)
{
	IFXUnknown*			pUnknown = NULL;
	IFXCoreServices*	pCoreServices = NULL;

	try
	{
		// create the encoder
		IFXRELEASE(rpEncoderX);
		IFXCreateComponent( in_CID, IID_IFXEncoderX, (void**)&rpEncoderX );
		if (!rpEncoderX) return;

		// link this calling object to the encoder
		IFXCHECKX(this->QueryInterface(IID_IFXUnknown, (void**)&pUnknown));
		IFXCHECKX( m_pSceneGraph->GetCoreServices( &pCoreServices ) );
		rpEncoderX->InitializeX( *pCoreServices );
		IFXRELEASE(pCoreServices);
		rpEncoderX->SetObjectX(*pUnknown);
		IFXRELEASE(pUnknown);
	}
	catch(...)
	{
		IFXRELEASE(rpEncoderX);
		IFXRELEASE(pCoreServices);
		IFXRELEASE(pUnknown);
		throw;
	}
}

// IFXMetaData
void CIFXMarker::GetCountX(U32& rCount)const
{
	m_pMetaData->GetCountX(rCount); 
}

IFXRESULT CIFXMarker::GetIndex(const IFXString& rKey, U32& uIndex)
{ 
	return m_pMetaData->GetIndex(rKey, uIndex); 
}

void CIFXMarker::GetKeyX(U32 index, IFXString& rOutKey)
{
	m_pMetaData->GetKeyX(index, rOutKey); 
}

void CIFXMarker::GetAttributeX(U32 uIndex, IFXMetaDataAttribute& rValueType)
{
	m_pMetaData->GetAttributeX(uIndex, rValueType); 
}

void CIFXMarker::SetAttributeX(U32 uIndex, const IFXMetaDataAttribute& rValueType)
{
	m_pMetaData->SetAttributeX(uIndex, rValueType); 
}

void CIFXMarker::GetBinaryX(U32 uIndex, U8* pDataBuffer)
{
	m_pMetaData->GetBinaryX(uIndex, pDataBuffer); 
}

void CIFXMarker::GetBinarySizeX(U32 uIndex, U32& rSize)
{
	m_pMetaData->GetBinarySizeX(uIndex, rSize); 
}

void CIFXMarker::GetStringX(U32 uIndex, IFXString& rValue)
{
	m_pMetaData->GetStringX(uIndex, rValue); 
}

void CIFXMarker::GetPersistenceX(U32 uIndex, BOOL& rPersistence)
{
	m_pMetaData->GetPersistenceX(uIndex, rPersistence); 
}

void CIFXMarker::SetBinaryValueX(const IFXString& rKey, U32 length, const U8* data)
{
	m_pMetaData->SetBinaryValueX(rKey, length, data); 
}

void CIFXMarker::SetStringValueX(const IFXString& rKey, const IFXString& rValue)
{
	m_pMetaData->SetStringValueX(rKey, rValue); 
}

void CIFXMarker::SetPersistenceX(U32 uIndex, BOOL value)
{
	m_pMetaData->SetPersistenceX(uIndex, value); 
}

void CIFXMarker::DeleteX(U32 uIndex)
{
	m_pMetaData->DeleteX(uIndex); 
}

void CIFXMarker::DeleteAll()
{
	m_pMetaData->DeleteAll(); 
}

void CIFXMarker::AppendX(IFXMetaDataX* pSource)
{
	m_pMetaData->AppendX(pSource); 
}

void CIFXMarker::GetEncodedKeyX(U32 uIndex, IFXString& rOutKey)
{
	m_pMetaData->GetEncodedKeyX(uIndex, rOutKey);
}

void CIFXMarker::GetSubattributesCountX(U32 uIndex, U32& rCountSubattributes)
{
	m_pMetaData->GetSubattributesCountX(uIndex, rCountSubattributes);
}

IFXRESULT CIFXMarker::GetSubattributeIndex(U32 uIndex, const IFXString& rSubattributeName, U32& rSubattributeIndex)
{
	return m_pMetaData->GetSubattributeIndex(uIndex, rSubattributeName, rSubattributeIndex);
}

void CIFXMarker::GetSubattributeNameX(U32 uIndex, U32 uSubattributeIndex, IFXString& rSubattributeName)
{
	m_pMetaData->GetSubattributeNameX(uIndex, uSubattributeIndex, rSubattributeName);
}

void CIFXMarker::GetSubattributeValueX(U32 uIndex, U32 uSubattributeIndex, IFXString*& pSubattributeValue)
{
	m_pMetaData->GetSubattributeValueX(uIndex, uSubattributeIndex, pSubattributeValue);
}

void CIFXMarker::SetSubattributeValueX(U32 uIndex, const IFXString& rSubattributeName, const IFXString* pSubattributeValue)
{
	m_pMetaData->SetSubattributeValueX(uIndex, rSubattributeName, pSubattributeValue);
}

void CIFXMarker::DeleteSubattributeX(U32 uIndex, U32 uSubattributeIndex)
{
	m_pMetaData->DeleteSubattributeX(uIndex, uSubattributeIndex);
}

void CIFXMarker::DeleteAllSubattributes(U32 uIndex)
{
	m_pMetaData->DeleteAllSubattributes(uIndex);
}
