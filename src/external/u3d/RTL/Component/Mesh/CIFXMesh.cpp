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

///	@file	CIFXMesh.cpp

#include "CIFXMesh.h"
#include "IFXCoreCIDs.h"
#include "IFXRenderingCIDs.h"
#include "IFXAttributes.h"
#include "IFXRenderServices.h"

static U32 g_sRenderElementId = 0;
U32 IFXGetNextRenderableId()
{
	return ++g_sRenderElementId;
}

//=============================
// Factory Function
//=============================
IFXRESULT IFXAPI_CALLTYPE CIFXMeshFactory(IFXREFIID intId, void** ppUnk)
{
	IFXRESULT rc = IFX_OK;
	if (ppUnk)
	{
		CIFXMesh* pPtr = new CIFXMesh;
		if (pPtr)
		{
			rc = pPtr->Construct();
			if (IFXFAILURE(rc))
				IFXDELETE(pPtr);
		}
		else
		{
			rc = IFX_E_OUT_OF_MEMORY;
		}
		if (IFXSUCCESS(rc))
		{
			pPtr->AddRef();
			rc = pPtr->QueryInterface(intId, ppUnk);
			pPtr->Release();
		}
	}
	else
	{
		rc = IFX_E_INVALID_POINTER;
	}
	return rc;
}

U32 CIFXMesh::AddRef()
{
	return ++m_refCount;
}

U32 CIFXMesh::Release()
{
	if (!(--m_refCount))
	{
		delete this;
		return 0;
	}
	return m_refCount;
}

IFXRESULT CIFXMesh::QueryInterface(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT result = IFX_OK;
	if (ppInterface)
	{
		if (interfaceId == IID_IFXMesh)
		{
			*(IFXMesh**)ppInterface = (IFXMesh*) this;
		}
		else
			if (interfaceId == CID_IFXMesh)
			{
				*(CIFXMesh**)ppInterface = (CIFXMesh*) this;
			}
			else
				if (interfaceId == IID_IFXUnknown)
				{
					*(IFXUnknown**)ppInterface = (IFXUnknown*) this;
				}
				else
				{
					*ppInterface = NULL;
					result = IFX_E_UNSUPPORTED;
				}
				if (IFXSUCCESS(result))
					AddRef();
	}
	else
	{
		result = IFX_E_INVALID_POINTER;
	}
	return result;
}

//================================
// Public Methods
//================================
CIFXMesh::CIFXMesh()
{
	m_refCount = 0;
	m_uId = IFXGetNextRenderableId();
	m_offsetTransform.MakeIdentity();
	m_renderableType = IFXRENDERABLE_ELEMENT_TYPE_MESH;
}

CIFXMesh::~CIFXMesh()
{
	Destroy();
}

IFXRESULT CIFXMesh::Allocate(IFXVertexAttributes vaAttribs,
							 U32 uNumVertices,
							 U32 uNumFaces )
{
	IFXRESULT rc = IFX_OK;

	if(uNumVertices > 0xffff)
	{
		vaAttribs.m_uData.m_b32BitIndices = TRUE;
	}
	else
	{
		vaAttribs.m_uData.m_b32BitIndices = FALSE;
	}

	m_uNumTCsUsed = vaAttribs.m_uData.m_uNumTexCoordLayers;

	if(IFXSUCCESS(rc))
	{
		m_vaAttribs = vaAttribs;
		U32 uFaceSize = sizeof(IFXU16Face);
		if(m_vaAttribs.m_uData.m_b32BitIndices)
		{
			uFaceSize = sizeof(IFXU32Face);
		}
		if(!m_pspMeshData[IFX_MESH_FACE].IsValid())
			rc = m_pspMeshData[IFX_MESH_FACE].Create(CID_IFXInterleavedData, IID_IFXInterleavedData);
		IFXRUN(rc, m_pspMeshData[IFX_MESH_FACE]->Allocate(1, &uFaceSize, uNumFaces));
	}

	if(IFXSUCCESS(rc))
	{
		U32 uSize = 0;
		if(m_vaAttribs.m_uData.m_bHasPositions)
		{
			uSize = sizeof(IFXVector3);
			if(!m_pspMeshData[IFX_MESH_POSITION].IsValid())
				rc = m_pspMeshData[IFX_MESH_POSITION].Create(CID_IFXInterleavedData, IID_IFXInterleavedData);
			rc = m_pspMeshData[IFX_MESH_POSITION]->Allocate(1, &uSize, uNumVertices);
		}

		if(IFXSUCCESS(rc) && m_vaAttribs.m_uData.m_bHasNormals)
		{
			uSize = sizeof(IFXVector3);
			if(!m_pspMeshData[IFX_MESH_NORMAL].IsValid())
				rc = m_pspMeshData[IFX_MESH_NORMAL].Create(CID_IFXInterleavedData, IID_IFXInterleavedData);
			rc = m_pspMeshData[IFX_MESH_NORMAL]->Allocate(1, &uSize, uNumVertices);
		}

		if(IFXSUCCESS(rc) && m_vaAttribs.m_uData.m_bHasDiffuseColors)
		{
			uSize = sizeof(U32);
			if(!m_pspMeshData[IFX_MESH_DIFFUSE_COLOR].IsValid())
				rc = m_pspMeshData[IFX_MESH_DIFFUSE_COLOR].Create(CID_IFXInterleavedData, IID_IFXInterleavedData);
			rc = m_pspMeshData[IFX_MESH_DIFFUSE_COLOR]->Allocate(1, &uSize, uNumVertices);
		}

		if(IFXSUCCESS(rc) && m_vaAttribs.m_uData.m_bHasSpecularColors)
		{
			uSize = sizeof(U32);
			if(!m_pspMeshData[IFX_MESH_SPECULAR_COLOR].IsValid())
				rc = m_pspMeshData[IFX_MESH_SPECULAR_COLOR].Create(CID_IFXInterleavedData, IID_IFXInterleavedData);
			rc = m_pspMeshData[IFX_MESH_SPECULAR_COLOR]->Allocate(1, &uSize, uNumVertices);
		}

		U32 i;
		for( i = 0; i < m_vaAttribs.m_uData.m_uNumTexCoordLayers; i++)
		{
			if(IFXSUCCESS(rc))
			{
				uSize = m_vaAttribs.GetTexCoordSize(i) * sizeof(F32);
				if(!m_pspMeshData[IFX_MESH_TC0 + i].IsValid())
					rc = m_pspMeshData[IFX_MESH_TC0 + i].Create(CID_IFXInterleavedData, IID_IFXInterleavedData);
				rc = m_pspMeshData[IFX_MESH_TC0 + i]->Allocate(1, &uSize, uNumVertices);
			}

		}
	}

	if(IFXSUCCESS(rc))
	{
		m_uNumAllocatedVertices = uNumVertices;
		m_uNumAllocatedFaces = uNumFaces;
		m_uMaxNumVertices = uNumVertices;
		m_uMaxNumFaces = uNumFaces;
		m_uNumVertices = uNumVertices;
		m_uNumFaces = uNumFaces;
		PurgeRenderData();
	}

	UpdateVersionWord(IFX_MESH_POSITION);

	return rc;
}


