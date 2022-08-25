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
	@file	CIFXAuthorCLODDecoder.h
			
			Declaration of CIFXAuthorCLODDecoder class implementation
*/

#ifndef CIFXAuthorCLODDecoder_H
#define CIFXAuthorCLODDecoder_H

#include "IFXDecoderX.h"
#include "IFXDataBlockQueueX.h"
#include "IFXAuthorCLODResource.h"
#include "IFXAutoRelease.h"
#include "IFXBitStreamCompressedX.h"
#include "IFXSetAdjacencyX.h"
#include "IFXSceneGraph.h"
#include "IFXModifierBaseDecoder.h"

class CIFXAuthorCLODDecoder : virtual	public IFXDecoderX, 
										public IFXModifierBaseDecoder
{
public:
	// IFXUnknown
	U32 IFXAPI  AddRef ( void );
	U32 IFXAPI  Release ( void );
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID interfaceId, void** ppInterface);

	// IFXDecoderX
	virtual void IFXAPI  InitializeX( const IFXLoadConfig &lc );
	virtual void IFXAPI  PutNextBlockX(IFXDataBlockX &rDataBlockX);
	virtual void IFXAPI  TransferX(IFXRESULT &rWarningPartialTransfer);

	// Factory function
	friend IFXRESULT IFXAPI_CALLTYPE CIFXAuthorCLODDecoder_Factory(IFXREFIID interfaceId,
		void** ppInterface);

private:

	CIFXAuthorCLODDecoder();
	virtual ~CIFXAuthorCLODDecoder();

	void CalculateFaceNormalX(U32 uFaceIndex, IFXVector3& rvFaceNormal);

	/// Process the author CLOD model resource declaration block
	void ProcessAuthorCLODDeclarationBlockX( U32 uBlockType );

	/// Process the static geometry block
	void ProcessAuthorCLODStaticGeometryBlockX( U32 uBlockType );

	/// Process the progressive geometry blocks
	void ProcessAuthorCLODProgressiveGeometryBlocksX( U32 uBlockType );

	BOOL m_bDone; ///< Completion status; are there more data blocks to process

	IFXDECLAREMEMBER(IFXBitStreamCompressedX,m_pBitStreamX);
	enum EIFXWIPType
	{
			WIP_NONE,
            WIP_DECLARATION_U3D,
            WIP_STATIC_U3D,
            WIP_PROGRESSIVE_U3D
	};
	EIFXWIPType m_eWIPType;
	U32 m_uWIPi;
	BOOL m_bExcludeNormals;
	U32 m_uStartResolution;
	U32 m_uEndResolution;
	U32 m_uCurrentResolution;
	U32 m_uCurrentTexCoordCount;
	U32 m_uCurrentNumDiffuseColors;
	U32 m_uCurrentNumSpecularColors;
	U32 m_uPrevSplitDiffColor;
	U32 m_uPrevUpdateDiffColor;
	U32 m_uPrevThirdDiffColor;
	U32 m_uPrevSplitSpecColor;
	U32 m_uPrevUpdateSpecColor;
	U32 m_uPrevThirdSpecColor;
	U32 m_uPrevSplitTexCoord;
	U32 m_uPrevUpdateTexCoord;
	U32 m_uPrevThirdTexCoord;
	static const U32 m_uMaximumUpdatesPerCall;

	IFXAuthorMeshDesc samdMinMeshDescription;
	IFXAuthorMeshDesc samdMaxMeshDescription;
	const IFXAuthorMeshDesc* pcReconstructedMeshDescription;

	// Inverse Quantization Factors
	F32 m_fInverseQuantPosition;
	F32 m_fInverseQuantNormal;
	F32 m_fInverseQuantTexCoord;
	F32 m_fInverseQuantDiffuseColor;
	F32 m_fInverseQuantSpecularColor;

	// Resource Parameters
	F32 m_fNormalCreaseParameter;
	F32 m_fNormalUpdateParameter;
	F32 m_fNormalTolerance;

	F64 m_unitScale;

	IFXDECLAREMEMBER(IFXAuthorCLODResource,m_pAuthorCLODResource);
	IFXDECLAREMEMBER(IFXAuthorCLODMesh,m_pReconstructedAuthorCLODMesh);
	IFXDECLAREMEMBER(IFXSetAdjacencyX,m_pSetAdjacencyX);
};

#endif
