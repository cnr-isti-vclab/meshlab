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

#include "CIFXMeshCompiler.h"
#include "IFXMesh.h"
#include "IFXVertexMap.h"
#include "IFXCoreCIDs.h"
#include "IFXExportingCIDs.h"

IFXRESULT CIFXMeshCompiler::InitCompile(
									IFXAuthorCLODMesh* pInputMesh, 
									U32 numSizes, 
									IFXMeshSize* pOutSizes)
{
	m_InitComplete = FALSE;
	m_StaticCompileComplete = FALSE;
	m_LastResolution = 0;

	if(pInputMesh == NULL || numSizes == 0 || pOutSizes == NULL)
		return IFX_E_INVALID_POINTER;

	m_pAM = pInputMesh;
	m_numMaterials = numSizes;
	
	if(m_pAM->GetMinResolution() == 0)
		m_StaticCompileComplete = TRUE;
	
	// make a copy of the mesh size info
	if((m_pMeshSizes = new IFXMeshSize[numSizes]) == NULL) 
		return IFX_E_OUT_OF_MEMORY;
    U32 i;
	for(i = 0; i<numSizes; i++)
		m_pMeshSizes[i] = pOutSizes[i];

	if(commonInit() != IFX_OK) 
		return IFX_E_OUT_OF_MEMORY;

	m_InitComplete = TRUE;	
	return IFX_OK;
}


IFXRESULT CIFXMeshCompiler::Compile()
{
	if(!m_InitComplete)
		return IFX_E_NOT_INITIALIZED;
	if(m_StaticCompileComplete)
	{
		StreamCompile();
	}
	else
	{
		// if the static protion has fully arrived, compile it.
		if(m_pAM->GetMaxResolution() >= m_pAM->GetMinResolution())
		{
			StaticCompile();
			m_StaticCompileComplete = TRUE;
		}
	}

	return IFX_OK;
}

