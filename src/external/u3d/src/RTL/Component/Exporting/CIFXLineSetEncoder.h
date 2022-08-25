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
	@file	CIFXLineSetEncoder.h

			Declaration of CIFXLineSetEncoder class implementation.
*/

#ifndef CIFXLineSetEncoder_H
#define CIFXLineSetEncoder_H

#include "IFXCoreServices.h"
#include "IFXEncoderX.h"
#include "IFXAutoRelease.h"
#include "IFXBitStreamCompressedX.h"
#include "IFXArray.h"
#include "IFXAuthorLineSetResource.h"
#include "IFXAuthorLineSetAnalyzer.h"


class CIFXLineSetEncoder : virtual public IFXEncoderX
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
	void IFXAPI	EncodeX(IFXString &rName, IFXDataBlockQueueX &rDataBlockQueue, F64 units = 1.0f);

	// Factory function
	friend IFXRESULT IFXAPI_CALLTYPE CIFXLineSetEncoder_Factory(IFXREFIID interfaceId, void** ppInterface);

private:

	enum AttribType 
	{
		NORMAL,
		DIFFUSECOLOR,
		SPECULARCOLOR
	};

	CIFXLineSetEncoder();
	virtual ~CIFXLineSetEncoder();
	
	/// Writes the model resource declaration block
	void MakeDeclarationBlockX(	IFXString &rName, IFXDataBlockQueueX &rDataBlockQueue	);

	/// Write the contiuation blocks
	void MakeContinuationBlocksX(IFXString &rName, IFXDataBlockQueueX &rDataBlockQueue	);
	
	/// returns number of lines encoded with written update assoiated with Poistion at currPosInd
	U32 WriteLineUpdateX(U32 currPosInd, IFXBitStreamCompressedX* pBitStreamX);

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

	void GetLineAttribIndexes(U32 lineInd, U32 splitPos, CIFXLineSetEncoder::AttribType attrib,
							  U32& splitInd, U32& endInd );
	void GetLineTexCoord(U32 texLayer, U32 lineInd, U32 currPos, U32& currInd, U32& endInd );

#if _DEBUG
	void ReconstructPosition(U8 u8Signs, U32 udX, U32 udY, U32 udZ, IFXVector3& vReconstructedPosition);
#endif

	void PrepareForWriting();

	U32		m_uRefCount;
	U32		m_uPriorityIncrement;
	U32		m_uPriorityCurrent;

	IFXDECLAREMEMBER(IFXCoreServices, m_pCoreServices);
	IFXDECLAREMEMBER(IFXAuthorLineSetResource, m_pLineSetResource);
	IFXDECLAREMEMBER(IFXAuthorLineSet,m_pAuthorLineSet);

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


	BOOL	m_bBaseBlockPresent;
	U32		m_uPositionsWritten;
	IFXAuthorLineSetDesc*			m_pLineSetDescription;
	IFXAuthorLineSetAnalyzer*		m_pLineSetAnalyzer;

	U32		m_uCurrentTexCoordCount;
	U32		m_uCurrentNumDiffuseColors;
	U32		m_uCurrentNumSpecularColors;
	U32     m_uCurrentNumTexCoord;
	U32		m_uLastDiffuseColorInd;
	U32		m_uLastSpecularColorInd;
	U32		m_uLastTexCoordInd;

	F64 m_unitScale;
};


#endif
