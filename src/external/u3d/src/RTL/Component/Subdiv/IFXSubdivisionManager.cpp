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

#include "IFXSubdivisionManager.h"
#include "IFXScreenSpaceMetric.h"
#include "IFXCoreCIDs.h"
#include "IFXMesh.h"

// Allocate space for static constants:
const F32 IFXSubdivisionManager::m_fMaxSurfaceTensionParam = 1.0f;
const F32 IFXSubdivisionManager::m_fMinSurfaceTensionParam = 0.0f;
const U32 IFXSubdivisionManager::m_uMaxBaseMeshSize = 5000;


U32 IFXGetMaxBaseMeshSize()
{
	return IFXSubdivisionManager::m_uMaxBaseMeshSize;
}


IFXSubdivisionManager::IFXSubdivisionManager()
{
	m_uNumBaseTriangles = 0;
	m_uNumBaseVertices  = 0;
	m_pBaseTriangle                 = NULL;
	m_ppBaseVertex                   = NULL;

	m_pTriangleAllocator            = NULL;
	m_pVertexAllocator        = NULL;

	m_pOutputMeshGrp                = NULL;

	SetDefaultProperties();

	// Initialize butterfly scheme:
	m_pButterflyScheme              = new IFXButterflyScheme;
	m_pButterflyScheme->SetSmoothingFactor(m_pfFloat[SurfaceTension]);
	m_pButterflyScheme->SetSubdivMgr(this);
}


IFXSubdivisionManager::~IFXSubdivisionManager()
{
	DeallocateOutputMesh();
	m_pOutputMeshGrp = NULL;

	DeallocateFreeLists();

	if (m_pBaseTriangle)    delete [] m_pBaseTriangle;
	if (m_ppBaseVertex)      delete []   m_ppBaseVertex;
	m_pBaseTriangle     = NULL;
	m_ppBaseVertex      = NULL;

	if (m_pButterflyScheme)
	{
		delete m_pButterflyScheme;
		m_pButterflyScheme = NULL;
	}
}


void IFXSubdivisionManager::SetDefaultProperties()
{
	// Boolean properties:

	m_pbBoolean[Adaptive]               = IFX_FALSE;

	m_pbBoolean[CrackFilling]           = IFX_TRUE;

	m_pbBoolean[NormalSubdivision]      = IFX_TRUE;
	m_pbBoolean[TexCoordSubdivision]    = IFX_FALSE;
	m_pbBoolean[LazyEvaluation]     = IFX_TRUE;

	// Integer properties:
	m_puInteger[MaxComputeDepth]            = 5;
	m_puInteger[MaxRenderDepth]             = 5;
	m_puInteger[CurrentComputeDepth]        = 0;

	m_puInteger[MaxTriangleAllocation]      = 600000;
	m_puInteger[MaxVertexAllocation]        = 400000;

	m_puInteger[InitialTriangleAllocation]  = 5000;
	m_puInteger[InitialVertexAllocation]    = 2500;

	m_puInteger[GrowTriangleAllocation]     = 1000;
	m_puInteger[GrowVertexAllocation]       = 500;

	m_puInteger[LazyEvaluationLimit]    = 1;

	// Floating point properties:
	m_pfFloat[SurfaceTension]               = 0.65f;

	// Other properties:
	m_pAdaptiveMetric                       = NULL;

	m_bDontUpdate                           = IFX_FALSE;
	m_bUpdateRequired           = IFX_TRUE;

	// OutputMesh allocation mgt:
	m_uOutMshInitial_ratio          = 0.020f; // Percentatge of total allowed allocation to initially allocate.
	m_numOutVertsIncr           = 3000;   // # of vertices by which we grow output meshes:
	m_numOutFacesIncr           = 5000;   // # of faces by which we grow output meshses:
	m_bReallocateOutputMesh         = IFX_FALSE;
	m_uNumOutMeshes             = 0;
	m_pOutMshVtxCnt             = NULL;
}


IFXRESULT IFXSubdivisionManager::AllocateFreeLists()
{
	// Create triangle and vertex allocators, allocate inital space:
	m_pTriangleAllocator  = new IFXTQTTriangleAllocator
		(m_puInteger[InitialTriangleAllocation], m_puInteger[GrowTriangleAllocation]);

	m_pVertexAllocator    = new IFXVertexAllocator
		(m_puInteger[InitialVertexAllocation], m_puInteger[GrowVertexAllocation]);

	if (!m_pTriangleAllocator || !m_pVertexAllocator)
		return IFX_E_OUT_OF_MEMORY;

	return IFX_OK;
}


IFXRESULT IFXSubdivisionManager::DeallocateFreeLists()
{
	if (m_pTriangleAllocator)           delete m_pTriangleAllocator;
	m_pTriangleAllocator            = NULL;

	if (m_pVertexAllocator)   delete m_pVertexAllocator;
	m_pVertexAllocator    = NULL;

	return IFX_OK;
}