IFXRESULT CIFXMeshCompiler::PreviewCompile(IFXAuthorCLODMesh* pInputMesh)
{
	IFXRESULT result = IFX_OK;
	U32 i;

	IFXASSERT(pInputMesh);

	m_pAM = pInputMesh;
	IFXADDREF( m_pAM );

	m_numMaterials = m_pAM->GetMaxMeshDesc()->NumMaterials;
	IFXDELETE_ARRAY( m_pMeshSizes );
	m_pMeshSizes = new IFXMeshSize[m_numMaterials];

	if( NULL == m_pMeshSizes )
		result = IFX_E_OUT_OF_MEMORY;

	if( IFXSUCCESS( result ) )
	{
		// count number of faces per material
		m_pAM->Lock();
		m_pAM->GetFaceMaterials(&m_pFaceMaterials);
		m_pAM->Unlock();
		m_pAM->SetResolution(m_pAM->GetMaxResolution());
		U32 numFaces = m_pAM->GetMeshDesc()->NumFaces;

		for(i = 0; i < numFaces; i++)
		{
			IFXASSERT(m_pFaceMaterials[i]<m_numMaterials);
			m_pMeshSizes[m_pFaceMaterials[i]].NumFaces++;
		}

		// fill out vertex attribute info, estimate number of vertices
		m_pAM->Lock();
		m_pAM->GetMaterials(&m_pMaterials);
		m_pAM->Unlock();

		for(i = 0; i < m_numMaterials; i++)
		{
			m_pMeshSizes[i].VertexAttributes.m_uData.m_bHasDiffuseColors = 
				m_pMaterials[i].m_uDiffuseColors;
			m_pMeshSizes[i].VertexAttributes.m_uData.m_bHasSpecularColors = 
				m_pMaterials[i].m_uSpecularColors;
			m_pMeshSizes[i].VertexAttributes.m_uData.m_uNumTexCoordLayers = 
				m_pMaterials[i].m_uNumTextureLayers;
			m_pMeshSizes[i].VertexAttributes.m_uData.m_bHasPositions = TRUE;
			
			m_pMeshSizes[i].VertexAttributes.m_uData.m_bHasNormals =
				m_pAM->GetMaxMeshDesc()->NumNormals > 0? TRUE: FALSE;

			// take into account the number of dimentions of texture coordinates
			m_pMeshSizes[i].VertexAttributes.m_uData.m_uTexCoordSize0 = 
				m_pMaterials[i].m_uTexCoordDimensions[0];
			m_pMeshSizes[i].VertexAttributes.m_uData.m_uTexCoordSize1 = 
				m_pMaterials[i].m_uTexCoordDimensions[1];
			m_pMeshSizes[i].VertexAttributes.m_uData.m_uTexCoordSize2 = 
				m_pMaterials[i].m_uTexCoordDimensions[2];
			m_pMeshSizes[i].VertexAttributes.m_uData.m_uTexCoordSize3 = 
				m_pMaterials[i].m_uTexCoordDimensions[3];
			m_pMeshSizes[i].VertexAttributes.m_uData.m_uTexCoordSize4 = 
				m_pMaterials[i].m_uTexCoordDimensions[4];
			m_pMeshSizes[i].VertexAttributes.m_uData.m_uTexCoordSize5 = 
				m_pMaterials[i].m_uTexCoordDimensions[5];
			m_pMeshSizes[i].VertexAttributes.m_uData.m_uTexCoordSize6 = 
				m_pMaterials[i].m_uTexCoordDimensions[6];
			m_pMeshSizes[i].VertexAttributes.m_uData.m_uTexCoordSize7 = 
				m_pMaterials[i].m_uTexCoordDimensions[7];

			/**
				@todo	Develop more flexible approach - reallocate arrays if 
						underestimated over estimate number of vertices, then 
						trim back later
			*/
			m_pMeshSizes[i].NumVertices = m_pMeshSizes[i].NumFaces * 12 + 100;

			// over estimate res changes
			m_pMeshSizes[i].NumResolutionChanges = 
				m_pMeshSizes[i].NumFaces * 4 + 100;

			// over estimate updates
			m_pMeshSizes[i].NumFaceUpdates = 
				m_pMeshSizes[i].NumResolutionChanges * 18 + 100;		
		}
	}

	// Do necessary setup
	if( IFXSUCCESS( result ) )
		result = commonInit();

	if( IFXSUCCESS( result ) )
	{	
		// Now do the compile
		m_pAM->SetResolution(m_pAM->GetMinResolution());

		// Special compile for static portion would give fewer vertices.
		StaticCompile();
		m_pAM->SetResolution(m_pAM->GetMinResolution());
		
		// compile from MinResolution to MaxResolution, thus removing 
		// anything beyond FinalMaxResolution.
		StreamCompile();
		
		IFXMesh* pMesh = NULL;
		// set mesh size info to the actual values.
		for(i = 0; i < m_numMaterials; i++)
		{
			m_pMG->GetMesh(i, pMesh);
			m_pMeshSizes[i].NumVertices = pMesh->GetNumVertices();
			
			m_pMeshSizes[i].NumResolutionChanges = m_pUG->GetUpdates(i)->numResChanges;
			m_pMeshSizes[i].NumFaceUpdates = m_pUG->GetUpdates(i)->numFaceUpdates;
			IFXRELEASE(pMesh);
		}

		m_pUG->SetFinalMaxResolution(m_pUG->GetMaxResolution());
	}

	return result;
}


// Compile from last resolution to MaxResolution.
IFXRESULT CIFXMeshCompiler::StreamCompile()
{
	IFXASSERT(m_pAM);

	m_Stream = TRUE;
	
	IFXRESULT result = IFX_OK;
	IFXAuthorFaceUpdate* pFaceUpdates;
	U32 m, r, f, firstFace, lastFace, numUpdates, update;
	
	if(m_LastResolution == 0)
		m_pAM->SetResolution(m_pAM->GetMinResolution());
	else
		m_pAM->SetResolution(m_LastResolution);
	
	for(r= m_LastResolution+1; r <= m_pAM->GetMaxResolution(); r++)
	{
		for(m=0; m<m_numMaterials; m++) m_MaterialUpdated[m] = FALSE;

		firstFace = m_pAM->GetMeshDesc()->NumFaces;
		m_pAM->SetResolution(r);
		lastFace = m_pAM->GetMeshDesc()->NumFaces;
		for(f = firstFace; f < lastFace; f++)
			compileFace(f);
	
		pFaceUpdates = m_pUpdates[r-1].pFaceUpdates;
		numUpdates = m_pUpdates[r-1].NumFaceUpdates;

		for(update = 0; update < numUpdates; update++)
			compileUpdate(&pFaceUpdates[update]);

		// fill in sync table and keep count of resolution changes for each material.
		for(m = 0; m < m_numMaterials; m++) 
		{
			if(m_MaterialUpdated[m]) 
			{
				m_pUG->GetSyncTable(m)[m_pUG->GetUpdates(m)->numResChanges++] = r - 1;
				IFXASSERT(
					m_pUG->GetUpdates(m)->numResChanges <= 
					m_pMeshSizes[m].NumResolutionChanges);
			}
		}
	}

	m_pUG->SetMaxResolution(m_pAM->GetMaxResolution());
	
	// keep track of how much we have compiled, 
	// decompressor will have changed resolution between calls to compile.
	m_LastResolution = m_pAM->GetResolution();
	
	return result;
}


