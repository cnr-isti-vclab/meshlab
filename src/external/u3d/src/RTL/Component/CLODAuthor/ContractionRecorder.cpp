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
#include "Pair.h"
#include "Vertex.h"
#include "Face.h"
#include "ContractionRecorder.h"
#include "SmallPtrSet.h"

#define VTXIDX(x) ((x) - vpcVertices)
#define CA_EPSILON  0.05f

// This can be big since typically only one allocation per run.
// Controls initial size of face update pool.
// The pool can grow if needed but that should be a very rare event.
#define INITIAL_FACE_UPDATES_POOL_SIZE 5000

/// @todo: write allocation function.
ContractionRecorder::ContractionRecorder(CLODGenerator::Params *pParams)
{
	m_pParams = pParams;
	IFXAuthorCLODMesh* pMesh = pParams->pMesh;
	U32* pVMap = pParams->pMeshMap->GetPositionMap();
	U32* pFMap = pParams->pMeshMap->GetFaceMap();
	vpcVertices     = 0;
	vpcFaces      = 0;
	numVertexRemovals = 0;
	numFaceRemovals   = 0;
	pMesh->QueryInterface(IID_IFXAuthorCLODAccess, (void**)&outIMesh);
	U32 nv        = outIMesh->GetMaxMeshDesc()->NumPositions;
	numFaceUpdates    = INITIAL_FACE_UPDATES_POOL_SIZE;
	pFaceUpdatePool   = new IFXAuthorFaceUpdate[numFaceUpdates];
	currVertexUpdate  = nv - 1;
	pCLODUpdates      = new IFXAuthorVertexUpdate[nv];

	// Grab ptrs to the re-map arrays:
	vertexMap = pVMap;
	faceMap   = pFMap;
	U32 i;
	for ( i = 0; i < outIMesh->GetMaxMeshDesc()->NumFaces; i++)
	{
		faceMap[i] = UNDEFINED_INDEX;
	}
	m_pRemoveIndex = new U32[nv];

	if(pMesh->GetMaxMeshDesc()->NumTexCoords ||
		pMesh->GetMaxMeshDesc()->NumDiffuseColors ||
		pMesh->GetMaxMeshDesc()->NumSpecularColors)
		m_hasTexturesOrColors = TRUE;
	else
		m_hasTexturesOrColors = FALSE;

	outIMesh->GetMaterials(&m_pMaterials);
	// find out how many texture layers are actually in use.
	m_NumTexLayersUsed = 0;
	U32 m;
	for( m=0; m < outIMesh->GetMaxMeshDesc()->NumMaterials; m++)
		if(m_pMaterials[m].m_uNumTextureLayers > m_NumTexLayersUsed)
			m_NumTexLayersUsed = m_pMaterials[m].m_uNumTextureLayers;

	// crack open the author mesh so we don't have to pay call overhead on every face.
	U32 layer;
	for( layer = 0; layer < m_NumTexLayersUsed; layer++)
	{
		IFXAuthorFace *pFaces;
		outIMesh->GetTexFaces(layer,&pFaces);
		m_TextureLayerFaces[layer] = pFaces;
	}
	outIMesh->GetFaceMaterials(&m_pMaterialFaces);
	outIMesh->GetDiffuseFaces(&m_pDiffuseFaces);
	outIMesh->GetSpecularFaces(&m_pSpecularFaces);
}

ContractionRecorder::~ContractionRecorder()
{
	//Normal map cleanup:
	if (normalsMode == CLODGenerator::TrackSurfaceChanges)
	{
		delete feList;
		delete sortedFeList;
		delete nMap;
	}

	IFXDELETE_ARRAY( pFaceUpdatePool );
	IFXRELEASE( outIMesh );
}

void ContractionRecorder::init ( CLODGenerator::Params *pParams, Vertex* v, Face* f)
{
	normalsMode = pParams->normalsMode;
	meshDamage = pParams->meshDamage;
	vpcVertices     = v;
	vpcFaces      = f;
	F32 cA = pParams->normalsCreaseAngle;

	// Re-map the crease angle from degrees to radians to cosine units:
	if (cA < 0.0f) cA = 0.0f;
	if (cA > 180.0f) cA = 180;
	float caRadians = (PIE / 180.0f) * cA;
	normalsCreaseAngle = cosf(caRadians);
	if (fabs(normalsCreaseAngle) < CA_EPSILON)
		normalsCreaseAngle = 0.0f;

	if (normalsMode == CLODGenerator::TrackSurfaceChanges)
	{
		// Normal map init:
		feList = new FaceExamList;
		sortedFeList = new FaceExamList;
		nMap = new NormalMap (outIMesh->GetMaxMeshDesc()->NumNormals);
		IFXVector3 *pNormals;
		outIMesh->GetNormals(&pNormals);
		nMap->insertNormals((IV3D_TAG*)pNormals);
	}
}

U32 ContractionRecorder::recordVerticesInFaceUpdates(SmallPtrSet* updated, IFXAuthorFaceUpdate* faceUpdates,
													 Vertex* keep, Vertex* remove)
{
	// Store the attribute updates resulting from this contraction into FaceUpdate records:
	// Set each Face update record :
	U32 fu = 0;
	U32 corner = 0, SetCtx = 0;
	Face* pFace = (Face*)updated->Begin(SetCtx);
	IFXAuthorFace *pFaces;
	outIMesh->GetPositionFaces(&pFaces);
	Vertex *a,*b,*c;

	while(pFace)
	{
		a = pFace->c->getCommonVertex(pFace->a);
		b = pFace->a->getCommonVertex(pFace->b);
		c = pFace->b->getCommonVertex(pFace->c);

		// Determine which token is appropriate:
		if (a == remove)
			corner = 0;
		if (b == remove)
			corner = 1;
		if (c == remove)
			corner = 2;

		faceUpdates[fu].FaceIndex = pFace->index;
		faceUpdates[fu].Attribute = IFXAuthorFaceUpdate::Position;
		faceUpdates[fu].Corner = corner;
		faceUpdates[fu].DecrValue = VTXIDX(keep);
		faceUpdates[fu].IncrValue = VTXIDX(remove);
		fu++;

		// Apply the face update records:
		pFaces[pFace->index].corner[corner] = VTXIDX(keep);
		pFace = (Face*)updated->Next(SetCtx);
	}
	return fu;
}



