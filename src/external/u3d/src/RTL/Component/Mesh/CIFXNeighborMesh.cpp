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
// CIFXNeighborMesh.cpp

#include <memory.h>
#include "IFXMesh.h"
#include "CIFXNeighborMesh.h"
#include "IFXFaceLists.h"
#include "IFXCoincidentVertexMap.h"

IFXRESULT IFXAPI_CALLTYPE CIFXNeighborMeshFactory(IFXREFIID intId, void** ppPtr)
{
	IFXRESULT rc = IFX_E_INVALID_POINTER;
	if(ppPtr)
	{
		CIFXNeighborMesh* pPtr = new CIFXNeighborMesh;
		if(pPtr)
		{
			rc = pPtr->Construct();
			
			if(IFXSUCCESS(rc))
			{
				rc = pPtr->QueryInterface(intId, ppPtr);
			}
			
			if(IFXFAILURE(rc))
			{
				delete pPtr;
			}
		}
		else
		{
			rc = IFX_E_OUT_OF_MEMORY;
		}
	}
	return rc;
}

U32 CIFXNeighborMesh::AddRef()
{
	return ++m_refCount;
}

U32 CIFXNeighborMesh::Release()
{
	if (!(--m_refCount))
	{
		delete this;
		return 0;
	}
	return m_refCount;
}