IFXRESULT IFXSubdivisionManager::AllocateOutputMesh(IFXMeshGroup *pInputMeshGrp)
{
	IFXASSERT (m_pOutputMeshGrp == NULL);


	// Allocate the necessary outputmeshes:
	IFXCreateComponent(CID_IFXMeshGroup, IID_IFXMeshGroup, (void**)&m_pOutputMeshGrp);
	m_pOutputMeshGrp->Allocate(pInputMeshGrp->GetNumMeshes());

	m_uNumOutMeshes = pInputMeshGrp->GetNumMeshes();
	m_pOutMshVtxCnt = new U32 [m_uNumOutMeshes];

	// Iterate over each output mesh:
	U32 uMeshNum;
	for (uMeshNum = 0; uMeshNum < m_uNumOutMeshes; uMeshNum++)
	{
		// Get the input IFXMesh
		IFXMesh *pMeshIn          = 0;
		pInputMeshGrp->GetMesh(uMeshNum, pMeshIn);
		IFXVertexAttributes attributeFlags  = pMeshIn->GetAttributes();

		// Figure out what percentage of the total mesh group this one mesh represents:
		// Store the vertex mesh count incase of need to reallocate the output mesh later.
		m_pOutMshVtxCnt[uMeshNum] = pMeshIn->GetNumVertices();
		F32 fShare = (((F32) m_pOutMshVtxCnt[uMeshNum] ) / ((F32) m_uNumBaseVertices));
		U32 uNumAllocatedVertices = (U32) (fShare * m_uOutMshInitial_ratio * m_puInteger[MaxVertexAllocation]);
		U32 uNumAllocatedFaces    = (U32) (fShare * m_uOutMshInitial_ratio * m_puInteger[MaxTriangleAllocation]);

		// Allocate an output IFXMesh:
		IFXMesh *pMeshNew = 0;
		IFXCreateComponent(CID_IFXMesh, IID_IFXMesh, (void**)&pMeshNew);

		// Now allocate the output vertex and face data, plug into the mesh group:
		pMeshNew->Allocate(attributeFlags, uNumAllocatedVertices, uNumAllocatedFaces);
		m_pOutputMeshGrp->SetMesh(uMeshNum, pMeshNew);
		IFXRELEASE(pMeshIn);
		IFXRELEASE(pMeshNew);
	}

	m_bReallocateOutputMesh = IFX_FALSE;

	return IFX_OK;
}


IFXRESULT IFXSubdivisionManager::ReallocateOutputMesh ()
{
	IFXRESULT rc = IFX_OK;
	// Iterate over each input mesh:
	U32 uMeshNum;
	for (uMeshNum = 0; uMeshNum < m_uNumOutMeshes; uMeshNum++)
	{
		// Figure out what percentage of the total allowable mesh group dimensions
		// this one mesh represents:
		F32 fShare = ((F32) m_pOutMshVtxCnt[uMeshNum]) / ((F32) m_uNumBaseVertices);
		U32 uNumAllocatedVertices = (U32) (fShare * m_puInteger[MaxVertexAllocation]);
		U32 uNumAllocatedFaces    = (U32) (fShare * m_puInteger[MaxTriangleAllocation]);

		// See if each output mesh is out of space, if so, then re-allocate
		IFXMesh *pMeshOut = 0;
		m_pOutputMeshGrp->GetMesh(uMeshNum, pMeshOut);
		if (IFXSUCCESS(rc) && ((pMeshOut->GetNumVertices() == pMeshOut->GetMaxNumVertices()) ||
			(pMeshOut->GetNumFaces() == pMeshOut->GetMaxNumFaces())))

		{
			U32 uNumFacesIncreased = 2* pMeshOut->GetNumFaces();// + m_numOutFacesIncr;
			U32 uNumVertsIncreased = 2* pMeshOut->GetNumVertices();// + m_numOutVertsIncr;

			if ((uNumFacesIncreased < uNumAllocatedFaces) &&
				(uNumVertsIncreased < uNumAllocatedVertices))
			{
				rc = pMeshOut->IncreaseSize(uNumFacesIncreased, uNumVertsIncreased);
			}
			else
				rc = IFX_E_UNDEFINED;
		}

		IFXRELEASE(pMeshOut);

	}

	m_bReallocateOutputMesh = IFX_FALSE;

	return rc;
}



IFXRESULT IFXSubdivisionManager::DeallocateOutputMesh()
{
	IFXRELEASE(m_pOutputMeshGrp);

	if (m_pOutMshVtxCnt)
	{
		delete m_pOutMshVtxCnt;
		m_pOutMshVtxCnt = NULL;
	}

	return IFX_OK;
}