IFXRESULT CIFXMesh::Copy(IFXMesh& rSrcMesh)
{
	// If allocated, same type and same size
	// Then do a fast copy without allocating
	IFXRESULT rc = IFX_OK;

	// If the vertex attributes are the same, or we haven't been allocated yet,
	// do a fast copy.
	if(m_vaAttribs.m_uAllData == rSrcMesh.GetAttributes().m_uAllData ||
		m_uNumAllocatedVertices == 0 || m_uNumAllocatedFaces == 0)
	{
		// If mesh is too small, re-allocate
		if(m_uNumAllocatedVertices < rSrcMesh.GetMaxNumVertices() ||
			m_uNumAllocatedFaces < rSrcMesh.GetMaxNumFaces())
		{
			rc = Allocate(rSrcMesh.GetAttributes(),
				rSrcMesh.GetMaxNumVertices(),
				rSrcMesh.GetMaxNumFaces());
		}

		if(IFXSUCCESS(rc))
		{
			rc = FastCopy(rSrcMesh,
				0, rSrcMesh.GetMaxNumVertices(),
				0, rSrcMesh.GetMaxNumFaces());
		}

		if(IFXSUCCESS(rc))
		{
			rSrcMesh.SetMaxNumVertices(m_uMaxNumVertices);
			rSrcMesh.SetMaxNumFaces(m_uMaxNumFaces);

			rSrcMesh.SetNumVertices(m_uNumVertices);
			rSrcMesh.SetNumFaces(m_uNumFaces);
		}

		return rc;
	}

	// If we get this far, we are allocated.  Do a smart copy to fill allocation
	// use iterators to copy the data
	IFXVertexAttributes srcAttribs;
	IFXFaceIter   pSrcFaceIter, pTgtFaceIter;
	IFXVertexIter pSrcVertIter, pTgtVertIter;
	IFXVector3    *pSrcVector, *pTgtVector;
	IFXVector2    *pSrcTex, *pTgtTex;
	IFXFace     *pSrcFace, *pTgtFace;
	U32       *pSrcColor, *pTgtColor;
	U32 i;

	m_uNumVertices = rSrcMesh.GetNumVertices();
	m_uNumFaces = rSrcMesh.GetNumFaces();

	// get the iterators
	rSrcMesh.GetFaceIter(pSrcFaceIter);
	GetFaceIter(pTgtFaceIter);
	rSrcMesh.GetVertexIter(pSrcVertIter);
	GetVertexIter(pTgtVertIter);

	U32 limit = (m_uNumAllocatedFaces < rSrcMesh.GetMaxNumFaces())
		? m_uNumAllocatedFaces : rSrcMesh.GetMaxNumFaces();

	srcAttribs = rSrcMesh.GetAttributes();

	// generic copy of faces
	for (i = 0; i < limit; i++) {
		pSrcFace = pSrcFaceIter.Next();
		pTgtFace = pTgtFaceIter.Next();
		pTgtFace->Set(pSrcFace->VertexA(), pSrcFace->VertexB(), pSrcFace->VertexC());
	}

	limit = (m_uNumAllocatedVertices < rSrcMesh.GetMaxNumVertices())
		? m_uNumAllocatedVertices : rSrcMesh.GetMaxNumVertices();

	int minLayers = (srcAttribs.m_uData.m_uNumTexCoordLayers < m_vaAttribs.m_uData.m_uNumTexCoordLayers)
		? srcAttribs.m_uData.m_uNumTexCoordLayers : m_vaAttribs.m_uData.m_uNumTexCoordLayers;

	// generic copy of verts
	for (i = 0; i < limit; i++) {
		if (m_vaAttribs.m_uData.m_bHasNormals && srcAttribs.m_uData.m_bHasNormals) {
			pTgtVector = pTgtVertIter.GetNormal();
			pSrcVector = pSrcVertIter.GetNormal();
			*pTgtVector = *pSrcVector;
		}
		if (m_vaAttribs.m_uData.m_bHasPositions && srcAttribs.m_uData.m_bHasPositions) {
			pTgtVector = pTgtVertIter.GetPosition();
			pSrcVector = pSrcVertIter.GetPosition();
			*pTgtVector = *pSrcVector;
		}
		if (m_vaAttribs.m_uData.m_bHasDiffuseColors && srcAttribs.m_uData.m_bHasDiffuseColors) {
			pTgtColor = pTgtVertIter.GetColor(0);
			pSrcColor = pSrcVertIter.GetColor(0);
			*pTgtColor = *pSrcColor;
		}

		if (m_vaAttribs.m_uData.m_bHasSpecularColors && srcAttribs.m_uData.m_bHasSpecularColors) {
			pTgtColor = pTgtVertIter.GetColor(1);
			pSrcColor = pSrcVertIter.GetColor(1);
			*pTgtColor = *pSrcColor;
		}

		int j;
		for ( j = 0; j < minLayers; j++) {
			pSrcTex = pSrcVertIter.GetTexCoord(j);
			pTgtTex = pTgtVertIter.GetTexCoord(j);
			pTgtTex->Set(pSrcTex->U(), pSrcTex->V());
		}

		// advance
		pSrcVertIter.Next();
		pTgtVertIter.Next();
	}
	m_uId = rSrcMesh.GetId();
	UpdateVersionWord(IFX_MESH_POSITION);

	PurgeRenderData();

	return IFX_OK;
}

// Vertex attributes must be identical.  If they are not, the function will fail.
// This method will do a fast copy of a partial mesh from the rSrcMesh mesh to
// this mesh.
IFXRESULT IFXAPI CIFXMesh::FastCopy(IFXMesh& rSrcMesh,
									U32 uStartVertex,
									U32 uNumVertices,
									U32 uStartFace,
									U32 uNumFaces )
{
	IFXRESULT rc = IFX_OK;

	if(m_vaAttribs.m_uAllData != rSrcMesh.GetAttributes().m_uAllData)
	{
		rc = IFX_E_INVALID_POINTER;
	}

	// First get a CIFXMesh ptr so that we can modify data directly.
	CIFXMesh* pMesh = 0;

	if(IFXSUCCESS(rc))
	{
		rc = rSrcMesh.QueryInterface(CID_IFXMesh, (void**)&pMesh);
	}

	// First copy vertices
	if(IFXSUCCESS(rc))
	{
		U32 i;
		for( i = 0; i < IFX_MESH_FACE && IFXSUCCESS(rc); i++)
		{
			if(m_pspMeshData[i].IsValid() && m_pspMeshData[i]->GetStride())
			{
				rc = m_pspMeshData[i]->CopyData(*(pMesh->m_pspMeshData[i].GetPointerNR()),
					uStartVertex, uNumVertices);
			}
		}
	}

	// Now copy faces
	if(IFXSUCCESS(rc))
	{
		if(m_pspMeshData[IFX_MESH_FACE].IsValid() && m_pspMeshData[IFX_MESH_FACE]->GetStride())
		{
			rc = m_pspMeshData[IFX_MESH_FACE]->CopyData(
				*(pMesh->m_pspMeshData[IFX_MESH_FACE].GetPointerNR()),
				uStartFace, uNumFaces );
		}
	}

	// Now copy lines
	if(IFXSUCCESS(rc))
	{
		if(m_pspMeshData[IFX_MESH_LINE].IsValid() && m_pspMeshData[IFX_MESH_LINE]->GetStride())
		{
			rc = m_pspMeshData[IFX_MESH_LINE]->CopyData(
				*(pMesh->m_pspMeshData[IFX_MESH_LINE].GetPointerNR()),
				uStartFace, uNumFaces );
		}
	}

	IFXRELEASE(pMesh);

	// Dirty the any existing interleaved array
	UpdateVersionWord(IFX_MESH_POSITION);

	return rc;
}