/**
This Method attempts to repair texture coordinates (and colored verts) in the updated face list
so that textures don't jump much as a result of the contraction.   The method
loops over all of the updated faces.  The inner loop walks over all of the
deleted faces.   If the updated face has the same material ID as the deleted
face, AND if those two faces had the same texture coodinate at the remove
vertex, then the updated face will receive the texture coordinate of the
deleted face at the keep vertex.
**/

// this thing fills in the update records for the diffuse and specular vertex colors
// and all eight texture layers using method described above.
UPTR ContractionRecorder::recordAttributesInFaceUpdates(FacePtrSet* deleted, SmallPtrSet* updated,
													   IFXAuthorFaceUpdate *pFaceUpdate,
													   Vertex *keep, Vertex *remove)
{
	IFXAuthorFaceUpdate *pFirstUpdate = pFaceUpdate;
	U32 i, numLayers, layer, DelMatID, UpMatID, upFaceIndex;
	Vertex *Ua,*Ub,*Uc,*Da,*Db,*Dc;
	U32 updatedCorner=(U32)-1, deletedKeepCorner=(U32)-1, deletedRemoveCorner=(U32)-1;
	BOOL allDone=FALSE, diffuseDone, specularDone, layerDone[IFX_MAX_TEXUNITS];
	IFXAuthorMaterial *pMat;

	// Loop over all the updated faces:
	U32 SetCtx = 0;
	Face* pUpdatedFace = (Face*) updated->Begin(SetCtx);

	while(pUpdatedFace)
	{
		upFaceIndex = pUpdatedFace->index;
		UpMatID = m_pMaterialFaces[upFaceIndex];
		pMat = &m_pMaterials[UpMatID];

		diffuseDone = FALSE;
		specularDone = FALSE;
		for(i=0; i<m_NumTexLayersUsed; i++) layerDone[i] = FALSE;

		// see if there are any attributes to update for this face.
		if( pMat->m_uNumTextureLayers  || pMat->m_uDiffuseColors || pMat->m_uSpecularColors )
		{

			Ua = pUpdatedFace->c->getCommonVertex(pUpdatedFace->a);
			Ub = pUpdatedFace->a->getCommonVertex(pUpdatedFace->b);
			Uc = pUpdatedFace->b->getCommonVertex(pUpdatedFace->c);

			U32 DeletedFacesCtx = 0;
			Face* pDeletedFace = (Face*)deleted->Begin(DeletedFacesCtx);
			// Loop over the deleted faces (often 2 of them):
			while(pDeletedFace)
			{
				U32 delFaceIndex = pDeletedFace->index;
				DelMatID = m_pMaterialFaces[delFaceIndex];

				if(DelMatID == UpMatID)
				{
					// Grab the vertices from the deleted face:
					Da = pDeletedFace->c->getCommonVertex(pDeletedFace->a);
					Db = pDeletedFace->a->getCommonVertex(pDeletedFace->b);
					Dc = pDeletedFace->b->getCommonVertex(pDeletedFace->c);

					// find corner index of the keep vertex on the deleted face
					if (Da == keep) deletedKeepCorner = 0;
					if (Db == keep) deletedKeepCorner = 1;
					if (Dc == keep) deletedKeepCorner = 2;

					if (Da == remove) deletedRemoveCorner = 0;
					if (Db == remove) deletedRemoveCorner = 1;
					if (Dc == remove) deletedRemoveCorner = 2;

					if (Ua == remove) updatedCorner = 0;
					if (Ub == remove) updatedCorner = 1;
					if (Uc == remove) updatedCorner = 2;

					IFXAuthorFaceUpdate::AttributeType attributeCode;

					numLayers = m_pMaterials[DelMatID].m_uNumTextureLayers;

					allDone = TRUE;
					for(layer = 0; layer < numLayers; layer++)
					{
						if(!layerDone[layer])
						{
							IFXAuthorFace *delFace = &m_TextureLayerFaces[layer][delFaceIndex];
							IFXAuthorFace *upFace  = &m_TextureLayerFaces[layer][upFaceIndex];
							attributeCode = (IFXAuthorFaceUpdate::AttributeType) 
								(IFXAuthorFaceUpdate::Tex0 + layer);

							layerDone[layer] = fillUpdateRecord(delFace, upFace, 
																upFaceIndex, attributeCode,
																deletedKeepCorner, deletedRemoveCorner, updatedCorner,
																pFaceUpdate);

							if (layerDone[layer])
								pFaceUpdate++;
							else
								allDone = FALSE;  // need to try next deleted face
						}
					}
					if(pMat->m_uDiffuseColors && !diffuseDone)
					{
						IFXAuthorFace *delFace = &m_pDiffuseFaces[delFaceIndex];
						IFXAuthorFace *upFace  = &m_pDiffuseFaces[upFaceIndex];
						attributeCode = IFXAuthorFaceUpdate::Diffuse;

						diffuseDone = fillUpdateRecord( delFace, upFace, upFaceIndex, attributeCode,
														deletedKeepCorner, deletedRemoveCorner, updatedCorner,
														pFaceUpdate);
						if (diffuseDone)
							pFaceUpdate++;
						else
							allDone = FALSE;
					}
					if(pMat->m_uSpecularColors && !specularDone)
					{
						IFXAuthorFace *delFace = &m_pSpecularFaces[delFaceIndex];
						IFXAuthorFace *upFace  = &m_pSpecularFaces[upFaceIndex];
						attributeCode = IFXAuthorFaceUpdate::Specular;
						specularDone = fillUpdateRecord(delFace, upFace, upFaceIndex, attributeCode,
														deletedKeepCorner, deletedRemoveCorner, updatedCorner,
														pFaceUpdate);

						if (specularDone)
							pFaceUpdate++;
						else
							allDone = FALSE;
					}


				}
				// every attribute was updated so don't check against the next 
				// deleted face, proceed with the next updated face:
				if (allDone) break;
				pDeletedFace = (Face*)deleted->Next(DeletedFacesCtx);
			}
		}
		pUpdatedFace = (Face*) updated->Next(SetCtx);
	}
	return (pFaceUpdate - pFirstUpdate);  // number of face updates
}