IFXRESULT IFXSubdivisionManager::SetBool  (BooleanProperty property,  BOOL bValue)
{
	// Validate parameters:
	if (property >= MAX_NUM_BOOLEAN_PROPERTIES)
		return IFX_E_UNDEFINED;

	m_pbBoolean[property] = bValue;

	if (property == Adaptive)
	{
		if (bValue == IFX_FALSE)
		{
			m_bUpdateRequired = IFX_TRUE;
			ResetAll();
		}
	}

	return IFX_OK;
}


IFXRESULT IFXSubdivisionManager::GetBool  (BooleanProperty property,  BOOL *pbValue)
{
	// Validate parameters:
	if (property >= MAX_NUM_BOOLEAN_PROPERTIES)
		return IFX_E_UNDEFINED;

	if (!pbValue)
		return IFX_E_UNDEFINED;

	*pbValue = m_pbBoolean[property];

	return IFX_OK;
}


IFXRESULT IFXSubdivisionManager::SetInteger (IntegerProperty property,  U32 uValue)
{
	// Validate parameters:
	if (property >= MAX_NUM_INTEGER_PROPERTIES)
		return IFX_E_UNDEFINED;

	if (property == MaxComputeDepth)
	{
		if (m_puInteger[MaxComputeDepth] != uValue)
			m_bUpdateRequired = IFX_TRUE;

	}

	m_puInteger[property] = uValue;

	return IFX_OK;
}


IFXRESULT IFXSubdivisionManager::GetInteger (IntegerProperty property,  U32 *puValue)
{
	// Validate parameters:
	if (property >= MAX_NUM_INTEGER_PROPERTIES)
		return IFX_E_UNDEFINED;

	if (!puValue)
		return IFX_E_UNDEFINED;

	*puValue = m_puInteger[property];

	return IFX_OK;
}



// Returns IFX_TRUE if floating point value is ok.
BOOL IFXSubdivisionManager::CheckRangeAndScaling (FloatProperty property,   F32 *pfValue)
{
	// Do Property specify evaluation if necessary:
	if (property == SurfaceTension)
	{
		// Check range:
		if (*pfValue < m_fMinSurfaceTensionParam)
			*pfValue = m_fMinSurfaceTensionParam;
		if (*pfValue > m_fMaxSurfaceTensionParam)
			*pfValue = m_fMaxSurfaceTensionParam;

		m_pButterflyScheme->SetSmoothingFactor(*pfValue);

		m_bUpdateRequired = IFX_TRUE;
	}

	return IFX_TRUE;
}

IFXRESULT IFXSubdivisionManager::SetFloat (FloatProperty property,  F32 fValue)
{
	// Validate parameters:
	if (property >= MAX_NUM_FLOAT_PROPERTIES)
		return IFX_E_UNDEFINED;

	CheckRangeAndScaling (property, &fValue);

	m_pfFloat[property] = fValue;

	return IFX_OK;
}


IFXRESULT IFXSubdivisionManager::GetFloat (FloatProperty property,  F32 *pfValue)
{
	// Validate parameters:
	if (property >= MAX_NUM_FLOAT_PROPERTIES)
		return IFX_E_UNDEFINED;

	if (!pfValue)
		return IFX_E_UNDEFINED;

	*pfValue = m_pfFloat[property];

	return IFX_OK;
}


IFXRESULT IFXSubdivisionManager::SetAdaptiveMetric (IFXAdaptiveMetric *pAdaptiveMetric)
{
	if (!pAdaptiveMetric)
		return IFX_E_UNDEFINED;

	m_pAdaptiveMetric = pAdaptiveMetric;

	return IFX_OK;
}


IFXRESULT IFXSubdivisionManager::ConsolidateLevel (U32 uLevel)
{
	if (m_bDontUpdate)
		return IFX_OK;

	m_bDontUpdate = IFX_TRUE;

	U32 f;
	for (f = 0; f < m_uNumBaseTriangles; f++)
	{
		m_pBaseTriangle[f].ConsolidateLevel(uLevel);
	}

	m_bDontUpdate = IFX_FALSE;

	return IFX_OK;
}



void IFXSubdivisionManager::Update()
{
	// If max depth is 0, don't bother doing anything:
	//if (!m_puInteger[MaxComputeDepth])
	//    return;

	U32 f;

	if (m_pbBoolean[Adaptive])
	{
		// 1) Apply user specified adaptive subdiv metric and subdivide if directed:
		for (f = 0; f < m_uNumBaseTriangles; f++)
			m_pBaseTriangle[f].BreadthFirstEvaluate(this);

		// 2) Consolidate any triangles marked for consolidation:
		for (f = 0; f < m_uNumBaseTriangles; f++)
			m_pBaseTriangle[f].Consolidate(this);

		// 3) Generate any required crack fill triangles:
		if (m_pbBoolean[CrackFilling] && m_puInteger[MaxRenderDepth])
			for (f = 0; f < m_uNumBaseTriangles; f++)
				m_pBaseTriangle[f].FindCracks(this);
	}
	else // Subdivide uniformly:
	{
		if (m_puInteger[CurrentComputeDepth] == m_puInteger[MaxComputeDepth])
			return;

		for (f = 0; f < m_uNumBaseTriangles; f++)
			m_pBaseTriangle[f].SubdivideLeaves(this);

		if (m_puInteger[CurrentComputeDepth] > m_puInteger[MaxComputeDepth])
			m_puInteger[CurrentComputeDepth]--;
		else
			m_puInteger[CurrentComputeDepth]++;

	}
}