IFXRESULT CIFXMesh::TransferData(IFXMesh& rSrcMesh,
								 IFXMeshAttributes uTransferAttr,
								 IFXMeshAttributes uCopyAttr )
{
	IFXRESULT rc = IFX_OK;
	IFXASSERT(! (uTransferAttr & uCopyAttr).toBool()); // the transfer and the copy attributes should not overlap

	IFXMeshAttributes eAllAttribs, eDirtyAttribs;
	eDirtyAttribs = CalculateDirtyAttribs(eAllAttribs);

	if(&rSrcMesh == (IFXMesh*)this)
	{
		rc = IFX_E_BAD_PARAM;
	}

	if(IFXSUCCESS(rc) && (uTransferAttr & uCopyAttr).toBool())
	{
		// the transfer and the coppy attributes should not overlap
		rc = IFX_E_BAD_PARAM;
	}

	if(IFXSUCCESS(rc))
	{
		// Get rid of any renderable info in the src mesh.
		rSrcMesh.PurgeRenderData();
	}

	// free all the current attribute arrays.
	if(IFXSUCCESS(rc))
	{
		m_uNumAllocatedVertices = rSrcMesh.GetNumAllocatedVertices();
		m_uMaxNumVertices = rSrcMesh.GetMaxNumVertices();
		m_uNumVertices = rSrcMesh.GetNumVertices();

		m_uNumAllocatedFaces = rSrcMesh.GetNumAllocatedFaces();
		m_uMaxNumFaces = rSrcMesh.GetMaxNumFaces();
		m_uNumFaces = rSrcMesh.GetNumFaces();

		m_uNumAllocatedLines = rSrcMesh.GetNumAllocatedLines();
		m_uMaxNumLines = rSrcMesh.GetMaxNumLines();
		m_uNumLines = rSrcMesh.GetNumLines();

		m_vaAttribs.m_uAllData = 0;
	}

	// Fixup the VertexAttributes
	if(IFXSUCCESS(rc))
	{
		m_uId = rSrcMesh.GetId();
		IFXVertexAttributes va = rSrcMesh.GetAttributes();
		if((uTransferAttr[IFX_MESH_POSITION] && va.m_uData.m_bHasPositions)
			|| uCopyAttr[IFX_MESH_POSITION])
		{
			m_vaAttribs.m_uData.m_bHasPositions = 1;
		}

		if((uTransferAttr[IFX_MESH_NORMAL] && va.m_uData.m_bHasNormals)
			|| uCopyAttr[IFX_MESH_NORMAL])

		{
			m_vaAttribs.m_uData.m_bHasNormals = 1;
		}

		if(((uTransferAttr[IFX_MESH_DIFFUSE_COLOR] && va.m_uData.m_bHasDiffuseColors)
			|| uCopyAttr[IFX_MESH_DIFFUSE_COLOR])
			)
		{
			m_vaAttribs.m_uData.m_bHasDiffuseColors= 1;
		}
		if((uTransferAttr[IFX_MESH_SPECULAR_COLOR] && va.m_uData.m_bHasSpecularColors)
			|| uCopyAttr[IFX_MESH_SPECULAR_COLOR])

		{
			m_vaAttribs.m_uData.m_bHasSpecularColors = 1;
		}

		m_vaAttribs.m_uData.m_uNumTexCoordLayers = 0;
		m_uNumTCsUsed = 0;
		U32 TestBit = IFX_MESH_TC0;
		while(TestBit < (IFX_MESH_TC0+IFX_MAX_TEXUNITS)
			&& ((uTransferAttr[TestBit] && m_vaAttribs.m_uData.m_uNumTexCoordLayers < va.m_uData.m_uNumTexCoordLayers)
			|| uCopyAttr[TestBit]) )
		{
			m_vaAttribs.m_uData.m_uNumTexCoordLayers++;
			m_uNumTCsUsed++;
			TestBit++;
		}

		m_vaAttribs.m_uData.m_uTexCoordSize0 =  va.m_uData.m_uTexCoordSize0;
		m_vaAttribs.m_uData.m_uTexCoordSize1 =  va.m_uData.m_uTexCoordSize1;
		m_vaAttribs.m_uData.m_uTexCoordSize2 =  va.m_uData.m_uTexCoordSize2;
		m_vaAttribs.m_uData.m_uTexCoordSize3 =  va.m_uData.m_uTexCoordSize3;
		m_vaAttribs.m_uData.m_uTexCoordSize4 =  va.m_uData.m_uTexCoordSize4;
		m_vaAttribs.m_uData.m_uTexCoordSize5 =  va.m_uData.m_uTexCoordSize5;
		m_vaAttribs.m_uData.m_uTexCoordSize6 =  va.m_uData.m_uTexCoordSize6;
		m_vaAttribs.m_uData.m_uTexCoordSize7 =  va.m_uData.m_uTexCoordSize7;

		// use the same size of indeces
		m_vaAttribs.m_uData.m_b32BitIndices =  va.m_uData.m_b32BitIndices;

		CalculateVectorSizes();
	}

	// Transfer and copy the selected arrays
	if(IFXSUCCESS(rc))
	{
		U32 TransferIdx = 0;
		IFXInterleavedData* pTmpData = NULL;
		U32 vctrSize = 0;
		U32 NumVerts = 0;
		U32 NumVctrs = 0;

		while(IFXSUCCESS(rc) && TransferIdx < IFX_MESH_NUM_ATTRIBUTES)
		{
			if(uTransferAttr[0])
			{
				if(IFXSUCCESS(rSrcMesh.GetMeshData(TransferIdx, pTmpData)))
				{
					m_pspMeshData[TransferIdx] = pTmpData;
					IFXRELEASE(pTmpData);
				}
			}
			else if(uCopyAttr[0])
			{
				if(IFXSUCCESS(rSrcMesh.GetMeshData(TransferIdx, pTmpData)))
				{
					if(!m_pspMeshData[TransferIdx].IsValid())
						m_pspMeshData[TransferIdx].Create(CID_IFXInterleavedData, IID_IFXInterleavedData);

					NumVerts = 0;

					if(pTmpData)
					{
						pTmpData->GetInfo(NumVctrs, &vctrSize, NumVerts);
						if(NumVerts)
						{
							rc = m_pspMeshData[TransferIdx]->Allocate(1, &vctrSize, NumVerts);
							if(IFXSUCCESS(rc))
							{
								rc = m_pspMeshData[TransferIdx]->CopyData(*pTmpData, 0, NumVerts);
								m_pspMeshData[TransferIdx]->UpdateVersionWord(0);
							}
						}
						pTmpData->Release();
					}

					if(NumVerts == 0)
					{ // allocate a new array
						if(TransferIdx != IFX_MESH_FACE && TransferIdx != IFX_MESH_LINE)
						{
							rc = m_pspMeshData[TransferIdx]->Allocate(1, &m_puVectorSizes[TransferIdx], m_uMaxNumVertices);
						}
						else if(TransferIdx == IFX_MESH_FACE)// face array
						{
							rc = m_pspMeshData[TransferIdx]->Allocate(1, &m_puVectorSizes[TransferIdx], m_uMaxNumFaces);
						}
						else // line array
						{
							rc = m_pspMeshData[TransferIdx]->Allocate(1, &m_puVectorSizes[TransferIdx], m_uMaxNumLines);
						}
					}
				}
			}
			else
			{ // Set the array pointer to NULL
				m_pspMeshData[TransferIdx] = 0;
				m_puVectorSizes[TransferIdx] = 0;
			}

			uCopyAttr = uCopyAttr >>1;
			uTransferAttr = uTransferAttr >>1;
			++TransferIdx;
		}
	}

	if(IFXSUCCESS(rc))
	{
		IFXMeshAttributes eNewAllAttribs;
		eDirtyAttribs = CalculateDirtyAttribs(eNewAllAttribs);

		if(eNewAllAttribs != eAllAttribs)
			PurgeRenderData();
	}

	return rc;
}


