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
#ifndef CONTRACTIONRECORDER_DOT_H
#define CONTRACTIONRECORDER_DOT_H
#include <limits.h>
#include "FaceExam.h"
#include "NormalMap.h"
#include "Pair.h"
#include "Vertex.h"
#include "Face.h"
#include "SmallPtrSet.h"
#include "CLODGenerator.h"
#include "IFXEnums.h"

#define UNDEFINED_INDEX U32_MAX

class ContractionRecorder
{	
private:
	Vertex *vpcVertices;	///< pointer to the VPC's vertex array;
	Face *vpcFaces;		///< pointer to the VPC's face array.

	// Ptrs to the re-order maps:
	U32 numVertexRemovals;	///< Num vertex update records also.
	U32 numFaceRemovals;
	U32 *faceMap;
	U32 *vertexMap;
	U32 *m_pRemoveIndex;  

	// This is where we store the contractions:
	U32				currVertexUpdate;
	IFXAuthorVertexUpdate *pCLODUpdates;
	IFXAuthorCLODAccess *outIMesh;

	// We need these to compute optimal normals:
	FaceExamList *feList;
	FaceExamList *sortedFeList;
	NormalMap	 *nMap;

	// Shared by record and record vertex attribs
	IFXAuthorVertexUpdate *vertexUpdate;
	
	U32 numFaceUpdates;			  ///< max number of updates in the pFaceUpdatePool.
	U32 usedFaceUpdates;          ///< keeps track of the number of face updates used in pool.
	IFXAuthorFaceUpdate *pFaceUpdatePool;  ///< working array of face updates.
	float *meshDamage; ///< array of floats to record mesh damage for each vertex removal.

	// Other Normal support:
	CLODGenerator::NormalsMode normalsMode;
	float normalsCreaseAngle;

	// Direct pointers to author mesh to avoid function overhead.
	IFXAuthorFace* m_TextureLayerFaces[IFX_MAX_TEXUNITS];
	IFXAuthorFace* m_pDiffuseFaces;
	IFXAuthorFace* m_pSpecularFaces;
	U32 *m_pMaterialFaces;
	IFXAuthorMaterial *m_pMaterials;
	U32 m_NumTexLayersUsed;  ///< max number of layers used by any material in the this mesh.
	BOOL m_hasTexturesOrColors;  ///< true if mesh has any vert colors or textures.
	CLODGenerator::Params *m_pParams;
	
	// Record implementation functions:
	U32 recordVerticesInFaceUpdates(SmallPtrSet* updated, IFXAuthorFaceUpdate *faceUpdates, 
									Vertex* keep, Vertex* remove);
	U32 recordTexCoordsInFaceUpdates(FacePtrSet* deleted, SmallPtrSet* updated, 
									IFXAuthorFaceUpdate *faceUpdates,
									Vertex* keep, Vertex* remove);
	UPTR recordAttributesInFaceUpdates(FacePtrSet* deleted, SmallPtrSet* updated, 
									  IFXAuthorFaceUpdate *pFaceUpdate,
									  Vertex *keep, Vertex *remove);

	BOOL fillUpdateRecord(IFXAuthorFace *delFace, IFXAuthorFace *upFace, U32 upFaceIndex,
					 IFXAuthorFaceUpdate::AttributeType attribute,
					 U32 deletedKeepCorner, U32 deletedRemoveCorner, U32 updatedCorner,
				     IFXAuthorFaceUpdate *pFaceUpdate);

	U32 recordNormalsInFaceUpdates(Vertex* keep, SmallPtrSet* faceSet, 
									IFXAuthorFaceUpdate *faceUpdates);
	U32 recordNormalUpdate (U32 faceIndex, U32 keepVertex, 
									U32 normalIndex, IFXAuthorFaceUpdate *faceUpdate);
	inline BOOL computeFaceNormal (IFXAuthorFace *face, IFXVector3 *normal);
	inline BOOL texCoordsAreSame (U32 texCoord1, U32 texCoord2);

	// Re-order implementation methods:
	void generateVertexMap();
	void generateFaceMap();
	void reOrderNormals();
	void reOrderTexCoords();
	U32* reOrderColors(IFXAuthorFaceUpdate::AttributeType type);

	// Normal generation methods:
	void computeOtherVertices (SmallPtrSet* vertices, SmallPtrSet* updated, Vertex* keep);