IFXRESULT CIFXNeighborMesh::QueryInterface(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT result = IFX_OK;
	if (ppInterface)
	{
		if (interfaceId == IID_IFXNeighborMesh)
		{
			*(IFXNeighborMesh**)ppInterface = (IFXNeighborMesh*) this;
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
CIFXNeighborMesh::CIFXNeighborMesh()
{
	m_refCount = 0;
}

CIFXNeighborMesh::~CIFXNeighborMesh()
{
	Deallocate();
}





//===============================
// Protected Methods
//===============================
IFXRESULT CIFXNeighborMesh::Construct()
{
	IFXRESULT rc = IFX_OK;

	if(IFXSUCCESS(rc))
	{
		m_validState = FALSE;
		m_pMeshGroup = NULL;
		m_ppNeighborFaces = NULL;
		m_numMeshes = 0;
		m_pVertexMapGroup = 0;
	}
	
	return rc;
}

void CIFXNeighborMesh::Deallocate()
{
	if (m_ppNeighborFaces)
	{
		U32 i;
		for ( i = 0; i < m_numMeshes; ++i)
		{
			if (m_ppNeighborFaces[i])
			{
				delete [] m_ppNeighborFaces[i];
			}
		}
		delete [] m_ppNeighborFaces;
		m_ppNeighborFaces = NULL;
	}
	m_validState = FALSE;
	m_numMeshes = 0;
}	


// Allocate memory for neighbor mesh in a parallel array
// fashion, according to the sizes in IFXMeshGroup.
IFXRESULT CIFXNeighborMesh::Allocate(IFXMeshGroup& rMeshGroup)
{
	IFXRESULT result = IFX_OK;

	m_pMeshGroup = &rMeshGroup;

	m_numMeshes = rMeshGroup.GetNumMeshes();
	if (m_numMeshes)
	{
		m_ppNeighborFaces = new IFXNeighborFace* [m_numMeshes];

		if (m_ppNeighborFaces)
			result = IFX_OK;
		else
			result = IFX_E_OUT_OF_MEMORY;

		U32 i;
		for ( i = 0; result == IFX_OK  &&  i < m_numMeshes; ++i)
		{
			// Allocated array of neighbor faces for mesh i
			IFXMesh* pMesh = 0;
			rMeshGroup.GetMesh(i, pMesh);
			U32 maxNumFaces = pMesh->GetMaxNumFaces();
			m_ppNeighborFaces[i] = new IFXNeighborFace[maxNumFaces];
			if (m_ppNeighborFaces[i] == NULL)
			{
				result = IFX_E_OUT_OF_MEMORY;
			}
			else
			{
				memset(m_ppNeighborFaces[i], 0, sizeof(IFXNeighborFace)*maxNumFaces);
			}

			IFXRELEASE(pMesh);
		}
	}
		
	return result;
}

IFXRESULT CIFXNeighborMesh::Build(	/*in*/		IFXMeshGroup& rMeshGroup,
									/*in*/		IFXVertexMapGroup* pVertexMapGroup)
{
	//IFXRESULT result = IFX_OK;

	// Verify that Allocated has been called on pNeighborMesh
	IFXASSERT(IsAllocated());

	// Make sure the mesh indices are not too large for the
	// neighbor mesh indices.
	U32 meshIndex;
	for (meshIndex = 0; meshIndex < rMeshGroup.GetNumMeshes(); ++meshIndex)
	{
		IFXMesh* pMesh = 0;
		rMeshGroup.GetMesh(meshIndex, pMesh);

		if( pMesh )
		{
			U32 numFaces = pMesh->GetNumFaces();
			U32 numVertices = pMesh->GetNumVertices();
			if (numFaces > MAX_NBR_MESH_INDEX || numVertices > MAX_NBR_MESH_INDEX)
				return IFX_E_MESH_TOO_LARGE;
		}

		IFXRELEASE(pMesh);
	}

	m_pMeshGroup = &rMeshGroup;
	m_pVertexMapGroup = pVertexMapGroup;

	return BuildLinks();
}


// The first step is to walk through all faces and
// build a set of edge lists containing all the edges in 
// the mesh.  Duplicate vertices (vertices with same 
// position but different attributes) are resolved 
// to a representative vertex used in the edge list.
// 
// An edge list contains the two vertices that form
// the edge and a list of faces incident on the edge.
// 
// Each face in an edge list has a neighbor pointer linked
// to the next face in the list.  This forms a circular linked
// list around the edge using the neighbor pointers.
IFXRESULT CIFXNeighborMesh::BuildLinks()
{
	IFXRESULT result = IFX_OK;
	
	//
	// Build "mesh vertex index" to "coincident vertex ID" map
	//

	IFXCoincidentVertexMap vertexMap;

	if (m_pVertexMapGroup)
		result = vertexMap.Initialize(m_pMeshGroup, m_pVertexMapGroup);
	else
		result = vertexMap.Initialize(m_pMeshGroup);

	//
	// Walk through all faces for each mesh and add edges
	//

	IFXFaceLists faceLists;
	if (result == IFX_OK)
	{
		result = faceLists.Initialize(vertexMap.GetNumVertexID());
	}

	U32 faceIndex, meshIndex;
	if (result == IFX_OK)
	{
		IFXFaceIter faceIter;
		U32 numMeshes = m_pMeshGroup->GetNumMeshes();
		for (meshIndex = 0; (result == IFX_OK) && (meshIndex < numMeshes); ++meshIndex)
		{
			IFXMesh* pMesh = 0;
			m_pMeshGroup->GetMesh(meshIndex, pMesh);
			pMesh->GetFaceIter(faceIter);

			U32 numFaces = pMesh->GetNumFaces();
			for (faceIndex = 0; (result == IFX_OK) && (faceIndex < numFaces); ++faceIndex)
			{
				IFXFace* pFace = faceIter.Next();

				//U32 ta = pFace->VertexA();
				//U32 tb = pFace->VertexB();
				//U32 tc = pFace->VertexC();

				U32 a = vertexMap.Convert(meshIndex, pFace->VertexA());
				U32 b = vertexMap.Convert(meshIndex, pFace->VertexB());
				U32 c = vertexMap.Convert(meshIndex, pFace->VertexC());

				/* 
				   a
				   |\ 
				   |0 \
				   |    \ 
				   |   1/ b
				   |2 /
				   |/
				   c
				*/
				//  Corner 2 is across from edge ab.
				//  Corner 0 is across from edge bc.
				//  Corner 1 is across from edge ca
				result = faceLists.AddFace(meshIndex, faceIndex, 2, a, b);
				if (result == IFX_OK)
				{
					result = faceLists.AddFace(meshIndex, faceIndex, 0, b, c);
				}
				if (result == IFX_OK)
				{
					result = faceLists.AddFace(meshIndex, faceIndex, 1, c, a);
				}
			}

			IFXRELEASE(pMesh);
		}
	}

	if (result != IFX_OK)
		return result;

	// Walk through all face lists and build circular linked
	// lists in the neighbor mesh.

	IFXNeighborFace* pNFace;
	BOOL hasEdges = faceLists.FirstEdge();
	if (hasEdges == FALSE)
		return IFX_OK;		// empty neighbor mesh is valid

	U32 startMeshIndex, startFaceIndex, startCornerInfo;
	U32 prevMeshIndex, prevFaceIndex, prevCornerInfo;
	U32 cornerInfo;
	do
	{
		faceLists.GetFace(&startMeshIndex, &startFaceIndex, &startCornerInfo);
		prevMeshIndex = startMeshIndex;
		prevFaceIndex = startFaceIndex;
		prevCornerInfo = startCornerInfo;
		
		while (faceLists.NextFace())
		{
			//
			// Set the neighbor record for the previous face to point to
			//  the current face and corner.
			//

			faceLists.GetFace(&meshIndex, &faceIndex, &cornerInfo);

			// Build flags (for prevFaceIndex)
			IFXCornerFlags flags = {0, 0, 0, 0, 0, 0, 0};
			flags.cornerIndex = cornerInfo & IFX_CORNER_INDEX_MASK;
			flags.flippedFlag = (prevCornerInfo & IFX_EDGE_ORDER_FLIPPED_MASK) != 0;

			// Get the neighbor record and set it.
			U32 prevCornerIndex = prevCornerInfo & IFX_CORNER_INDEX_MASK;
			pNFace = GetNeighborFaceArray(prevMeshIndex) + prevFaceIndex;
			pNFace->SetNeighbor(prevCornerIndex, meshIndex, faceIndex, flags);

			prevMeshIndex = meshIndex;
			prevFaceIndex = faceIndex;
			prevCornerInfo = cornerInfo;
		}

		// Build corner flags (for prevFaceIndex)
		IFXCornerFlags flags = {0, 0, 0, 0, 0, 0, 0};
		flags.cornerIndex = startCornerInfo & IFX_CORNER_INDEX_MASK;
		flags.flippedFlag = (prevCornerInfo & IFX_EDGE_ORDER_FLIPPED_MASK) != 0;

		// Get the neighbor record and set it.
		U32 prevCornerIndex = prevCornerInfo & IFX_CORNER_INDEX_MASK;
		pNFace = GetNeighborFaceArray(prevMeshIndex) + prevFaceIndex;
		pNFace->SetNeighbor(prevCornerIndex, startMeshIndex, startFaceIndex, flags);
		
	} while (faceLists.NextEdge());

	return IFX_OK;
}



#define IFXTEXCOORD_EPSILON                     0.000000000001f
#define IFXNORMAL_EPSILON                       0.000000000001f

IFXRESULT CIFXNeighborMesh::MarkAttributeDiscontinuities(IFXMeshGroup& rMeshGrp)
{

    U32 uNumMeshes = rMeshGrp.GetNumMeshes();

    // Iterate over each mesh:
    U32 m;
    for ( m = 0; m < uNumMeshes; m++)
    {
        IFXMesh *pMesh = 0;
		rMeshGrp.GetMesh(m, pMesh);
        U32 uNumFaces = pMesh->GetNumFaces();

        // Look to see what attributes are present:
        IFXVertexAttributes vertexAttribs = pMesh->GetAttributes();

        // Iterate over each face:
        U32 f;
        for ( f = 0; f < uNumFaces; f++)
        {
            // Iterate over each corner:
            U32 c;
            for ( c = 0; c < 3; c++)
            {
                IFXFatCornerIter    cornerIter;
                IFXCornerFlags      *pCornerFlags; //, *pCornerFlags2;
                
                GetFatCornerIter(0,m,f,c, cornerIter);
                pCornerFlags = cornerIter.GetCornerFlags();
               

                if (!pCornerFlags->CORNER_FLAG_DISCONTINUOUS_FINISHED)
                {
                    // mark flags as processed:
                    pCornerFlags->CORNER_FLAG_DISCONTINUOUS_FINISHED = 1;

                    // Grab the local attributes:
                    IFXVector3 *pF1N1, *pF1N2, *pF2N1, *pF2N2;
                    IFXVector2 *pF1TC1, *pF1TC2, *pF2TC1, *pF2TC2;
                    cornerIter.MoveClockwise();
                    pF1N1 = cornerIter.GetNormal();
                    pF1TC1 = cornerIter.GetTexCoord();
                    cornerIter.MoveClockwise();
                    pF1N2 = cornerIter.GetNormal();
                    pF1TC2 = cornerIter.GetTexCoord();   
                    
                    // Now reset the iterator and jump to the neighbor face's far corner:
                    cornerIter.MoveClockwise();
                    cornerIter.JumpAcross();
                    // pCornerFlags2 = cornerIter.GetCornerFlags();

                    // Look for surface flips, (non-manifold surface disconitnuities)
                    //if (pCornerFlags->flippedFlag == pCornerFlags2->flippedFlag)
                    //    pCornerFlags->CORNER_FLAG_DISCONTINUOUS_SURFACE = 1;
                    //else
                    {
                        pCornerFlags->CORNER_FLAG_DISCONTINUOUS_SURFACE = 0;

                        // Note if the face is on a mesh boundary, the JumpAcross operator
                        // will return the same face.  flips flags will be equal and the
                        // edge will be marked with discontinuous attributes because the
                        // vertex associations will be reversed.  No need, but we could
                        // check to see if JumpAcross's face has the same mesh and face index
                        // as the original face.

                        // Now grab its local attributes:
                        cornerIter.MoveCounterClockwise();
                        pF2N1 = cornerIter.GetNormal();
                        pF2TC1 = cornerIter.GetTexCoord();
                        cornerIter.MoveCounterClockwise();
                        pF2N2 = cornerIter.GetNormal();
                        pF2TC2 = cornerIter.GetTexCoord();               

                        if (vertexAttribs.m_uData.m_bHasNormals)
                        {
                            // Now compare attribute values and set the appropriate flags:
                            if (pF1N1->IsApproximately(*pF2N1, IFXNORMAL_EPSILON) && pF1N2->IsApproximately(*pF2N2, IFXNORMAL_EPSILON))
                                pCornerFlags->CORNER_FLAG_DISCONTINUOUS_NORMAL = 0;
                            else
                                pCornerFlags->CORNER_FLAG_DISCONTINUOUS_NORMAL = 1;
                        }
                    
                        if (vertexAttribs.m_uData.m_uNumTexCoordLayers)
                        {
                            if ((pF2TC1 && pF1TC2) && pF1TC1->IsApproximately(*pF2TC1, IFXTEXCOORD_EPSILON) && pF1TC2->IsApproximately(*pF2TC2, IFXTEXCOORD_EPSILON))
                                pCornerFlags->CORNER_FLAG_DISCONTINUOUS_TEXCOORD = 0;
                            else
                                pCornerFlags->CORNER_FLAG_DISCONTINUOUS_TEXCOORD = 1;
                        }
                    }
                }
            }
        }

		IFXRELEASE(pMesh);
    }  

    return IFX_OK;
}

#if 0

//
//  Neighbor Mesh Debugging Utilities
//

#define MAX_PREVENT_INFINITE_LOOP 1000


void CIFXNeighborMesh::VerifyEdgesAndLoops(IFXMeshGroup& rMeshGroup,
											  IFXVertexMapGroup* pVertexMapGroup)
{
	IFXNeighborMesh* pNeighborMesh = rMeshGroup.GetNeighborMesh();
	IFXASSERT(pNeighborMesh);
	IFXASSERT(pNeighborMesh->IsAllocated());
	IFXASSERT(pNeighborMesh->IsValid());

	IFXCoincidentVertexMap vertexMap;
	vertexMap.Initialize(pMeshGroup, pVertexMapGroup);

	U32 numMeshes = pNeighborMesh->GetNumMeshes();
	U32 meshIndex;
	for ( meshIndex = 0; meshIndex < numMeshes; ++meshIndex)
	{
		U32 numFaces = rMeshGroup.GetMesh(meshIndex)->GetNumFaces();
		U32 faceIndex;
		for ( faceIndex = 0; faceIndex < numFaces; ++faceIndex)
		{
			U32 cornerIndex;
			for ( cornerIndex = 0; cornerIndex < 3; ++cornerIndex)
			{
				IFXFatCornerIter cornerIter;
				pNeighborMesh->GetFatCornerIter(0, meshIndex, faceIndex, cornerIndex, &cornerIter);
				CheckEdge(&cornerIter, pMeshGroup, &vertexMap);
			}
		}
	}
	
	IFXRELEASE(pNeighborMesh);
}


void CIFXNeighborMesh::CheckEdge(IFXFatCornerIter* cornerIter,
									IFXMeshGroup& rMeshGroup,
									IFXCoincidentVertexMap* pCoVertexMap)
{
	U32 meshIndex = cornerIter->GetMeshIndex();
	U32 faceIndex = cornerIter->GetFaceIndex();
	U32 cornerIndex = cornerIter->GetCornerIndex();

	U32 count = 0;

	do
	{
		// Get data on current face
		U32 meshIndex = cornerIter->GetMeshIndex();
		U32 faceIndex = cornerIter->GetFaceIndex();
		IFXMesh* pMesh = rMeshGroup.GetMesh(meshIndex);
		IFXFaceIter faceIter;
		pMesh->GetFaceIter(&faceIter);
		IFXFace* pFace = faceIter.Index(faceIndex);
		
		
		U32 ci = cornerIter->GetCornerIndex();

		cornerIter->MoveClockwise();
		U32 orgVtx1A = pCoVertexMap->Convert(meshIndex, pFace->Vertex(cornerIter->GetCornerIndex()));
		IFXVector3* pVect1A = cornerIter->GetPosition();

		cornerIter->MoveClockwise();
		U32 orgVtx1B = pCoVertexMap->Convert(meshIndex, pFace->Vertex(cornerIter->GetCornerIndex()));
		IFXVector3* pVect1B = cornerIter->GetPosition();

		cornerIter->MoveClockwise();
		IFXASSERT(ci == cornerIter->GetCornerIndex());


		IFXCornerFlags* cornerFlags;
		cornerIter->JumpAcross(&cornerFlags);
		++count;

		// Get data on current face
		meshIndex = cornerIter->GetMeshIndex();
		faceIndex = cornerIter->GetFaceIndex();
		pMesh = rMeshGroup.GetMesh(meshIndex);
		pMesh->GetFaceIter(&faceIter);
		pFace = faceIter.Index(faceIndex);
		

		ci = cornerIter->GetCornerIndex();

		cornerIter->MoveClockwise();
		U32 orgVtx2A = pCoVertexMap->Convert(meshIndex, pFace->Vertex(cornerIter->GetCornerIndex()));
		IFXVector3* pVect2A = cornerIter->GetPosition();

		cornerIter->MoveClockwise();
		U32 orgVtx2B = pCoVertexMap->Convert(meshIndex, pFace->Vertex(cornerIter->GetCornerIndex()));
		IFXVector3* pVect2B = cornerIter->GetPosition();

		cornerIter->MoveClockwise();
		IFXASSERT(ci == cornerIter->GetCornerIndex());

		IFXCornerFlags* cornerFlags2;
		cornerFlags2 = cornerIter->GetCurrentFace()->GetCornerFlags(ci);
		// Adjacent manifold faces have opposite flipped flags.
		if (cornerFlags->flippedFlag != cornerFlags2->flippedFlag)
		{
			IFXASSERT(orgVtx1A == orgVtx2B);
			IFXASSERT(orgVtx1B == orgVtx2A);

			// Uncomment the following to compare vertex positions.
			// NOTE: Animation does not guarantee that vertices along
			//		 discontinuities are rotated the same.
//			IFXASSERT(*pVect1A == *pVect2B);
//			IFXASSERT(*pVect1B == *pVect2A);
		}
		else
		{
			IFXASSERT(orgVtx1A == orgVtx2A);
			IFXASSERT(orgVtx1B == orgVtx2B);

			// Uncomment the following to compare vertex positions.
			// NOTE: Animation does not guarantee that vertices along
			//		 discontinuities are rotated the same.
//			IFXASSERT(*pVect1A == *pVect2A);
//			IFXASSERT(*pVect1B == *pVect2B);
		}

	}
	while ((meshIndex != cornerIter->GetMeshIndex() ||
			faceIndex != cornerIter->GetFaceIndex() ||
			cornerIndex != cornerIter->GetCornerIndex()) &&
			count < MAX_PREVENT_INFINITE_LOOP);

	// Off in some possibly infinite loop that 
	// does not contain the original corner.
	if (count >= MAX_PREVENT_INFINITE_LOOP)
	{
		IFXTRACE_GENERIC("POSSIBLE INFINITE LOOP at mesh %d, face %d, opposite corner %d\n", meshIndex, faceIndex, cornerIndex);
	}
		
	if (count != 2)
	{
		IFXTRACE_GENERIC("Non-manifold edge with %d faces at mesh %d, face %d, opposite corner %d\n", count, meshIndex, faceIndex, cornerIndex);
	}
}



void CIFXNeighborMesh::VerifyVertexMap(IFXMeshGroup& rMeshGroup, IFXVertexMapGroup* pVertexMapGroup)
{
	U32 numMaps = pVertexMapGroup->GetNumMaps();
	U32 i;
	for ( i = 0; i < numMaps; ++i)
	{
		IFXVertexMap* pVertexMap = pVertexMapGroup->GetMap(i);

		U32 numOrigVerts = pVertexMap->GetNumMapEntries();
		U32 v;
		for ( v = 0; v < numOrigVerts; ++v)
		{
			IFXVector3 pos;
			BOOL posSet = FALSE;

			U32 numCopies = pVertexMap->GetNumVertexCopies(v);
			U32 c;
			for ( c = 0; c < numCopies; ++c)
			{
				U32 ifxMesh = 0xFFFFFFFF;
				U32 ifxVertex = 0xFFFFFFFF;
				pVertexMap->GetVertexCopy(v, c, &ifxMesh, &ifxVertex);

				IFXMesh* pMesh = rMeshGroup.GetMesh(ifxMesh);
				IFXVector3Iter pIter;
				pMesh->GetPositionIter(&pIter);

				IFXVector3* pPos = pIter.Index(ifxVertex);
				if (posSet)
				{
					IFXASSERT(pos == *pPos);
				}
				else
				{
					pos = *pPos;
					posSet = TRUE;
				}
			}
		}
	}
}

#endif // #if 0
// END OF FILE

