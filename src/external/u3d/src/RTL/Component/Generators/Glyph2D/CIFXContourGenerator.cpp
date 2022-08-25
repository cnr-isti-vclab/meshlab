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
//	CIFXContourGenerator.cpp
//
//	DESCRIPTION
//		Implementation file for the contour generator class.
//
//	NOTES
//      None.
//
//***************************************************************************

#include "CIFXContourGenerator.h"
#include "IFXVector3.h"
#include "IFXVector2.h"
#include <float.h>
#include "IFXCoreCIDs.h"

//#define DBL_MAX         1.7976931348623158e+308 /* max value */


// IFXUnknown methods

//---------------------------------------------------------------------------
//	CIFXContourGenerator::AddRef
//
//	This method increments the reference count for an interface on a
//	component.  It should be called for every new copy of a pointer to an
//	interface on a given component.  It returns a U32 that contains a value
//	from 1 to 2^32 - 1 that defines the new reference count.  The return
//	value should only be used for debugging purposes.
//---------------------------------------------------------------------------
U32 CIFXContourGenerator::AddRef(void)
{
	return ++m_uRefCount;
} 	

//---------------------------------------------------------------------------
//	CIFXContourGenerator::Release
//
//	This method decrements the reference count for the calling interface on a
//	component.  It returns a U32 that contains a value from 1 to 2^32 - 1
//	that defines the new reference count.  The return value should only be
//	used for debugging purposes.  If the reference count on a component falls
//	to zero, the component is destroyed.
//---------------------------------------------------------------------------
U32 CIFXContourGenerator::Release(void)
{
	if( 1 == m_uRefCount )
	{
//		CIFXModifier::PreDestruct();
		delete this;
		return 0;
	}

	return --m_uRefCount;
} 	