// Treverse the mesh group and count up all of the vertices and faces.
// Then allocate base mesh data of the total size.
void IFXSubdivisionManager::AllocateBaseMesh (IFXMeshGroup *pMeshGrp, U32 *pFaceOffsetTable)
{
	U32 uMeshNum;

	m_uNumBaseTriangles = 0;
	m_uNumBaseVertices  = 0;

	for (uMeshNum = 0; uMeshNum < pMeshGrp->GetNumMeshes(); uMeshNum++)
	{
		pFaceOffsetTable[uMeshNum] = m_uNumBaseTriangles;

		// Get the 1st mesh:
		IFXMesh  *pMesh = 0;
		pMeshGrp->GetMesh(uMeshNum, pMesh);

		// Copy dimensions
		m_uNumBaseTriangles += pMesh->GetNumFaces();
		m_uNumBaseVertices  += pMesh->GetNumVertices();

		IFXRELEASE(pMesh);
	}

	// Allocate memory for base triangle and base corners:
	m_pBaseTriangle = new  IFXTQTBaseTriangle [m_uNumBaseTriangles]  ;
	m_ppBaseVertex  = new  IFXTQTVertex* [m_uNumBaseVertices];

	// Initialize all base vertex pointers to null:
	U32 bv;
	for (bv= 0; bv < m_uNumBaseVertices; bv++)
		m_ppBaseVertex[bv] = NULL;
}

void IFXSubdivisionManager::CopyVertexData(IFXMeshGroup *pMeshGrp)
{
	IFXASSERT(pMeshGrp);
	// Traverse each mesh, collecting faces and vertices to initilize the base mesh:
	U32 uMeshNum;
	U32 uVertexOffset = 0;
	//U32 uFaceOffset   = 0;
	U32 uNumberOfMeshes = pMeshGrp->GetNumMeshes();
	for (uMeshNum = 0; uMeshNum < uNumberOfMeshes; uMeshNum++)
	{
		// Get a mesh:
		IFXMesh *pMesh = 0;
		pMeshGrp->GetMesh(uMeshNum, pMesh);
		IFXVertexAttributes vAttributes = pMesh->GetAttributes();

		U32 uNumMeshVertices  = pMesh->GetNumVertices();
		//U32 uNumMeshFaces   = pMesh->GetNumFaces();

		// Get a vertex iterator to the mesh vertices:
		IFXVertexIter vertexIter;
		pMesh->GetVertexIter(vertexIter);

		// 1) Copy vertex data, full version:
		U32 v;
		for (v = 0; v < uNumMeshVertices; v++)
		{
			// Allocate a base vertex, if not already allocated:
			if (NULL == m_ppBaseVertex[uVertexOffset + v])
			{
				m_ppBaseVertex[uVertexOffset + v] = m_pVertexAllocator->Allocate();
			}

			m_ppBaseVertex[uVertexOffset + v]->m_position =  *vertexIter.GetPosition();
			m_ppBaseVertex[uVertexOffset + v]->m_normal =  *vertexIter.GetNormal();
			if (vAttributes.m_uData.m_uNumTexCoordLayers)
			{
				m_ppBaseVertex[uVertexOffset + v]->m_texCoord = *vertexIter.GetTexCoord();
			}
			vertexIter.Next();
		}

		// Update offsets:
		uVertexOffset += uNumMeshVertices;

		IFXRELEASE(pMesh);
	} // End of mesh traversal.

	m_bUpdateRequired = IFX_TRUE;
}

