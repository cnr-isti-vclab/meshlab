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
//
//	CIFXBoundUtil.cpp
//
//	DESCRIPTION
//
//		Source file for class CIFXBoundUtil.
//
//	NOTES
//      
//
//*****************************************************************************

//*****************************************************************************
//	Includes
//***************************************************************************** 
#include "CIFXBoundUtil.h"
#include "IFXMesh.h"

//*****************************************************************************
//	Defines
//***************************************************************************** 
#define SCALEFACTOR 0.3333333f

//*****************************************************************************
//	Constants
//***************************************************************************** 

//*****************************************************************************
//	Enumerations
//*****************************************************************************

//*****************************************************************************
//	Global data
//*****************************************************************************

//*****************************************************************************
//	Local data
//*****************************************************************************

//*****************************************************************************
//	Classes, structures and types
//*****************************************************************************

//-----------------------------------------------------------------------------
// CIFXBoundUtil::ComputeLongestAxis
//
// Retrieves face and vertex information from the input IFXMeshGroup and
// initializes the internal bounding volume face and vertex lists
//-----------------------------------------------------------------------------

U32 CIFXBoundUtil::ComputeLongestAxis(IFXVector3 *pMin, IFXVector3 *pMax)
{
	// Sort Edge Length - HIGH to LOW
	//

	F32 fLength[3];

	fLength[0] = fabsf(pMin->X() - pMax->X());
	fLength[1] = fabsf(pMin->Y() - pMax->Y());
	fLength[2] = fabsf(pMin->Z() - pMax->Z());

	U32 uAxisIndex = 0;

	if( fLength[1] > fLength[uAxisIndex] ) uAxisIndex = 1;
	if( fLength[2] > fLength[uAxisIndex] ) uAxisIndex = 2;

	return uAxisIndex;
}


//-----------------------------------------------------------------------------
// CIFXBoundUtil::ComputeTriangleArea
//
// Compute the area of an arbitrary triangle
//
// Notes:
//
//    A = 0.5 * ||(q - p) CP (r - p)||
//
//    where: p, q, and r represent the vertices of the triangle.
// 
//-----------------------------------------------------------------------------

IFXRESULT CIFXBoundUtil::ComputeTriangleArea(IFXVector3 vTriangle[3], F32* pfTriangleArea)
{
	IFXRESULT result = IFX_OK;

	if( pfTriangleArea )
	{
		IFXVector3 vVec1, vVec2;
		IFXVector3 vVec1CrossVec2;

		vVec1.Subtract(vTriangle[1], vTriangle[0]);
		vVec2.Subtract(vTriangle[2], vTriangle[0]);

		vVec1CrossVec2.CrossProduct(vVec1, vVec2);

		*pfTriangleArea = 0.5f * vVec1CrossVec2.CalcMagnitude();

	}
	else result = IFX_E_INVALID_POINTER;
	return result;
}


//-----------------------------------------------------------------------------
// CIFXBoundUtil::InitFaceNode
//
// Retrieves face and vertex information from the input IFXMeshGroup and
// initializes the internal bounding volume face and vertex lists
//-----------------------------------------------------------------------------