IFXRESULT CIFXMesh::IncreaseSize(U32 uNumNewFaces, U32 uNumNewVerts)
{
	IFXRESULT rc = IFX_OK;

	U32 uTotalFaces = m_uMaxNumFaces + uNumNewFaces;
	U32 uTotalVerts = m_uMaxNumVertices + uNumNewVerts;

	if(uTotalFaces < m_uNumAllocatedFaces)
	{
		// We already have enough faces allocated.
		m_uMaxNumFaces = uTotalFaces;
	}
	else
	{
		// Need to reallocate faces and copy old faces to it.
		IFXInterleavedData* pNewFaces = 0;
		rc = IFXCreateComponent(CID_IFXInterleavedData,
			IID_IFXInterleavedData,
			(void**)&pNewFaces);

		if(IFXSUCCESS(rc))
		{
			U32 uIndexSize = 16;
			U32 uFaceSize = sizeof(IFXU16Face);
			if(m_vaAttribs.m_uData.m_b32BitIndices)
			{
				uFaceSize = sizeof(IFXU32Face);
				uIndexSize = 32;
			}

			if(uTotalVerts > 0xFFFF)
			{
				m_vaAttribs.m_uData.m_b32BitIndices = TRUE;
				uFaceSize = sizeof(IFXU32Face);
			}

			rc = pNewFaces->Allocate(1, &uFaceSize, uTotalFaces);

			if(IFXSUCCESS(rc))
			{
				if( (m_vaAttribs.m_uData.m_b32BitIndices && (uIndexSize == 32)) ||
					((!m_vaAttribs.m_uData.m_b32BitIndices) && (uIndexSize == 16)) )
				{
					// We can do a memcpy
					rc = pNewFaces->CopyData( *(m_pspMeshData[IFX_MESH_FACE].GetPointerNR()),
						0, m_uMaxNumFaces);
				}
				else
				{
					// We need to convert 16 bit faces to 32 bit faces
					IFXFaceIter srcIter, dstIter;
					srcIter.Set32Bit(FALSE);
					dstIter.Set32Bit(TRUE);
					m_pspMeshData[IFX_MESH_FACE]->GetVertexIter(srcIter);
					pNewFaces->GetVertexIter(dstIter);

					IFXFace* pSrc;
					IFXFace* pDst;
					U32 i;
					for( i = 0; i < m_uMaxNumFaces; i++)
					{
						pSrc = srcIter.Next();
						pDst = dstIter.Next();

						pDst->Set(pSrc->VertexA(), pSrc->VertexB(), pSrc->VertexC());
					}
				}
			}
		}

		if(IFXSUCCESS(rc))
		{
			m_pspMeshData[IFX_MESH_FACE] = pNewFaces;
			IFXRELEASE(pNewFaces);

			m_uMaxNumFaces = uTotalFaces;
			m_uNumAllocatedFaces = uTotalFaces;
		}
	}

	if(m_uNumAllocatedVertices < uTotalVerts)
	{
		// Iterate over the vertex data, create a bigger memory pool,
		// copy the old pool into the new pool, release the old pool,
		// and keep the new pool.  We need to check each existing pool
		// to see if it needs to be re-allocated.
		U32 i;
		for( i = 0; (i < IFX_MESH_FACE) && IFXSUCCESS(rc); i++)
		{
			if(m_pspMeshData[i].IsValid() && m_pspMeshData[i]->GetStride())
			{
				U32 uNumVectors = 0;
				U32 uNumVertices = 0;
				m_pspMeshData[i]->GetInfo(uNumVectors, 0, uNumVertices);

				if(uNumVertices < uTotalVerts)
				{
					IFXInterleavedData* pData = 0;

					rc = IFXCreateComponent(CID_IFXInterleavedData,
						IID_IFXInterleavedData,
						(void**)&pData);

					if(IFXSUCCESS(rc))
					{
						U32 uStride = m_pspMeshData[i]->GetStride();
						rc = pData->Allocate(1, &uStride, uTotalVerts);
					}

					if(IFXSUCCESS(rc))
					{
						rc = pData->CopyData(*(m_pspMeshData[i].GetPointerNR()), 0, m_uMaxNumVertices);
					}

					if(IFXSUCCESS(rc))
					{
						m_pspMeshData[i] = pData;
						IFXRELEASE(pData);
					}
				}
			}
		}

		if(IFXSUCCESS(rc))
		{
			m_uNumAllocatedVertices = uTotalVerts;
			PurgeRenderData();
		}
	}

	if(IFXSUCCESS(rc))
	{
		m_uMaxNumVertices = uTotalVerts;
	}

	return rc;
}

IFXRESULT CIFXMesh::GetVertexIter(IFXVertexIter& iter)
{
	iter.SetVertexAttributes(m_vaAttribs);

	U32 i;
	for( i = 0; i < IFX_MESH_FACE; i++)
	{
		if(m_pspMeshData[i].IsValid())
		{
			iter.SetDataPtr(i, m_pspMeshData[i]->GetDataPtr());
			iter.SetDataStride(i, m_pspMeshData[i]->GetStride());
		}
		else
		{
			iter.SetDataPtr(i, 0);
			iter.SetDataStride(i, 0);
		}
	}

	return IFX_OK;
}

IFXRESULT CIFXMesh::GetVectorIter(IFXenum eType, IFXIterator& iter)
{
	IFXRESULT rc = IFX_OK;

	if(eType < IFX_MESH_NUM_ATTRIBUTES)
	{
		if(m_pspMeshData[eType].IsValid())
		{
			rc = m_pspMeshData[eType]->GetVectorIter(0, iter);
		}
		else
		{
			rc = IFX_E_INVALID_RANGE;
		}
	}
	else
	{
		rc = IFX_E_INVALID_RANGE;
	}

	return rc;
}

IFXRESULT CIFXMesh::GetFaceIter(IFXFaceIter& iter)
{
	if(m_pspMeshData[IFX_MESH_FACE].IsValid())
	{
		iter.Set32Bit(m_vaAttribs.m_uData.m_b32BitIndices);
		return m_pspMeshData[IFX_MESH_FACE]->GetVertexIter(iter);
	}

	return IFX_E_UNSUPPORTED;
}

IFXRESULT CIFXMesh::GetU16FaceIter(IFXU16FaceIter& iter)
{
	if(m_pspMeshData[IFX_MESH_FACE].IsValid())
	{
		return m_pspMeshData[IFX_MESH_FACE]->GetVertexIter(iter);
	}

	return IFX_E_UNSUPPORTED;
}

IFXRESULT CIFXMesh::GetU32FaceIter(IFXU32FaceIter& iter)
{
	if(m_pspMeshData[IFX_MESH_FACE].IsValid())
	{
		return m_pspMeshData[IFX_MESH_FACE]->GetVertexIter(iter);
	}

	return IFX_E_UNSUPPORTED;
}


IFXRESULT CIFXMesh::GetLineIter(IFXLineIter& iter)
{
	if(m_pspMeshData[IFX_MESH_LINE].IsValid())
	{
		iter.Set32Bit(m_vaAttribs.m_uData.m_b32BitIndices);
		return m_pspMeshData[IFX_MESH_LINE]->GetVertexIter(iter);
	}

	return IFX_E_UNSUPPORTED;
}

IFXRESULT CIFXMesh::GetU16LineIter(IFXU16LineIter& iter)
{
	if(m_pspMeshData[IFX_MESH_LINE].IsValid())
	{
		return m_pspMeshData[IFX_MESH_LINE]->GetVertexIter(iter);
	}

	return IFX_E_UNSUPPORTED;
}

IFXRESULT CIFXMesh::GetU32LineIter(IFXU32LineIter& iter)
{
	if(m_pspMeshData[IFX_MESH_LINE].IsValid())
	{
		return m_pspMeshData[IFX_MESH_LINE]->GetVertexIter(iter);
	}

	return IFX_E_UNSUPPORTED;
}