IFXRESULT IFXSubdivisionManager::InitMesh( IFXMeshGroup*    pMeshGrp,
										  IFXNeighborMesh* pNeighborMesh )
{
	if ( (!pMeshGrp) || (!pNeighborMesh) )
		return IFX_E_INVALID_POINTER;

	// If we have already done InitMesh(), we should be done.
	if(m_pBaseTriangle)
		return IFX_OK;

	// Make sure that all attribute discontinuites are marked in the neighbor mesh:
	IFXRESULT result = pNeighborMesh->MarkAttributeDiscontinuities(*pMeshGrp);


	U32 uNumberOfMeshes = pMeshGrp->GetNumMeshes();
	U32 *pFaceOffsetTable = new U32 [ uNumberOfMeshes * sizeof (U32)]; /// @todo: Should sizeof U32 be here?

	if (!pFaceOffsetTable)
	{
		return IFX_E_UNDEFINED;
	}

	// Allocate free lists of triangles and vertices:
	result = AllocateFreeLists();
	if (result != IFX_OK)
	{
		IFXDELETE_ARRAY( pFaceOffsetTable );
		return result;
	}

	// Determine size and allocate space for the base (control) mesh:
	AllocateBaseMesh (pMeshGrp, pFaceOffsetTable);

	// Big base meshes aren't worth subdividing and eat too much memory:
	if (m_uNumBaseTriangles > m_uMaxBaseMeshSize)
	{
		IFXDELETE_ARRAY( pFaceOffsetTable );
		return IFX_E_SUBDIVMGR_LARGE_MESHES_UNSUPPORTED;
	}

	// copy the vertex data to initialize the base mesh.
	// note that this originally was part of the loop below
	// but by encapsulating it, this functionality can be
	// re-used to update a dynamically changing mesh.

	CopyVertexData(pMeshGrp);

	// Traverse each mesh, collecting faces to initilize the base mesh:
	U32 uMeshNum;
	U32 uVertexOffset = 0;
	U32 uFaceOffset   = 0;
	for (uMeshNum = 0; uMeshNum < uNumberOfMeshes; uMeshNum++)
	{
		// Get a mesh:
		IFXMesh *pMesh = 0;
		pMeshGrp->GetMesh(uMeshNum, pMesh);

		U32 uNumMeshFaces   = pMesh->GetNumFaces();
		U32 uNumMeshVertices  = pMesh->GetNumVertices();

		IFXFaceIter faceIter;
		pMesh->GetFaceIter(faceIter);

		IFXTQTAddress emptyAddress;
		emptyAddress.Reset();

		IFXNeighborFace *pNeighborFaceArray = pNeighborMesh->GetNeighborFaceArray(uMeshNum);

		IFXFace*            pFace = NULL;
		IFXNeighborFace*    pNeighborFace = NULL;
		U32                 uMeshIndex[3], puNeighborFaceIndex[3];
		IFXCornerFlags*     ppNeighborFlags[3] = {NULL, NULL, NULL};
		IFXTQTBaseTriangle* pNeighborTriangles[3] = {NULL, NULL, NULL};


		// 2) Copy face data:
		U32 f;
		for (f = 0; f < uNumMeshFaces; f++)
		{
			pFace = faceIter.Next();

			U32 a,b,c;
			a = pFace->VertexA();
			b = pFace->VertexB();
			c = pFace->VertexC();

			m_pBaseTriangle[uFaceOffset + f].Init
				(&m_pBaseTriangle[uFaceOffset + f], 0, emptyAddress, IFXTQTTriangle::TipUp,
				m_ppBaseVertex[uVertexOffset + a],
				m_ppBaseVertex[uVertexOffset + b],
				m_ppBaseVertex[uVertexOffset + c]);

			// Grab the 3 neighbor indices:
			pNeighborFace = &pNeighborFaceArray[f];

			pNeighborFace->GetNeighbor(0, &uMeshIndex[0], &puNeighborFaceIndex[0],
				&ppNeighborFlags[0]);
			pNeighborFace->GetNeighbor(1, &uMeshIndex[1], &puNeighborFaceIndex[1],
				&ppNeighborFlags[1]);
			pNeighborFace->GetNeighbor(2, &uMeshIndex[2], &puNeighborFaceIndex[2],
				&ppNeighborFlags[2]);

			IFXEdgeContinuityDescriptor *pEdgeDescr;

			U32 e;
			for ( e = 0; e < 3; e++)
			{
				pEdgeDescr = m_pBaseTriangle[uFaceOffset + f].GetEdgeDescriptor((IFXTQTAddress::Direction) e);
				pEdgeDescr->AllAttributes = 0;
				if (ppNeighborFlags[e]->CORNER_FLAG_DISCONTINUOUS_NORMAL)
				{
					pEdgeDescr->bAttribute.bNormal = 1;
				}

				if (ppNeighborFlags[e]->CORNER_FLAG_DISCONTINUOUS_TEXCOORD)
				{
					pEdgeDescr->bAttribute.bTexCoord = 1;
				}

				if (uMeshIndex[e] != uMeshNum)
				{
					pEdgeDescr->bAttribute.bMaterial = 1;
				}
			}

			// Fix up neighbor face Indices:
			puNeighborFaceIndex[0] = puNeighborFaceIndex[0] + pFaceOffsetTable[uMeshIndex[0]];
			puNeighborFaceIndex[1] = puNeighborFaceIndex[1] + pFaceOffsetTable[uMeshIndex[1]];
			puNeighborFaceIndex[2] = puNeighborFaceIndex[2] + pFaceOffsetTable[uMeshIndex[2]];

			// Grab pointers to the base triangles:
			pNeighborTriangles[0] = &m_pBaseTriangle[puNeighborFaceIndex[0]];
			pNeighborTriangles[1] = &m_pBaseTriangle[puNeighborFaceIndex[1]];
			pNeighborTriangles[2] = &m_pBaseTriangle[puNeighborFaceIndex[2]];

			// Check for boundary edges (no neighbors) by checking to see if GetNeighbor()
			// returns the current face's index, instead of a neighbor face's index:
			if (puNeighborFaceIndex[0] == uFaceOffset + f)
				pNeighborTriangles[0] = NULL;

			if (puNeighborFaceIndex[1] == uFaceOffset + f)
				pNeighborTriangles[1] = NULL;
			if (puNeighborFaceIndex[2] == uFaceOffset + f)
				pNeighborTriangles[2] = NULL;

			m_pBaseTriangle[uFaceOffset+f].SetBaseNeighbors (pNeighborTriangles[0],
				pNeighborTriangles[1],
				pNeighborTriangles[2]);

			m_pBaseTriangle[uFaceOffset+f].SetMeshIndex(uMeshNum);

			// For Debug:
			m_pBaseTriangle[f].m_usId = uFaceOffset + f;
		}

		// Update offsets:
		uFaceOffset   += uNumMeshFaces;
		uVertexOffset += uNumMeshVertices;

		IFXRELEASE(pMesh);

	} // End of mesh traversal.

	IFXDELETE_ARRAY( pFaceOffsetTable );

	// Reset the rendering markers::
	U32 f;
	for (f = 0; f < this->m_uNumBaseTriangles; f++)
		this->m_pBaseTriangle[f].ResetMarkers(0);

	result = AllocateOutputMesh(pMeshGrp);

	if (result != IFX_OK)
		return result;

	return IFX_OK;
}


