//***************************************************************************
//
//  Copyright (c) 2000 - 2006 Intel Corporation
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
//	CIFXContourExtruder.cpp
//
//	DESCRIPTION
//		Implementation file for the contour extruder class.
//
//	NOTES
//      None.
//
//***************************************************************************

#include "IFXCoreCIDs.h"
#include "CIFXContourExtruder.h"

#include "IFXContour.h"

#ifdef CONTOUREXTRUDER_BUILD_OPENGL_TESTCODE
#include <time.h>
#endif


#define MAX_BEVEL 5

// IFXUnknown methods

//---------------------------------------------------------------------------
//	CIFXContourExtruder::AddRef
//
//	This method increments the reference count for an interface on a
//	component.  It should be called for every new copy of a pointer to an
//	interface on a given component.  It returns a U32 that contains a value
//	from 1 to 2^32 - 1 that defines the new reference count.  The return
//	value should only be used for debugging purposes.
//---------------------------------------------------------------------------
U32 CIFXContourExtruder::AddRef(void) {
	return ++m_uRefCount;
} 	

//---------------------------------------------------------------------------
//	CIFXContourExtruder::Release
//
//	This method decrements the reference count for the calling interface on a
//	component.  It returns a U32 that contains a value from 1 to 2^32 - 1
//	that defines the new reference count.  The return value should only be
//	used for debugging purposes.  If the reference count on a component falls
//	to zero, the component is destroyed.
//---------------------------------------------------------------------------
U32 CIFXContourExtruder::Release(void) {
	if ( !( --m_uRefCount ) )
	{
		delete this;

		// This second return point is used so that the deleted object's
		// reference count isn't referenced after the memory is released.
		return 0;
	}

	return m_uRefCount;
} 	

//---------------------------------------------------------------------------
//	CIFXContourExtruder::QueryInterface
//
//	This method provides access to the various interfaces supported by a
//	component.  Upon success, it increments the component's reference count,
//	hands back a pointer to the specified interface and returns IFX_OK.
//	Otherwise, it returns either IFX_E_INVALID_POINTER or IFX_E_UNSUPPORTED.
//
//	A number of rules must be adhered to by all implementations of
//	QueryInterface.  For a list of such rules, refer to the Microsoft COM
//	description of the IUnknown::QueryInterface method.
//---------------------------------------------------------------------------
IFXRESULT CIFXContourExtruder::QueryInterface( IFXREFIID interfaceId, void** ppInterface ){
	IFXRESULT	result	= IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXContourExtruder ||
			 interfaceId == IID_IFXUnknown )
			*ppInterface = ( IFXContourExtruder* ) this;
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

