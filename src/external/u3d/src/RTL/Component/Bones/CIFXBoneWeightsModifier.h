//***************************************************************************
//
//  Copyright (c) 2000 - 2006 Intel Corporation
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
	@file CIFXBoneWeightsModifier.h
*/

#ifndef CIFXBONESWEIGHTSMODIFIER_H
#define CIFXBONESWEIGHTSMODIFIER_H

#include "IFXBoneWeightsModifier.h"
#include "CIFXModifier.h"
#include "IFXPackWeights.h"
#include "IFXMeshMap.h"
#include "IFXCharacter.h"

/** 
	This class is used as temp storage of bones data for single vertex 
*/
class IFXVertexBoneWeights
{
public:
	IFXVertexBoneWeights();
	~IFXVertexBoneWeights();

	U32 GetVertexIdx( void ) { return m_vertexIdx; };
	U32 GetWeightCnt( void ) { return m_weightCnt; };
	U32 *GetBoneIDs( void ) { return m_pBoneIDs; };
	F32 *GetWeights( void ) { return m_pWeights; };
	void SetVertexIdx( U32 v ) { m_vertexIdx = v; };
	void SetWeightCnt( U32 w );
	void SetBoneID( U32 i, U32 id );
	void SetWeight( U32 i, F32 wg );

	/// Marker, used to mark uninitialized vertices
	static const U32 m_sVertexIdxUndefinedMarker = U32(-1);

private:
	U32 m_vertexIdx;
	U32 m_weightCnt;

	U32 *m_pBoneIDs;
	F32 *m_pWeights;
};

class CIFXBoneWeightsModifier :     virtual public IFXBoneWeightsModifier,
									private CIFXModifier
{
public:
	friend IFXRESULT IFXAPI_CALLTYPE CIFXBoneWeightsModifier_Factory(IFXREFIID interfaceId, void** ppInterface);
	
	// IFXUnknown
	U32 IFXAPI          AddRef (void);
	U32 IFXAPI          Release (void);

	IFXRESULT IFXAPI    QueryInterface (IFXREFIID riid, void **ppv);
	
	// IFXMarkerX interface...
	void IFXAPI         GetEncoderX (IFXEncoderX*& rpEncoderX);

	// IFXModifier
	IFXRESULT IFXAPI  GetOutputs ( IFXGUID**& rppOutOutputs, 
						  U32&       rOutNumberOfOutputs,
						  U32*&      rpOutOutputDepAttrs );
	IFXRESULT IFXAPI  GetDependencies (    IFXGUID*   pInOutputDID,
								  IFXGUID**& rppOutInputDependencies,
								  U32&       rOutNumberInputDependencies,
								  IFXGUID**& rppOutOutputDependencies,
								  U32&       rOutNumberOfOutputDependencies,
								  U32*&      rpOutOutputDepAttrs );

	IFXRESULT IFXAPI  GenerateOutput ( 
									U32    inOutputDataElementIndex, 
									void*& rpOutData, BOOL& rNeedRelease ); 
	
	IFXRESULT IFXAPI  SetDataPacket ( IFXModifierDataPacket* pInInputDataPacket, 
									 IFXModifierDataPacket* pInDataPacket );
	IFXRESULT IFXAPI  Notify (IFXModifierMessage eInMessage, void *pMessageContext );

	// Bone weight operation methods
	IFXRESULT SetTotalBoneWeightCount(  U32 meshIndex,
										U32 totalVertexCount,
										U32 totalWeightCount );

/** Sets the total number of vertices with weights and the sum total of
			weights for all the vertices.  This needs to set for every mesh
			in the model. This is required for space allocation before
			setting any weight data. */
	IFXRESULT  IFXAPI   GetTotalBoneWeightCount(    
										U32 meshIndex,
										U32 *pTotalWeightCount );

					/// set RT bone data from given buffers
/** Sets the weights for a particular vertex on a specified mesh.
			The pointers are arrays of length uWeightCount containing the
			bone index and fractional weight of each influence.
			The total weights should add up to very nearly one. */
	IFXRESULT SetBoneWeights(   U32 vertexIndex,
								U32 meshIndex,
								U32 weightCount,
								U32 *pBoneIDs,
								F32 *pBoneWeights );

	IFXRESULT GotoVertex(   U32 uVertexIndex, 
							U32 uMeshIndex );

					/// get size of buffers required
	IFXRESULT  IFXAPI   GetBoneWeightCount( U32 vertexIndex,
									U32 meshIndex,
									U32 *pWeightCount );
					/// copy RT bone data into given buffers
	IFXRESULT  IFXAPI   GetBoneWeights( U32 uVertexIndex,
								U32 uMeshIndex,
								U32 uWeightCount,
								U32 *puBoneIDs,
								F32 *pfBoneWeights );

	// Bone weight storage methods
	IFXRESULT IFXAPI SetTotalBoneWeightCountForAuthorMesh(  U32 totalVertexCount );

	IFXRESULT IFXAPI SetBoneWeightsForAuthorMesh( U32 vertexIndex,
								U32 weightCount,
								U32 *pBoneIDs,
								F32 *pBoneWeights );

	/// Check if bone weights data initialized and the data was generated 
	/// by this object (so the data is in consistent state)
	BOOL IFXAPI boneWeightsAreValid()
	{
		BOOL ret = TRUE;

		if (!m_pBoneWeights || !m_pBonesMgr)
		{
			ret = FALSE;
		}
		else
		{
			IFXCharacter *pCharacter = NULL;
			IFXRESULT rc = m_pBonesMgr->GetCharacter(&pCharacter);
		
			if(IFXSUCCESS(rc))
				ret = (pCharacter->GetSkin()->GetPackVertexWeightsCreatorID() == this);
			else
				ret = FALSE;
		}

		return ret;
	}

protected:

	IFXRESULT Initialize( void );

	CIFXBoneWeightsModifier();
	virtual ~CIFXBoneWeightsModifier();

	virtual IFXRESULT IFXAPI  Construct();

	U32 m_BonesManagerIndex;
	U32 m_BoneWeightsIndex;

	IFXBonesManager*            m_pBonesMgr;
	IFXArray<IFXPackWeights>*   m_pBoneWeights; ///< @note Points to Character's resource

	IFXArray<U32>                   m_totalWeights;
	IFXArray<U32>                   m_lastGoodVertID;
	IFXArray<U32>                   m_lastReadOffset;
	IFXArray<const IFXPackVertex*>  m_pLastVertex;
	U32                             m_LastResolution;

	IFXVertexBoneWeights    *m_pAuthorBoneWeights; ///< Author Mesh Bone Weights
	U32                      m_uABWSize;

private:
	/// @todo Develop 1 universal solution
	IFXRESULT InitializeMeshMapFromCLODResource( IFXMeshMap **ppMeshMap);
	IFXRESULT InitializeMeshMapFromLineSetResource( IFXMeshMap **ppMeshMap);
	IFXRESULT InitializeMeshMapFromPointSetResource( IFXMeshMap **ppMeshMap);

	U32 m_uRefCount;
	
	/// 0 - CLOD, 1 - LineSet, 2 -PointSet 
	/// @todo Change to enums?
	U32 m_resourceType; 
};

#endif
