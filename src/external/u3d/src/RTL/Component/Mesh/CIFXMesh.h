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
// CIFXMesh.h
#ifndef CIFX_MESH_H
#define CIFX_MESH_H

#include "IFXMesh.h"
#include "IFXIDManager.h"
#include "IFXUVGenerator.h"
#include "IFXRenderContext.h"

class CIFXMesh : virtual public IFXMesh, virtual public IFXUnknown
{
	U32 m_refCount;
public:
	U32 IFXAPI  AddRef ();
	U32 IFXAPI  Release ();
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID interfaceId, void** ppInterface);

	friend IFXRESULT IFXAPI_CALLTYPE CIFXMeshFactory(IFXREFIID intId, void** ppPtr);
	
	//============================
	// IFXMesh Methods
	//============================
	// Allocate data for specified mesh format and size.
	virtual IFXRESULT	IFXAPI Allocate(	IFXVertexAttributes vaAttribs,
											U32 uNumVertices, 
											U32 uNumFaces );
	
	
	// Copy the entire source mesh.  Mesh will be re-allocated if not able to hold
	// all data from source mesh
	virtual IFXRESULT	IFXAPI Copy( IFXMesh& pSrcMesh );
	
	// Vertex attributes must be identical.  If they are not, the function will fail.
	// This method will do a fast copy of a partial mesh from the rSrcMesh mesh to 
	// this mesh.
	virtual IFXRESULT	IFXAPI FastCopy(	IFXMesh& rSrcMesh, 
											U32 uStartVertex,
											U32 uNumVertices,
											U32 uStartFace,
											U32 uNumFaces );
	
	virtual IFXRESULT	IFXAPI TransferData(	IFXMesh& rSrcMesh, 
												IFXMeshAttributes uTransferAttr,
												IFXMeshAttributes uCopyAttr );

	virtual IFXRESULT	IFXAPI IncreaseSize(U32 uNumNewFaces, U32 uNumNewVerts);
		
	virtual IFXRESULT	IFXAPI GetVertexIter(IFXVertexIter& iter);

	virtual IFXRESULT	IFXAPI GetVectorIter(IFXenum eType, IFXIterator& iter);

	virtual IFXRESULT	IFXAPI GetFaceIter(IFXFaceIter& iter);
	virtual IFXRESULT	IFXAPI GetU16FaceIter(IFXU16FaceIter& iter);
	virtual IFXRESULT	IFXAPI GetU32FaceIter(IFXU32FaceIter& iter);

	virtual IFXRESULT	IFXAPI GetLineIter(IFXLineIter& iter);
	virtual IFXRESULT	IFXAPI GetU16LineIter(IFXU16LineIter& iter);
	virtual IFXRESULT	IFXAPI GetU32LineIter(IFXU32LineIter& iter);

	virtual IFXRESULT	IFXAPI GetMeshData(	IFXenum eMeshAttribute, IFXInterleavedData*& pMeshData);
	virtual IFXRESULT	IFXAPI SetMeshData(	IFXenum eMeshAttribute, IFXInterleavedData* pMeshData);
	virtual IFXRESULT	IFXAPI GetInterleavedVertexData(IFXInterleavedData*& pMeshData);
	virtual IFXRESULT	IFXAPI PurgeRenderData();

	virtual IFXRESULT	IFXAPI GetVersionWord(IFXenum eMeshAttribute, U32& uVersionWord);

	virtual IFXRESULT	IFXAPI UpdateVersionWord(IFXenum eMeshAttribute);
	
	virtual IFXVertexAttributes IFXAPI GetAttributes();
	
	virtual U32			IFXAPI GetNumAllocatedVertices();
	virtual U32			IFXAPI GetMaxNumVertices();
	virtual U32			IFXAPI GetNumVertices();

	virtual U32			IFXAPI GetNumAllocatedFaces();
	virtual U32			IFXAPI GetMaxNumFaces();
	virtual U32			IFXAPI GetNumFaces();

	virtual IFXRESULT	IFXAPI AllocateLines(U32 uNumLines);
	virtual U32			IFXAPI GetNumAllocatedLines();
	virtual U32			IFXAPI GetMaxNumLines();
	virtual U32			IFXAPI GetNumLines();

	virtual IFXRESULT	IFXAPI SetMaxNumVertices(U32 uNumVertices);
	virtual IFXRESULT	IFXAPI SetMaxNumFaces(U32 uNumFaces);
	virtual IFXRESULT	IFXAPI SetMaxNumLines(U32 uNumFaces);
	virtual IFXRESULT	IFXAPI SetNumVertices(U32 uNumVertices);
	virtual IFXRESULT	IFXAPI SetNumFaces(U32 uNumFaces);
	virtual IFXRESULT	IFXAPI SetNumLines(U32 uNumFaces);

	virtual IFXRESULT	IFXAPI GetBoundingSphere(IFXVector4& vBoundSphere);
	virtual IFXRESULT	IFXAPI SetBoundingSphere(const IFXVector4& vBoundSphere);
	virtual IFXRESULT	IFXAPI CalcBoundingSphere();

	virtual BOOL		IFXAPI IsColorBGR(U32 uColorNum=0);	
	virtual IFXRESULT	IFXAPI SwizzleDiffuseColors();
	virtual IFXRESULT	IFXAPI SwizzleSpecularColors();

	U32					IFXAPI GetId() const;
	U32					IFXAPI NewId();
	void				IFXAPI SetId(U32);

	IFXRESULT			IFXAPI SetRenderTexCoordsInUse(IFXMeshAttributes eTexCoordBits);
	IFXMeshAttributes	IFXAPI GetRenderTexCoordsInUse();
	IFXUVMapParameters& IFXAPI GetUVMapParameters(U32 uTexLayer);

	IFXRESULT			IFXAPI SetNumTextureCoords(U32 uNumTextureCoords);
	U32					IFXAPI GetNumTextureCoords() const;

	virtual IFXRESULT   IFXAPI ReverseNormals();

	IFXRESULT IFXAPI   GetOffsetTransform( IFXMatrix4x4* pMatrix ) const;
	IFXRESULT IFXAPI   SetOffsetTransform( IFXMatrix4x4* pMatrix );

	virtual EIFXRenderable IFXAPI GetRenderableType() const;
	virtual void IFXAPI SetRenderableType( EIFXRenderable type );