BOOL ContractionRecorder::fillUpdateRecord(IFXAuthorFace *delFace, IFXAuthorFace *upFace, U32 upFaceIndex,
										   IFXAuthorFaceUpdate::AttributeType attribute,
										   U32 deletedKeepCorner, U32 deletedRemoveCorner, U32 updatedCorner,
										   IFXAuthorFaceUpdate *pFaceUpdate)
{
	U32 attributeIndexDecr      = delFace->corner[deletedKeepCorner];
	U32 attributeIndexDelRemove = delFace->corner[deletedRemoveCorner];
	U32 attributeIndexIncr      = upFace->corner[updatedCorner];

	// Verify that the attributes between the del face and the updated face were continuous:
	// We'll only update the attributes if the faces were previously continous:
	if (attributeIndexIncr == attributeIndexDelRemove)
	{
		// Record the face update for the texcoords:
		pFaceUpdate->FaceIndex = upFaceIndex;
		pFaceUpdate->Attribute = attribute;
		pFaceUpdate->Corner    = updatedCorner;
		pFaceUpdate->DecrValue = attributeIndexDecr;
		pFaceUpdate->IncrValue = attributeIndexIncr;

		// Apply the update record to the face, keep author mesh in sync with current resolution
		upFace->corner[updatedCorner] = attributeIndexDecr;
		return TRUE;
	}
	return FALSE;
}


/** Create Update face update for Normal change
*/

U32 ContractionRecorder::recordNormalUpdate (U32 faceIndex,
											 U32 keepVertex,
											 U32 normalIndex,
											 IFXAuthorFaceUpdate *faceUpdate)
{
	IFXAuthorFace *pFaces, *face;
	outIMesh->GetPositionFaces(&pFaces);
	face = &pFaces[faceIndex];

	IFXAuthorFace *pNormalFaces, *faceAttrib;
	outIMesh->GetNormalFaces(&pNormalFaces);
	faceAttrib = &pNormalFaces[faceIndex];

	U32 decrNormal = normalIndex;
	U32 incrNormal;
	U32 corner = 0;

	// Determine the token and the increasing Normal index:
	if (face->VertexA() == keepVertex)
		corner = 0;
	else if (face->VertexB() == keepVertex)
		corner = 1;
	else if (face->VertexC() == keepVertex)
		corner = 2;

	incrNormal = faceAttrib->corner[corner];

	// No need to record a normal update if they are the same!:
	if (incrNormal == decrNormal)
		return 0;

	// Now record the update:
	faceUpdate->FaceIndex = faceIndex;
	faceUpdate->Attribute = IFXAuthorFaceUpdate::Normal;
	faceUpdate->Corner = corner;
	faceUpdate->DecrValue = decrNormal;
	faceUpdate->IncrValue = incrNormal;

	// Now apply the update record:
	faceAttrib->corner[corner] = decrNormal;

	return 1;
}


void ContractionRecorder::sortFaceExamList (U32 numFaces)
{
	U32 lastFe = 0;
	BOOL allFacesMarked = FALSE;

	memcpy (&sortedFeList->pFaceExam[lastFe], &feList->pFaceExam[0], sizeof (FaceExam));

	while (!allFacesMarked)
	{
		allFacesMarked = TRUE;
		FaceExam *unMarked = NULL;
		FaceExam *lastSortedFE = &sortedFeList->pFaceExam[lastFe];
		U32 f;
		for ( f = 1; f < numFaces; f++)
		{
			FaceExam *fe = &feList->pFaceExam[f];
			if (!fe->marked)
			{
				unMarked = fe;
				allFacesMarked = FALSE;
				if (adjacentFaceExams(lastSortedFE, fe))
				{
					lastFe++;
					memcpy (&sortedFeList->pFaceExam[lastFe], fe, sizeof (FaceExam));
					fe->marked = TRUE;
					break;
				}//if
			}//if
		}//for

		// In case no adjacency was discovered, grab the last unmarked face:
		if (!allFacesMarked && !unMarked->marked)
		{
			lastFe++;
			memcpy (&sortedFeList->pFaceExam[lastFe], unMarked, sizeof (FaceExam));
			unMarked->marked = TRUE;
		}

	}//while

	// Did we get all of the faces:
	IFXASSERT (numFaces == (lastFe+1));
}



void  ContractionRecorder::computeOtherVertices (SmallPtrSet* vertices,
												 SmallPtrSet* updated,
												 Vertex* keep)
{
	U32 SetCtx = 0;
	Face* pFace = (Face*) updated->Begin(SetCtx);
	while(pFace)
	{
		Vertex *v1, *v2;

		v1 = pFace->a->getv1();
		v2 = pFace->a->getv2();
		if (v1 != keep) vertices->Insert (v1);
		if (v2 != keep) vertices->Insert (v2);

		v1 = pFace->b->getv1();
		v2 = pFace->b->getv2();
		if (v1 != keep) vertices->Insert (v1);
		if (v2 != keep) vertices->Insert (v2);

		v1 = pFace->c->getv1();
		v2 = pFace->c->getv2();
		if (v1 != keep) vertices->Insert (v1);
		if (v2 != keep) vertices->Insert (v2);

		pFace = (Face*) updated->Next(SetCtx);
	}
}


#define MAX_NUM_NORMAL_UPDATES  200

U32 ContractionRecorder::recordNormalsInFaceUpdates_NormalNeighborhood(Vertex *keep,
																	   SmallPtrSet *updatedFaces,
																	   IFXAuthorFaceUpdate *faceUpdates,
																	   BOOL smallNormalChange)
{
	U32 usedFaceUpdates = 0;

	//For the keep vertex:
	SmallPtrSet kvFaces;
	keep->computeFaceSet (kvFaces);
	usedFaceUpdates += recordNormalsInFaceUpdates_CreaseAngle (keep, &kvFaces, normalsCreaseAngle, &faceUpdates[usedFaceUpdates]);

	// try to be more efficient, probably no need to update neighborhood when updated
	// faces didn't change much.

	// Compute the other vertices attached to the updated faces (The normal neighborhood):
	SmallPtrSet otherVerts;
	computeOtherVertices (&otherVerts, updatedFaces, keep);

	// Now record normals at other vertex sites:
	U32 SetCtx = 0;
	Vertex* pVert = (Vertex*)otherVerts.Begin(SetCtx);
	while(pVert)
	{
		SmallPtrSet faces;

		/// @todo: Slow!
		pVert->computeFaceSet (faces);

		// Make sure we don't exceed expected memory allocation:
		if ((usedFaceUpdates + faces.Size()) > numFaceUpdates)
			return usedFaceUpdates;

		usedFaceUpdates += recordNormalsInFaceUpdates_CreaseAngle (pVert, &faces, normalsCreaseAngle, &faceUpdates[usedFaceUpdates]);

		pVert = (Vertex*)otherVerts.Next(SetCtx);
	}
	return usedFaceUpdates;
}