IFXRESULT CIFXMesh::GetInterleavedVertexData(IFXInterleavedData*& pMeshData)
{
	IFXRESULT rc = IFX_OK;

	CalculateVectorSizes();

	IFXMeshAttributes eAllAttribs;
	IFXMeshAttributes eDirtyAttribs = CalculateDirtyAttribs(eAllAttribs);

	if(eDirtyAttribs.toBool())
	{
		// First see if we need to allocate a new interleaved array
		BOOL bNeedToAllocate = FALSE;

		U32 uNumVectors = 0;
		U32* puVectorSizes = 0;
		U32 uVectorNum = 0;

		if(m_spInterleavedData.IsValid() == FALSE)
		{
			bNeedToAllocate = TRUE;
			m_spInterleavedData.Create(CID_IFXInterleavedData, IID_IFXInterleavedData);
		}
		else if(m_uNumVertices > m_spInterleavedData->GetNumVertices())
		{
			bNeedToAllocate = TRUE;
		}
		else if(eAllAttribs != m_eInterleavedAttribs)
		{
			bNeedToAllocate = TRUE;
		}

		if(bNeedToAllocate && IFXSUCCESS(rc))
		{
			m_eInterleavedAttribs = eAllAttribs;
			eDirtyAttribs = eAllAttribs;
			uVectorNum = 0;
			U32 i = 0;
			for(i = 0; i <= IFX_MESH_NUM_ATTRIBUTES; i++)
			{
				if(eAllAttribs[i])
				{
					uVectorNum++;
				}
			}

			IFXDELETE_ARRAY(puVectorSizes);
			uNumVectors = uVectorNum;
			puVectorSizes = new U32[uNumVectors];

			uVectorNum = 0;
			for(i = 0; i <= IFX_MESH_NUM_ATTRIBUTES; i++)
			{
				if(eAllAttribs[i])
				{
					puVectorSizes[uVectorNum++] = m_pspMeshData[i]->GetStride();
				}
			}

			rc = m_spInterleavedData->Allocate(uNumVectors, puVectorSizes, m_uMaxNumVertices);
		}

		IFXDELETE_ARRAY(puVectorSizes);

		if(IFXSUCCESS(rc))
		{
			rc = InterleaveData(eDirtyAttribs, eAllAttribs);
		}

	}

	if(IFXSUCCESS(rc))
	{
		pMeshData = m_spInterleavedData.GetPointerAR();
	}


	return rc;
}

IFXRESULT CIFXMesh::InterleaveData(IFXMeshAttributes eDirtyAttribs, IFXMeshAttributes eAllAttribs)
{
	// Now we're allocated, we just need to copy the data!  This is the slow part...
	IFXRESULT rc = IFX_OK;

	U32 uVectorNum = 0;

	IFXVector3Iter  PosSrcIter, PosDstIter;
	IFXVector3Iter  NormSrcIter, NormDstIter;
	IFXU32Iter    DiffuseSrcIter, DiffuseDstIter;
	IFXU32Iter    SpecularSrcIter, SpecularDstIter;
	IFXVector2Iter  TCSrcIter[IFX_MAX_TEXUNITS], TCDstIter[IFX_MAX_TEXUNITS];

	// If the Dirty attribs and all attribs are just one attribute, namely
	// just positions, then we can get away with a memcpy
	if((eDirtyAttribs == eAllAttribs) && (eAllAttribs == IFXMeshAttributes(IFX_MESH_POSITION)))
	{
		// just memcpy the positions into the interleaved data
		m_spInterleavedData->CopyData(*m_pspMeshData[IFX_MESH_POSITION].GetPointerNR(),
			0, m_uNumVertices);

		m_uNumVerticesInterleaved = m_uNumVertices;
	}
	else
	{
		if(eDirtyAttribs[IFX_MESH_POSITION])
		{
			m_pspMeshData[IFX_MESH_POSITION]->GetVectorIter(0, PosSrcIter);
			m_spInterleavedData->GetVectorIter(uVectorNum, PosDstIter);
		}

		if(eAllAttribs[IFX_MESH_POSITION])
		{
			uVectorNum++;
		}

		if(eDirtyAttribs[IFX_MESH_NORMAL])
		{
			m_pspMeshData[IFX_MESH_NORMAL]->GetVectorIter(0, NormSrcIter);
			m_spInterleavedData->GetVectorIter(uVectorNum, NormDstIter);
		}

		if(eAllAttribs[IFX_MESH_NORMAL])
		{
			uVectorNum++;
		}

		if(eDirtyAttribs[IFX_MESH_DIFFUSE_COLOR])
		{
			m_pspMeshData[IFX_MESH_DIFFUSE_COLOR]->GetVectorIter(0, DiffuseSrcIter);
			m_spInterleavedData->GetVectorIter(uVectorNum, DiffuseDstIter);
		}

		if(eAllAttribs[IFX_MESH_DIFFUSE_COLOR])
		{
			uVectorNum++;
		}

		if(eDirtyAttribs[IFX_MESH_SPECULAR_COLOR])
		{
			m_pspMeshData[IFX_MESH_SPECULAR_COLOR]->GetVectorIter(0, SpecularSrcIter);
			m_spInterleavedData->GetVectorIter(uVectorNum, SpecularDstIter);
		}

		if(eAllAttribs[IFX_MESH_SPECULAR_COLOR])
		{
			uVectorNum++;
		}

		U32 uMaxTex = 0;
		U32 i = 0;
		for(i = 0; i < IFX_MAX_TEXUNITS; i++)
		{
			if(eDirtyAttribs[IFX_MESH_TC0 + i])
			{
				m_pspMeshData[IFX_MESH_TC0+i]->GetVectorIter(0, TCSrcIter[i]);
				m_spInterleavedData->GetVectorIter(uVectorNum, TCDstIter[i]);
				uMaxTex = i;

				// This is so that we only have one test during the per vertex
				// loop below.
				eDirtyAttribs |= IFX_MESH_RENDER_TC0 + i;
			}
			else if(eDirtyAttribs[IFX_MESH_RENDER_TC0 + i])
			{
				m_pspMeshData[IFX_MESH_RENDER_TC0+i]->GetVectorIter(0, TCSrcIter[i]);
				m_spInterleavedData->GetVectorIter(uVectorNum, TCDstIter[i]);
				uMaxTex = i;
			}

			if(eAllAttribs[IFX_MESH_TC0 + i])
			{
				uVectorNum++;
			}
			else if(eAllAttribs[IFX_MESH_RENDER_TC0 + i])
			{
				uVectorNum++;
			}
		}

		// First copy positions, normals, and colors (everything that's fixed size)
		// This will also copy texture coordinates with dimension 2
		U32 uTex = 0;
		U32 uVert;
		for( uVert = 0; uVert < m_uNumVertices; uVert++)
		{
			if(eDirtyAttribs[IFX_MESH_POSITION])
			{
				*PosDstIter.Next() = *PosSrcIter.Next();
			}

			if(eDirtyAttribs[IFX_MESH_NORMAL])
			{
				*NormDstIter.Next() = *NormSrcIter.Next();
			}

			if(eDirtyAttribs[IFX_MESH_DIFFUSE_COLOR])
			{
				*DiffuseDstIter.Next() = *DiffuseSrcIter.Next();
			}

			if(eDirtyAttribs[IFX_MESH_SPECULAR_COLOR])
			{
				*SpecularDstIter.Next() = *SpecularSrcIter.Next();
			}

			for(i = 0, uTex = IFX_MESH_RENDER_TC0; i <= uMaxTex; i++)
			{
				if( eDirtyAttribs[uTex] )
				{
					*TCDstIter[i].Next() = *TCSrcIter[i].Next();
				}

				uTex++;
			}
		}

		m_uNumVerticesInterleaved = m_uNumVertices;
	}

	if(IFXSUCCESS(rc))
	{
		m_spInterleavedData->UpdateVersionWord(0);
	}

	return rc;
}

IFXRESULT CIFXMesh::PurgeRenderData()
{
	m_eInterleavedAttribs = 0;
	if(m_spInterleavedData.IsValid()) 
	{
		IFXRenderServices* pRenderServices = NULL;
		if (IFXSUCCESS(IFXCreateComponent(CID_IFXRenderServices, IID_IFXRenderServices, (void**)&pRenderServices))) 
		{
			pRenderServices->DeleteStaticMesh(m_spInterleavedData->GetId());
			pRenderServices->Release();
		}
	}
	m_spInterleavedData = 0;
	m_uNumVerticesInterleaved = 0;
	U32 i = 0;

	for(i = 0; i < (IFX_MESH_TC0+IFX_MAX_TEXUNITS); i++)
	{
		m_pVersionsInterleaved[i] = 0;
	}

	for(i = 0; i < IFX_MAX_TEXUNITS; i++)
	{
		m_pspMeshData[IFX_MESH_RENDER_TC0+i] = 0;
		m_pUVMapParams[i].Reset();
	}

	m_eRenderTCsUsed = 0;

	return IFX_OK;
}

