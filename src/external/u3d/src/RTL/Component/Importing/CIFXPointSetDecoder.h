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
	@file	CIFXPointSetDecoder.h

			Declaration of the CIFXPointSetDecoder.
			The CIFXPointSetDecoder is used by the CIFXLoadManager to load
			box primitive generators into the scene graph. CIFXPointSetDecoder
			exposes an IFXDecoder interface to the CIFXLoadManager for this
			purpose.
*/

#ifndef CIFXPointSetDecoder_H
#define CIFXPointSetDecoder_H

#include "IFXDecoderX.h"
#include "IFXSceneGraph.h"
#include "IFXCoreCIDs.h"
#include "IFXAuthorPointSetResource.h"
#include "IFXBitStreamCompressedX.h"
#include "IFXExportingCIDs.h"
#include "IFXModifierBaseDecoder.h"

class CIFXPointSetDecoder : virtual public IFXDecoderX, public IFXModifierBaseDecoder
{
public:
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
	CIFXPointSetDecoder();
	virtual ~CIFXPointSetDecoder();

	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXPointSetDecoder_Factory(IFXREFIID interfaceId, void** ppInterface);

	void ProcessAuthorPointSetDeclarationBlockX();

	void ProcessAuthorPointSetContinuationBlockX();
	
	void ReconstructPosition(U8 u8Signs, U32 udX, U32 udY, U32 udZ, 
		                          IFXVector3& vReconstructedPosition);

	void ReconstructNormal(U8 u8Signs, U32 udX, U32 udY, U32 udZ, 
		                          IFXVector3& vReconstructedNormal);

	void ReconstructTexCoord(U8 u8Signs, U32 udR, U32 udG, U32 udB, U32 udA, 
									IFXVector4& v4VertTexCoord);

	void GetPointsAtPosition(U32& posInd, IFXArray<U32>& Points);

	void CalculatePredictedNormalAtSplitPos(U32 currPosInd, U32 splitPosInd, IFXVector3& vPredictedNormal);
	
	void CalculatePredictedColorAtSplitPos(BOOL bDiffuse, U32 currPosInd, U32 splitPosInd,
											IFXVector4& v4PredictedVertColor);
	void CalculatePredictedTexCoordAtSplitPos(U32 texInd, U32 currPosInd, U32 splitPosInd, 
															  IFXVector4& v4PredictedTexCoord);

	void ReconstructColorDifference(BOOL bDiffuseColor, U8& u8Signs, U32& udR, U32&udG, U32& udB, U32& udA, 
							IFXVector4& v4VertColorDifference);

	BOOL m_bDone;       ///< Completion status; are there more data blocks to process
	IFXDECLAREMEMBER(IFXBitStreamCompressedX,m_pBitStreamX);

	void ReadPointUpdateX(U32 currPosInd);

	enum EIFXWIPType
	{
		WIP_NONE,
		WIP_DECLARATION_U3D,
		WIP_PROGRESSIVE_U3D
	};
	EIFXWIPType m_eWIPType;


	U32 m_uWIPi;
	U32 m_uStartResolution;
	U32 m_uEndResolution;
	U32 m_currentIndex;

	static const U32 m_uMaximumUpdatesPerCall;

	//Point Set Reserved is a reserved field
	U32 m_uReservedPointSetParameter0;

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
	// these are reserved PointSet resourse parametrs for the future definition
	U32 m_uReservedPointSetParameter1;
	U32 m_uReservedPointSetParameter2;
	U32 m_uReservedPointSetParameter3;

	F64 m_unitScale;

	IFXDECLAREMEMBER(IFXAuthorPointSetResource, m_pPointSetResource);
	IFXDECLAREMEMBER(IFXAuthorPointSet,m_pAuthorPointSet);
	
	IFXAuthorPointSetDesc*			m_pMaxPointSetDesc;
	IFXAuthorPointSetDesc*			m_pPointSetDesc;
	U32								m_uPositionsRead;
};

#endif