U32 ContractionRecorder::recordNormalsInFaceUpdates_CreaseAngle(Vertex *keep,
																SmallPtrSet *faceSet,
																float tolerance,
																IFXAuthorFaceUpdate *faceUpdates)
{
	U32 numFaces = faceSet->Size();

	if (numFaces == 0)
		return 0;

	if (numFaces > MAX_NUM_FACEEXAMS)
	{
		IFXASSERT (numFaces > MAX_NUM_FACEEXAMS);
		numFaces = MAX_NUM_FACEEXAMS;
	}

	// 1) Initialize the FaceExamList:
	//int n = 0;
	U32 SetCtx = 0;
	Face* pFace = (Face*) faceSet->Begin(SetCtx);
	U32 fe;
	IFXAuthorFace *pFaces;
	outIMesh->GetPositionFaces(&pFaces);
	for (fe = 0; fe < numFaces; fe++)
	{
		FaceExam *faceExam = &feList->pFaceExam[fe];
		U32 index = pFace->index;
		IFXAuthorFace *face = &pFaces[index];

		faceExam->marked = FALSE;
		/// @todo: fix or eliminate smooth id stuff?
		faceExam->smoothID = 0;
		faceExam->faceIndex = index;
		if (!computeFaceNormal (face, (IFXVector3*) &faceExam->normal))
			faceExam->faceIndex = UNDEFINED_INDEX;

		pFace = (Face*)faceSet->Next(SetCtx);
	}

	sortFaceExamList (numFaces);

	// Walk all the adjacent faces, examining their crease angle:
	// We will mark faces that *begin a new crease angle group*:
	// We will also mark the first face to exceed crease angle:
	U32 firstFace = 0;
	U32 prevFaceIndex = numFaces - 1;
	U32 nextFaceIndex;
	for ( nextFaceIndex = 0; nextFaceIndex < numFaces; nextFaceIndex++)
	{
		FaceExam *prevFace = &sortedFeList->pFaceExam[prevFaceIndex];
		FaceExam *nextFace = &sortedFeList->pFaceExam[nextFaceIndex];

		// Skip over any faces marked (index = UNDEFINED_INDEX) as having 0 area:
		if ((prevFace->faceIndex != UNDEFINED_INDEX) && (nextFace->faceIndex != UNDEFINED_INDEX))

		{
			float ca = creaseAngle(prevFace, nextFace);

			IFXASSERT((ca < (1.0f + CA_EPSILON)));
			IFXASSERT((ca > (-1.0f - CA_EPSILON)));

			if (fabs(ca) < CA_EPSILON)
				ca = 0.0f;

			if (ca <= tolerance)
			{
				nextFace->marked = TRUE;
				if (!firstFace)
					firstFace = nextFaceIndex;
			}
		}
		prevFaceIndex = nextFaceIndex;
	}//for


	// Now walk all of the crease angle groups and compute normals:
	IV3D avgNormal = {0.0f, 0.0f, 0.0f};
	U32 numNormals = 0;
	U32 numFaceUpdates = 0;

	for (fe = firstFace; fe < firstFace + numFaces; fe++)
	{
		U32 index = fe % numFaces; // wrap around
		FaceExam *faceExam = &sortedFeList->pFaceExam[index];

		// Skip over any faces marked (index = UNDEFINED_INDEX) as having 0 area:
		if (faceExam->faceIndex != UNDEFINED_INDEX)
		{
			// If we are entering a new crease angle group, then write the normal updates
			// for the last crease angle group:
			if ((faceExam->marked) && (fe != firstFace)) // end of a crease angle group
			{
				// Compute the average normal and look it up in the normal map
				scalarMultiply3D(&avgNormal, 1.0f / (float) numNormals, &avgNormal);
				// Find a pre-existing normal which is closest:
				unsigned long normalIndex;
				float distance;
				nMap->nearest(&avgNormal, &normalIndex, &distance);

				// Now write all of the normal update records:
				U32 start = fe - numNormals;
				U32 i;
				for ( i = start; i < fe; i++)
				{
					U32 index = i % numFaces;
					FaceExam *thisFe = &sortedFeList->pFaceExam[index];
					if (thisFe->faceIndex != UNDEFINED_INDEX)
					{
						numFaceUpdates += recordNormalUpdate (thisFe->faceIndex,
							VTXIDX(keep), normalIndex,
							&faceUpdates[numFaceUpdates]);
					}
				}

				// Reset for the next crease angle group:
				avgNormal.x = 0.0f; avgNormal.y = 0.0f; avgNormal.z = 0.0f;
				numNormals = 0;
			}

			// Sum up the normals:
			add3D(&avgNormal, &faceExam->normal, &avgNormal);
			numNormals++;
		}
	}

	// The last crease angle group:
	if (numNormals)
	{
		// Compute the average normal and look it up in the normal map
		scalarMultiply3D(&avgNormal, 1.0f / (float) numNormals, &avgNormal);
		// Find a pre-existing normal which is closest:
		unsigned long normalIndex;
		float distance;
		nMap->nearest(&avgNormal, &normalIndex, &distance);

		// Now write all of the normal update records:
		U32 start = fe - numNormals;
		U32 i;
		for ( i = start; i < fe; i++)
		{
			U32 index = i % numFaces;
			FaceExam *thisFe = &sortedFeList->pFaceExam[index];

			// Don't bother updating normals in 0 area faces:
			if (thisFe->faceIndex != UNDEFINED_INDEX)
			{
				numFaceUpdates += recordNormalUpdate (thisFe->faceIndex,
					VTXIDX(keep), normalIndex,
					&faceUpdates[numFaceUpdates]);
			}
		}
	}


	return numFaceUpdates;
}