protected:
	CIFXMesh();
	~CIFXMesh();

	virtual IFXRESULT	IFXAPI Construct();
	virtual IFXRESULT	IFXAPI Destroy();

	void				IFXAPI CalculateVectorSizes();
	U32					IFXAPI CalculateMeshVersion();
	IFXMeshAttributes	IFXAPI CalculateDirtyAttribs(IFXMeshAttributes& eInterleavedAttribs);
	IFXRESULT			IFXAPI InterleaveData(IFXMeshAttributes eDirtyAttribs, IFXMeshAttributes eAllAttribs);

	U32						m_puVectorSizes[IFX_MESH_NUM_ATTRIBUTES];

	IFXInterleavedDataPtr	m_pspMeshData[IFX_MESH_NUM_ATTRIBUTES];
	IFXInterleavedDataPtr	m_spInterleavedData;
	U32						m_pVersionsInterleaved[IFX_MESH_RENDER_TC0+IFX_MAX_TEXUNITS];
	U32						m_uNumVerticesInterleaved;

	IFXUVMapParameters		m_pUVMapParams[IFX_MAX_TEXUNITS];
	IFXMeshAttributes		m_eRenderTCsUsed;
	U32						m_uNumTCsUsed;
	IFXMeshAttributes		m_eInterleavedAttribs;

	U32						m_uNumAllocatedVertices;
	U32						m_uMaxNumVertices;
	U32						m_uNumVertices;

	U32						m_uNumAllocatedFaces;
	U32						m_uMaxNumFaces;
	U32						m_uNumFaces;

	U32						m_uNumAllocatedLines;
	U32						m_uMaxNumLines;
	U32						m_uNumLines;

	IFXVertexAttributes		m_vaAttribs;
	IFXVector4				m_vBoundingSphere;
	U32	m_uId;
	IFXMatrix4x4  m_offsetTransform;
	EIFXRenderable m_renderableType;
};

#endif