IFXRESULT CIFXMeshCompiler::StaticCompile()
{
	IFXRESULT result = IFX_OK;
	U32 numFaces,mat,face,corner,IFXFaceIndex, vertIndex;
	IFXMesh *pMesh;
	IFXFaceIter FaceIter;
	IFXFace *pIFXFace;
	IFXVertexMap *pFaceMap;

	pFaceMap = m_pMeshMap->GetFaceMap();
	
	m_Stream = FALSE;

	m_pAM->SetResolution(m_pAM->GetMinResolution());
	
	numFaces = m_pAM->GetMeshDesc()->NumFaces;
	for(face = 0; face < numFaces; face++)
	{
		mat = m_pFaceMaterials[face];
		m_pMG->GetMesh(mat, pMesh);
		pMesh->GetFaceIter(FaceIter);
		
		IFXFaceIndex = pMesh->GetNumFaces();
		pIFXFace = FaceIter.Index(IFXFaceIndex);
		
		//m_pMeshMap->AddFaceMapping(face, IFXFaceIndex, mat);
		pFaceMap->AddVertex(face,mat,IFXFaceIndex);
		
		for(corner = 0; corner < 3; corner++)
		{
			// look for existing vertex, if none create new vertex
			result= findOrBuildVertex(corner, face, mat, NULL, &vertIndex);
			if(result != IFX_OK) 
			{
				return IFX_E_OUT_OF_MEMORY;
			}
			// fill out ifxface data
			pIFXFace->Set(corner, vertIndex);
		}
		pMesh->SetNumFaces(pMesh->GetNumFaces() + 1);
		IFXRELEASE(pMesh);
	}
	m_LastResolution = m_pAM->GetMinResolution();
	return result;
}

IFXRESULT CIFXMeshCompiler::compileUpdate(IFXAuthorFaceUpdate* pAFU)
{
	IFXRESULT result;
	U32 newIndex, currentIndex, mat, corner, f = pAFU->FaceIndex;
	m_pAM->GetFaceMaterial(f,&mat);
	IFXUpdates *pUpdates = m_pUG->GetUpdates(mat);
	IFXResolutionChange *pRC = &pUpdates->pResChanges[pUpdates->numResChanges];
	IFXMesh *pMesh = NULL; 
	IFXFaceIter FaceIter;

	m_pMG->GetMesh(mat, pMesh);

	pMesh->GetFaceIter(FaceIter);
	
	U32 dc;
	U32 IFXFaceIndex = 0;  // look up f in face map to find out where it is in the ifx mesh.

	m_pFaceMap->GetVertexCopy(f,0,&dc,&IFXFaceIndex);
	
	IFXFace *pIFXFace = FaceIter.Index(IFXFaceIndex);

	corner = pAFU->Corner;
	result = findOrBuildVertex(corner, f, mat, pRC, &newIndex);
	if(result != IFX_OK) return IFX_E_OUT_OF_MEMORY;
	
	currentIndex = pIFXFace->Vertex(corner);

	if(newIndex != currentIndex)
	{
		IFXFaceUpdate *pIFU = &pUpdates->pFaceUpdates[pUpdates->numFaceUpdates];
		pIFU->face = IFXFaceIndex;
		pIFU->corner = corner;
		pIFU->newUp = newIndex;
		pIFU->newDown = currentIndex;
		
		// count number of face updates for this resolution change.
		pRC->numFaceUpdates++;
		
		// count total face updates for this material.
		IFXASSERT(pUpdates->numFaceUpdates < m_pMeshSizes[mat].NumFaceUpdates);
		pUpdates->numFaceUpdates++;
		
		// perform update to ifx mesh
		pIFXFace->Set(corner, newIndex);
		
		// remember that we have updated this material
		m_MaterialUpdated[mat] = TRUE;
	}
	IFXRELEASE(pMesh);
	return IFX_OK;
}