/*
* Create the update records to describe the vertex that was removed
*/
void ContractionRecorder::record(Vertex* keep, Vertex* remove,
								 FacePtrSet* deleted, SmallPtrSet* updated)
{
	numVertexRemovals++;

	// Record a mapping from old face index to new face index.
	U32 SetCtx = 0;
	Face* pFace = (Face*)deleted->Begin(SetCtx);
	// add code to order faces consistently
	while(pFace)
	{
		faceMap[pFace->index] = numFaceRemovals++;
		pFace = (Face*)deleted->Next(SetCtx);
	}

	// This is a little annoying...at this point we have not yet updated the topology.
	// (See pair::contract().)  But we need to know what faces will be resident on the
	// keep vertex once topology is updated to compute good normals for that vertex.
	// So...we'll figure it out here.  Note the kvFaceSet may be useful elsewhere; if so
	// I should investigate computing it in Pair::NormalFlips() and keeping it as a
	// static temporary inside of Pair.  At the moment, it's only needed here:

	/// @todo: preallocate Sets
	SmallPtrSet kvWithDelFaces;
	SmallPtrSet kvWithOutDelFaces;
	SmallPtrSet kvFaceSet;
	keep->computeFaceSet(kvWithDelFaces);
	SmallPtrSet_Difference (&kvWithDelFaces, deleted, &kvWithOutDelFaces);
	SmallPtrSet_Union (&kvWithOutDelFaces, updated, &kvFaceSet);


	// Grow the face update pool if we are seeing huge numbers of faces to update.
	// For each updated face need to update position,texture, normal + MAX_NUM_NORMAL_UPDATES for
	// normals in the surrounding neighbor hood.
	if( ( (updated->Size()*3) + MAX_NUM_NORMAL_UPDATES) > numFaceUpdates)
	{
		delete [] pFaceUpdatePool;
		numFaceUpdates = (updated->Size()*3)+MAX_NUM_NORMAL_UPDATES;
		pFaceUpdatePool = new IFXAuthorFaceUpdate[numFaceUpdates];
	}

	// Store the contraction into a VertexUpdate record:
	m_pRemoveIndex[currVertexUpdate] = VTXIDX(remove);
	vertexUpdate = &pCLODUpdates[currVertexUpdate--];

	vertexUpdate->NumNewFaces = deleted->Size();
	usedFaceUpdates = 0;

	// Record face updates requiring vertex attribute changes:
	// Note that this method will also update the outIMesh topology:
	usedFaceUpdates += recordVerticesInFaceUpdates(updated, &pFaceUpdatePool[usedFaceUpdates], keep, remove);

	if (m_hasTexturesOrColors)
	{
		// Record face updates requiring texture or vert color attribute changes:
		usedFaceUpdates += recordAttributesInFaceUpdates(deleted, updated, 
			&pFaceUpdatePool[usedFaceUpdates], keep, remove);
	}

	vertexUpdate->NumFaceUpdates = usedFaceUpdates; // Updates so far...more may be added by recordAttribChanges()...
	IFXASSERT (usedFaceUpdates <= numFaceUpdates);
}



void ContractionRecorder::recordAttribChanges(Vertex *keep,  SmallPtrSet *updated, BOOL smallNormalChange)
{
	if (normalsMode == CLODGenerator::TrackSurfaceChanges)
		usedFaceUpdates += recordNormalsInFaceUpdates_NormalNeighborhood (keep, updated, 
		&pFaceUpdatePool[usedFaceUpdates], smallNormalChange);

	vertexUpdate->NumFaceUpdates = usedFaceUpdates;
	// now that we know actual number of updates do the allocation.
	vertexUpdate->pFaceUpdates = new IFXAuthorFaceUpdate[usedFaceUpdates]; 

	// copy from the working pool into the actual face updates
	memcpy(vertexUpdate->pFaceUpdates, pFaceUpdatePool, sizeof(IFXAuthorFaceUpdate) * usedFaceUpdates);

	IFXASSERT (usedFaceUpdates <= numFaceUpdates);
}


void ContractionRecorder::generateVertexMap()
{
	// Memory for this array was allocated in ContractionRecorder::ContractionRecorder();

	// Initialize all map entries to undefined:
	U32 nv = outIMesh->GetMaxMeshDesc()->NumPositions;
	U32 i;
	for ( i = 0 ; i < nv; i++)
		vertexMap[i] = UNDEFINED_INDEX;

	// Now walk all of the vertex update records to discover the new order:
	// Note that any unused old vertices will map to UNDEFINED_INDEX when finished.
	//      Also be aware that if CLODGen removed any duplicated vertices, then there
	//      will be some unused update records are the front of the update record
	//      array.  The calculation for recordIndex makes sure we don't try to parse
	//      such records.

	U32 v;
	for ( v = 0; v < numVertexRemovals; v++)
	{
		// Only look at _used_ update records:
		unsigned long recordIndex = nv - (numVertexRemovals - v);
		unsigned long newIndex = v;
		unsigned long oldIndex = m_pRemoveIndex[recordIndex];

		// Make sure the update records are not goofed:
		IFXASSERT (oldIndex != UNDEFINED_INDEX);

		vertexMap[oldIndex] = newIndex;
	}
}



void ContractionRecorder::generateFaceMap()
{
	// The face map is actually generated in record, but it's generated backwards.
	// Here we just reverse it:
	U32 maxFaceIndex = numFaceRemovals - 1;

	U32 nf = outIMesh->GetMaxMeshDesc()->NumFaces;
	U32 i;
	for ( i = 0; i < nf; i++)
	{
		if (faceMap[i] != UNDEFINED_INDEX)
			faceMap[i] = maxFaceIndex - faceMap[i];
	}
}