IFXRESULT CIFXMesh::GetMeshData(IFXenum eMeshAttribute, IFXInterleavedData*& pMeshData)
{
	IFXRESULT rc = IFX_OK;
	IFXASSERTBOX(eMeshAttribute < IFX_MESH_NUM_ATTRIBUTES,
		"Invalid Mesh Attribute access.");
	pMeshData = m_pspMeshData[eMeshAttribute].GetPointerAR();

	return rc;
}

IFXRESULT CIFXMesh::SetMeshData(IFXenum eMeshAttribute, IFXInterleavedData* pMeshData)
{
	IFXRESULT rc = IFX_OK;

	IFXASSERTBOX(eMeshAttribute < IFX_MESH_NUM_ATTRIBUTES,
		"Invalid Mesh Attribute access.");
	BOOL bEnable = pMeshData && (pMeshData->GetNumVertices() > 0);
	U32 uTexLayer = eMeshAttribute - IFX_MESH_TC0;
	switch(eMeshAttribute)
	{
	case IFX_MESH_NORMAL:
		m_vaAttribs.m_uData.m_bHasNormals = bEnable;
		break;
	case IFX_MESH_DIFFUSE_COLOR:
		m_vaAttribs.m_uData.m_bHasDiffuseColors= bEnable;
		break;
	case IFX_MESH_SPECULAR_COLOR:
		m_vaAttribs.m_uData.m_bHasSpecularColors = bEnable;
		break;
	case IFX_MESH_FACE:
		if(bEnable)
			m_uMaxNumFaces = pMeshData->GetNumVertices();
		break;
	case IFX_MESH_LINE:
		if(bEnable)
			m_uMaxNumLines = pMeshData->GetNumVertices();
		break;
	default:
		if ((eMeshAttribute >= IFX_MESH_TC0) && (IFX_MESH_TC0 < (IFX_MESH_TC0+IFX_MAX_TEXUNITS))) {
			if(uTexLayer == m_vaAttribs.m_uData.m_uNumTexCoordLayers && bEnable)
			{
				m_vaAttribs.m_uData.m_uNumTexCoordLayers++;
				m_uNumTCsUsed++;
			}
			else if(bEnable && uTexLayer > m_vaAttribs.m_uData.m_uNumTexCoordLayers)
			{
				rc = IFX_E_INVALID_RANGE;
			}
		}
	}

	if(IFXSUCCESS(rc))
		m_pspMeshData[eMeshAttribute] = pMeshData;

	return rc;
}

IFXRESULT CIFXMesh::GetVersionWord( IFXenum eMeshAttribute, U32& uVersionWord)
{
	IFXRESULT rc = IFX_OK;
	IFXASSERTBOX(eMeshAttribute < IFX_MESH_NUM_ATTRIBUTES, "Invalid Attribute specified!");
	if(eMeshAttribute >= IFX_MESH_NUM_ATTRIBUTES)
	{
		rc = IFX_E_INVALID_RANGE;
	}
	else
	{
		uVersionWord = m_pspMeshData[eMeshAttribute]->GetVersionWord(0);
	}

	return rc;
}

IFXRESULT CIFXMesh::UpdateVersionWord(IFXenum eMeshAttribute)
{
	IFXRESULT rc = IFX_OK;

	IFXASSERTBOX(eMeshAttribute < IFX_MESH_NUM_ATTRIBUTES, "Invalid Attribute specified!");
	if(eMeshAttribute >= IFX_MESH_NUM_ATTRIBUTES)
	{
		rc = IFX_E_INVALID_RANGE;
	}
	else
	{
		if(m_pspMeshData[eMeshAttribute].IsValid())
			m_pspMeshData[eMeshAttribute]->UpdateVersionWord(0);
	}

	return rc;
}

IFXVertexAttributes CIFXMesh::GetAttributes()
{
	return m_vaAttribs;
}

U32 CIFXMesh::GetNumAllocatedVertices()
{
	return m_uNumAllocatedVertices;
}

U32 CIFXMesh::GetMaxNumVertices()
{
	return m_uMaxNumVertices;
}

U32 CIFXMesh::GetNumVertices()
{
	return m_uNumVertices;
}

U32 CIFXMesh::GetNumAllocatedFaces()
{
	if(m_pspMeshData[IFX_MESH_FACE].IsValid())
		return m_pspMeshData[IFX_MESH_FACE]->GetNumVertices();
	return 0;
}

U32 CIFXMesh::GetMaxNumFaces()
{
	return m_uMaxNumFaces;
}

U32 CIFXMesh::GetNumFaces()
{
	return m_uNumFaces;
}

IFXRESULT CIFXMesh::AllocateLines(U32 uNumLines)
{
	IFXRESULT rc = IFX_OK;
	U32   uSize = 0;

	if(m_pspMeshData[IFX_MESH_LINE].IsValid())
		return IFX_E_ALREADY_INITIALIZED;

	uSize = sizeof(IFXU16Line);
	if (GetAttributes().m_uData.m_b32BitIndices)
		uSize = sizeof(IFXU32Line);

	if(!m_pspMeshData[IFX_MESH_LINE].IsValid())
		rc = m_pspMeshData[IFX_MESH_LINE].Create(CID_IFXInterleavedData, IID_IFXInterleavedData);

	if(IFXSUCCESS(rc))
		rc = m_pspMeshData[IFX_MESH_LINE]->Allocate(1, &uSize, uNumLines);

	if(IFXSUCCESS(rc))
	{
		m_uMaxNumLines = uNumLines;
		m_uNumAllocatedLines = uNumLines;
		m_uNumLines = uNumLines;
		UpdateVersionWord(IFX_MESH_LINE);
	}

	return rc;
}


U32 CIFXMesh::GetNumAllocatedLines()
{
	if(m_pspMeshData[IFX_MESH_LINE].IsValid())
		return m_pspMeshData[IFX_MESH_LINE]->GetNumVertices();
	return 0;
}

U32 CIFXMesh::GetMaxNumLines()
{
	return m_uMaxNumLines;
}

U32 CIFXMesh::GetNumLines()
{
	return m_uNumLines;
}


IFXRESULT CIFXMesh::SetMaxNumVertices(U32 uNumVertices)
{
	IFXRESULT rc = IFX_OK;

	IFXASSERTBOX(uNumVertices <= m_uNumAllocatedVertices,
		"Can't set number of vertices higher than amount allocated!");
	if(uNumVertices > m_uNumAllocatedVertices)
	{
		rc = IFX_E_INVALID_RANGE;
	}
	else
	{
		m_uMaxNumVertices = uNumVertices;
	}

	return rc;
}

IFXRESULT CIFXMesh::SetMaxNumFaces(U32 uNumFaces)
{
	IFXRESULT rc = IFX_OK;

	IFXASSERTBOX(uNumFaces <= GetNumAllocatedFaces(),
		"Can't set number of faces higher than amount allocated!");
	if(uNumFaces > GetNumAllocatedFaces())
	{
		rc = IFX_E_INVALID_RANGE;
	}
	else
	{
		m_uMaxNumFaces = uNumFaces;
	}

	return rc;
}

IFXRESULT CIFXMesh::SetMaxNumLines(U32 uNumLines)
{
	IFXRESULT rc = IFX_OK;

	IFXASSERTBOX(uNumLines <= GetNumAllocatedLines(),
		"Can't set number of lines higher than amount allocated!");
	if(uNumLines > GetNumAllocatedLines())
	{
		rc = IFX_E_INVALID_RANGE;
	}
	else
	{
		m_uMaxNumLines = uNumLines;
	}

	return rc;
}

