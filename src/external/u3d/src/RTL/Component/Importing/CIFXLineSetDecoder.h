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
	@file	CIFXLineSetDecoder.h

			Declaration of the CIFXLineSetDecoder.
			The CIFXLineSetDecoder is used by the CIFXLoadManager to load
			box primitive generators into the scene graph. CIFXLineSetDecoder
			exposes an IFXDecoder interface to the CIFXLoadManager for this
			purpose.
*/

#ifndef CIFXLineSetDecoder_H
#define CIFXLineSetDecoder_H

#include "IFXDecoderX.h"
#include "IFXSceneGraph.h"
#include "IFXCoreCIDs.h"
#include "IFXAuthorLineSetResource.h"
#include "IFXBitStreamCompressedX.h"
#include "IFXExportingCIDs.h"
#include "IFXModifierBaseDecoder.h"
#include "IFXAuthorLineSetAnalyzer.h"

class CIFXLineSetDecoder : virtual public IFXDecoderX, public IFXModifierBaseDecoder
{
public:
	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXLineSetDecoder_Factory(IFXREFIID interfaceId, void** ppInterface);

	// IFXUnknown
	virtual U32 IFXAPI  AddRef();
	virtual U32 IFXAPI  Release();
	virtual IFXRESULT IFXAPI  QueryInterface( IFXREFIID interfaceId, void** ppInterface );

	// IFXDecoderX
	virtual void IFXAPI  InitializeX( const IFXLoadConfig &lc );
	virtual void IFXAPI  PutNextBlockX(IFXDataBlockX &rDataBlockX);
	virtual void IFXAPI  TransferX(IFXRESULT &rWarningPartialTransfer);


private:
	// Member functions.
	CIFXLineSetDecoder();

	virtual ~CIFXLineSetDecoder();

	void ProcessAuthorLineSetDeclarationBlockX();

	void ProcessAuthorLineSetContinuationBlockX();
	
	void ReconstructPosition(U8 u8Signs, U32 udX, U32 udY, U32 udZ, 
		                          IFXVector3& vReconstructedPosition);

	void ReconstructNormal(U8 u8Signs, U32 udX, U32 udY, U32 udZ, 
		                          IFXVector3& vReconstructedNormal);

	void ReconstructTexCoord(U8 u8Signs, U32 udR, U32 udG, U32 udB, U32 udA, 
		                          IFXVector4& v4VertTexCoord);

	void CalculatePredictedNormalAtSplitPosX(U32 currPosInd, U32 splitPosInd, IFXVector3& vPredictedNormal);
	
	void CalculatePredictedColorAtSplitPosX(BOOL bDiffuse, U32 currPosInd, U32 splitPosInd,
											IFXVector4& v4PredictedVertColor);
	void CalculatePredictedTexCoordAtSplitPosX(U32 texInd, U32 currPosInd, U32 splitPosInd, 
											  IFXVector4& v4PredictedTexCoord);

	void ReconstructColorDifference(BOOL bDiffuseColor, U8& u8Signs, U32& udR, U32&udG, U32& udB, U32& udA, 
							IFXVector4& v4VertColorDifference);
	
	void NormalizeNormals();

	BOOL m_bDone;       ///< Completion status; are there more data blocks to process
	IFXDECLAREMEMBER(IFXBitStreamCompressedX,m_pBitStreamX);

	void ReadLineUpdateX(U32 currPosInd);

	enum EIFXWIPType
	{
			WIP_NONE,
			WIP_DECLARATION,
			WIP_STATIC,
			WIP_PROGRESSIVE,
            WIP_DECLARATION_U3D,
            WIP_STATIC_U3D,
            WIP_PROGRESSIVE_U3D
	};
	EIFXWIPType m_eWIPType;

	
	U32 m_uWIPi;
	U32 m_uStartResolution;
	U32 m_uEndResolution;
	U32 m_uCurrentIndex;

	static const U32 m_uMaximumUpdatesPerCall;

	// Resolution
	F32 m_fMinResolution;
	F32 m_fMaxResolution;

	// Inverse Quantization Factors
	F32 m_fInverseQuantPosition;
	F32 m_fInverseQuantNormal;
	F32 m_fInverseQuantTexCoord;
	F32 m_fInverseQuantDiffuseColor;
	F32 m_fInverseQuantSpecularColor;

	// Resource Parameters 
	// these are reserved LineSet resourse parametrs for the future definition
	U32 m_uReservedLineSetParameter0;
	U32 m_uReservedLineSetParameter1;
	U32 m_uReservedLineSetParameter2;
	U32 m_uReservedLineSetParameter3;

	F64 m_unitScale;

	IFXDECLAREMEMBER(IFXAuthorLineSetResource, m_pLineSetResource);
	IFXDECLAREMEMBER(IFXAuthorLineSet,m_pAuthorLineSet);
	
	IFXAuthorLineSetDesc*			m_pMaxLineSetDesc;
	IFXAuthorLineSetDesc*			m_pLineSetDesc;
	U32								m_uPositionsRead;

	IFXDECLAREMEMBER(IFXAuthorLineSetAnalyzer,m_pLineSetAnalyzer);
};

#endif