// reorders the color arrays and returns a map
U32* ContractionRecorder::reOrderColors(IFXAuthorFaceUpdate::AttributeType type)
{
	U32 numOrigColors;
	IFXAuthorFace* pFaces;
	U32 *map;

	if(type == IFXAuthorFaceUpdate::Diffuse)
	{
		outIMesh->GetDiffuseFaces(&pFaces);
		numOrigColors = outIMesh->GetMaxMeshDesc()->NumDiffuseColors;
		map = m_pParams->pMeshMap->GetDiffuseMap();
	}
	else
	{
		outIMesh->GetSpecularFaces(&pFaces);
		numOrigColors = outIMesh->GetMaxMeshDesc()->NumSpecularColors;
		map = m_pParams->pMeshMap->GetSpecularMap();
	}

	U32 numColors = 0, numFaces = 0, index;
	U32 i, f, v, corner;

	// Initialize the map:
	for (i = 0; i < numOrigColors; i++)
		map[i] = IFX_BAD_INDEX32;

	// Now look for attribute in resolution order:
	for (v = 0; v < numVertexRemovals; v++)
	{
		// Look in new faces, faces have allready been re-ordered at this point.
		IFXAuthorVertexUpdate *vu = &pCLODUpdates[v];
		int numNew = 0;
		for (f = 0; f < vu->NumNewFaces; f++)
		{
			int newFaceIndex = numFaces + f;
			IFXAuthorFace *faceAttrib = &pFaces[newFaceIndex];

			for(corner = 0; corner<3; corner++)
			{
				index = faceAttrib->corner[corner];
				if (map[index] == IFX_BAD_INDEX32)
				{
					map[index] = numColors++;
					numNew++;
				}
				faceAttrib->corner[corner] = map[index];
			}
		}

		numFaces += vu->NumNewFaces;

		// Look for colors in the face update records:
		for (f = 0; f < vu->NumFaceUpdates; f++)
		{
			IFXAuthorFaceUpdate *fu = &vu->pFaceUpdates[f];
			if (fu->Attribute == type)
			{
				// Decreasing normals (should already be in the map):
				index = fu->DecrValue;
				if (map[index] == IFX_BAD_INDEX32)
				{
					map[index] = numColors++;
					numNew++;
				}
				fu->DecrValue = map[index];

				// Increasing normals:
				index = fu->IncrValue;
				if (map[index] == IFX_BAD_INDEX32)
				{
					map[index] = numColors++;
					numNew++;
				}
				fu->IncrValue = map[index];
			}
		} // for num face updates

		// Record the number of new x that result from this vertex update record:
		if(type == IFXAuthorFaceUpdate::Diffuse)
			vu->NumNewDiffuseColors = numNew;
		else
			vu->NumNewSpecularColors = numNew;
	} // for num vertics

	// Now re-write the colors in the new order:
	IFXVector4 *pNew = new IFXVector4[numColors];
	U32 numOrig, newLocation, n;
	IFXVector4* pOrig;

	if(type == IFXAuthorFaceUpdate::Diffuse)
	{
		numOrig = outIMesh->GetMaxMeshDesc()->NumDiffuseColors;
		outIMesh->GetDiffuseColors(&pOrig);
	}
	else
	{
		numOrig = outIMesh->GetMaxMeshDesc()->NumSpecularColors;
		outIMesh->GetSpecularColors(&pOrig);
	}

	for (n = 0; n < numOrig; n++)
		if ( (newLocation = map[n]) != IFX_BAD_INDEX32)
			pNew[newLocation] = pOrig[n];

	// Load re-order normals into mesh and delete old normals:
	if(type == IFXAuthorFaceUpdate::Diffuse)
		outIMesh->SetDiffuseColorArray(pNew, numColors);
	else
		outIMesh->SetSpecularColorArray(pNew, numColors);

	return map;
}


// Generates a normal re-order map and applies it to the update records.
void ContractionRecorder::reOrderNormals()
{
	U32 nn = outIMesh->GetMaxMeshDesc()->NumNormals;
	U32 *normalMap = m_pParams->pMeshMap->GetNormalMap();
	U32 numNormals = 0;
	U32 numFaces = 0;
	U32 ni;
	U32 corner;

	IFXAuthorFace *pNormalFaces;
	outIMesh->GetNormalFaces(&pNormalFaces);

	// Initialize the map:
	U32 i;
	for ( i = 0; i < nn; i++)
		normalMap[i] = (U32)-1;

	// Now look for normals in resolution order:
	U32 v;
	for ( v = 0; v < numVertexRemovals; v++)
	{
		// Look in new faces:
		IFXAuthorVertexUpdate *vu = &pCLODUpdates[v];
		int numNewNormals = 0;
		int f;
		for (f = 0; f < vu->NumNewFaces; f++)
		{
			int newFaceIndex = numFaces + f;
			IFXAuthorFace *faceAttrib = &pNormalFaces[newFaceIndex];

			for(corner = 0; corner<3; corner++)
			{
				ni = faceAttrib->corner[corner];
				if (normalMap[ni] == (U32)-1)
				{
					normalMap[ni] = numNormals++;
					numNewNormals++;
				}
				faceAttrib->corner[corner] = normalMap[ni];
			}
		}

		numFaces += vu->NumNewFaces;

		// Look for normals in the face update records:
		for (f = 0; f < vu->NumFaceUpdates; f++)
		{
			IFXAuthorFaceUpdate *fu = &vu->pFaceUpdates[f];
			if (fu->Attribute == IFXAuthorFaceUpdate::Normal)
			{
				// Decreasing normals (should already be in the normalMap):
				ni = fu->DecrValue;
				if (normalMap[ni] == (U32)-1)
				{
					normalMap[ni] = numNormals++;
					numNewNormals++;
				}
				fu->DecrValue = normalMap[ni];

				// Increasing normals:
				ni = fu->IncrValue;
				if (normalMap[ni] == (U32)-1)
				{
					normalMap[ni] = numNormals++;
					numNewNormals++;
				}
				fu->IncrValue = normalMap[ni];
			}
		} // for num face updates

		// Record the number of new normals that result from this vertex update record:
		vu->NumNewNormals = numNewNormals;
	} // for num vertics

	// Now re-write the normals in the new order:
	IFXVector3 *pNewNormals = new IFXVector3[numNormals];
	int newLocation;
	U32 numOrigNormals = outIMesh->GetMaxMeshDesc()->NumNormals;
	IFXVector3 *pOrigNormals;
	outIMesh->GetNormals(&pOrigNormals);

	U32 n;
	for ( n = 0; n < numOrigNormals; n++)
		if ((newLocation = normalMap[n]) != -1)
			pNewNormals[newLocation] = pOrigNormals[n];

	// Load re-order normals into imesh and delete old normals:
	outIMesh->SetNormalArray(pNewNormals, numNormals);
}