	U32 recordNormalsInFaceUpdates_NormalNeighborhood(Vertex* keep,  SmallPtrSet* updatedFaces, 
									IFXAuthorFaceUpdate *faceUpdates, BOOL smallNormalChange);

	inline BOOL adjacentFaceExams(FaceExam *fe1, FaceExam *fe2);
	void sortFaceExamList (U32 numFaces);
	inline float creaseAngle (FaceExam *fe1, FaceExam *fe2);
	U32 recordNormalsInFaceUpdates_CreaseAngle(Vertex* keep, SmallPtrSet* faceSet, float tolerance, 
									IFXAuthorFaceUpdate *faceUpdates);
	void computePerVertexNormals ();
	
public:
	ContractionRecorder(CLODGenerator::Params *pParams);
	~ContractionRecorder();
	
	void init( CLODGenerator::Params *pParams, Vertex* v, Face* f);
	void record(Vertex* keep, Vertex* remain, FacePtrSet* deletedFaces, SmallPtrSet* updatedFaces);
	void recordAttribChanges(Vertex* keep,  SmallPtrSet* updated, BOOL smallNormalChange);
	void reOrderIndices();

	IFXAuthorVertexUpdate* GetUpdates( ) { return pCLODUpdates; };

};


inline BOOL ContractionRecorder::adjacentFaceExams(FaceExam *fe1, FaceExam *fe2)
{
	// We'll return FALSE (not adjacent) for faces marked as 0 area:
	if ((fe1->faceIndex == UNDEFINED_INDEX) || (fe2->faceIndex == UNDEFINED_INDEX))
		return FALSE;

	// Check adjacency by checking for shared vertices:
	IFXAuthorFace face1;
	outIMesh->GetPositionFace(fe1->faceIndex, &face1);
	IFXAuthorFace face2;
	outIMesh->GetPositionFace(fe2->faceIndex, &face2);
	
	U32 numSharedVerts = 0;
	U32& face1VertexA = face1.VertexA();
	U32& face1VertexB = face1.VertexB();
	U32& face1VertexC = face1.VertexC();
	U32& face2VertexA = face2.VertexA();
	U32& face2VertexB = face2.VertexB();
	U32& face2VertexC = face2.VertexC();

	if ((face1VertexA == face2VertexA) || (face1VertexA == face2VertexB) || (face1VertexA  == face2VertexC))
		numSharedVerts++;
	if ((face1VertexB == face2VertexA) || (face1VertexB == face2VertexB) || (face1VertexB  == face2VertexC))
		numSharedVerts++;
	if ((face1VertexC == face2VertexA) || (face1VertexC == face2VertexB) || (face1VertexC  == face2VertexC))
		numSharedVerts++;

	if (numSharedVerts == 2)
		return TRUE;
	else
		return FALSE;
}


inline float ContractionRecorder::creaseAngle (FaceExam *fe1, FaceExam *fe2)
{
	return dotProduct3D(&fe1->normal, &fe2->normal);
}


inline BOOL ContractionRecorder::computeFaceNormal (IFXAuthorFace *face, IFXVector3 *normal)
{
	IFXVector3 *pPositions, *v1,*v2,*v3;
	outIMesh->GetPositions(&pPositions);
	v1 = &pPositions[face->VertexA()];
	v2 = &pPositions[face->VertexB()];
	v3 = &pPositions[face->VertexC()];
	
	// Compute the normal:
	IFXVector3 u, v;
	u.Subtract(*v2, *v1);
	v.Subtract(*v3, *v1);
	normal->CrossProduct(u,v);

	// Test for 0 area face:
	if (((u.X() == v.X()) && (u.Y() == v.Y()) && (u.Z() == v.Z()))
		|| ((normal->X() == 0.0f) && (normal->Y() == 0.0f) && (normal->Z() == 0.0f)))
	{
		return FALSE;
	}
	normal->Normalize();
	IFXASSERT(normal->CalcMagnitude() > 0.0f);
	return TRUE;
}


inline BOOL ContractionRecorder::texCoordsAreSame (U32 texCoord1, U32 texCoord2)
{
	IFXVector4 tx1, tx2;
	outIMesh->GetTexCoord(texCoord1,&tx1);
	outIMesh->GetTexCoord(texCoord2,&tx2);

	return (tx1 == tx2);
}

#endif
