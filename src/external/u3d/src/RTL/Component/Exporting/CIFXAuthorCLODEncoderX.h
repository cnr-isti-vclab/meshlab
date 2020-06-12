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
	@file	CIFXAuthorCLODEncoderX.h
			
			Declaration of CIFXAuthorCLODEncoderX class implementation.
*/

#ifndef CIFXAuthorCLODEncoderX_H__
#define CIFXAuthorCLODEncoderX_H__

#include "IFXEncoderX.h"
#include "IFXBitStreamCompressedX.h"
#include "IFXDataBlockQueueX.h"
#include "IFXAuthorCLODResource.h"
#include "IFXSetAdjacencyX.h"
#include "IFXAutoRelease.h"

/**
	@note
	The compression algorithm uses a type of predictive coding.
	The part of the mesh that has been already decoded is used to make
	a prediction of the next piece of information. The difference between
	the predicted value and the actual value is transmitted.  For this
	system to work correctly, the encoder and decoder must make the same
	prediction.  Because the predictions must be the same, the meshes which
	form the basis for the predictions must also be the same. The reconstructed
	mesh is the mesh that is created in the decoder.  The encoder creates an
	identical reconstructed mesh so the encoder and decoder can make the same
	predictions.
	The original mesh is also needed by the encoder to calculate the
	differences between the predicted and actual values.  The decoder does not
	have the original mesh.
*/
class CIFXAuthorCLODEncoderX : virtual public IFXEncoderX
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
	friend IFXRESULT IFXAPI_CALLTYPE CIFXAuthorCLODEncoderX_Factory(IFXREFIID interfaceId,
		void** ppInterface);

private:

	CIFXAuthorCLODEncoderX();
	virtual ~CIFXAuthorCLODEncoderX();

	/// Calculate the quantization factors
	void CalculateQuantizationFactorsX();

	void PrepareForMakingBlocksX();

	/// Make the author CLOD model resource declaration block
	void MakeAuthorCLODDeclarationBlockX(IFXString &rName, IFXDataBlockQueueX &rDataBlockQueue);

	/// Make the static geometry block
	void MakeAuthorCLODStaticGeometryBlockX(IFXString &rName, IFXDataBlockQueueX &rDataBlockQueue, IFXMeshMap &rMeshMap);

	/// Make the progressive geometry blocks
	void MakeAuthorCLODProgressiveGeometryBlocksX(IFXString &rName, IFXDataBlockQueueX &rDataBlockQueue, IFXMeshMap*& rpMeshMap);

	void CalculateFaceNormalX(U32 uFaceIndex, IFXVector3& rvFaceNormal);

	U32  m_uRefCount; // Reference count for IFXUnknown
	U32 m_uPriorityIncrement;
	U32 m_uPriorityCurrent;

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
	F32 m_fNormalCreaseParameter;
	F32 m_fNormalUpdateParameter;
	F32 m_fNormalTolerance;

	F64 m_unitScale;

	static const U32 m_puNumResChangesPerGeomBlock[] ;
	static const U32 m_uNumGeomBlockResChanges ;

	IFXDECLAREMEMBER(IFXCoreServices,m_pCoreServices);
	IFXDECLAREMEMBER(IFXAuthorCLODResource,m_pAuthorCLODResource);
	IFXDECLAREMEMBER(IFXAuthorCLODMesh,m_pOriginalAuthorCLODMesh);
	IFXDECLAREMEMBER(IFXAuthorCLODMesh,m_pReconstructedAuthorCLODMesh);
};

#endif