IFXRESULT CIFXMeshCompiler::compileFace(U32 f)
{
	IFXRESULT result;
	U32 mat, vertIndex;
	mat = m_pFaceMaterials[f];
	IFXMesh *pMesh = NULL; 
	IFXFaceIter FaceIter;
	m_pMG->GetMesh(mat, pMesh);
	pMesh->GetFaceIter(FaceIter);
	
	U32 IFXFaceIndex = pMesh->GetNumFaces();
	IFXFace *pIFXFace = FaceIter.Index(IFXFaceIndex);
	
	m_pFaceMap->AddVertex(f, mat, IFXFaceIndex);
	
	// bump numnewfaces in ifx update record
	IFXUpdates *pUpdates = m_pUG->GetUpdates(mat);
	IFXResolutionChange *pRC = &pUpdates->pResChanges[pUpdates->numResChanges];
	pRC->deltaFaces++;
	m_MaterialUpdated[mat] = TRUE;
	
	U32 corner;
	for( corner = 0; corner < 3; corner++)
	{
		// look for existing vertex, if none create new vertex
		result= findOrBuildVertex(corner, f, mat, pRC, &vertIndex);
		if(result != IFX_OK) return IFX_E_OUT_OF_MEMORY;

		// fill out ifxface data
		pIFXFace->Set(corner, vertIndex);
	}
	pMesh->SetNumFaces(pMesh->GetNumFaces() + 1);
	IFXRELEASE(pMesh);
	return IFX_OK;
}