//---------------------------------------------------------------------------
//	CIFXContourExtruder_Factory (non-singleton)
//
//	This is the CIFXContourExtruder component factory function.  The
//	CIFXContourExtruder component is NOT a singleton.  This function creates the
//  TextGenerator object, addref()'s it and returns it.
//---------------------------------------------------------------------------
IFXRESULT IFXAPI_CALLTYPE CIFXContourExtruder_Factory( IFXREFIID	interfaceId,
									void**		ppInterface )
{
	IFXRESULT	result;

	if ( ppInterface )
	{
		// It doesn't exist, so try to create it.
		CIFXContourExtruder	*pComponent	= new CIFXContourExtruder;

		if ( pComponent )
		{
			// Perform a temporary AddRef for our usage of the component.
			pComponent->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			result = pComponent->QueryInterface( interfaceId, ppInterface );

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the QI fails, this will cause the
			// component to be destroyed.
			pComponent->Release();
		}
		else
			result = IFX_E_OUT_OF_MEMORY;
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}

// IFXContourExtruder methods

IFXRESULT CIFXContourExtruder::Extrude(SIFXExtruderProperties* pExtruderProperties, IFXSimpleList* pGlyphList, IFXMeshGroup** ppMeshGroup) {
	IFXRESULT iResult=IFX_OK;

	if(pExtruderProperties==NULL || pGlyphList==NULL || ppMeshGroup==NULL) 
		iResult=IFX_E_INVALID_POINTER;
	
	// build a mesh group big enough to contain the glyph (list of contours)
	if(IFXSUCCESS(iResult)) {
		U32 uMeshGroupCount=0;
		pGlyphList->GetCount(&uMeshGroupCount);

		if(pExtruderProperties->eFacing==IFXGlyphBothFacing) 
			uMeshGroupCount*=2;
		

		IFXCreateComponent(CID_IFXMeshGroup, IID_IFXMeshGroup, (void**)ppMeshGroup);
		if(*ppMeshGroup)
			iResult=(*ppMeshGroup)->Allocate(uMeshGroupCount);
	}
	
	// build the mesh if the mesh and group built successfully
	if(IFXSUCCESS(iResult)) {
		U32 uPathCount, uPathIndex;
		IFXUnknown* pUnknown=NULL;
		IFXContour* pContour=NULL;
		IFXMesh* pMesh=NULL;
		
		uPathIndex=0;
		if(pExtruderProperties->eFacing==IFXGlyphOutFacing ||
			pExtruderProperties->eFacing==IFXGlyphBothFacing) {
//			pExtruderProperties->bWindClockWise=IFX_FALSE;
//			pExtruderProperties->eNormalOrientation=IFXGlyphRegularNormals;
			pExtruderProperties->bWindClockWise=IFX_TRUE;
			pExtruderProperties->eNormalOrientation=IFXGlyphInverseNormals;
			// walk the path list
			pGlyphList->GetCount(&uPathCount);
			while(uPathIndex<uPathCount) {
				pUnknown=NULL;
				pGlyphList->Get(uPathIndex, &pUnknown);
				iResult=pUnknown->QueryInterface(IID_IFXContour, (void**)&pContour);
				if(IFXSUCCESS(iResult) && pContour) {
					iResult=Extrude(pExtruderProperties, pContour, &pMesh);
					if(IFXSUCCESS(iResult)) {
						// add to mesh group
						(*ppMeshGroup)->SetMesh(uPathIndex, pMesh);
						IFXRELEASE(pMesh);
					}
				}
				uPathIndex++;
				IFXRELEASE(pContour);
				IFXRELEASE(pUnknown);
				}
		}
		
		U32 uPathIndex2=0;	
		if(pExtruderProperties->eFacing==IFXGlyphInFacing ||
			pExtruderProperties->eFacing==IFXGlyphBothFacing) {
//			pExtruderProperties->bWindClockWise=IFX_TRUE;
//			pExtruderProperties->eNormalOrientation=IFXGlyphInverseNormals;
			pExtruderProperties->bWindClockWise=IFX_FALSE;
			pExtruderProperties->eNormalOrientation=IFXGlyphRegularNormals;
			// walk the path list
			pGlyphList->GetCount(&uPathCount);
			while(uPathIndex2<uPathCount) {
				pUnknown=NULL;
				pGlyphList->Get(uPathIndex2, &pUnknown);
				iResult=pUnknown->QueryInterface(IID_IFXContour, (void**)&pContour);
				if(IFXSUCCESS(iResult) && pContour) {
					iResult=Extrude(pExtruderProperties, pContour, &pMesh);
					if(IFXSUCCESS(iResult)) {
						// add to mesh group
						(*ppMeshGroup)->SetMesh(uPathIndex, pMesh);
						IFXRELEASE(pMesh);
					}
				}
				uPathIndex++;
				uPathIndex2++;
				IFXRELEASE(pContour);
				IFXRELEASE(pUnknown);

			}
		}

	}
	
	return iResult;
}


IFXRESULT CIFXContourExtruder::Extrude(SIFXExtruderProperties* pExtruderProperties, IFXContour* pContour, IFXMesh** ppMesh) {
	IFXRESULT iResult=IFX_OK;
	
	
	if(pExtruderProperties==NULL || pContour==NULL || ppMesh==NULL)
		iResult=IFX_E_INVALID_POINTER;
	
	IFXMesh* pMesh=NULL;

	U32 uVertexCount=0;
	U32 uFaceCount=0;

	

	// allocate an appropriate sized mesh
	if(IFXSUCCESS(iResult)) {
		// Verify extruder properties and prevent divide by zero's
		if(pExtruderProperties->uDepthSteps==0)  {
			pExtruderProperties->uDepthSteps=1;
			pExtruderProperties->fExtrusionDepth=0.0001f;
		}
		if(pExtruderProperties->fExtrusionDepth==0) 
			pExtruderProperties->fExtrusionDepth=0.0001f;

		if(IFXSUCCESS(iResult)) {
			
			iResult=DetermineVertexCount(pContour, &uVertexCount, &uFaceCount);
			
//			uVertexCount=uVertexCount+(uVertexCount*(pExtruderProperties->uDepthSteps+2));
//			uFaceCount=uFaceCount*(pExtruderProperties->uDepthSteps+2);

			IFXCreateComponent(CID_IFXMesh, IID_IFXMesh, (void**)&pMesh);
			if(pMesh==NULL)
				iResult=IFX_E_OUT_OF_MEMORY;
			if(IFXSUCCESS(iResult)) {
				IFXVertexAttributes vertexAttributes;

				if(pExtruderProperties->bBevel) {
					iResult=pMesh->Allocate(vertexAttributes, 
						uVertexCount*2*(2+1 /*pExtruderProperties->uDepthSteps*/), 
						uFaceCount*(2+ 1/*pExtruderProperties->uDepthSteps*/)
						);	
				}
				else
				{
					iResult=pMesh->Allocate(vertexAttributes, 
						uVertexCount*2*(1 /*pExtruderProperties->uDepthSteps*/), 
						uFaceCount*(1 /*pExtruderProperties->uDepthSteps*/)
						);	
				}
			}
		}
	}
	// fill up the mesh with geometry
	if(IFXSUCCESS(iResult)) {

		U32 uVertexIndex;
		U32 uCurrentMeshVertexIndex=0;
		U32 uCurrentFaceCount=0;

		F32 fDepth;

		F32 eps = (F32)1e-6;

		//IFXUnknown* pUnknown=NULL;

		SIFXContourPoint vContourPosition;
		SIFXContourPoint vContourNormal;

		SIFXContourPoint vPrevContourPosition;
		SIFXContourPoint vPrevContourNormal;

		SIFXContourPoint vNextContourPosition;
		SIFXContourPoint vNextContourNormal;

		IFXVector3 vBevel;
		IFXVector3 vZDirection(0,0,1);
		F32 fBevelDepth;
		BOOL bRoundBevel=IFX_FALSE;
		BOOL bBevel=IFX_FALSE;
		BOOL bBevelFront = IFX_FALSE;
		BOOL bBevelBack = IFX_FALSE;

		IFXVector3Iter v3iPosition;
		IFXVector3* pvPosition;

		IFXVector3Iter v3iNormal;
		IFXVector3* pvNormal;
		IFXVector3 vNormal;
		IFXVector3 vPrevNormal;
		IFXVector3 vNextNormal;

		IFXFaceIter fiFace;
		IFXFace*	pfFace;


		// get the iterators
		pMesh->GetPositionIter(v3iPosition);
		pMesh->GetNormalIter(v3iNormal);
		pMesh->GetFaceIter(fiFace);

		fDepth=pExtruderProperties->fExtrusionDepth;
		bBevel=pExtruderProperties->bBevel;
		if(bBevel) {
			fBevelDepth=pExtruderProperties->fBevelDepth;
			if(IFXGlyphBevelRound==pExtruderProperties->eBevelType)
			{
				bRoundBevel=IFX_TRUE;
			}
			else
			{
				bRoundBevel=IFX_FALSE;
			}

			if(	IFXGlyphBevelFaceFront==pExtruderProperties->eBevelFace ||
				IFXGlyphBevelFaceBoth==pExtruderProperties->eBevelFace)
			{
				bBevelFront=IFX_TRUE;

			}

			if(	IFXGlyphBevelFaceBack==pExtruderProperties->eBevelFace ||
				IFXGlyphBevelFaceBoth==pExtruderProperties->eBevelFace)
			{
				bBevelBack=IFX_TRUE;

			}
		}
		else {
			// if bBevel is set to false, then
			// bBevelFront and bBevelBack must
			// also be set to false.
			// Otherwise, cracks will appear between
			// the faces and the tunnel.
			bBevelFront=IFX_FALSE;
			bBevelBack=IFX_FALSE;
			bRoundBevel=IFX_TRUE;
			fBevelDepth=0;
		}

		//Backside Bevel
		if(bBevel) 
		for(uVertexIndex=0;uVertexIndex<uVertexCount;uVertexIndex+=2) 
		{
		// Step through contour, one line segment at a time.
		// Each line segment has two vertices.  

			{	// Make two copies of the fisrt vertex.  Place one at the front of the bevel,
				// and one at the back of the bevel.

				pContour->Get(uVertexIndex, &vContourPosition, &vContourNormal);
				pContour->Get((uVertexCount+uVertexIndex-1)%uVertexCount, &vPrevContourPosition, &vPrevContourNormal);

				vNormal.Set((F32)vContourNormal.x, (F32)vContourNormal.y, 0.0);
				vPrevNormal.Set((F32)vPrevContourNormal.x, (F32)vPrevContourNormal.y, 0.0);

				//Calculate Bevel
				if(vNormal.IsApproximately(vPrevNormal,eps)) {
					// Smooth Curve
					vBevel=vNormal.Scale(fBevelDepth);
				}
				else { 
					// Hard Edge
					F32 fTemp;
					IFXVector3 vTemp;
					if(pExtruderProperties->eNormalOrientation==IFXGlyphInverseNormals)
					{
						vTemp.CrossProduct(vPrevNormal,vNormal);
					}
					else
					{
						vTemp.CrossProduct(vNormal,vPrevNormal);
					}
					fTemp = IFXVector3::DotProduct(vTemp, vZDirection);
						// Test if current vertex is convex
					if (  fTemp > 0.0)  {
						// if convex, calulate interscection point of expaned countour
						F32 fCosHalfAngle = (F32) sqrt( (1.0 + IFXVector3::DotProduct(vNormal,vPrevNormal)) / 2.0 );
						if(fCosHalfAngle < (F32) 1.0/MAX_BEVEL)
						{
							fCosHalfAngle = (F32) 1.0/MAX_BEVEL;
						}
						F32 fMag = (F32) fBevelDepth / fCosHalfAngle;

						IFXVector3 a,b,c;
						a.Subtract(vPrevNormal,vNormal);
						b.CrossProduct(a,IFXVector3(0,0,-1));
						c.Add(vPrevNormal,vNormal);
						vBevel.Add(b,c);

						vBevel.Normalize();
						vBevel.Scale(fMag);
					}
					else {
						// if not convex, create overlapping bevel 
						vBevel=vNormal.Scale(fBevelDepth);
					}

				}
		
				// Copy fist vertext at the back of the bevel
				pvPosition=v3iPosition.Next(); 

				// Move position by the bevel depth.  If the back face is not beveled, set the position so as 
				// to create flat faces out to the expanded contour.
				if(IFX_TRUE==bBevelBack)
				{
					pvPosition->Set((F32)vContourPosition.x, (F32)vContourPosition.y, -fDepth+fBevelDepth);
				}
				else
				{
					pvPosition->Set((F32)vContourPosition.x, (F32)vContourPosition.y, -fDepth);
				}
				// add vBevel to move contour point outwards to the correct position.
				if(pExtruderProperties->eNormalOrientation==IFXGlyphInverseNormals)
				{
					pvPosition->Subtract(vBevel);
				}
				else
				{
					pvPosition->Add(vBevel);
				}

				//Now set the normal for the current position.
				pvNormal=v3iNormal.Next();
				if(pExtruderProperties->bNormals==IFX_FALSE)
					pvNormal->Set(0, 0, 0);
				else if(bBevelBack==IFX_TRUE)
				{
					if(pExtruderProperties->eNormalOrientation==IFXGlyphInverseNormals) {
						pvNormal->Set(-(F32)vContourNormal.x, -(F32)vContourNormal.y, 0);
					}
					else {
						pvNormal->Set((F32)vContourNormal.x, (F32)vContourNormal.y, 0);
					}
					if(bRoundBevel==IFX_FALSE) {
						// aim normal to create a hard edge.
						// this calculation assumes bevel is always pi/4.
						pvNormal->Subtract(vZDirection);
						pvNormal->Normalize();
					}
				}
				else // No bevel on the backside.  Normals are just pointing in the -z direction.
				{
						pvNormal->Set(0, 0, -1);
				}
				uCurrentMeshVertexIndex++;


				// Copy fist vertex to the front of the bevel.
				// This is the original contour
				pvPosition=v3iPosition.Next(); 
				pvPosition->Set((F32)vContourPosition.x, (F32)vContourPosition.y, -fDepth);

				pvNormal=v3iNormal.Next();
				if(pExtruderProperties->bNormals==IFX_FALSE)
					pvNormal->Set(0, 0, 0);
				else if(bBevelBack==IFX_TRUE)
				{
					if(pExtruderProperties->eNormalOrientation==IFXGlyphInverseNormals) 
					{
						pvNormal->Set(-(F32)vContourNormal.x, -(F32)vContourNormal.y, 0);
					}
					else 
					{
						pvNormal->Set((F32)vContourNormal.x, (F32)vContourNormal.y, 0);
					}
					// always leave a hard edge around the original contour.
					// this calculation assumes bevel is always pi/4.
					pvNormal->Subtract(vZDirection);
					pvNormal->Normalize();
				}
				else // No bevel on the backside.  Normals are just pointing in the -z direction.
				{
						pvNormal->Set(0, 0, -1);
				}
				uCurrentMeshVertexIndex++;
			}


			{
				// Make two copies of the second vertex.  Place one at the front of the bevel,
				// and one at the back of the bevel.

				pContour->Get((uVertexIndex+1)%uVertexCount, &vContourPosition, &vContourNormal);
				pContour->Get((uVertexIndex+1+1)%uVertexCount, &vNextContourPosition, &vNextContourNormal);

				vNormal.Set((F32)vContourNormal.x, (F32)vContourNormal.y, 0.0);
				vNextNormal.Set((F32)vNextContourNormal.x, (F32)vNextContourNormal.y, 0.0);

				// cacluate bevel
				if(vNormal.IsApproximately(vNextNormal,eps))
				{
					// smooth contour
					vBevel=vNormal.Scale(fBevelDepth);
				}
				else 
				{	// hard edge
					F32 fTemp;
					IFXVector3 vTemp;
					if(pExtruderProperties->eNormalOrientation==IFXGlyphInverseNormals)
					{
						vTemp.CrossProduct(vNextNormal,vNormal);
					}
					else
					{
						vTemp.CrossProduct(vNormal,vNextNormal);
					}
					fTemp = IFXVector3::DotProduct(vTemp, vZDirection);
					if (  fTemp < 0.0)  {
						F32 fCosHalfAngle = (F32) sqrt( (1.0 + IFXVector3::DotProduct(vNormal,vNextNormal)) / 2.0 );
						if(fCosHalfAngle < (F32) 1.0/MAX_BEVEL)
						{
							fCosHalfAngle = (F32) 1.0/MAX_BEVEL;
						}
						F32 fMag = (F32) fBevelDepth / fCosHalfAngle;

						IFXVector3 a,b,c;
						a.Subtract(vNormal,vNextNormal);
						b.CrossProduct(a,IFXVector3(0,0,-1));
						c.Add(vNextNormal,vNormal);
						vBevel.Add(b,c);

						vBevel.Normalize();
						vBevel.Scale(fMag);
					}
					else {
						vBevel=vNormal.Scale(fBevelDepth);
					}
				}

				pvPosition=v3iPosition.Next(); 
				if(bBevelBack==IFX_TRUE) 
				{
					pvPosition->Set((F32)vContourPosition.x, (F32)vContourPosition.y, -fDepth+fBevelDepth);
				}
				else
				{
					pvPosition->Set((F32)vContourPosition.x, (F32)vContourPosition.y, -fDepth);
				}

				if(pExtruderProperties->eNormalOrientation==IFXGlyphInverseNormals)
				{
					pvPosition->Subtract(vBevel);
				}
				else
				{
					pvPosition->Add(vBevel);
				}

				pvNormal=v3iNormal.Next();
				if(pExtruderProperties->bNormals==IFX_FALSE)
					pvNormal->Set(0, 0, 0);
				else if(bBevelBack==IFX_TRUE)
				{
					if(pExtruderProperties->eNormalOrientation==IFXGlyphInverseNormals) 
					{
						pvNormal->Set(-(F32)vContourNormal.x, -(F32)vContourNormal.y, 0);
					}
					else 
					{
						pvNormal->Set((F32)vContourNormal.x, (F32)vContourNormal.y, 0);
					}
					if(bRoundBevel==IFX_FALSE) {
						pvNormal->Subtract(vZDirection);
						pvNormal->Normalize();
					}
				}
				else // No bevel on the backside.  Normals are just pointing in the -z direction.
				{
						pvNormal->Set(0, 0, -1);
				}
				uCurrentMeshVertexIndex++;


				pvPosition=v3iPosition.Next(); 
				pvPosition->Set((F32)vContourPosition.x, (F32)vContourPosition.y, -fDepth);

				pvNormal=v3iNormal.Next();
				if(pExtruderProperties->bNormals==IFX_FALSE)
					pvNormal->Set(0, 0, 0);
				else if(bBevelBack==IFX_TRUE)
				{
					if(pExtruderProperties->eNormalOrientation==IFXGlyphInverseNormals) 
					{
						pvNormal->Set(-(F32)vContourNormal.x, -(F32)vContourNormal.y, 0);
					}
					else 
					{
						pvNormal->Set((F32)vContourNormal.x, (F32)vContourNormal.y, 0);
					}
					pvNormal->Subtract(vZDirection);
					pvNormal->Normalize();
				}
				else // No bevel on the backside.  Normals are just pointing in the -z direction.
				{
						pvNormal->Set(0, 0, -1);
				}
				uCurrentMeshVertexIndex++;
			}

			{// create two faces from the four vertices copied.
				pfFace=fiFace.Next();
				uCurrentFaceCount++;
				if(pExtruderProperties->bWindClockWise==IFX_TRUE) 
					pfFace->Set(uCurrentMeshVertexIndex-2, uCurrentMeshVertexIndex-4, uCurrentMeshVertexIndex-3);
				else
					pfFace->Set(uCurrentMeshVertexIndex-2, uCurrentMeshVertexIndex-3, uCurrentMeshVertexIndex-4);
				
				pfFace=fiFace.Next();
				uCurrentFaceCount++;
				if(pExtruderProperties->bWindClockWise==IFX_TRUE) 
					pfFace->Set(uCurrentMeshVertexIndex-2, uCurrentMeshVertexIndex-3, uCurrentMeshVertexIndex-1);
				else
					pfFace->Set(uCurrentMeshVertexIndex-2, uCurrentMeshVertexIndex-1, uCurrentMeshVertexIndex-3);

			}

		}


		//Tunnel
		//Same procedure as above, except normals are not re-adjusted, and contours are expanded by the bevel depth
		//if bBevel is true.  If there is no beveling, this is the only segment of code that adds to the model.  Comments 
		//are identical to the above section.  
		//Note - if no beveling, fBevelDepth must be set to 0.
		uVertexIndex=0;
		while(uVertexIndex<uVertexCount) {

			{
				pContour->Get(uVertexIndex, &vContourPosition, &vContourNormal);
				pContour->Get((uVertexCount+uVertexIndex-1)%uVertexCount, &vPrevContourPosition, &vPrevContourNormal);

				vNormal.Set((F32)vContourNormal.x, (F32)vContourNormal.y, 0.0);
				vPrevNormal.Set((F32)vPrevContourNormal.x, (F32)vPrevContourNormal.y, 0.0);

				if(bBevel) {
					if(vNormal.IsApproximately(vPrevNormal,eps)) {
						vBevel=vNormal.Scale(fBevelDepth);
					}
					else {
						F32 fTemp;
						IFXVector3 vTemp;
						if(pExtruderProperties->eNormalOrientation==IFXGlyphInverseNormals)
						{
							vTemp.CrossProduct(vPrevNormal,vNormal);
						}
						else
						{
							vTemp.CrossProduct(vNormal,vPrevNormal);
						}
						fTemp = IFXVector3::DotProduct(vTemp, vZDirection);
						if (  fTemp > 0.0)  {
							F32 fCosHalfAngle = (F32) sqrt( (1.0 + IFXVector3::DotProduct(vNormal,vPrevNormal)) / 2.0 );
							if(fCosHalfAngle < (F32) 1.0/MAX_BEVEL)
							{
								fCosHalfAngle = (F32) 1.0/MAX_BEVEL;
							}
							F32 fMag = (F32) fBevelDepth / fCosHalfAngle;

							IFXVector3 a,b,c;
							a.Subtract(vPrevNormal,vNormal);
							b.CrossProduct(a,IFXVector3(0,0,-1));
							c.Add(vPrevNormal,vNormal);
							vBevel.Add(b,c);

							vBevel.Normalize();
							vBevel.Scale(fMag);
						}
						else {
							vBevel=vNormal.Scale(fBevelDepth);
						}

					}
				}
				else
				{
					vBevel.Set(0.0,0.0,0.0);
				}

				pvPosition=v3iPosition.Next(); 
				if(IFX_TRUE==bBevelFront)
				{
					pvPosition->Set((F32)vContourPosition.x, (F32)vContourPosition.y, -fBevelDepth);
				}
				else
				{
					pvPosition->Set((F32)vContourPosition.x, (F32)vContourPosition.y, 0);
				}

				if(pExtruderProperties->eNormalOrientation==IFXGlyphInverseNormals)
				{
					pvPosition->Subtract(vBevel);
				}
				else
				{
					pvPosition->Add(vBevel);
				}

				pvNormal=v3iNormal.Next();
				if(pExtruderProperties->bNormals==IFX_FALSE)
					pvNormal->Set(0, 0, 0);
				else {
					if(pExtruderProperties->eNormalOrientation==IFXGlyphInverseNormals) {
						pvNormal->Set(-(F32)vContourNormal.x, -(F32)vContourNormal.y, 0);
					}
					else {
						pvNormal->Set((F32)vContourNormal.x, (F32)vContourNormal.y, 0);
					}
				}
				uCurrentMeshVertexIndex++;


				pvPosition=v3iPosition.Next(); 
				if(IFX_TRUE==bBevelBack)
				{
					pvPosition->Set((F32)vContourPosition.x, (F32)vContourPosition.y, -fDepth+fBevelDepth);
				}
				else
				{
					pvPosition->Set((F32)vContourPosition.x, (F32)vContourPosition.y, -fDepth);
				}

				if(pExtruderProperties->eNormalOrientation==IFXGlyphInverseNormals)
				{
					pvPosition->Subtract(vBevel);
				}
				else
				{
					pvPosition->Add(vBevel);
				}

				pvNormal=v3iNormal.Next();
				if(pExtruderProperties->bNormals==IFX_FALSE)
					pvNormal->Set(0, 0, 0);
				else {
					if(pExtruderProperties->eNormalOrientation==IFXGlyphInverseNormals) {
						pvNormal->Set(-(F32)vContourNormal.x, -(F32)vContourNormal.y, 0);
					}
					else {
						pvNormal->Set((F32)vContourNormal.x, (F32)vContourNormal.y, 0);
					}
				}
				uCurrentMeshVertexIndex++;


			}


			{
				pContour->Get((uVertexIndex+1)%uVertexCount, &vContourPosition, &vContourNormal);
				pContour->Get((uVertexIndex+1+1)%uVertexCount, &vNextContourPosition, &vNextContourNormal);

				vNormal.Set((F32)vContourNormal.x, (F32)vContourNormal.y, 0.0);
				vNextNormal.Set((F32)vNextContourNormal.x, (F32)vNextContourNormal.y, 0.0);

				if(bBevel) {
					if(vNormal.IsApproximately(vNextNormal,eps))
						vBevel=vNormal.Scale(fBevelDepth);
					else {
						F32 fTemp;
						IFXVector3 vTemp;
						if(pExtruderProperties->eNormalOrientation==IFXGlyphInverseNormals)
						{
							vTemp.CrossProduct(vNextNormal,vNormal);
						}
						else
						{
							vTemp.CrossProduct(vNormal,vNextNormal);
						}
						fTemp = IFXVector3::DotProduct(vTemp, vZDirection);
						if (  fTemp < 0.0)  
						{
							F32 fCosHalfAngle = (F32) sqrt( (1.0 + IFXVector3::DotProduct(vNormal,vNextNormal)) / 2.0 );
							if(fCosHalfAngle < (F32) 1.0/MAX_BEVEL)
							{
								fCosHalfAngle = (F32) 1.0/MAX_BEVEL;
							}
							F32 fMag = (F32) fBevelDepth / fCosHalfAngle;

							IFXVector3 a,b,c;
							a.Subtract(vNormal,vNextNormal);
							b.CrossProduct(a,IFXVector3(0,0,-1));
							c.Add(vNextNormal,vNormal);
							vBevel.Add(b,c);

							vBevel.Normalize();
							vBevel.Scale(fMag);
						}
						else 
						{
							vBevel=vNormal.Scale(fBevelDepth);
						}
					}
				}
				else
				{
					vBevel.Set(0.0,0.0,0.0);
				}

				pvPosition=v3iPosition.Next(); 
				if(IFX_TRUE==bBevelFront)
				{
					pvPosition->Set((F32)vContourPosition.x, (F32)vContourPosition.y, -fBevelDepth);
				}
				else
				{
					pvPosition->Set((F32)vContourPosition.x, (F32)vContourPosition.y, 0.0);
				}

				if(pExtruderProperties->eNormalOrientation==IFXGlyphInverseNormals)
				{
					pvPosition->Subtract(vBevel);
				}
				else
				{
					pvPosition->Add(vBevel);
				}

				pvNormal=v3iNormal.Next();
				if(pExtruderProperties->bNormals==IFX_FALSE)
					pvNormal->Set(0, 0, 0);
				else {
					if(pExtruderProperties->eNormalOrientation==IFXGlyphInverseNormals) {
						pvNormal->Set(-(F32)vContourNormal.x, -(F32)vContourNormal.y, 0);
					}
					else {
						pvNormal->Set((F32)vContourNormal.x, (F32)vContourNormal.y, 0);
					}
				}
				uCurrentMeshVertexIndex++;


				pvPosition=v3iPosition.Next(); 
				if(IFX_TRUE==bBevelBack)
				{
					pvPosition->Set((F32)vContourPosition.x, (F32)vContourPosition.y, -fDepth+fBevelDepth);
				}
				else
				{
					pvPosition->Set((F32)vContourPosition.x, (F32)vContourPosition.y, -fDepth);
				}

				if(pExtruderProperties->eNormalOrientation==IFXGlyphInverseNormals)
				{
					pvPosition->Subtract(vBevel);
				}
				else
				{
					pvPosition->Add(vBevel);
				}

				pvNormal=v3iNormal.Next();
				if(pExtruderProperties->bNormals==IFX_FALSE)
					pvNormal->Set(0, 0, 0);
				else {
					if(pExtruderProperties->eNormalOrientation==IFXGlyphInverseNormals) {
						pvNormal->Set(-(F32)vContourNormal.x, -(F32)vContourNormal.y, 0);
					}
					else {
						pvNormal->Set((F32)vContourNormal.x, (F32)vContourNormal.y, 0);
					}
				}
				uCurrentMeshVertexIndex++;
			}

			{// create two faces
				pfFace=fiFace.Next();
				uCurrentFaceCount++;
				if(pExtruderProperties->bWindClockWise==IFX_TRUE) 
					pfFace->Set(uCurrentMeshVertexIndex-2, uCurrentMeshVertexIndex-4, uCurrentMeshVertexIndex-3);
				else
					pfFace->Set(uCurrentMeshVertexIndex-2, uCurrentMeshVertexIndex-3, uCurrentMeshVertexIndex-4);

				pfFace=fiFace.Next();
				uCurrentFaceCount++;
				if(pExtruderProperties->bWindClockWise==IFX_TRUE) 
					pfFace->Set(uCurrentMeshVertexIndex-2, uCurrentMeshVertexIndex-3, uCurrentMeshVertexIndex-1);
				else
					pfFace->Set(uCurrentMeshVertexIndex-2, uCurrentMeshVertexIndex-1, uCurrentMeshVertexIndex-3);

			}

			uVertexIndex+=2;
		}


		//Frontside Bevel
		if(bBevel) 
		for(uVertexIndex=0;uVertexIndex<uVertexCount;uVertexIndex+=2) {

			{
				pContour->Get(uVertexIndex, &vContourPosition, &vContourNormal);
				pContour->Get((uVertexCount+uVertexIndex-1)%uVertexCount, &vPrevContourPosition, &vPrevContourNormal);

				vNormal.Set((F32)vContourNormal.x, (F32)vContourNormal.y, 0.0);
				vPrevNormal.Set((F32)vPrevContourNormal.x, (F32)vPrevContourNormal.y, 0.0);

				if(vNormal.IsApproximately(vPrevNormal,eps))
					vBevel=vNormal.Scale(fBevelDepth);
				else 
				{
					F32 fTemp;
					IFXVector3 vTemp;
					if(pExtruderProperties->eNormalOrientation==IFXGlyphInverseNormals)
					{
						vTemp.CrossProduct(vPrevNormal,vNormal);
					}
					else
					{
						vTemp.CrossProduct(vNormal,vPrevNormal);
					}
					fTemp = IFXVector3::DotProduct(vTemp, vZDirection);
					if (  fTemp > 0.0)  {
						F32 fCosHalfAngle = (F32) sqrt( (1.0 + IFXVector3::DotProduct(vNormal,vPrevNormal)) / 2.0 );
						if(fCosHalfAngle < (F32) 1.0/MAX_BEVEL)
						{
							fCosHalfAngle = (F32) 1.0/MAX_BEVEL;
						}
						F32 fMag = (F32) fBevelDepth / fCosHalfAngle;

						IFXVector3 a,b,c;
						a.Subtract(vPrevNormal,vNormal);
						b.CrossProduct(a,IFXVector3(0,0,-1));
						c.Add(vPrevNormal,vNormal);
						vBevel.Add(b,c);

						vBevel.Normalize();
						vBevel.Scale(fMag);
					}
					else {
						vBevel=vNormal.Scale(fBevelDepth);
					}
				}

				pvPosition=v3iPosition.Next(); 
				pvPosition->Set((F32)vContourPosition.x, (F32)vContourPosition.y, 0);

				pvNormal=v3iNormal.Next();
				if(pExtruderProperties->bNormals==IFX_FALSE)
					pvNormal->Set(0, 0, 0);
				else if( IFX_TRUE == bBevelFront)
				{
					if(pExtruderProperties->eNormalOrientation==IFXGlyphInverseNormals) 
					{
						pvNormal->Set(-(F32)vContourNormal.x, -(F32)vContourNormal.y, 0);
					}
					else 
					{
						pvNormal->Set((F32)vContourNormal.x, (F32)vContourNormal.y, 0);
					}
					pvNormal->Add(vZDirection);
					pvNormal->Normalize();
				}
				else  // No bevel on the front size.  Normals just point in the z direction.
				{
						pvNormal->Set(0, 0, 1);
				}
				uCurrentMeshVertexIndex++;


				pvPosition=v3iPosition.Next(); 
				if( IFX_TRUE == bBevelFront) 
				{
					pvPosition->Set((F32)vContourPosition.x, (F32)vContourPosition.y, -fBevelDepth);
				}
				else
				{
					pvPosition->Set((F32)vContourPosition.x, (F32)vContourPosition.y, 0);
				}

				if(pExtruderProperties->eNormalOrientation==IFXGlyphInverseNormals)
				{
					pvPosition->Subtract(vBevel);
				}
				else
				{
					pvPosition->Add(vBevel);
				}

				pvNormal=v3iNormal.Next();
				if(pExtruderProperties->bNormals==IFX_FALSE)
					pvNormal->Set(0, 0, 0);
				else if( IFX_TRUE == bBevelFront)
				{
					if(pExtruderProperties->eNormalOrientation==IFXGlyphInverseNormals) 
					{
						pvNormal->Set(-(F32)vContourNormal.x, -(F32)vContourNormal.y, 0);
					}
					else 
					{
						pvNormal->Set((F32)vContourNormal.x, (F32)vContourNormal.y, 0);
					}
					if(bRoundBevel==IFX_FALSE) {
						pvNormal->Add(vZDirection);
						pvNormal->Normalize();
					}
				}
				else  // No bevel on the front size.  Normals just point in the z direction.
				{
						pvNormal->Set(0, 0, 1);
				}
				uCurrentMeshVertexIndex++;


			}

			{
				pContour->Get((uVertexIndex+1)%uVertexCount, &vContourPosition, &vContourNormal);
				pContour->Get((uVertexIndex+1+1)%uVertexCount, &vNextContourPosition, &vNextContourNormal);

				vNormal.Set((F32)vContourNormal.x, (F32)vContourNormal.y, 0.0);
				vNextNormal.Set((F32)vNextContourNormal.x, (F32)vNextContourNormal.y, 0.0);

				if(vNormal.IsApproximately(vNextNormal,eps))
					vBevel=vNormal.Scale(fBevelDepth);
				else 
				{
					F32 fTemp;
					IFXVector3 vTemp;
					if(pExtruderProperties->eNormalOrientation==IFXGlyphInverseNormals)
					{
						vTemp.CrossProduct(vNextNormal,vNormal);
					}
					else
					{
						vTemp.CrossProduct(vNormal,vNextNormal);
					}
					fTemp = IFXVector3::DotProduct(vTemp, vZDirection);
					if (  fTemp < 0.0)  {
						F32 fCosHalfAngle = (F32) sqrt( (1.0 + IFXVector3::DotProduct(vNormal,vNextNormal)) / 2.0 );
						if(fCosHalfAngle < (F32) 1.0/MAX_BEVEL)
						{
							fCosHalfAngle = (F32) 1.0/MAX_BEVEL;
						}
						F32 fMag = (F32) fBevelDepth / fCosHalfAngle;

						IFXVector3 a,b,c;
						a.Subtract(vNormal,vNextNormal);
						b.CrossProduct(a,IFXVector3(0,0,-1));
						c.Add(vNextNormal,vNormal);
						vBevel.Add(b,c);

						vBevel.Normalize();
						vBevel.Scale(fMag);
					}
					else {
						vBevel=vNormal.Scale(fBevelDepth);
					}
				}

				pvPosition=v3iPosition.Next(); 
				pvPosition->Set((F32)vContourPosition.x, (F32)vContourPosition.y, 0);

				pvNormal=v3iNormal.Next();
				if(pExtruderProperties->bNormals==IFX_FALSE)
					pvNormal->Set(0, 0, 0);
				else if( IFX_TRUE == bBevelFront)
				{
					if(pExtruderProperties->eNormalOrientation==IFXGlyphInverseNormals) 
					{
						pvNormal->Set(-(F32)vContourNormal.x, -(F32)vContourNormal.y, 0);
					}
					else 
					{
						pvNormal->Set((F32)vContourNormal.x, (F32)vContourNormal.y, 0);
					}
					pvNormal->Add(vZDirection);
					pvNormal->Normalize();
				}
				else  // No bevel on the front size.  Normals just point in the z direction.
				{
						pvNormal->Set(0, 0, 1);
				}
				uCurrentMeshVertexIndex++;


				pvPosition=v3iPosition.Next(); 
				if( IFX_TRUE == bBevelFront) 
				{
					pvPosition->Set((F32)vContourPosition.x, (F32)vContourPosition.y, -fBevelDepth);
				}
				else
				{
					pvPosition->Set((F32)vContourPosition.x, (F32)vContourPosition.y, 0);
				}

				if(pExtruderProperties->eNormalOrientation==IFXGlyphInverseNormals)
				{
					pvPosition->Subtract(vBevel);
				}
				else
				{
					pvPosition->Add(vBevel);
				}

				pvNormal=v3iNormal.Next();
				if(pExtruderProperties->bNormals==IFX_FALSE)
					pvNormal->Set(0, 0, 0);
				else if( IFX_TRUE == bBevelFront)
				{
					if(pExtruderProperties->eNormalOrientation==IFXGlyphInverseNormals) 
					{
						pvNormal->Set(-(F32)vContourNormal.x, -(F32)vContourNormal.y, 0);
					}
					else 
					{
						pvNormal->Set((F32)vContourNormal.x, (F32)vContourNormal.y, 0);
					}
					if(bRoundBevel==IFX_FALSE) {
						pvNormal->Add(vZDirection);
						pvNormal->Normalize();
					}
				}
				else  // No bevel on the front size.  Normals just point in the z direction.
				{
						pvNormal->Set(0, 0, 1);
				}
				uCurrentMeshVertexIndex++;

			}

			{// create two faces
				pfFace=fiFace.Next();
				uCurrentFaceCount++;
				if(pExtruderProperties->bWindClockWise==IFX_TRUE) 
					pfFace->Set(uCurrentMeshVertexIndex-2, uCurrentMeshVertexIndex-4, uCurrentMeshVertexIndex-3);
				else
					pfFace->Set(uCurrentMeshVertexIndex-2, uCurrentMeshVertexIndex-3, uCurrentMeshVertexIndex-4);

				pfFace=fiFace.Next();
				uCurrentFaceCount++;
				if(pExtruderProperties->bWindClockWise==IFX_TRUE) 
					pfFace->Set(uCurrentMeshVertexIndex-2, uCurrentMeshVertexIndex-3, uCurrentMeshVertexIndex-1);
				else
					pfFace->Set(uCurrentMeshVertexIndex-2, uCurrentMeshVertexIndex-1, uCurrentMeshVertexIndex-3);

			}
		}


		if(IFXSUCCESS(iResult)) {
			*ppMesh=pMesh;
		}

		
	}
	return iResult;
}




// CIFXContourExtruder private methods

IFXRESULT CIFXContourExtruder::DetermineVertexCount(IFXContour* pContour, U32* pVertexCount, U32* pFaceCount) {
	IFXRESULT iResult=IFX_OK;
	
	if(pContour==NULL || pVertexCount==NULL || pFaceCount==NULL) 
		iResult=IFX_E_INVALID_POINTER;
	
	if(IFXSUCCESS(iResult)) {
		// walk the glyph, and determine how many unique vertices we have
		U32 uVertexCount=0;
		//U32 uVertexIndex=0;
		iResult = pContour->GetCount(&uVertexCount);

		*pVertexCount=uVertexCount;
		*pFaceCount=uVertexCount;

		return iResult;
	}

	return iResult;
}
CIFXContourExtruder::CIFXContourExtruder() {
	m_uRefCount=0;
}

CIFXContourExtruder::~CIFXContourExtruder() {
}
