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
//	CIFXContourTessellator.cpp
//
//	DESCRIPTION
//		Implementation file for the Contour tesselator class.
//
//	NOTES
//      None.
//
//***************************************************************************


//#define TESSELLATE_DEBUG
#ifndef TESSELLATE_DEBUG
#else
#include <stdio.h>
#endif

#include "IFXCoreCIDs.h"

#include "CIFXContourTessellator.h"

#include "float.h"
#include "CIFXQuadEdge.h"
#include "predicates.h"


// IFXUnknown methods

//---------------------------------------------------------------------------
//	CIFXContourTessellator::AddRef
//
//	This method increments the reference count for an interface on a
//	component.  It should be called for every new copy of a pointer to an
//	interface on a given component.  It returns a U32 that contains a value
//	from 1 to 2^32 - 1 that defines the new reference count.  The return
//	value should only be used for debugging purposes.
//---------------------------------------------------------------------------
U32 CIFXContourTessellator::AddRef(void) {
	return ++m_uRefCount;
} 	

//---------------------------------------------------------------------------
//	CIFXContourTessellator::Release
//
//	This method decrements the reference count for the calling interface on a
//	component.  It returns a U32 that contains a value from 1 to 2^32 - 1
//	that defines the new reference count.  The return value should only be
//	used for debugging purposes.  If the reference count on a component falls
//	to zero, the component is destroyed.
//---------------------------------------------------------------------------
U32 CIFXContourTessellator::Release(void) {
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
//	CIFXContourTessellator::QueryInterface
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
IFXRESULT CIFXContourTessellator::QueryInterface( IFXREFIID interfaceId, void** ppInterface ){
	IFXRESULT	result	= IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXContourTessellator ||
			 interfaceId == IID_IFXUnknown )
			*ppInterface = ( IFXContourTessellator* ) this;
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
//	CIFXContourTessellator_Factory (non-singleton)
//
//	This is the CIFXContourTessellator component factory function.  The
//	CIFXContourTessellator component is NOT a singleton.  This function creates the
//  TextGenerator object, addref()'s it and returns it.
//---------------------------------------------------------------------------
IFXRESULT IFXAPI_CALLTYPE CIFXContourTessellator_Factory( IFXREFIID	interfaceId,
									void**		ppInterface )
{
	IFXRESULT	result=IFX_OK;

	if ( ppInterface )
	{
		// It doesn't exist, so try to create it.
		CIFXContourTessellator	*pComponent	= new CIFXContourTessellator;

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
		{
			IFXASSERT(0);
			result = IFX_E_OUT_OF_MEMORY;
		}
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}

static
IFXRESULT TessellateMeshBuilder
(
CIFXSubdivision *pCDTAlgo,
SIFXTessellatorProperties* pTessellatorProperties, 
IFXMeshGroup** ppMeshGroup
) 
{

	IFXRESULT iResult = IFX_OK;

	IFXVector3Iter v3iPosition;
	IFXVector3Iter v3iNormal;

	IFXFaceIter fiFace;
	IFXVector3* pvPosition;
	IFXVector3* pvNormal;
	IFXFace*	pfFace;

	IFXVector3 v3PositionA;
	//IFXVector3 v3PositionB;
	//IFXVector3 v3PositionC;

	U16 indexA;
	U16 indexB;
	U16 indexC;

	U32 uVertexIndex;

	//IFXUnknown* pUnknown=NULL;
	//IFXContour* pContour=NULL;
	
	
	
	if(IFXSUCCESS(iResult)) {

		U32 uTriangleCount = pCDTAlgo->m_triList.GetNumberElements();
		U32	uVertexCount=pCDTAlgo->m_uNPoints;

		IFXMesh* pMesh=NULL;

			// build a mesh group big enough to contain the glyph (list of contours)
		U32 uMeshIndex = 0;
		if(IFXSUCCESS(iResult)) {
			U32 uMeshGroupCount=1;
			if(pTessellatorProperties->eFacing==IFXGlyphBothFacing) 
				uMeshGroupCount=2;

			IFXCreateComponent(CID_IFXMeshGroup, IID_IFXMeshGroup, (void**)ppMeshGroup);
			if(*ppMeshGroup)
				iResult=(*ppMeshGroup)->Allocate(uMeshGroupCount);
		}


		SIFXPoint2d vert;
		F32 fDepth = static_cast<F32>(pTessellatorProperties->fDepth);

		if(pTessellatorProperties->eFacing==IFXGlyphOutFacing ||
			pTessellatorProperties->eFacing==IFXGlyphBothFacing) 
		{
	
			// allocate an appropriate sized mesh
			if(IFXSUCCESS(iResult)) {

				if(IFXSUCCESS(iResult)) {
				
					IFXCreateComponent(CID_IFXMesh, IID_IFXMesh, (void**)&pMesh);
					if(pMesh==NULL)
					{
						IFXASSERT(0);
						iResult=IFX_E_OUT_OF_MEMORY;
					}
					if(IFXSUCCESS(iResult)) {
						IFXVertexAttributes vertexAttributes;
						iResult=pMesh->Allocate(vertexAttributes, uVertexCount, uTriangleCount);	
					}
				}
			}

			if(IFXSUCCESS(iResult)) {

				// get the iterators
				pMesh->GetPositionIter(v3iPosition);
				pMesh->GetNormalIter(v3iNormal);
				pMesh->GetFaceIter(fiFace);

				for(uVertexIndex=0;uVertexIndex<pCDTAlgo->m_uNPoints;uVertexIndex++) 
				{
					vert = pCDTAlgo->m_pPointArray[uVertexIndex];
					v3PositionA.Set(static_cast<F32>(vert.x), static_cast<F32>(vert.y), - fDepth);

					pvPosition=v3iPosition.Next(); 
					pvNormal=v3iNormal.Next();

					//CF  Hack subtract fminPosition.  Algo fails in release mode if contour crosses x axis.
					pvPosition->Set((F32)(v3PositionA.X()), (F32)v3PositionA.Y(), (F32)v3PositionA.Z());

					if(pTessellatorProperties->bNormals==IFX_FALSE)
					{
						pvNormal->Set(0.0, 0.0, 0.0);
					}
					else 
					{
						if(pTessellatorProperties->eNormalOrientation==IFXGlyphInverseNormals)
							pvNormal->Set(0.0, 0.0, -1.0);
						else
							pvNormal->Set(0.0, 0.0, 1.0);
					}
				}


				IFXListContext context;
				pCDTAlgo->m_triList.ToHead(context);
				SIFXIndexTriangle *tnode;

				U32 count=0;
				while((tnode=pCDTAlgo->m_triList.PostIncrement(context)) != NULL)
				{
					count++;

					if(count > uTriangleCount)
					{
						IFXASSERT(0);
						break;
					}

					indexA = tnode->ia;
					indexB = tnode->ib;
					indexC = tnode->ic;

					pfFace=fiFace.Next();

					if(pTessellatorProperties->bWindClockWise==IFX_TRUE) 
						pfFace->Set(indexC, indexB, indexA);
					else
						pfFace->Set(indexA, indexB, indexC);

				}

				(*ppMeshGroup)->SetMesh(uMeshIndex, pMesh);
				IFXRELEASE(pMesh);
				uMeshIndex++;
			}

		}

		if(pTessellatorProperties->eFacing==IFXGlyphInFacing ||
			pTessellatorProperties->eFacing==IFXGlyphBothFacing) 
		{

			// allocate an appropriate sized mesh
			if(IFXSUCCESS(iResult)) {
			
				if(IFXSUCCESS(iResult)) {
					
					IFXCreateComponent(CID_IFXMesh, IID_IFXMesh, (void**)&pMesh);
					if(pMesh==NULL)
					{
						IFXASSERT(0);
						iResult=IFX_E_OUT_OF_MEMORY;
					}
					if(IFXSUCCESS(iResult)) {
						IFXVertexAttributes vertexAttributes;
						iResult=pMesh->Allocate(vertexAttributes, uVertexCount, uTriangleCount);	
					}
				}
			}

			if(IFXSUCCESS(iResult)) {
				// get the iterators
				pMesh->GetPositionIter(v3iPosition);
				pMesh->GetNormalIter(v3iNormal);
				pMesh->GetFaceIter(fiFace);


				for(uVertexIndex=0;uVertexIndex<pCDTAlgo->m_uNPoints;uVertexIndex++) 
				{

					vert = pCDTAlgo->m_pPointArray[uVertexIndex];
					v3PositionA.Set(static_cast<F32>(vert.x), static_cast<F32>(vert.y), - fDepth);

					pvPosition=v3iPosition.Next(); 
					pvNormal=v3iNormal.Next();

					//CF  Hack subtract fminPosition.  Algo fails in release mode if contour crosses x axis.
					pvPosition->Set((F32)(v3PositionA.X()), (F32)v3PositionA.Y(), (F32)v3PositionA.Z());

					if(pTessellatorProperties->bNormals==IFX_FALSE)
					{
						pvNormal->Set(0.0, 0.0, 0.0);
					}
					else 
					{
						if(pTessellatorProperties->eNormalOrientation==IFXGlyphInverseNormals)
							pvNormal->Set(0.0, 0.0, 1.0);
						else
							pvNormal->Set(0.0, 0.0, -1.0);
					}
				
				}

				IFXListContext context;
				pCDTAlgo->m_triList.ToHead(context);
				SIFXIndexTriangle *tnode;
				U32 count = 0;
				while((tnode=pCDTAlgo->m_triList.PostIncrement(context)) != NULL)
				{
					count++;

					if(count > uTriangleCount)
					{
						IFXASSERT(0);
						break;
					}

					indexA = tnode->ia;
					indexB = tnode->ib;
					indexC = tnode->ic;

					pfFace=fiFace.Next();

					if(pTessellatorProperties->bWindClockWise==IFX_TRUE) 
						pfFace->Set(indexA, indexB, indexC);
					else
						pfFace->Set(indexC, indexB, indexA);
				
				}

				(*ppMeshGroup)->SetMesh(uMeshIndex, pMesh);
				IFXRELEASE(pMesh);
				uMeshIndex++;
			}
		}
	}

	return iResult;

}


// IFXTextGenerator methods

IFXRESULT CIFXContourTessellator::Tessellate
(
 IFXSimpleList* pGlyphList,
 SIFXTessellatorProperties* pTessellatorPropertiesFront, 
 IFXMeshGroup** ppMeshGroupFront,
 SIFXTessellatorProperties* pTessellatorPropertiesBack, 
 IFXMeshGroup** ppMeshGroupBack
) 
{

	IFXRESULT iResult=IFX_OK;

	if(	pGlyphList==NULL  ) 
	{
		IFXASSERT(0);
		iResult=IFX_E_INVALID_POINTER;
	}

	//IFXVector3Iter v3iPosition;
	//IFXVector3Iter v3iNormal;

	IFXFaceIter fiFace;

	//IFXVector3 v3PositionA;
	//IFXVector3 v3PositionB;
	//IFXVector3 v3PositionC;


	U32 uVertexIndex;
	U32 uPathCount, uPathIndex;
	U32 uVertexCount;

	IFXUnknown* pUnknown=NULL;
	IFXContour* pContour=NULL;
	SIFXContourPoint vContourPosition;
	

	SIFXPoint2d p1(-FLT_MAX,-FLT_MAX), p2(FLT_MAX,FLT_MAX); 
	CIFXSubdivision cdtAlgo;
	iResult = cdtAlgo.Initialize(p1, p2);
	IFXList<SIFXEdge> EdgeSet;
	EdgeSet.SetAutoDestruct(TRUE);

	if(IFXSUCCESS(iResult)) {


		SIFXPoint2d point0,point1;
		void **error;

		uPathCount = 0;
		iResult = pGlyphList->GetCount(&uPathCount);

#ifdef TESSELLATE_DEBUG

		U32 uPolygonCount=0;

		FILE *fd = fopen("TessellateDebug.b","wb");


		// walk the path list

		for(uPathIndex=0;uPathIndex<uPathCount && IFXSUCCESS(iResult);uPathIndex++) 
		{

			pUnknown=NULL;
			pGlyphList->Get(uPathIndex, &pUnknown);
			iResult=pUnknown->QueryInterface(IID_IFXContour, (void**)&pContour);
			if(IFXSUCCESS(iResult) && pContour) 
			{
				uVertexCount=0;
				pContour->GetCount(&uVertexCount);
		
				for(uVertexIndex=0;uVertexIndex<uVertexCount;uVertexIndex+=2) {
					pContour->GetPosition(uVertexIndex,&vContourPosition);

					point0.x = vContourPosition.x;
					point0.y = vContourPosition.y;

					pContour->GetPosition(uVertexIndex+1,&vContourPosition);

					point1.x = vContourPosition.x;
					point1.y = vContourPosition.y;

//					if(point0.x != point1.x || point0.y != point1.y)
//					{
						fwrite((void*)&uPolygonCount,sizeof(U32),1,fd);
						fwrite((void*)&point0.x,sizeof(F64),1,fd);
						fwrite((void*)&point0.y,sizeof(F64),1,fd);

						fwrite((void*)&uPolygonCount,sizeof(U32),1,fd);
						fwrite((void*)&point1.x,sizeof(F64),1,fd);
						fwrite((void*)&point1.y,sizeof(F64),1,fd);
//					}

				}
				uPolygonCount++;
				IFXRELEASE(pContour);
				IFXRELEASE(pUnknown);
			}
		}

		fclose(fd);
#endif //TESSELLATE_DEBUG


		IFXRESULT iTesselateResult=IFX_OK;
		for(uPathIndex=0;uPathIndex<uPathCount && IFXSUCCESS(iResult); uPathIndex++) 
		{
			pUnknown=NULL;
			pGlyphList->Get(uPathIndex, &pUnknown);
			iResult=pUnknown->QueryInterface(IID_IFXContour, (void**)&pContour);
			if(IFXSUCCESS(iResult) && pContour) 
			{
				uVertexCount=0;
				pContour->GetCount(&uVertexCount);
		
				for(uVertexIndex=0;uVertexIndex<uVertexCount;uVertexIndex+=2) {
					pContour->GetPosition(uVertexIndex,&vContourPosition);

					point0.x = vContourPosition.x;
					point0.y = vContourPosition.y;

					pContour->GetPosition(uVertexIndex+1,&vContourPosition);

					point1.x = vContourPosition.x;
					point1.y = vContourPosition.y;

					if(point0.x != point1.x || point0.y != point1.y)
					{
						SIFXEdge *newEdge = new SIFXEdge;
						error = reinterpret_cast<void **>(EdgeSet.Append(newEdge));
						if(error==NULL)
						{
							IFXASSERT(0);
							iResult = IFX_E_OUT_OF_MEMORY;
							break;
						}

						newEdge->a = point0;
						newEdge->b = point1;

						if(IFXSUCCESS(iTesselateResult))
						{
							iTesselateResult = cdtAlgo.IntersectInsertEdge(point0, point1);
						}

					}

				}
				IFXRELEASE(pContour);
				IFXRELEASE(pUnknown);
			}
		}


		if(IFXSUCCESS(iResult)) 
		{
			iResult = iTesselateResult;
		}


	}


	if(IFXSUCCESS(iResult)) 
	{
		iResult = cdtAlgo.GenerateTriangleList(EdgeSet);
	}


	if(IFXSUCCESS(iResult)) 
	{
		if(cdtAlgo.m_pPointArray != NULL &&
		cdtAlgo.m_triList.GetNumberElements() > 0)
		{
			//cdtAlgo has valid mesh data.  continue;
		}
		else
		{
			//cdtdata is not vaild.
			iResult = IFX_E_ABORTED;
		}

	}

	if(IFXSUCCESS(iResult)) 
	{
		if(pTessellatorPropertiesFront!=NULL && ppMeshGroupFront!=NULL)
			iResult = TessellateMeshBuilder(&cdtAlgo,pTessellatorPropertiesFront, ppMeshGroupFront);
	}
	
	if(IFXSUCCESS(iResult)) 
	{
		if(pTessellatorPropertiesBack!=NULL && ppMeshGroupBack!=NULL )
			iResult = TessellateMeshBuilder(&cdtAlgo,pTessellatorPropertiesBack, ppMeshGroupBack); 
	}

	EdgeSet.DeleteAll();
	return iResult;

}

IFXRESULT CIFXContourTessellator::Tessellate(SIFXTessellatorProperties* pTessellatorProperties, IFXContour* pGlyph, IFXMesh** ppMesh) {
	IFXRESULT iResult=IFX_E_UNSUPPORTED;
	return iResult;
}

IFXRESULT CIFXContourTessellator::Tessellate(SIFXTessellatorProperties* pTessellatorProperties, IFXSimpleList* pGlyphList, IFXMeshGroup** ppMeshGroup) 
{
	IFXRESULT iResult=IFX_E_UNSUPPORTED;
	return iResult;
}

// CIFXContourTessellator private methods

CIFXContourTessellator::CIFXContourTessellator() {
	m_uRefCount=0;
}

CIFXContourTessellator::~CIFXContourTessellator() {
//	m_TessellateAlgorithm.Clear();
}