IFXRESULT CIFXMeshCompiler::findOrBuildVertex(
									U32 corner, 
									U32 authorFaceIndex, 
									U32 mat, 
									IFXResolutionChange* pRC, 
									U32* pOutIFXIndex)
{
	VertexDescriptor *pMatch, *pQ = &m_pQV[mat];  // grab pre allocated query vertex
	IFXAuthorMaterial *pMatDesc = &m_pMaterials[mat];
	U32 layer, numDim;
	U32 attributeIndex = 1; // the first one is always the material id which is 
							// already set when the query verts were allocated. 
	U32 AuthorIndex = (U32)-1;

	// fill in query vertex descriptor (was partially filled in at allocate 
	// time for this material).
	if(pMatDesc->m_uNormals)
	{
		if(m_Stream)
		{
			// match normals by value, copy the actual normal into the vertex descriptor
			AuthorIndex = m_pNormalFaces[authorFaceIndex].corner[corner];
			pQ->Normal = m_pNormals[ AuthorIndex ];
			attributeIndex++;
		}
		else 	
		{
			// static mode, match normals by index
			pQ->pAttributes[attributeIndex++] = 
				m_pNormalFaces[authorFaceIndex].corner[corner];
		}
	}

	for(layer = 0; layer < pMatDesc->m_uNumTextureLayers; layer++)
	{
		pQ->pAttributes[attributeIndex++] = 
			m_pTextureFaces[layer][authorFaceIndex].corner[corner];
	}

	if(pMatDesc->m_uDiffuseColors)
	{
		pQ->pAttributes[attributeIndex++] = 
			m_pDiffuseFaces[authorFaceIndex].corner[corner];
	}

	if(pMatDesc->m_uSpecularColors)
	{
		pQ->pAttributes[attributeIndex++] = 
			m_pSpecularFaces[authorFaceIndex].corner[corner];
	}
	
	pQ->AuthorIndex = m_pPositionFaces[authorFaceIndex].corner[corner];

	// query with vertex description that we just filled in
	if(m_Stream)
 	{
   		pMatch = m_pVertexHash->FindBestMatch(pQ);
 		// pMatch is the best existing vertex from the m_VertexHash, check to see if
 		// it is within the allowed MaxNormalError.  
		// If not will build a new vertex that matches. 
 		if( pMatch && pMatDesc->m_uNormals && 
			pMatch->Normal.DotProduct(pQ->Normal) < m_CosMaxNormalError)
		{
 			pMatch = NULL;
		}
 	}
	else
		pMatch = m_pVertexHash->FindExactMatch(pQ);

	if(pMatch)
	{
		// we have a matching vertex who's normal is 
		// within tolerance (or normals don't exist).
		*pOutIFXIndex = pMatch->IFXIndex;

		if(m_Stream && pMatDesc->m_uNormals)
		{
 			// Copy ideal normal into ifxmesh, this will make the normals
 			// in the mesh correct at high res.   The normal in the m_VertexHash is
 			// left alone so that we will tend to add vertices as needed to support
 			// new normals that exist at lower resolutions.
  			IFXVector3Iter NormIter;
			IFXMesh *pMesh = NULL; 
			m_pMG->GetMesh(mat, pMesh);
			pMesh->GetNormalIter(NormIter);
			*NormIter.Index(pMatch->IFXIndex) = pQ->Normal;
			IFXRELEASE(pMesh);
		}
		return IFX_OK;
	}
	else
	{
		// no match or normal not within tolerance so build 
		// new vertex that exactly matches 
		VertexDescriptor *pNewVertex = pQ->Clone();
		if(pNewVertex == NULL) 
		{
			IFXASSERT( FALSE );
			return IFX_E_OUT_OF_MEMORY;
		}
		

		IFXMesh* pMesh = NULL;
		m_pMG->GetMesh(mat, pMesh);
		U32 numVerts = pMesh->GetNumVertices();
		pNewVertex->IFXIndex = numVerts;
		// bump vertex count on output mesh.
		IFXASSERT(numVerts < m_pMeshSizes[mat].NumVertices);

		pMesh->SetNumVertices(numVerts + 1);
	
		m_pVertexHash->Insert(pNewVertex);
		
		// Fill out ifx mesh vertex.
		
		IFXVertexIter *vi;
		vi = &m_pIteratorCache[mat];  // get the iterator for this mesh.
		U32 IFXVertIndex = pNewVertex->IFXIndex;
		attributeIndex = 1;  // get past 0 which is always matid;

		*vi->GetPosition() = m_pPositions[ pNewVertex->AuthorIndex ];
		// update vertex position map
		m_pPositionMap->AddVertex(pNewVertex->AuthorIndex,mat, IFXVertIndex);
		
		if(pMatDesc->m_uNormals)
		{
			if(m_Stream)
			{
				*vi->GetNormal() = pNewVertex->Normal;
				m_pNormalMap->AddVertex(AuthorIndex, mat, IFXVertIndex);
				attributeIndex++;
			}
			else
			{
				AuthorIndex = pNewVertex->pAttributes[attributeIndex++];
				if(IFXSUCCESS(m_pNormalMap->AddVertex(AuthorIndex, mat, IFXVertIndex)))
					*vi->GetNormal() = m_pNormals[ AuthorIndex ];
			}		
		}

		IFXVector2 *pV2;
		IFXVector3 *pV3;
		IFXVector4 *pAuthorV4;
		for(layer = 0; layer < pMatDesc->m_uNumTextureLayers; layer++)
		{
			numDim = pMatDesc->m_uTexCoordDimensions[layer];
			AuthorIndex = pQ->pAttributes[attributeIndex++];
			if(IFXSUCCESS(m_pTextureMap->AddVertex(AuthorIndex, mat, IFXVertIndex)))
			{
				switch(numDim) 
				{
				case 1:
					*vi->GetTexCoordV1(layer) = m_pTextures[ AuthorIndex ].Value(0);
					break;
				case 2:
					pV2 = vi->GetTexCoordV2(layer);
					pAuthorV4 = &m_pTextures[ AuthorIndex ];
					pV2->U() = pAuthorV4->U();
					pV2->V() = pAuthorV4->V();
					break;
				case 3:
					pV3 = vi->GetTexCoordV3(layer);
					pAuthorV4 = &m_pTextures[ AuthorIndex ];
					pV3->X() = pAuthorV4->U();
					pV3->Y() = pAuthorV4->V();
					pV3->Z() = pAuthorV4->W();
					break;
				case 4:
					*vi->GetTexCoordV4(layer) = m_pTextures[ AuthorIndex ];
					break;
				default:
					IFXASSERT(0);  // bogus number of texture dimensions
				}
			}
		}

		if(pMatDesc->m_uDiffuseColors)
		{
			AuthorIndex = pQ->pAttributes[attributeIndex++];
			if( IFXSUCCESS( m_pDiffuseMap->AddVertex(AuthorIndex, mat, IFXVertIndex) ) )
				*vi->GetColor(0) = m_pDiffuses[ AuthorIndex ].GetRGBA();
		}

		if(pMatDesc->m_uSpecularColors)
		{
			AuthorIndex = pQ->pAttributes[attributeIndex++];
			if( IFXSUCCESS( m_pSpecularMap->AddVertex(AuthorIndex, mat, IFXVertIndex) ) )
				*vi->GetColor(1) = m_pSpeculars[ AuthorIndex ].GetRGBA();
		}

		vi->Next(); // point to the next vertex. 

		// update the update record
		if(m_Stream) 
		{
			pRC->deltaVerts++;
		}
		
		*pOutIFXIndex = pNewVertex->IFXIndex;
		IFXRELEASE(pMesh);

		return IFX_OK;
	}
}