IFXRESULT IFXSubdivisionManager::AddRenderTriangle(IFXTQTTriangle *pTriangle)
{
	IFXTQTVertex **ppCorner = pTriangle->GetVertices();

	IFXVertexIter vertexIter;
	IFXTQTBaseTriangle *pBaseTriangle = pTriangle->GetBaseTriangle();
	U32 uMeshIndex;

	pBaseTriangle->GetMeshIndex(&uMeshIndex);
	IFXMesh *pMesh          = 0;
	m_pOutputMeshGrp->GetMesh(uMeshIndex, pMesh);
	IFXVertexAttributes vAttributes = pMesh->GetAttributes();

	IFXASSERT (ppCorner[0] && ppCorner[1] && ppCorner[2]);

	// Update render index if necessary on the 3 corners:
	int v;
	for ( v = 0; v < 3; v++)
	{
		if (ppCorner[v]->m_uRenderIndex == IFXTQTVertex::NotLabeled)
		{
			U32 uNumVertices = pMesh->GetNumVertices();

			// We have to punt if we have exceeded our initial vertex allocation:
			//IFXASSERT (uNumVertices < pMesh->GetMaxNumVertices());
			if (uNumVertices >= pMesh->GetMaxNumVertices())
			{
				m_bReallocateOutputMesh = IFX_TRUE;
                IFXRELEASE(pMesh);
				return IFX_OK;
			}

			ppCorner[v]->m_uRenderIndex = uNumVertices++;
			pMesh->SetNumVertices(uNumVertices);

			// Copy corner into it's position in the IFXMesh:
			pMesh->GetVertexIter(vertexIter);

			vertexIter.PointAt(ppCorner[v]->m_uRenderIndex);
			IFXVector3 *pPosition = vertexIter.GetPosition();
			IFXVector3 *pNormal  = vertexIter.GetNormal();
			*pPosition  = ppCorner[v]->m_position;
			*pNormal    = ppCorner[v]->m_normal;

			if (vAttributes.m_uData.m_uNumTexCoordLayers)
			{
				IFXVector2 *pTexCoord = vertexIter.GetTexCoord();
				*pTexCoord = ppCorner[v]->m_texCoord;
			}
		}
	}

	// Now Copy the triangle, use the RenderIndex to refer to vertex data:
	IFXFaceIter faceIter;
	pMesh->GetFaceIter(faceIter);
	U32 uNumFaces = pMesh->GetNumFaces();

	// We have to punt if we have exceeded our initial triangle allocation:
	if (uNumFaces >= pMesh->GetMaxNumFaces())
	{
		m_bReallocateOutputMesh = IFX_TRUE;
        IFXRELEASE(pMesh);
		return IFX_OK;
	}

	faceIter.PointAt(uNumFaces++);
	IFXFace *pFace = faceIter.Get();
	pMesh->SetNumFaces(uNumFaces);

	IFXTQTTriangle::SametOrientation orientation = pTriangle->GetSametOrientation();

	if (orientation == IFXTQTTriangle::TipUp)
	{
		pFace->SetA(ppCorner[IFXTQTAddress::Left]->m_uRenderIndex);
		pFace->SetB(ppCorner[IFXTQTAddress::Base]->m_uRenderIndex);
		pFace->SetC(ppCorner[IFXTQTAddress::Right]->m_uRenderIndex);
	}
	else
	{
		pFace->SetA(ppCorner[IFXTQTAddress::Left]->m_uRenderIndex);
		pFace->SetB(ppCorner[IFXTQTAddress::Right]->m_uRenderIndex);
		pFace->SetC(ppCorner[IFXTQTAddress::Base]->m_uRenderIndex);
	}

	IFXRELEASE(pMesh);

	return IFX_OK;
}