IFXRESULT CIFXBoundUtil::InitFaceNode(CIFXBoundFace** ppFaceList, 
									  U32*			  pNumFaces,
									  IFXVector3**	  ppVertexList,
									  U32*			  pNumVerts,
									  IFXMeshGroup*   pMeshGroup)
{
	IFXRESULT result = IFX_OK;

	if( ppFaceList && pNumFaces && ppVertexList && pNumVerts && pMeshGroup )
	{
		U32 uNumMeshes = pMeshGroup->GetNumMeshes();
		U32 uNumFaces  = 0;
		U32 uNumVerts  = 0;

		U32 i, j;

		// Compute total IFXMeshGroup faces and vertices
		IFXMesh* pMesh = NULL;
		for(i=0; i<uNumMeshes; i++)
		{	
			pMeshGroup->GetMesh(i, pMesh);
			
			if( pMesh ) 
			{
				uNumFaces += pMesh->GetNumFaces();
				uNumVerts += pMesh->GetNumVertices();
			}

			IFXRELEASE(pMesh);
		}

		*pNumFaces = uNumFaces;
		*pNumVerts = uNumVerts;

		if( uNumFaces > 0 && uNumVerts > 0 )
		{
			// Create new FaceNode and vertex lists
			*ppFaceList   = new CIFXBoundFace[uNumFaces];
			*ppVertexList = new IFXVector3[uNumVerts];

			if( *ppFaceList && *ppVertexList ) 
			{
				IFXVertexIter vertexIter;
				IFXMesh*      pMesh		 = NULL;
				U32			  uVertCount = 0;
				U32			  uFaceCount = 0;

				// Copy IFXMeshGroup vertices and faces into local structs
				for(i=0; i<uNumMeshes && IFXSUCCESS(result); i++)
				{
					pMeshGroup->GetMesh(i, pMesh);
					
					if( pMesh ) 
					{
						// Grab vertex and face iterators
						result = pMesh->GetVertexIter(vertexIter);
						
						U32 uVertStart = 0;
						
						if( IFXSUCCESS(result) )
						{
							uNumVerts = pMesh->GetNumVertices();
							
							// Copy mesh vertices
							for(j=0; j<uNumVerts; j++)
							{
								(*ppVertexList)[uVertCount] = *(vertexIter.GetPosition());
								uVertCount ++;
								
								vertexIter.Next();
							}
							
							// Compute the begining index for current vertices
							uVertStart = uVertCount - uNumVerts;
							
							// Reset vertex iterator
							result = pMesh->GetVertexIter(vertexIter);
						}

						IFXFaceIter faceIter;
						// Get a face iterator
						if( IFXSUCCESS(result) )
							result = pMesh->GetFaceIter(faceIter);
						
						if( IFXSUCCESS(result) )
							uNumFaces = pMesh->GetNumFaces();

						IFXVertexIter tmpVertexIter;
						IFXVector3	  vCentroid;
						IFXU32Face	  newFace;
						IFXFace*	  pFace = NULL;

						for(j=0; j<uNumFaces && IFXSUCCESS(result); j++)
						{
							pFace = faceIter.Get();

							if( pFace ) 
							{
								// Set intersect flag to FALSE
								(*ppFaceList)[uFaceCount].SetIntersect(FALSE);
								
								// Set mesh and face ID for late index into original mesh
								(*ppFaceList)[uFaceCount].SetMeshID(i);
								(*ppFaceList)[uFaceCount].SetFaceID(j);
								
								// Retrieve the face from the facelist

								newFace.SetA(pFace->VertexA() + uVertStart);
								newFace.SetB(pFace->VertexB() + uVertStart);
								newFace.SetC(pFace->VertexC() + uVertStart);
								result = (*ppFaceList)[uFaceCount].SetFace(&newFace);

								// Compute and store face centroid
								vCentroid.Set(0, 0, 0);
								
								// Get vertex 0
								tmpVertexIter = vertexIter;
								tmpVertexIter.PointAt(pFace->Vertex(0));
								vCentroid.Add( *(tmpVertexIter.GetPosition()) );

								// Get vertex 1
								tmpVertexIter = vertexIter;
								tmpVertexIter.PointAt(pFace->Vertex(1));
								vCentroid.Add( *(tmpVertexIter.GetPosition()) );

								// Get vertex 2
								tmpVertexIter = vertexIter;
								tmpVertexIter.PointAt(pFace->Vertex(2));
								vCentroid.Add( *(tmpVertexIter.GetPosition()) );
								
								vCentroid.Scale(SCALEFACTOR);
							
								(*ppFaceList)[uFaceCount].SetCentroid(vCentroid);

								// Increment the face count
								uFaceCount ++;
								
								// Advance faceIter
								faceIter.Next();
							}
							else
								result = IFX_E_NOT_INITIALIZED;
						}
					}
					else
						result = IFX_E_NOT_INITIALIZED;

					IFXRELEASE(pMesh);
				}
			}
			else
				result = IFX_E_OUT_OF_MEMORY;
		}
		else
			result = IFX_CANCEL;
	}
	else 
		result = IFX_E_INVALID_POINTER;

	return result;
}


//-----------------------------------------------------------------------------
// CIFXBoundUtil::SplitFaceList_Median
//
// Simply compute the median of the face list and return it's value.
//-----------------------------------------------------------------------------

IFXRESULT CIFXBoundUtil::SplitFaceList_Median(U32 uNumFaces, U32 *pMedian)
{
	IFXRESULT result = IFX_OK;

	if( pMedian )
	{
		if( !(uNumFaces % 2) ) *pMedian = uNumFaces / 2;
		else *pMedian = uNumFaces / 2 + 1;
	}
	else result = IFX_E_INVALID_POINTER;
	return result;
}


//-----------------------------------------------------------------------------
// CIFXBoundUtil::SplitFaceList_Midpoint
//
// Split the current facelist into left and right face lists.  The split point
// is computed by taking the average of the largest {x,y,z} extent component.
// The lists are then constructed around this point: Check the corresponding
// component of each triangles centroid.  Values less than the split point are
// placed into the left bin while those greater than or equal to the split point
// are place in the right bin.  In the event that one list remains empty at the
// end of the test, the list is split along the median.
//-----------------------------------------------------------------------------

