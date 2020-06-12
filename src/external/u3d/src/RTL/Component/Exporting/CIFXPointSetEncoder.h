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
	@file	CIFXPointSetEncoder.h
		
			Declaration of CIFXPointSetEncoder class implementation                           
*/

#ifndef CIFXPointSetEncoder_H__
#define CIFXPointSetEncoder_H__

#include "IFXCoreServices.h"
#include "IFXEncoderX.h"
#include "IFXAutoRelease.h"
#include "IFXBitStreamCompressedX.h"
#include "IFXAuthorPointSetResource.h"
#include "IFXArray.h"

class CIFXPointSetEncoder : virtual public IFXEncoderX
{
public:
	
	// IFXUnknown
	U32 IFXAPI  AddRef ( void );
	U32 IFXAPI  Release ( void );
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID interfaceId, void** ppInterface);

	// IFXEncoderX
	/// Provide the encoder with a pointer to the object which is to be encoded.
	void IFXAPI	SetObjectX(IFXUnknown &rObject);
	/// Initialize and get a reference to the core services
	void IFXAPI	InitializeX(IFXCoreServices &rCoreServices);
	/// Encode data into data blocks and place these blocks in a queue
	void IFXAPI	EncodeX(IFXString &rName, IFXDataBlockQueueX &rDataBlockQueue, F64 units = 1.0f );

	// Factory function
	friend IFXRESULT IFXAPI_CALLTYPE CIFXPointSetEncoder_Factory(IFXREFIID interfaceId, void** ppInterface);

private:

	enum AttribType 
	{
		NORMAL,
		DIFFUSECOLOR,
		SPECULARCOLOR
	};

	CIFXPointSetEncoder();
	virtual ~CIFXPointSetEncoder();
	
	// Writes the model resource declaration block
	void MakeDeclarationBlockX(	IFXString &rName, IFXDataBlockQueueX &rDataBlockQueue	);

	// Write the contiuation block
	void MakeContinuationBlockX(IFXString &rName, IFXDataBlockQueueX &rDataBlockQueue	);
	
	/// returns number of Points encoded with written update assoiated with Poistion at currPosInd
	U32 WritePointUpdateX(U32 currPosInd, IFXBitStreamCompressedX* pBitStreamX);

	void GetPointsAtPosition(U32 splitPos, IFXArray<U32>& Points) ;

	void CalculateQuantizationFactorsX();

	void CalculatePredictedNormalAtSplitPosX(U32 currPosInd, U32 splitPosInd, IFXVector3& vPredictedNormal);
	
	void CalculatePredictedColorAtSplitPosX(BOOL bDiffuseColor, U32 currPosInd, U32 splitPosInd,
											IFXVector4& v4PredictedVertColor);
	
	void CalculatePredictedTexCoordAtSplitPosX(U32 texInd, U32 currPosInd, U32 splitPosInd, IFXVector4& v4PredictedTexCoord);

	void QuantizePositionForWrite(IFXVector3& pos, U8& u8Signs, U32& udX, U32& udY, U32& udZ);

	void QuantizeNormalForWrite(IFXVector3& pos, U8& u8Signs, U32& udX, U32& udY, U32& udZ);

	void QuantizeColorForWrite(BOOL bDiffuseColor, IFXVector4& color, 
								U8& u8Signs, U32& udR, U32& udG, U32& udB, U32& udA);
	
	void QuantizeTexCoordForWrite(IFXVector4& texCoord, 
								U8& u8Signs, U32& udR, U32& udG, U32& udB, U32& udA);

	void GetPointAttribIndexes(U32 PointInd, CIFXPointSetEncoder::AttribType attrib, U32& attrInd);

	void PrepareForWriting();

	U32		m_uRefCount;

	IFXDECLAREMEMBER(IFXCoreServices, m_pCoreServices);
	IFXDECLAREMEMBER(IFXAuthorPointSetResource, m_pPointSetResource);
	IFXDECLAREMEMBER(IFXAuthorPointSet,m_pAuthorPointSet);

	// Quantization Factors
	F32 m_fQuantPosition;
	F32 m_fQuantNormal;
	F32 m_fQuantTexCoord;
	F32 m_fQuantDiffuseColor;
	F32 m_fQuantSpecularColor;

	F32 m_fInverseQuantPosition;
	F32 m_fInverseQuantNormal;
	F32 m_fInverseQuantTexCoord;
	F32 m_fInverseQuantDiffuseColor;
	F32 m_fInverseQuantSpecularColor;

	// Resource Parameters 
	// these are reserved PointSet resourse parametrs for the future definition
	U32 m_uReservedPointSetParameter0;
	U32 m_uReservedPointSetParameter1;
	U32 m_uReservedPointSetParameter2;
	U32 m_uReservedPointSetParameter3;		

	BOOL	m_bBaseBlockPresent;
	U32		m_uPositionsWritten;
	IFXAuthorPointSetDesc*	m_pPointSetDescription;

	U32		m_uCurrentTexCoordCount;
	U32		m_uCurrentNumDiffuseColors;
	U32		m_uCurrentNumSpecularColors;
	U32     m_uCurrentNumTexCoord;
	U32		m_uLastDiffuseColorInd;
	U32		m_uLastSpecularColorInd;
	U32		m_uLastTexCoordInd;

	U32		m_uPriorityIncrement;
	U32		m_uPriorityCurrent;

	F64 m_unitScale;
};

#endif