IFXRESULT CIFXMesh::SetNumVertices(U32 uNumVertices)
{
	IFXRESULT rc = IFX_OK;

	IFXASSERTBOX(uNumVertices <= m_pspMeshData[IFX_MESH_POSITION]->GetNumVertices(),
		"Can't set number of vertices higher than maximum!");

	if(uNumVertices > m_uMaxNumVertices)
	{
		m_uNumAllocatedVertices = m_pspMeshData[IFX_MESH_POSITION]->GetNumVertices();
		m_uMaxNumVertices = m_uNumAllocatedVertices;
	}

	if(uNumVertices > m_uMaxNumVertices)
	{
		m_uNumAllocatedVertices = m_pspMeshData[IFX_MESH_POSITION]->GetNumVertices();
		m_uMaxNumVertices = m_uNumAllocatedVertices;
		rc = IFX_E_INVALID_RANGE;
	}
	else
	{
		m_uNumVertices = uNumVertices;
	}

	return rc;
}

IFXRESULT CIFXMesh::SetNumFaces(U32 uNumFaces)
{
	IFXRESULT rc = IFX_OK;

	IFXASSERTBOX(uNumFaces <= m_pspMeshData[IFX_MESH_FACE]->GetNumVertices(),
		"Can't set number of faces higher than maximum!");
	if(uNumFaces > m_uMaxNumFaces)
	{
		m_uNumAllocatedFaces = m_pspMeshData[IFX_MESH_FACE]->GetNumVertices();
		m_uMaxNumFaces = m_uNumAllocatedFaces;
	}

	if(uNumFaces > m_uMaxNumFaces)
	{
		rc = IFX_E_INVALID_RANGE;
	}
	else
	{
		m_uNumFaces = uNumFaces;
	}

	return rc;
}

IFXRESULT CIFXMesh::SetNumLines(U32 uNumLines)
{
	IFXRESULT rc = IFX_OK;

	IFXASSERTBOX(uNumLines <= m_pspMeshData[IFX_MESH_LINE]->GetNumVertices(),
		"Can't set number of faces higher than maximum!");
	if(uNumLines > m_uMaxNumFaces)
	{
		m_uNumAllocatedLines = m_pspMeshData[IFX_MESH_LINE]->GetNumVertices();
		m_uMaxNumLines = m_uNumAllocatedLines;
	}

	if(uNumLines > m_uMaxNumLines)
	{
		rc = IFX_E_INVALID_RANGE;
	}
	else
	{
		m_uNumLines = uNumLines;
	}

	return rc;
}


IFXRESULT CIFXMesh::GetBoundingSphere(IFXVector4& vBoundSphere)
{
	vBoundSphere = m_vBoundingSphere;

	return IFX_OK;
}

IFXRESULT CIFXMesh::SetBoundingSphere(const IFXVector4& vBoundSphere)
{
	m_vBoundingSphere = vBoundSphere;

	return IFX_OK;
}


IFXRESULT CIFXMesh::CalcBoundingSphere()
{
	IFXVector4 vMin(FLT_MAX, FLT_MAX, FLT_MAX, 0);
	IFXVector4 vMax(-FLT_MAX,-FLT_MAX,-FLT_MAX, 0);

	// Determine the axis aligned bounding box and the number of verticies.
	IFXVector3* vertex;
	IFXVector3Iter vIter;
	GetPositionIter(vIter);
	U32 i;
	for(  i = m_uNumVertices; i--; )
	{
		vertex = vIter.Next();
		if ( vertex->X() < vMin.R() ) vMin.R() = vertex->X();
		if ( vertex->X() > vMax.R() ) vMax.R() = vertex->X();
		if ( vertex->Y() < vMin.G() ) vMin.G() = vertex->Y();
		if ( vertex->Y() > vMax.G() ) vMax.G() = vertex->Y();
		if ( vertex->Z() < vMin.B() ) vMin.B() = vertex->Z();
		if ( vertex->Z() > vMax.B() ) vMax.B() = vertex->Z();
	}

	// If there are any verticies, find the average position as the center,
	// and the distance to the furthest point as the radius.
	if ( m_uNumVertices )
	{
		vMin.Add(vMax);
		vMin.Scale3(0.5f);
		m_vBoundingSphere = vMin;
		F32 fMaxSquaredDistance = -FLT_MAX;
		F32 fSquaredDistance;
		IFXVector3 d;
		GetPositionIter(vIter);
		U32 i;
		for(  i = m_uNumVertices; i--; )
		{
			vertex = vIter.Next();
			d.X() = vertex->X() - m_vBoundingSphere.R();
			d.Y() = vertex->Y() - m_vBoundingSphere.G();
			d.Z() = vertex->Z() - m_vBoundingSphere.B();
			fSquaredDistance = d.DotProduct(d);
			if ( fSquaredDistance > fMaxSquaredDistance )
				fMaxSquaredDistance = fSquaredDistance;
		}
		m_vBoundingSphere.A() = sqrtf( fMaxSquaredDistance );
	}
	else
		m_vBoundingSphere.Set( 0.0, 0.0, 0.0, 0.0 );

	return IFX_OK;
}


BOOL CIFXMesh::IsColorBGR(U32 uColorNum/* =0 */)
{
	if(uColorNum)
	{
		return m_vaAttribs.m_uData.m_bSpecularIsBGR;
	}

	return m_vaAttribs.m_uData.m_bDiffuseIsBGR;
}

#define SWAP_RED_BLUE(color)                        \
	color = ((color & 0xFF000000) | ((color & 0x00FF0000) >> 16) |      \
	(color & 0x0000FF00) | ((color & 0x000000FF) << 16))

IFXRESULT CIFXMesh::SwizzleDiffuseColors()
{
	IFXRESULT rc = IFX_OK;

	if(m_vaAttribs.m_uData.m_bHasDiffuseColors)
	{
		m_vaAttribs.m_uData.m_bDiffuseIsBGR = !m_vaAttribs.m_uData.m_bDiffuseIsBGR;

		IFXU32Iter uIter;
		GetVectorIter(IFX_MESH_DIFFUSE_COLOR, uIter);
		U32* puColor;
		U32 i;
		for( i = 0; i < GetMaxNumVertices(); i++)
		{
			puColor = uIter.Next();
			SWAP_RED_BLUE(*puColor);
		}

		UpdateVersionWord(IFX_MESH_DIFFUSE_COLOR);
	}

	return rc;
}

IFXRESULT CIFXMesh::SwizzleSpecularColors()
{
	IFXRESULT rc = IFX_OK;

	if(m_vaAttribs.m_uData.m_bHasSpecularColors)
	{
		m_vaAttribs.m_uData.m_bSpecularIsBGR = !m_vaAttribs.m_uData.m_bSpecularIsBGR;

		IFXU32Iter uIter;
		GetVectorIter(IFX_MESH_SPECULAR_COLOR, uIter);
		U32* puColor;
		U32 i;
		for( i = 0; i < GetMaxNumVertices(); i++)
		{
			puColor = uIter.Next();
			SWAP_RED_BLUE(*puColor);
		}
	}

	return rc;
}

#undef SWAP_RED_BLUE

U32 CIFXMesh::GetId() const
{
	return m_uId;
}

U32   CIFXMesh::NewId()
{
	m_uId = IFXGetNextRenderableId();
	return m_uId;
}

void  CIFXMesh::SetId(U32 in_Id)
{
	m_uId = in_Id;
}


IFXRESULT CIFXMesh::SetRenderTexCoordsInUse(IFXMeshAttributes eTexCoordBits)
{
	m_eRenderTCsUsed = eTexCoordBits;

	return IFX_OK;
}

IFXMeshAttributes CIFXMesh::GetRenderTexCoordsInUse()
{
	return m_eRenderTCsUsed;
}