IFXRESULT CIFXMeshCompiler::commonInit()
{
	IFXRESULT result;
	U32 i;

	m_pAM->Lock();

	result = allocateOutputs(m_numMaterials, m_pMeshSizes);

	if( IFXSUCCESS( result ) )
	{
		m_pAM->GetMaterials(&m_pMaterials);	
		m_pAM->GetFaceMaterials(&m_pFaceMaterials);
	
		// allocate mesh map.
		IFXRELEASE(m_pMeshMap);

		result = IFXCreateComponent(CID_IFXMeshMap,
								IID_IFXMeshMap, (void**)&m_pMeshMap);
	}

	if( IFXSUCCESS( result ) )
	{
		result = m_pMeshMap->Allocate(m_pAM);
	}

	if( IFXSUCCESS( result ) )
	{
		m_pFaceMap = m_pMeshMap->GetFaceMap();
		m_pPositionMap = m_pMeshMap->GetPositionMap();
		m_pNormalMap = m_pMeshMap->GetNormalMap();
		m_pTextureMap = m_pMeshMap->GetTextureMap();
		m_pDiffuseMap = m_pMeshMap->GetDiffuseMap();
		m_pSpecularMap = m_pMeshMap->GetSpecularMap();	

		m_MaterialUpdated = new BOOL[m_numMaterials];
		if(m_MaterialUpdated == NULL) 
			result = IFX_E_OUT_OF_MEMORY;
	}

	if( IFXSUCCESS( result ) )
	{
		// allocate vertex hash
		m_pVertexHash = new VertexHash;
		if(m_pVertexHash == NULL) 
			result = IFX_E_OUT_OF_MEMORY;
	}

	if( IFXSUCCESS( result ) )
	{
		result = m_pVertexHash->Allocate(m_pAM->GetMaxMeshDesc()->NumPositions);
	}

	if( IFXSUCCESS( result ) )
	{
		result = allocateQueryVerts();
	}

	if( IFXSUCCESS( result ) )
	{
		m_pIteratorCache = new IFXVertexIter[m_numMaterials];
		if(m_pIteratorCache == NULL) 
			result = IFX_E_OUT_OF_MEMORY;
	}

	if( IFXSUCCESS( result ) )
	{
		IFXMesh *pMesh;
		for(i=0; i<m_numMaterials; i++)
		{
			m_pMG->GetMesh(i, pMesh);
			pMesh->GetVertexIter(m_pIteratorCache[i]);
			IFXRELEASE(pMesh);
		}
	
		// crack open the author mesh.
		m_pAM->GetUpdates(&m_pUpdates);
		m_pAM->GetPositions(&m_pPositions);
		m_pAM->GetNormals(&m_pNormals);
		m_pAM->GetTexCoords(&m_pTextures);
		m_pAM->GetDiffuseColors(&m_pDiffuses);
		m_pAM->GetSpecularColors(&m_pSpeculars);
		m_pAM->GetPositionFaces(&m_pPositionFaces);
		m_pAM->GetNormalFaces(&m_pNormalFaces);
		m_pAM->GetDiffuseFaces(&m_pDiffuseFaces);
		m_pAM->GetSpecularFaces(&m_pSpecularFaces);

		// find max num texture layers
		U32 maxLayers = 0;
		for(i=0;i<m_numMaterials; i++)
			if(m_pMaterials[i].m_uNumTextureLayers > maxLayers)
				maxLayers = m_pMaterials[i].m_uNumTextureLayers;
		
		for(i=0; i<maxLayers; i++)
			m_pAM->GetTexFaces(i, &m_pTextureFaces[i]);
	}
	
	m_pAM->Unlock();
	
	return result;
}

