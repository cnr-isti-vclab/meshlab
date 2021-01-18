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
//	CIFXAxisAlignedBBox.h
//
//	DESCRIPTION
//
//		Header file for CIFXAxisAlignedBBox class.  This class declares the
//		structure of and methods that act on an axis aligned bounding box.
//
//	NOTES
//
//		Although each object hierarchy is initially constructed as an axis
//		aligned bounding box hierarchy, at runtime each is treated as an
//		oriented bounding box.  This overcomes the problem of recomputing 
//		the box extents when the underlying object is rotated.
//
//***************************************************************************

#ifndef __CIFXAXISALIGNEDBBOX_H__
#define __CIFXAXISALIGNEDBBOX_H__

#include "IFXBoundVolume.h"
#include "IFXMatrix4x4.h"
#include "CIFXBoundFace.h"
#include "IFXBoundHierarchy.h"

class CIFXAxisAlignedBBox : public IFXBoundVolume
{
public:

	CIFXAxisAlignedBBox();
	virtual  ~CIFXAxisAlignedBBox();

	IFXRESULT Build();
	IFXRESULT GetCentroid(IFXVector3 *pvCentroid);
	IFXRESULT GetHalfWidth(IFXVector3 *pHalfWidth);
	IFXRESULT GetMaxPosition(IFXVector3 *pvPosition);
	IFXRESULT GetMinPosition(IFXVector3 *pvPosition);

	IFXRESULT Initialize(IFXBoundHierarchy *pHierarchy);

	IFXRESULT IFXAPI 	IntersectBound(IFXBoundVolume *pBound);
	
	IFXRESULT IFXAPI 	IntersectBoxSphere(IFXBoundVolume* pBound, 
								 F32             fSphereRadiusSquared,	
								 IFXVector3&     vSphereCentroid,
								 IFXVector3&     vContactPoint, 
								 IFXVector3      vContactNormal[2]);

	IFXRESULT IFXAPI 	IntersectRay(IFXVector3 vOrigin, 
						   IFXVector3 vDirection, 
						   U32        uPickType);

	IFXRESULT IFXAPI 	IntersectRayTriangle(IFXVector3 vOrigin, 
								   IFXVector3 vDirection, 
								   U32        uPickType);
	
	IFXRESULT IFXAPI 	IntersectTriangle(IFXBoundVolume *pBound);

	IFXRESULT SetFaceList(CIFXBoundFace **pFaceList);

	void      SetNumFaces(U32 uNumFaces);

	IFXRESULT SplitBound(CIFXAxisAlignedBBox** ppLeftBound,
						 CIFXAxisAlignedBBox** ppRightBound);

	// Inlines
	IFXBoundHierarchy* GetBoundHierarchy() { return m_pBoundHierarchy; }
	CIFXBoundFace*     GetFaceList()       { return m_pFaceList;       }

	U32 GetNumFaces() { return m_uNumFaces; }

private:

	IFXBoundHierarchy* m_pBoundHierarchy;
	CIFXBoundFace*     m_pFaceList;
	IFXVector3         m_vMin;          // Minimum {x,y,z} vertex values
	IFXVector3	       m_vMax;          // Maximum {x,y,z} vertex values
	IFXVector3		   m_vHalfWidth;    // Positive {x,y,z} half widths
	IFXVector3		   m_vCentroid;     // Box centroid
//	BOOL               m_bModelIndex;
	U32                m_uNumFaces;
};

#endif
