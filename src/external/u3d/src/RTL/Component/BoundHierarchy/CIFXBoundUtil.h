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

/**
	@file	CIFXBoundUtil.h

			Header file for class CIFXBoundUtil.  Contains the declaration for a
			utility class that contains functionality that is non-specific to a
			particular bounding volume type.
*/

#ifndef __CIFXBOUNDUTIL_H__
#define __CIFXBOUNDUTIL_H__

#include "IFXMeshGroup.h"
#include "CIFXBoundFace.h"

class CIFXBoundUtil
{
public:

	U32 ComputeLongestAxis(IFXVector3* pvMin, 
						   IFXVector3* pvMax);

	IFXRESULT ComputeTriangleArea(IFXVector3 vTriangle[3], 
								  F32*       pfTriangleArea);

	IFXRESULT InitFaceNode(CIFXBoundFace** ppFaceList, 
						   U32*            pNumFaces, 
						   IFXVector3**    ppVertexList,
						   U32*            pNumVerts,
						   IFXMeshGroup*   pMeshGroup);

	IFXRESULT SplitFaceList_Median(U32  uNumFaces,
								   U32* pMedian);

	IFXRESULT SplitFaceList_Midpoint(CIFXBoundFace*  pFaceList, 
									 CIFXBoundFace** ppLeftFaces, 
									 CIFXBoundFace** ppRightFaces, 
									 U32             uNumFaces, 
									 U32*            pNumLeftFaces,
									 U32*            pNumRightFaces,
									 IFXVector3*     pvMin,
									 IFXVector3*     pvMax);

	IFXRESULT SplitFaceList_SortedMedian(CIFXBoundFace* pFaceList,
										 U32            uNumFaces,
				 					     IFXVector3*    pvMin,
										 IFXVector3*    pvMax,
										 U32*           pMedian);
};

#endif