IFXRESULT CIFXMeshCompiler::allocateQueryVerts()
{
	IFXAuthorMaterial *pMats, *pMat;
	U32 i,j,numAttributes;
	m_pAM->GetMaterials(&pMats);

	U32 numMats = m_pAM->GetMaxMeshDesc()->NumMaterials;
	m_pQV = new VertexDescriptor[numMats];
	if(m_pQV == NULL) 
		return IFX_E_OUT_OF_MEMORY;

	for(i=0;i<numMats; i++)
	{
		pMat = &pMats[i];
		
		// the 1 is for material id which always exists, other attributes are optional
		numAttributes = 1 + (pMat->m_uNormals?1:0) + pMat->m_uNumTextureLayers + 
						(pMat->m_uDiffuseColors?1:0) + (pMat->m_uSpecularColors?1:0);

		m_pQV[i].NumAttributes  = numAttributes;
		m_pQV[i].pAttributes    = new U32[numAttributes];

		if(m_pQV[i].pAttributes == NULL) 
			return IFX_E_OUT_OF_MEMORY;

		m_pQV[i].pAttributes[0] = i;  // material id is always the first attribute
		for(j=1; j < numAttributes; j++)
		{
			// Init with bad value so we can tell if we failed to fill it in 
			m_pQV[i].pAttributes[j] = IFX_BAD_INDEX32;
		}
	}

	return IFX_OK;
}

// allocate the IFXMeshGroup,IFXMeshes, IFXUpdatesGroup, IFXUpdates
IFXRESULT CIFXMeshCompiler::allocateOutputs(U32 numSizes, IFXMeshSize* pOutSizes)
{
	IFXRESULT result = IFX_OK;
	IFXRELEASE(m_pMG);
	result = IFXCreateComponent(CID_IFXMeshGroup, IID_IFXMeshGroup, (void**)&m_pMG);

	if(IFXSUCCESS(result))
	{
		result = m_pMG->Allocate(numSizes);	
	}
	
	
	if(IFXSUCCESS(result))
	{
		IFXDELETE(m_pUG);
		m_pUG = new IFXUpdatesGroup;
		if(!m_pUG)
		{
			result = IFX_E_OUT_OF_MEMORY;
		}
	}

	if(IFXSUCCESS(result))
	{
		result = m_pUG->Allocate(numSizes);		
	}

	if(IFXSUCCESS(result))
	{
		U32 i;
		for( i = 0; i < numSizes; i++)
		{
			IFXMesh *pM = NULL;
			IFXUpdates *pU = NULL;
			U32 *pSyncTable = NULL;

			result = IFXCreateComponent(CID_IFXMesh, IID_IFXMesh, (void**) &pM);
			if(IFXSUCCESS(result))
			{
				result = pM->Allocate(
								pOutSizes[i].VertexAttributes, 
								pOutSizes[i].NumVertices, 
								pOutSizes[i].NumFaces);
			}
			
			if(IFXSUCCESS(result))
			{
				m_pMG->SetMesh(i,pM);
				pM->SetNumFaces(0);    // bump this up as we fill in the data
				pM->SetNumVertices(0);

				pU = new IFXUpdates;
				if(!pU)
				{
					result = IFX_E_OUT_OF_MEMORY;
				}
			}
			if(IFXSUCCESS(result))
			{
				result = pU->Allocate(
								pOutSizes[i].NumResolutionChanges, 
								pOutSizes[i].NumFaceUpdates);	
			}
			
			if(IFXSUCCESS(result))
			{
				m_pUG->SetUpdates(i,pU);
				pU->numFaceUpdates = 0; // bump this up as we fill
				pU->numResChanges = 0;
				
				pSyncTable = new U32[pOutSizes[i].NumResolutionChanges];
				if(pSyncTable == NULL) 
				{
					result = IFX_E_OUT_OF_MEMORY;
				}
				else
				{
					m_pUG->SetSyncTable(i, pSyncTable);
				}
			}
			
			IFXRELEASE(pM);
		}	
	}
	return result;
}