// Generates a texcoord re-order map and applies it to the update records.
void ContractionRecorder::reOrderTexCoords()
{
	U32 numInputTexCoords = outIMesh->GetMaxMeshDesc()->NumTexCoords;
	U32 *texCoordMap = m_pParams->pMeshMap->GetTextureMap();
	U32 numTexCoords = 0;
	U32 numFaces = 0;
	U32 ni;
	U32 numLayers, layer, corner;

	// Initialize the map:
	U32 i;
	for ( i = 0; i < numInputTexCoords; i++)
		texCoordMap[i] = (U32)-1;

	// Now look for texcoords in resolution order:
	U32 v;
	for ( v = 0; v < numVertexRemovals; v++)
	{
		// Look in new faces:
		IFXAuthorVertexUpdate *vu = &pCLODUpdates[v];
		U32 numNewTexCoords = 0;
		U32 f;
		for (f = 0; f < vu->NumNewFaces; f++)
		{
			U32 newFaceIndex = numFaces + f;
			numLayers = m_pMaterials[ m_pMaterialFaces[newFaceIndex] ].m_uNumTextureLayers;
			for(layer=0; layer<numLayers; layer++)
			{
				IFXAuthorFace *texFace = &m_TextureLayerFaces[layer][newFaceIndex];
				for(corner=0; corner<3; corner++)
				{
					ni = texFace->corner[corner];
					if(ni != IFX_BAD_INDEX32)
					{
						if (texCoordMap[ni] == (U32)-1)
						{
							texCoordMap[ni] = numTexCoords++;
							numNewTexCoords++;
						}
						texFace->corner[corner] = texCoordMap[ni];
					}
				}
			}
		}

		numFaces += vu->NumNewFaces;

		// Look for texcoords in the face update records:
		for (f = 0; f < vu->NumFaceUpdates; f++)
		{
			IFXAuthorFaceUpdate *fu = &vu->pFaceUpdates[f];
			if (fu->Attribute >= IFXAuthorFaceUpdate::Tex0)
			{
				// Decreasing texCoords (should already be in the texcoordMap):
				ni = fu->DecrValue;
				if (texCoordMap[ni] == (U32)-1)
				{
					texCoordMap[ni] = numTexCoords++;
					numNewTexCoords++;
				}
				fu->DecrValue = texCoordMap[ni];

				// Increasing texcoords:
				ni = fu->IncrValue;
				if (texCoordMap[ni] == (U32)-1)
				{
					texCoordMap[ni] = numTexCoords++;
					numNewTexCoords++;
				}
				fu->IncrValue = texCoordMap[ni];
			}
		}
		// Record the number of new texture coordinates that are produce by this vertex update record:
		vu->NumNewTexCoords = numNewTexCoords;
	}

	// Now re-write the texCoords in the new order:
	IFXVector4 *pNewTexCoords = new IFXVector4[numTexCoords];
	U32 newLocation;
	IFXVector4 *pOrigTexCoords;
	outIMesh->GetTexCoords(&pOrigTexCoords);

	U32 n;
	for ( n = 0; n < numInputTexCoords; n++)
		if ((newLocation = texCoordMap[n]) != (U32)-1)
			pNewTexCoords[newLocation] = pOrigTexCoords[n];

	// Load re-order texcoords into imesh:
	outIMesh->SetTexCoordArray(pNewTexCoords,numTexCoords);
}