IFXRESULT IFXSubdivisionManager::AddRenderCrackFillTriangle(IFXTQTTriangle *pTriangle,
															IFXTQTVertex *ppCorner[3], IFXTQTTriangle::SametOrientation bSametOrientation)
{
	IFXVertexIter vertexIter;
	U32 uMeshIndex;
	IFXTQTBaseTriangle *pBaseTriangle = pTriangle->GetBaseTriangle();

	pBaseTriangle->GetMeshIndex(&uMeshIndex);
	IFXMesh *pMesh          = 0;
	m_pOutputMeshGrp->GetMesh(uMeshIndex, pMesh);
	IFXVertexAttributes vAttributes = pMesh->GetAttributes();

	IFXASSERT (ppCorner[0] && ppCorner[1] && ppCorner[2]);

	// Update render index if necessary on the 3 corners:
	int v;
	for ( v = 0; v < 3; v++)
	{
		if (ppCorner[v]->m_uRenderIndex == IFXTQTVertex::NotLabeled)
		{
			U32 uNumVertices = pMesh->GetNumVertices();

			if (uNumVertices >= pMesh->GetMaxNumVertices())
			{
				m_bReallocateOutputMesh = IFX_TRUE;
                IFXRELEASE(pMesh);
				return IFX_OK;
			}

			ppCorner[v]->m_uRenderIndex = uNumVertices++;
			pMesh->SetNumVertices(uNumVertices);

			// Copy corner into it's position in the IFXMesh:
			pMesh->GetVertexIter(vertexIter);
			vertexIter.PointAt(ppCorner[v]->m_uRenderIndex);
			IFXVector3 *pPosition = vertexIter.GetPosition();
			IFXVector3 *pNormal  = vertexIter.GetNormal();
			*pPosition  = ppCorner[v]->m_position;
			*pNormal    = ppCorner[v]->m_normal;

			if (vAttributes.m_uData.m_uNumTexCoordLayers)
			{
				IFXVector2 *pTexCoord = vertexIter.GetTexCoord();
				*pTexCoord = ppCorner[v]->m_texCoord;
			}
		}
	}


	// Now Copy the triangle, use the RenderIndex to refer to vertex data:
	IFXFaceIter faceIter;
	pMesh->GetFaceIter(faceIter);
	U32 uNumFaces = pMesh->GetNumFaces();

	// We have to punt if we have exceeded our initial triangle allocation:
	if (uNumFaces >= pMesh->GetMaxNumFaces())
	{
		m_bReallocateOutputMesh = IFX_TRUE;
        IFXRELEASE(pMesh);
		return IFX_OK;
	}

	faceIter.PointAt(uNumFaces++);
	IFXFace *pFace = faceIter.Get();
	pMesh->SetNumFaces(uNumFaces);

	if (bSametOrientation == IFXTQTTriangle::TipUp)
	{
		pFace->SetA(ppCorner[IFXTQTAddress::Left]->m_uRenderIndex);
		pFace->SetB(ppCorner[IFXTQTAddress::Base]->m_uRenderIndex);
		pFace->SetC(ppCorner[IFXTQTAddress::Right]->m_uRenderIndex);
	}
	else
	{
		pFace->SetA(ppCorner[IFXTQTAddress::Left]->m_uRenderIndex);
		pFace->SetB(ppCorner[IFXTQTAddress::Right]->m_uRenderIndex);
		pFace->SetC(ppCorner[IFXTQTAddress::Base]->m_uRenderIndex);
	}

	IFXRELEASE(pMesh);

	return IFX_OK;
}



void IFXSubdivisionManager::ResetOutputMeshGroup (IFXMeshGroup  *pMeshGrp)
{
	U32 uMeshNum = 0 ;
	U32 uNumOfMeshes = 0 ;

	if ( NULL != pMeshGrp )
	{
		uNumOfMeshes = pMeshGrp->GetNumMeshes();

		for (uMeshNum = 0; uMeshNum < uNumOfMeshes; uMeshNum++)
		{
			IFXMesh *pMesh = 0;
			pMeshGrp->GetMesh(uMeshNum, pMesh);
			pMesh->SetNumFaces(0);
			pMesh->SetNumVertices(0);
			IFXRELEASE(pMesh);
		}
	} // end if ( NULL != pMeshGrp )
}