IFXRESULT CIFXMesh::SetNumTextureCoords(U32 uNumTextureCoords)
{
	IFXRESULT rc = IFX_OK;

	if(uNumTextureCoords <= m_vaAttribs.m_uData.m_uNumTexCoordLayers)
	{
		m_uNumTCsUsed = uNumTextureCoords;
	}
	else
		rc = IFX_E_INVALID_RANGE;

	return rc;
}

U32 CIFXMesh::GetNumTextureCoords() const
{
	return m_uNumTCsUsed;
}

IFXUVMapParameters& CIFXMesh::GetUVMapParameters(U32 uTexLayer)
{
	if(uTexLayer >= IFX_MAX_TEXUNITS)
	{
		IFXASSERTBOX(uTexLayer < IFX_MAX_TEXUNITS, "Invalid texture coordinate layer");
		uTexLayer = 0;
	}

	return m_pUVMapParams[uTexLayer];
}

//===============================
// Protected Methods
//===============================
IFXRESULT CIFXMesh::Construct()
{
	IFXRESULT rc = IFX_OK;
	U32   i;

	m_uId = IFXGetNextRenderableId();

	if(IFXSUCCESS(rc))
	{
		for(i = 0; (i < IFX_MESH_NUM_ATTRIBUTES); i++)
		{
			m_pspMeshData[i] = 0;
			m_puVectorSizes[i] = 0;
		}
	}

	m_uNumAllocatedVertices = 0;
	m_uMaxNumVertices = 0;
	m_uNumVertices = 0;

	m_uNumAllocatedFaces = 0;
	m_uMaxNumFaces = 0;
	m_uNumFaces = 0;

	m_uNumAllocatedLines = 0;
	m_uMaxNumLines = 0;
	m_uNumLines = 0;

	for(i = 0; i < (IFX_MESH_TC0+IFX_MAX_TEXUNITS); i++)
	{
		m_pVersionsInterleaved[i] = 0;
	}

	for (i = 0; i < IFX_MAX_TEXUNITS; i++)
	{
		m_pUVMapParams[i].Reset();
	}


	m_eRenderTCsUsed = 0;
	m_uNumTCsUsed = 0;
	m_eInterleavedAttribs = 0;
	m_uNumVerticesInterleaved = 0;

	return rc;
}

IFXRESULT CIFXMesh::Destroy()
{
	if(m_spInterleavedData.IsValid()) 
	{
		IFXRenderServices* pRenderServices = NULL;
		if (IFXSUCCESS(IFXCreateComponent(CID_IFXRenderServices, IID_IFXRenderServices, (void**)&pRenderServices))) 
		{
			pRenderServices->DeleteStaticMesh(m_spInterleavedData->GetId());
			pRenderServices->Release();
		}
	}

	return IFX_OK;
}

void CIFXMesh::CalculateVectorSizes()
{
	m_puVectorSizes[IFX_MESH_POSITION] = m_vaAttribs.m_uData.m_bHasPositions * sizeof(IFXVector3);
	m_puVectorSizes[IFX_MESH_NORMAL] = m_vaAttribs.m_uData.m_bHasNormals * sizeof(IFXVector3);
	m_puVectorSizes[IFX_MESH_DIFFUSE_COLOR] = m_vaAttribs.m_uData.m_bHasDiffuseColors * sizeof(U32);
	m_puVectorSizes[IFX_MESH_SPECULAR_COLOR] = m_vaAttribs.m_uData.m_bHasSpecularColors * sizeof(U32);

	U32 i = 0;
	for(i = 0; i < m_vaAttribs.m_uData.m_uNumTexCoordLayers; i++)
	{
		m_puVectorSizes[IFX_MESH_TC0 + i] = m_vaAttribs.GetTexCoordSize(i) * sizeof(F32);
	}
	for(i = m_vaAttribs.m_uData.m_uNumTexCoordLayers; i < IFX_MAX_TEXUNITS; i++)
	{
		m_puVectorSizes[IFX_MESH_TC0 + i] = 0;
	}
}

IFXMeshAttributes CIFXMesh::CalculateDirtyAttribs(IFXMeshAttributes& eAllAttribs)
{
	IFXMeshAttributes eVal;
	U32 i = 0;

	eAllAttribs.clear();

	for(i = 0; i < IFX_MESH_TC0; i++)
	{
		if(m_pspMeshData[i].IsValid())
		{
			if(m_pVersionsInterleaved[i] != m_pspMeshData[i]->GetVersionWord(0))
			{
				m_pVersionsInterleaved[i] = m_pspMeshData[i]->GetVersionWord(0);
				eVal |= i;
			}

			if(m_pVersionsInterleaved[i])
			{
				eAllAttribs |= i;
			}
		}
	}

	for(i = 0; i < IFX_MAX_TEXUNITS; i++)
	{
		if(m_eRenderTCsUsed[IFX_MESH_RENDER_TC0+i] && (i < m_uNumTCsUsed))
		{
			if(m_pspMeshData[IFX_MESH_RENDER_TC0+i].IsValid())
			{
				if(m_pVersionsInterleaved[IFX_MESH_TC0 + i] != m_pspMeshData[IFX_MESH_RENDER_TC0+i]->GetVersionWord(0))
				{
					m_pVersionsInterleaved[IFX_MESH_TC0 + i] =
						m_pspMeshData[IFX_MESH_RENDER_TC0 + i]->GetVersionWord(0);
					eVal |= IFX_MESH_RENDER_TC0+i;
				}

				if(m_pVersionsInterleaved[IFX_MESH_TC0 + i])
				{
					eAllAttribs |= IFX_MESH_RENDER_TC0+i;
				}
			}
		}
		else
		{
			if(i < m_uNumTCsUsed && m_pspMeshData[IFX_MESH_TC0+i].IsValid())
			{
				if(m_pVersionsInterleaved[IFX_MESH_TC0 + i] != m_pspMeshData[IFX_MESH_TC0+i]->GetVersionWord(0))
				{
					m_pVersionsInterleaved[IFX_MESH_TC0 + i] =
						m_pspMeshData[IFX_MESH_TC0 + i]->GetVersionWord(0);
					eVal |= IFX_MESH_TC0+i;
				}

				if(m_pVersionsInterleaved[IFX_MESH_TC0 + i])
				{
					eAllAttribs |= IFX_MESH_TC0+i;
				}
			}
		}
	}

	if(m_uNumVerticesInterleaved < m_uNumVertices)
		eVal = eAllAttribs;

	return eVal;
}

U32 CIFXMesh::CalculateMeshVersion()
{
	U32 uVal = 0;

	U32 i;
	for( i = 0; i < IFX_MESH_RENDER_TC0; i++)
	{
		if(m_pspMeshData[i].IsValid())
		{
			uVal += m_pspMeshData[i]->GetVersionWord(0);
		}
	}

	return uVal;
}

IFXRESULT CIFXMesh::ReverseNormals()
{
	IFXVector3Iter normIter;
	IFXVector3*    pNorm;

	GetNormalIter(normIter);

	U32 n;
	for( n = 0; n < GetNumVertices(); n++)
	{
		pNorm = normIter.Next();
		pNorm->X() = -pNorm->X();
		pNorm->Y() = -pNorm->Y();
		pNorm->Z() = -pNorm->Z();
	}

	UpdateVersionWord( IFX_MESH_NORMAL );

	return IFX_OK;
}

IFXRESULT CIFXMesh::GetOffsetTransform( IFXMatrix4x4* pMatrix ) const
{
	*pMatrix = m_offsetTransform;
	return IFX_OK;
}

IFXRESULT CIFXMesh::SetOffsetTransform(IFXMatrix4x4* pMatrix)
{
	m_offsetTransform = *pMatrix;
	return IFX_OK;
}

EIFXRenderable CIFXMesh::GetRenderableType() const
{
	return m_renderableType;
}

void CIFXMesh::SetRenderableType( EIFXRenderable type )
{
	m_renderableType = type;
}