CIFXMeshCompiler::CIFXMeshCompiler()
{
	m_refCount = 0;
	m_numMaterials = 0;
	m_pAM = NULL;
	m_pMG = NULL;
	m_pUG = NULL;
	m_pMeshSizes = NULL;
	m_MaterialUpdated = NULL;
	m_pMeshMap = NULL;
	m_pVertexHash = NULL;
	m_pQV = NULL;
	m_Stream = FALSE;
	m_InitComplete = FALSE;
	m_StaticCompileComplete = FALSE;
	m_LastResolution = 0;
	SetCosMaxNormalError(0.985f); // default value 
	m_pIteratorCache = NULL;

	m_pUpdates = NULL;
	m_pMaterials = NULL;
	m_pPositions = NULL;
	m_pNormals = NULL;
	m_pTextures = NULL;
	m_pSpeculars = NULL;
	m_pDiffuses = NULL;
	m_pFaceMaterials = NULL;

	m_pPositionFaces = NULL;
	m_pNormalFaces = NULL;
	U32 i;
	for(i = 0; i < IFX_MAX_TEXUNITS; ++i)
	{
		m_pTextureFaces[i] = NULL;
	}
	m_pSpecularFaces = NULL;
	m_pDiffuseFaces = NULL;

	m_pFaceMap = NULL;
	m_pPositionMap = NULL;
	m_pNormalMap = NULL;
	m_pTextureMap = NULL;
	m_pDiffuseMap = NULL;
	m_pSpecularMap = NULL;
}

CIFXMeshCompiler::~CIFXMeshCompiler()
{
	IFXRELEASE(m_pAM);
	IFXRELEASE(m_pMG);
	IFXRELEASE(m_pUG);  

	IFXDELETE(m_pVertexHash);
	IFXRELEASE(m_pMeshMap);
	IFXDELETE_ARRAY(m_pMeshSizes);
	IFXDELETE_ARRAY(m_MaterialUpdated);
	IFXDELETE_ARRAY(m_pQV);
	IFXDELETE_ARRAY(m_pIteratorCache);
}

IFXMeshGroup* CIFXMeshCompiler::GetMeshGroup()
{
	IFXADDREF(m_pMG);
	return m_pMG;
}

IFXUpdatesGroup* CIFXMeshCompiler::GetUpdatesGroup()
{
	IFXADDREF(m_pUG);
	return m_pUG;
}

IFXMeshMap* CIFXMeshCompiler::GetMeshMap()
{
	IFXADDREF(m_pMeshMap);
	return m_pMeshMap;
}

IFXRESULT CIFXMeshCompiler::GetIFXMeshSizes(U32* pNumSizes, IFXMeshSize **ppMeshSizes)
{
	*pNumSizes = m_numMaterials;
	*ppMeshSizes = m_pMeshSizes;
	return IFX_OK;
}

void CIFXMeshCompiler::SetCosMaxNormalError(F32 fCosMaxNormalError)
{
	if(fCosMaxNormalError > 1.0f) fCosMaxNormalError = 1.0f;
	if(fCosMaxNormalError < -1.0f) fCosMaxNormalError = -1.0f;
	m_CosMaxNormalError =  fCosMaxNormalError;
}

F32 CIFXMeshCompiler::GetCosMaxNormalError()
{
	return m_CosMaxNormalError;
}

IFXRESULT CIFXMeshCompiler::QueryInterface( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT	result	= IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXMeshCompiler || interfaceId == IID_IFXUnknown )
		{
			*ppInterface = ( IFXMeshCompiler* ) this;
		}
		else
		{
			*ppInterface = NULL;
			result = IFX_E_UNSUPPORTED;
		}
		if ( IFXSUCCESS( result ) )
			( ( IFXUnknown* ) *ppInterface )->AddRef();
	}
	else
		result = IFX_E_INVALID_POINTER;
	return result;
}

IFXRESULT IFXAPI_CALLTYPE CIFXMeshCompiler_Factory(
								IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT	result;
	if ( ppInterface )
	{
		CIFXMeshCompiler	*pComponent	= new CIFXMeshCompiler;
		if ( pComponent )
		{
			pComponent->AddRef();
			result = pComponent->QueryInterface( interfaceId, ppInterface );
			pComponent->Release();
		}
		else
			result = IFX_E_OUT_OF_MEMORY;
	}
	else
		result = IFX_E_INVALID_POINTER;
	return result;
}

U32 CIFXMeshCompiler::AddRef()
{
	return ++m_refCount;
}

U32 CIFXMeshCompiler::Release()
{
	if ( !( --m_refCount ) )
	{
		delete this;
		return 0;
	}
	return m_refCount;
}