IFXRESULT CIFXBoundUtil::SplitFaceList_Midpoint(CIFXBoundFace *pFaceList, 
												CIFXBoundFace **ppLeftFaces, 
												CIFXBoundFace **ppRightFaces, 
												U32 uNumFaces, U32 *pNumLeftFaces, 
												U32 *pNumRightFaces,
												IFXVector3 *pvMin, IFXVector3 *pvMax) 
{
	IFXRESULT result = IFX_OK;

	if( pFaceList && ppLeftFaces && ppRightFaces && pNumLeftFaces && pNumRightFaces && pvMin && pvMax )
	{
		// Do some error checking
		if( uNumFaces <= 1 ) result = IFX_E_UNDEFINED;

		// Allocate memory for left/right lists	
		if( IFXSUCCESS(result) )
		{
			*ppLeftFaces = new CIFXBoundFace[uNumFaces];

			if( !(*ppLeftFaces) ) result = IFX_E_OUT_OF_MEMORY;
		}

		if( IFXSUCCESS(result) )
		{
			*ppRightFaces = new CIFXBoundFace[uNumFaces];

			if( !(*ppRightFaces) ) result = IFX_E_OUT_OF_MEMORY;
		}

		if( IFXSUCCESS(result) )
		{
			U32 uAxisIndex[3];

			// Compute Longest axis
			uAxisIndex[0] = ComputeLongestAxis(pvMin, pvMax);

			if(      uAxisIndex[0] == 0 ) { uAxisIndex[1] = 1; uAxisIndex[2] = 2; }
			else if( uAxisIndex[0] == 1 ) { uAxisIndex[1] = 0; uAxisIndex[2] = 2; }
			else                          { uAxisIndex[1] = 0; uAxisIndex[2] = 1; }

			U32 uLeftCount, uRightCount;
			U32 i, j;

			// Sort faces into the correct bins
			for(i=0; i<3; i++)
			{
				// Initialize counters for left/right lists
				uLeftCount = 0;
				uRightCount = 0;

				// Compute the split point 
				F32 fAverageSplit = ((*pvMin)[uAxisIndex[i]] + (*pvMax)[uAxisIndex[i]]) * 0.5f;

				// Test each face and place into the correct list
				for(j=0; j<uNumFaces; j++)
				{
					if( pFaceList[j].GetCentroidComponent(uAxisIndex[i]) <= fAverageSplit )
						(*ppLeftFaces)[uLeftCount++] = pFaceList[j];
					else
						(*ppRightFaces)[uRightCount++] = pFaceList[j];
				}

				// If both lists contain faces break the loop
				if( uLeftCount && uRightCount ) break;
			}

			// If either list is empty, split the facelist on the median
			if( !uLeftCount || !uRightCount )
			{
				uLeftCount = 0;
				uRightCount = 0;

				U32 uMedian;
				if( uNumFaces % 2 )
					uMedian = uNumFaces / 2;
				else 
					uMedian = uNumFaces / 2 - 1;

				for(j=0; j<=uMedian; j++)
					(*ppLeftFaces)[uLeftCount++] = pFaceList[j];

				for(j=uMedian+1; j<uNumFaces; j++)
					(*ppRightFaces)[uRightCount++] = pFaceList[j];
			}

			*pNumLeftFaces  = uLeftCount;
			*pNumRightFaces = uRightCount;
		}

		// Deallocate memory if error code present
		if( IFXFAILURE(result) )
		{
			IFXDELETE_ARRAY(ppRightFaces);
			IFXDELETE_ARRAY(ppLeftFaces);
		}
	}
	else result = IFX_E_INVALID_POINTER;
	return result;
}


//-----------------------------------------------------------------------------
// CIFXBoundUtil::SplitFaceList_SortedMedian
//
// Compute the median face by projecting the centroid of each face onto
// the longest computed axis and sorting the list.
//-----------------------------------------------------------------------------

IFXRESULT CIFXBoundUtil::SplitFaceList_SortedMedian(CIFXBoundFace *pFaceList, 
													U32 uNumFaces, IFXVector3 *pvMin, 
													IFXVector3 *pvMax, U32 *pMedian)
{
	IFXRESULT result = IFX_OK;

	if( pFaceList && pvMin  && pvMax && pMedian )
	{
		if( uNumFaces <= 0 ) result = IFX_E_UNDEFINED;

		if( IFXSUCCESS(result) )
		{
			// Compute Longest axis
			U32 uAxisIndex = ComputeLongestAxis(pvMin, pvMax);
			U32 i;

			// Use an insertion sort technique to sort pFaceList
			for(i=1; i<uNumFaces; i++)
			{
				CIFXBoundFace nextFace = pFaceList[i];

				I32 j = i - 1;
				I32 index = i;

				while( (j >= 0) && (nextFace.GetCentroidComponent(uAxisIndex) < 
								pFaceList[j].GetCentroidComponent(uAxisIndex)) )
				{
					pFaceList[index] = pFaceList[j];

					index = j;
					j --;
				}

				pFaceList[index] = nextFace;
			}

			// Set Median Point
			if( !(uNumFaces % 2) ) *pMedian = uNumFaces / 2;
			else *pMedian = uNumFaces / 2 + 1;
		}
	}
	else result = IFX_E_INVALID_POINTER;
	return result;
}