#define IFXNumReallocationAttempts 10

IFXRESULT IFXSubdivisionManager::UpdateMesh (IFXMeshGroup **ppOutMeshGrp, BOOL *pUpdated)
{
	IFXASSERT(pUpdated && ppOutMeshGrp);

	if (m_bDontUpdate)
	{
		*ppOutMeshGrp = NULL;
		return IFX_E_UNDEFINED;  /// @todo: need a descriptive warning flag.
	}

	m_bDontUpdate = IFX_TRUE;


	// If in Adaptive mode, force update:
	if (m_pbBoolean[Adaptive] || m_bUpdateRequired)
	{
		U32 uRetry;
		bool bOutMshReallocated = true;
		for (uRetry = 0; (uRetry < IFXNumReallocationAttempts) && (bOutMshReallocated == true); uRetry++)
		{
			if (!m_pOutputMeshGrp)
				return IFX_E_UNDEFINED;

			m_bReallocateOutputMesh = IFX_FALSE;

			// tell the calling procedure the mesh has changed.
			*pUpdated = TRUE;

			ResetOutputMeshGroup (m_pOutputMeshGrp);

			// Apply subdivision:
			Update();

			U32 f;
			// Gather the current subdivision for rendering:
			for (f = 0; f < this->m_uNumBaseTriangles; f++)
				this->m_pBaseTriangle[f].GatherRenderData(this, m_puInteger[MaxRenderDepth]);

			// Check for reallcoation of output mesh:
			if (m_bReallocateOutputMesh)
			{
				IFXRESULT rc;
				rc = ReallocateOutputMesh();
				if (IFXSUCCESS(rc))
				{
					bOutMshReallocated = true;
				}
				else
					bOutMshReallocated = false;
			}
			else
				bOutMshReallocated = false;

			// Reset the rendering markers::
			for (f = 0; f < this->m_uNumBaseTriangles; f++)
				this->m_pBaseTriangle[f].ResetMarkers(0);

			if (m_puInteger[MaxComputeDepth] == m_puInteger[CurrentComputeDepth])
				m_bUpdateRequired = IFX_FALSE;
			else
				m_bUpdateRequired = IFX_TRUE;
		}
	}

	*ppOutMeshGrp = m_pOutputMeshGrp;

	m_bDontUpdate = IFX_FALSE;

	return IFX_OK;
}


IFXButterflyScheme *IFXSubdivisionManager::GetSubdivisionScheme()
{
	return m_pButterflyScheme;
}


// This method invalidates all cached subdivisions and returns all
// triangles and vertices to the free list:
IFXRESULT IFXSubdivisionManager::ResetAll()
{
	// Guard against preemptive updates:
	if (m_bDontUpdate)
		return IFX_OK;

	m_bDontUpdate = IFX_TRUE;

	/* For memory Debug:
	U32 allocatedVerts, usedVerts, freeVerts;
	U32 allocatedFaces, usedFaces, freeFaces;
	// Before ResetAll():
	allocatedVerts  = m_pVertexAllocator->SizeList();
	usedVerts   = m_pVertexAllocator->NumAllocatedUnits();
	freeVerts   = m_pVertexAllocator->NumFreeUnits();
	allocatedFaces  = this->m_pTriangleAllocator->SizeList();
	usedFaces   = m_pTriangleAllocator->NumAllocatedUnits();
	freeFaces   = m_pTriangleAllocator->NumFreeUnits();
	*/

	// 1) Apply user specified adaptive subdiv metric and subdivide if directed:
	U32 f;
	for (f = 0; f < m_uNumBaseTriangles; f++)
		m_pBaseTriangle[f].ResetAll(this);


	m_puInteger[CurrentComputeDepth] = 0;

	/* For memory Debug:
	// After ResetAll():
	allocatedVerts  = m_pVertexAllocator->SizeList();
	usedVerts   = m_pVertexAllocator->NumAllocatedUnits();
	freeVerts   = m_pVertexAllocator->NumFreeUnits();
	allocatedFaces  = this->m_pTriangleAllocator->SizeList();
	usedFaces   = m_pTriangleAllocator->NumAllocatedUnits();
	freeFaces   = m_pTriangleAllocator->NumFreeUnits();
	*/
	// Get subdivision back up to current levels:
	U32 uLevel;
	for ( uLevel = 0; uLevel < this->m_puInteger[MaxComputeDepth]; uLevel++)
	{
		Update();

		// Reset the rendering markers::
		for (f = 0; f < this->m_uNumBaseTriangles; f++)
			this->m_pBaseTriangle[f].ResetMarkers(0);
	}


	m_bDontUpdate = IFX_FALSE;

	return IFX_OK;
}