//---------------------------------------------------------------------------
//	CIFXContourGenerator::QueryInterface
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
IFXRESULT CIFXContourGenerator::QueryInterface( IFXREFIID interfaceId, void** ppInterface ){
	IFXRESULT	result	= IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXContourGenerator ||
			 interfaceId == IID_IFXUnknown )
			*ppInterface = ( IFXContourGenerator* ) this;
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
//	CIFXContourGenerator_Factory (non-singleton)
//
//	This is the CIFXContourGenerator component factory function.  The
//	CIFXContourGenerator component is NOT a singleton.  This function creates the
//  TextGenerator object, addref()'s it and returns it.
//---------------------------------------------------------------------------
IFXRESULT IFXAPI_CALLTYPE CIFXContourGenerator_Factory( IFXREFIID	interfaceId,
									void**		ppInterface )
{
	IFXRESULT	result;

	if ( ppInterface )
	{
		// It doesn't exist, so try to create it.
		CIFXContourGenerator	*pComponent	= new CIFXContourGenerator;

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



// IFX3DTextCallback methods

//---------------------------------------------------------------------------
//	CIFXContourGenerator::StartPath
//
//	This method begins a new contour in the list.  It closes out any previously
//  started contours before beginning a new one.
//---------------------------------------------------------------------------
IFXRESULT CIFXContourGenerator::StartPath(){
	IFXRESULT iResult=IFX_OK;
	
	if(m_pGlyphList==NULL)
		iResult=IFX_E_NOT_INITIALIZED;

	if(IFXSUCCESS(iResult)) {
		// someone tried to start a new path w/o closing the old one
		// save intermediate results and start new path
		if(m_pCurrentPath) 
			iResult=EndPath();
		
		// create new contour/path77
		if(IFXSUCCESS(iResult)) {
			iResult=IFXCreateComponent(CID_IFXContour, IID_IFXContour,
				(void**) &m_pCurrentPath);
			if(IFXSUCCESS(iResult) && m_pCurrentPath!=NULL) 
				iResult=m_pCurrentPath->Initialize(5);
		}
	}
	return iResult;
}

//---------------------------------------------------------------------------
//	CIFXContourGenerator::LineTo
//
//	This method adds another point to the current contour
//---------------------------------------------------------------------------
IFXRESULT CIFXContourGenerator::LineTo(F64 fX, F64 fY){
	IFXRESULT iResult=IFX_OK;
	if(m_pGlyphList==NULL)
		iResult=IFX_E_NOT_INITIALIZED;
	if(m_pCurrentPath==NULL)
		iResult=IFX_E_UNDEFINED;

	if(IFXSUCCESS(iResult)) {
		U32 uDummy=0;
		SIFXContourPoint vPoint;
		vPoint.x=fX; vPoint.y=fY; vPoint.z=0;
		SIFXContourPoint vNormal;
		vNormal.x=0; vNormal.y=0; vNormal.z=0;


		F64 eps = 1e-4;
		if
		(	fabs(vPoint.x - m_vLastPoint.x)  < eps 	&& 	
			fabs(vPoint.y - m_vLastPoint.y)  < eps 
		)
		{
			IFXRETURN(iResult);
		}


		// do cross product of prev point and this one
		//			IFXVector3 v3Line(vPoint.X()-vPrevPoint.X(), vPoint.Y()-vPrevPoint.Y(), 0);
		IFXVector3 v3Line((F32)(m_vLastPoint.x-vPoint.x), (F32)(m_vLastPoint.y-vPoint.y), (F32)(m_vLastPoint.z-vPoint.z));
		IFXVector3 v3Reference(0,0,1);
		IFXVector3 v3Normal;
		v3Normal.CrossProduct(v3Line, v3Reference);
		if(v3Normal.X()!=0 || v3Normal.Y()!=0 || v3Normal.Z()!=0)
			v3Normal.Normalize();
		
		// calculate normal to line you just drew
		vNormal.x=v3Normal.X(); vNormal.y=v3Normal.Y(); vNormal.z=v3Normal.Z();


		iResult=m_pCurrentPath->AddNext(&m_vLastPoint, &vNormal, &uDummy);
		if(!IFXSUCCESS(iResult)) { IFXRETURN(iResult); }
		AddBoundingBox(m_vLastPoint.x , m_vLastPoint.y);

		iResult=m_pCurrentPath->AddNext(&vPoint, &vNormal, &uDummy);
		if(!IFXSUCCESS(iResult)) { IFXRETURN(iResult); }
		AddBoundingBox(vPoint.x , vPoint.y);

		m_vLastPoint = vPoint;

	}
	return iResult;
}

IFXRESULT CIFXContourGenerator::MoveTo(F64 fX, F64 fY){
	IFXRESULT iResult=IFX_OK;
	if(m_pGlyphList==NULL)
		iResult=IFX_E_NOT_INITIALIZED;
	if(m_pCurrentPath==NULL)
		iResult=IFX_E_UNDEFINED;

	if(IFXSUCCESS(iResult)) {
		SIFXContourPoint vPoint;
		vPoint.x=fX; vPoint.y=fY; vPoint.z=0;
		m_vLastPoint = vPoint;
	}

	return iResult;
}
IFXRESULT CIFXContourGenerator::CurveTo(F64 fCx1, F64 fCy1, F64 fCx2, F64 fCy2, F64 fAx, F64 fAy, U32 uNumberOfSteps){
	IFXRESULT iResult=IFX_OK;
	if(m_pGlyphList==NULL)
		iResult=IFX_E_NOT_INITIALIZED;
	if(m_pCurrentPath==NULL)
		iResult=IFX_E_UNDEFINED;
	if(uNumberOfSteps < 1)
		iResult=IFX_E_INVALID_RANGE;

	// dummy implementation - need to calculate curve/normal 
	// tesselation here
	if(IFXSUCCESS(iResult)) {
		U32 uDummy=0;
		SIFXContourPoint vPoint;
		vPoint.x=fCx1; vPoint.y=fCy1; vPoint.z=0;
		SIFXContourPoint vNormal;
		vNormal.x=0; vNormal.y=0; vNormal.z=0;

		F64 fAx1, fAy1;

		fAx1=m_vLastPoint.x;
		fAy1=m_vLastPoint.y;


		// check to see if this curve goes anywhere.
		// could be aksing for numsteps of the same point.
		F64 eps = 1e-4;
		if( fabs(fAx1 - fAx)  < eps && 
			fabs(fAy1 - fAy)  < eps && 	
			fabs(fCx1 - fCx2) < eps && 
			fabs(fCy1 - fCy2) < eps 
			)
		{
			//IFXASSERT(0);
			//skip this curve.  end points and control points are the same.
		}
		else  {


			F64 fU, fU2, fU3;
			F64 f1MU, f1MU2, f1MU3;

			F64 fX,fY;
			F64 fDXDU;
			F64 fDYDU;

			F64 fStepSize = 1.0/uNumberOfSteps;
			U32 i;
			
		
			for(fU=0, i = 0; i <= uNumberOfSteps; i++, fU+=fStepSize) {
				
				fU2 = fU * fU; 
				fU3 = fU2 * fU;
				f1MU  = 1.0 - fU;
				f1MU2 = f1MU*f1MU;  
				f1MU3 = f1MU2*f1MU;

				// Positoin at U
				fX = 
					fAx1 * f1MU3 
					+ fCx1 * 3.0 * fU * f1MU2 
					+ fCx2 * 3.0 * fU2 * f1MU 
					+ fAx * fU3
					;

				fY = 
					fAy1 * f1MU3 
					+ fCy1 * 3.0 * fU * f1MU2 
					+ fCy2 * 3.0 * fU2 * f1MU 
					+ fAy * fU3
					;

				vPoint.x=fX; 	
				vPoint.y=fY; 	
				vPoint.z=0;

				// Tangent vector at U
				fDXDU = 
					- 3 * f1MU2 * fAx1
					- 6 * fU * f1MU * fCx1 + 3 * f1MU2 * fCx1
					+ 6 * fU * f1MU * fCx2 - 3 * fU2 * fCx2
					+ 3 * fU2 * fAx
					;

				fDYDU = 
					- 3 * f1MU2 * fAy1
					- 6 * fU * f1MU * fCy1 + 3 * f1MU2 * fCy1
					+ 6 * fU * f1MU * fCy2 - 3 * fU2 * fCy2
					+ 3 * fU2 * fAy
					;

				// check tangent vector.  could be Zero if the curve is a
				// straight line, (i.e. m_vLastPoint ==  [fCx1, fCy1] and [fCx2,fCy2] == [fAx, fAy] )
				IFXVector3 v3Normal;
				F64 eps = 1e-6;
				if ( fabs(fDXDU) > eps || fabs(fDYDU) > eps)
				{

					// calculate normal form tangent vector.
					// do cross product of tangent vector and unit vector z to find the normal
					IFXVector3 v3Line((F32)(fDXDU), (F32)(fDYDU), (F32)(0.0));
					IFXVector3 v3Reference(0,0,-1);
					v3Normal.CrossProduct(v3Line,v3Reference);

					if(v3Normal.X()!=0 || v3Normal.Y()!=0 || v3Normal.Z()!=0)
					{
						v3Normal.Normalize();
					}
					else
					{
						//IFXASSERT(0);  // should not go here. 
						// Normal sould not be zero if FDXDU and fDYDU are both > 0.
					}

				}
				else
				{
					// calculate normal if curve is a straight line.

					// do cross product of tangent vector and unit vector z to find the normal
					IFXVector3 v3Line((F32)(fAx - fAx1), (F32)(fAy - fAy1), (F32)(0.0));
					IFXVector3 v3Reference(0,0,-1);
					v3Normal.CrossProduct(v3Line,v3Reference);

					if(v3Normal.X()!=0 || v3Normal.Y()!=0 || v3Normal.Z()!=0)
					{
						v3Normal.Normalize();
					}
					else
					{
						// IFXASSERT(0);  
						// Curve goes nowhere.  Normal will just have to be (0,0,0).
					}
				}
			
				vNormal.x=v3Normal.X(); 
				vNormal.y=v3Normal.Y(); 
				vNormal.z=v3Normal.Z();

				iResult=m_pCurrentPath->AddNext(&vPoint, &vNormal, &uDummy);

				if(!IFXSUCCESS(iResult)) 
				{ 
					break; 
				}
				AddBoundingBox(vPoint.x , vPoint.y);

				if(i!=0 && i < uNumberOfSteps) {
					iResult=m_pCurrentPath->AddNext(&vPoint, &vNormal, &uDummy);
					if(!IFXSUCCESS(iResult)) 
					{ 
						break; 
					}
					AddBoundingBox(vPoint.x , vPoint.y);

				}

			}
		}
	}

	m_vLastPoint.x=fAx;
	m_vLastPoint.y=fAy;

	return iResult;
}

IFXRESULT CIFXContourGenerator::EndPath() {
	IFXRESULT iResult=IFX_OK;

	if(m_pGlyphList==NULL)
		iResult=IFX_E_NOT_INITIALIZED;

	if(IFXSUCCESS(iResult)) {
		if(m_pCurrentPath!=NULL) {
			// is path zero in length?
			U32 uCount=0;
			m_pCurrentPath->GetCount(&uCount);
			if(uCount!=0) {
				// close the path by repeating first point - flag??

				SIFXContourPoint vPoint0;
				SIFXContourPoint vPoint1;

				SIFXContourPoint vNormal;
				vNormal.x=0; vNormal.y=0; vNormal.z=0;
				U32 uDummy=0;

				// close the path by repeating first point - flag??
				if(IFXSUCCESS(iResult))	{
					vPoint0 = m_vLastPoint;
					//iResult=m_pCurrentPath->Get(uCount-1,&vPoint0, &vNormal);
				}

				if(IFXSUCCESS(iResult))	{
					iResult=m_pCurrentPath->Get(0,&vPoint1, &vNormal);
				
					// do cross product of prev point and this one
					//			IFXVector3 v3Line(vPoint.X()-vPrevPoint.X(), vPoint.Y()-vPrevPoint.Y(), 0);
					IFXVector3 v3Line((F32)(vPoint0.x-vPoint1.x), (F32)(vPoint0.y-vPoint1.y), (F32)(vPoint0.z-vPoint1.z));
					IFXVector3 v3Reference(0,0,1);
					IFXVector3 v3Normal;
					v3Normal.CrossProduct(v3Line,v3Reference);

					if(v3Normal.X()!=0 || v3Normal.Y()!=0 || v3Normal.Z()!=0)
						v3Normal.Normalize();
				
					// calculate normal to line you just drew
					vNormal.x=v3Normal.X(); vNormal.y=v3Normal.Y(); vNormal.z=v3Normal.Z();
					
				}


				// close contour if it is not already closed.  
				F64 eps = 1e-4;
				if( fabs(vPoint0.x - vPoint1.x) > eps || fabs(vPoint0.y - vPoint1.y ) > eps) 	{

					if(IFXSUCCESS(iResult))	{
						iResult=m_pCurrentPath->AddNext(&vPoint0, &vNormal, &uDummy);
						AddBoundingBox(vPoint0.x , vPoint0.y);
					}

					if(IFXSUCCESS(iResult))	{
						iResult=m_pCurrentPath->AddNext(&vPoint1, &vNormal, &uDummy);
						AddBoundingBox(vPoint1.x , vPoint1.y);
					}
				}

				if(IFXSUCCESS(iResult))	{	
					// remove any contour violations
					iResult=ContourCleanup();
				}
				
				if(IFXSUCCESS(iResult)) {
					// save path into contour list and release all ref's
					U32 uDummy=0;
					IFXUnknown* pObject=NULL;
					iResult=m_pCurrentPath->QueryInterface(IID_IFXUnknown, (void**)&pObject);
					if(IFXSUCCESS(iResult)) {
						m_pGlyphList->Add(pObject, &uDummy);
					}
					IFXRELEASE(pObject);
				}
			}

		}
	}
	IFXRELEASE(m_pCurrentPath);
	return iResult;
}

IFXRESULT CIFXContourGenerator::GetBoundingBox(F64 pMin[2], F64 pMax[2]){
	IFXRESULT iResult=IFX_OK;

	if(m_pGlyphList==NULL)
		iResult=IFX_E_NOT_INITIALIZED;

	if(IFXSUCCESS(iResult)) {

		//IFXRESULT iResult=IFX_OK;

		pMin[0]=m_pBoundingBoxMin[0];
		pMin[1]=m_pBoundingBoxMin[1];

		pMax[0]=m_pBoundingBoxMax[0];
		pMax[1]=m_pBoundingBoxMax[1];

	}
	return iResult;
}

void CIFXContourGenerator::AddBoundingBox(F64 fX, F64 fY)
{
	if( fX < m_pBoundingBoxMin[0] ) m_pBoundingBoxMin[0]=fX;
	if( fY < m_pBoundingBoxMin[1] ) m_pBoundingBoxMin[1]=fY;

	if( fX > m_pBoundingBoxMax[0] ) m_pBoundingBoxMax[0]=fX;
	if( fY > m_pBoundingBoxMax[1] ) m_pBoundingBoxMax[1]=fY;
}


// IFXContourGenerator methods

IFXRESULT CIFXContourGenerator::Initialize() {
	IFXRESULT iResult=IFX_OK;
	iResult=IFXCreateComponent(CID_IFXSimpleList, IID_IFXSimpleList,
							(void**) &m_pGlyphList);

	if(IFXSUCCESS(iResult) && m_pGlyphList)
		m_pGlyphList->Initialize(1);

	return iResult;
}

IFXRESULT CIFXContourGenerator::GetContourList(IFXSimpleList* pGlyphList){
	IFXRESULT iResult=IFX_OK;
	
	if(pGlyphList==NULL)
		iResult=IFX_E_INVALID_POINTER;
	if(m_pGlyphList==NULL)
		iResult=IFX_E_NOT_INITIALIZED;

	if(IFXSUCCESS(iResult)) 
		m_pGlyphList->Copy(pGlyphList);

	return iResult;
}


// CIFXContourGenerator private methods

IFXRESULT CIFXContourGenerator::ContourCleanup() {
	IFXRESULT iResult=IFX_OK;

	return iResult;
}

CIFXContourGenerator::CIFXContourGenerator() {
	m_uRefCount=0;
	m_pGlyphList=NULL;
	m_pCurrentPath=NULL;

	m_pBoundingBoxMin[0]=DBL_MAX;
	m_pBoundingBoxMax[0]=-DBL_MAX;
	m_pBoundingBoxMin[1]=DBL_MAX;
	m_pBoundingBoxMax[1]=-DBL_MAX;


}

CIFXContourGenerator::~CIFXContourGenerator() {


	IFXRELEASE(m_pGlyphList);
	IFXRELEASE(m_pCurrentPath);
}
