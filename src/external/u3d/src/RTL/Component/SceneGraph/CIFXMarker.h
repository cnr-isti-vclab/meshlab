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
	@file	CIFXMarker.h

			This class implements common Marker functionality. It is not
			ment to be instantiated by itself.  

	@note	The intent is to simplify the development of new Markers by 
			inheriting and forwarding calls to this implementation. 
*/

#ifndef __CIFXMARKER_H__
#define __CIFXMARKER_H__

#include "IFXDataTypes.h"
#include "IFXMarkerX.h"
class IFXSceneGraph;


class CIFXMarker : virtual public IFXMarkerX
{
protected:
					CIFXMarker();
	virtual			~CIFXMarker();

	// IFXMarker
	IFXRESULT	 IFXAPI			SetSceneGraph( IFXSceneGraph*	pInSceneGraph );
	IFXRESULT	 IFXAPI			GetSceneGraph( IFXSceneGraph**	ppOutSceneGraph );
	IFXRESULT	 IFXAPI 		Mark();
	IFXRESULT	 IFXAPI 		Marked(	BOOL* pbOutMarked );
	U32			 IFXAPI 		GetPriority() { return m_uPriority; }
	void		 IFXAPI 		SetPriority( U32 uInPriority, BOOL bRecursive, BOOL bPromotionOnly );
	void		 IFXAPI 		SetExternalFlag(BOOL bExternalFlag) { m_bExternalFlag = bExternalFlag; }
	void		 IFXAPI 		GetExternalFlag(BOOL* bExternalFlag) { *bExternalFlag = m_bExternalFlag; }

	// IFXMarkerX
	virtual void IFXAPI 	SetQualityFactorX(U32 uQualityFactor, U32 uQualityFactorMask = (U32)IFXMarkerX::ALL);
	virtual void IFXAPI 	GetQualityFactorX(U32& ruQualityFactor, U32 uQualityFactorMask = (U32)IFXMarkerX::ALL);
	using IFXMarkerX::GetEncoderX;
	void IFXAPI 			GetEncoderX (const IFXCID&, IFXEncoderX*& rpEncoderX);

	virtual IFXRESULT IFXAPI  InitializeObject();

	IFXSceneGraph*	m_pSceneGraph;
	U32				m_uMark;
	U32				m_uPriority;
	U32				m_uDefaultQualityFactor;
	U32				m_uPositionQualityFactor;
	U32				m_uTexCoordQualityFactor;
	U32				m_uNormalQualityFactor;
	U32				m_uDiffuseQualityFactor;
	U32				m_uSpecularQualityFactor;

public:
	// IFXMetaData
	virtual void IFXAPI  GetCountX(U32& rCount) const;
	virtual IFXRESULT IFXAPI  GetIndex(const IFXString& rKey, U32& uIndex);
	virtual void IFXAPI  GetKeyX(U32 index, IFXString& rOutKey);
	virtual void IFXAPI  GetAttributeX(U32 uIndex, IFXMetaDataAttribute& rValueType);
	virtual void IFXAPI  SetAttributeX(U32 uIndex, const IFXMetaDataAttribute& rValueType);
	virtual void IFXAPI  GetBinaryX(U32 uIndex, U8* pDataBuffer);
	virtual void IFXAPI  GetBinarySizeX(U32 uIndex, U32& rSize);
	virtual void IFXAPI  GetStringX(U32 uIndex, IFXString& rValue);
	virtual void IFXAPI  GetPersistenceX(U32 uIndex, BOOL& rPersistence);
	virtual void IFXAPI  SetBinaryValueX(const IFXString& rKey, U32 length, const U8* data);
	virtual void IFXAPI  SetStringValueX(const IFXString& rKey, const IFXString& rValue);
	virtual void IFXAPI  SetPersistenceX(U32 uIndex, BOOL value);
	virtual void IFXAPI  DeleteX(U32 uIndex);
	virtual void IFXAPI  DeleteAll();
	virtual void IFXAPI  AppendX(IFXMetaDataX* pSource);

	virtual void IFXAPI  GetEncodedKeyX(U32 uIndex, IFXString& rOutKey);
	virtual void IFXAPI  GetSubattributesCountX(U32 uIndex, U32& rCountSubattributes);
	virtual IFXRESULT IFXAPI  GetSubattributeIndex(U32 uIndex, const IFXString& rSubattributeName, U32& rSubattributeIndex);
	virtual void IFXAPI  GetSubattributeNameX(U32 uIndex, U32 uSubattributeIndex, IFXString& rSubattributeName);
	virtual void IFXAPI  GetSubattributeValueX(U32 uIndex, U32 uSubattributeIndex, IFXString*& pSubattributeValue);
	virtual void IFXAPI  SetSubattributeValueX(U32 uIndex, const IFXString& rSubattributeName, const IFXString* pSubattributeValue);
	virtual void IFXAPI  DeleteSubattributeX(U32 uIndex, U32 uSubattributeIndex);
	virtual void IFXAPI  DeleteAllSubattributes(U32 uIndex);

private:
	IFXMetaDataX* m_pMetaData;
	BOOL m_bExternalFlag;
};


#endif