void ContractionRecorder::reOrderIndices()
{
	// Allocate memory for new vertex list, new face list and new face mats list:
	IFXVector3* pNewVerts = NULL;
	IFXAuthorFace* pNewFaces = NULL;
	U32* pNewFaceMats = NULL;
	
	if( 0 != numVertexRemovals )
		pNewVerts = new IFXVector3[numVertexRemovals];

	if( 0 != numFaceRemovals )
	{
		pNewFaces = new IFXAuthorFace[numFaceRemovals];
		pNewFaceMats = new U32[numFaceRemovals];
	}

	// Allocate temporary re-order arrays for the attributes:
	IFXAuthorFace* pNewNormalFaces = NULL;
	IFXAuthorFace* pNewDiffuseFaces = NULL;
	IFXAuthorFace* pNewSpecularFaces = NULL;

	if(outIMesh->GetMaxMeshDesc()->NumNormals)
		pNewNormalFaces = new IFXAuthorFace[numFaceRemovals];

	if(outIMesh->GetMaxMeshDesc()->NumDiffuseColors)
		pNewDiffuseFaces = new IFXAuthorFace[numFaceRemovals];

	if(outIMesh->GetMaxMeshDesc()->NumSpecularColors)
		pNewSpecularFaces = new IFXAuthorFace[numFaceRemovals];

	IFXAuthorFace** pNewTexFaces = NULL;
	U32 layer;

	if( 0 != m_NumTexLayersUsed )
	{
		pNewTexFaces = new IFXAuthorFace*[m_NumTexLayersUsed];

		if( NULL != pNewTexFaces && 0 != numFaceRemovals )
		{
			for(layer=0; layer<m_NumTexLayersUsed; layer++)
				pNewTexFaces[layer] = new IFXAuthorFace[numFaceRemovals];
		}
	}

	// 1) Generate the re-order maps:
	generateVertexMap();

	IFXDELETE_ARRAY( m_pRemoveIndex );

	generateFaceMap();

	// 2) Re-order the vertex data:
	U32 numOrigVertices = outIMesh->GetMaxMeshDesc()->NumPositions;
	U32 i;
	for ( i = 0; i < numOrigVertices; i++)
	{
		unsigned long newindex = vertexMap[i];
		unsigned long oldindex = i;

		// Don't include any vertices in new order that have not been re-mapped.
		// (These may perhaps be unconnected verts in orig mesh)
		if (newindex != UNDEFINED_INDEX)
		{
			pNewVerts[newindex] = vpcVertices[oldindex].v; // we want the last contraction at the front of the array.
		}
	}

	// 3) Re-order the face data:.
	U32 *pOrigFaceMats, numOrigFaces = outIMesh->GetMaxMeshDesc()->NumFaces;
	IFXAuthorFace *pOrigFaces, *pOrigNormalFaces=NULL, *pOrigDiffuseFaces=NULL, *pOrigSpecularFaces=NULL;

	outIMesh->GetPositionFaces(&pOrigFaces);
	outIMesh->GetFaceMaterials(&pOrigFaceMats);
	outIMesh->GetNormalFaces(&pOrigNormalFaces);
	outIMesh->GetDiffuseFaces(&pOrigDiffuseFaces);
	outIMesh->GetSpecularFaces(&pOrigSpecularFaces);

	U32 j;
	for ( j = 0; j < numOrigFaces; j++)
	{
		U32 newindex = faceMap[j];
		U32 oldindex = j;

		// Don't include any face in new order that have not been re-mapped.
		// (These may perhaps be 0 area faces in orig mesh)
		if (newindex != UNDEFINED_INDEX)
		{
			IFXAuthorFace *f = &pNewFaces[newindex];
			IFXAuthorFace *oldFace = &pOrigFaces[oldindex];

			U32 olda = oldFace->VertexA();
			U32 oldb = oldFace->VertexB();
			U32 oldc = oldFace->VertexC();

			// Here we re-map the position indices of the old faces:
			f->VertexA() = vertexMap[olda];
			f->VertexB() = vertexMap[oldb];
			f->VertexC() = vertexMap[oldc];

			// Here we copy across the attribs
			pNewFaceMats[newindex]          = pOrigFaceMats[oldindex];
			if(pOrigNormalFaces)
				pNewNormalFaces[newindex]   = pOrigNormalFaces[oldindex];
			if(pOrigDiffuseFaces)
				pNewDiffuseFaces[newindex]  = pOrigDiffuseFaces[oldindex];
			if(pOrigSpecularFaces)
				pNewSpecularFaces[newindex] = pOrigSpecularFaces[oldindex];

			U32 layer;
			for( layer =0; layer<m_NumTexLayersUsed; layer++)
				pNewTexFaces[layer][newindex] = m_TextureLayerFaces[layer][oldindex];
		}
	}


	// 4.0) Some of the allocated vertex update records may not have been used.
	// This could happen if the original mesh contained unconnected vertices.
	// So, we'll bump the pCLODupdates pointer past any unused records. We need
	// tell the results structure about this, so that it will correct the ptr
	// when it's time to delete the vertex update records:
	U32 offSet = currVertexUpdate + 1;
	if(offSet)
	{
		IFXAuthorVertexUpdate* pTempUpdates = NULL;

		if( 0 != numVertexRemovals )
		{
			pTempUpdates = new IFXAuthorVertexUpdate[numVertexRemovals];
			// this will copy the update record and null out the pointer to the face updates so that
			// they don't get deleted by the destructor.
			U32 idx;
			for( idx = offSet; idx < numVertexRemovals + offSet; idx++)
				pCLODUpdates[idx].CopyTo(&pTempUpdates[idx-offSet]);

			IFXDELETE_ARRAY( pCLODUpdates );
			pCLODUpdates = pTempUpdates;
		}
	}


	// 4) Correct all of the indices in the VertexUpdate and FaceUpdate records:
	U32 vu;
	for ( vu = 0; vu < numVertexRemovals; vu++)
	{
		IFXAuthorVertexUpdate *vertexUpdate = &pCLODUpdates[vu];

		U32 newIndex;
		IFXAuthorFaceUpdate *faceUpdates = vertexUpdate->pFaceUpdates;
		U32 numFaceUpdates = vertexUpdate->NumFaceUpdates;

		// Fix up pointers in each FaceUpdate record
		U32 fu;
		for ( fu = 0; fu < numFaceUpdates; fu++)
		{
			// Use the faceMap to fix up faceIndex
			newIndex = faceMap[faceUpdates[fu].FaceIndex];
			faceUpdates[fu].FaceIndex = newIndex;

			// Use the vertexMap to fix up the vertex values
			if (faceUpdates[fu].Attribute == IFXAuthorFaceUpdate::Position)
			{
				U32 oldIndex = faceUpdates[fu].DecrValue;
				newIndex = vertexMap[oldIndex];
				faceUpdates[fu].DecrValue = newIndex;

				oldIndex = faceUpdates[fu].IncrValue;
				newIndex = vertexMap[oldIndex];
				faceUpdates[fu].IncrValue = newIndex;
			}
		}
	}

	// 5) Fix up data arrays in outImesh:
	outIMesh->SetPositionArray(pNewVerts, numVertexRemovals);
	outIMesh->SetPositionFaces(pNewFaces);
	outIMesh->SetFaceMaterials(pNewFaceMats);

	m_pMaterialFaces = pNewFaceMats;
	outIMesh->SetNormalFaces(pNewNormalFaces);
	outIMesh->SetDiffuseFaces(pNewDiffuseFaces);
	outIMesh->SetSpecularFaces(pNewSpecularFaces);
	for(layer=0; layer<m_NumTexLayersUsed; layer++)
	{
		outIMesh->SetTexFaces(layer,pNewTexFaces[layer]);
		m_TextureLayerFaces[layer] = pNewTexFaces[layer];
	}

	IFXDELETE_ARRAY( pNewTexFaces );

	outIMesh->SetMaxNumFaces(numFaceRemovals);

	// Re-order normals and texcoords if we use them and if they exist:
	if ((normalsMode > CLODGenerator::NoUpdates))
		reOrderNormals();
	if (outIMesh->GetMaxMeshDesc()->NumTexCoords > 0)
		reOrderTexCoords();
	if (outIMesh->GetMaxMeshDesc()->NumDiffuseColors > 0)
		reOrderColors(IFXAuthorFaceUpdate::Diffuse);
	if (outIMesh->GetMaxMeshDesc()->NumSpecularColors > 0)
		reOrderColors(IFXAuthorFaceUpdate::Specular);

	// Apply the update records once to set the imesh to maximum resolution:
	IFXAuthorMeshDesc d = *outIMesh->GetMeshDesc();
	d.NumFaces = 0;
	d.NumPositions = 0;
	d.NumNormals = 0;
	d.NumTexCoords = 0;
	d.NumDiffuseColors = 0;
	d.NumSpecularColors = 0;
	outIMesh->SetMeshDesc(&d);
	outIMesh->SetUpdates(pCLODUpdates);

	outIMesh->SetMinResolution(0);
	outIMesh->SetMaxResolution(numVertexRemovals);
	outIMesh->SetFinalMaxResolution(numVertexRemovals);
	outIMesh->Unlock();  // need to unlock for setresolution to work
	outIMesh->SetResolution(numVertexRemovals);
}
