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
//  CIFXGlyph3DGenerator.cpp
//
//  DESCRIPTION
//    Implementation file for the text generator classes.
//
//***************************************************************************

#include "IFXCoreCIDs.h"
#include "CIFXGlyph3DGenerator.h"
#include "IFXCheckX.h"
#include "IFXMarkerX.h"
#include <float.h>

U32 CIFXGlyph3DGenerator::AddRef(void)
{
	return ++m_uRefCount;
}


U32 CIFXGlyph3DGenerator::Release(void)
{
	if( 1 == m_uRefCount )
	{
		//    CIFXModifier::PreDestruct();
		delete this;
		return 0;
	}

	return --m_uRefCount;
}


IFXRESULT CIFXGlyph3DGenerator::QueryInterface( IFXREFIID interfaceId, void** ppInterface ){
	IFXRESULT iResult = IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXGlyph3DGenerator ||
			interfaceId == IID_IFXUnknown )
		{
			*ppInterface = ( IFXGlyph3DGenerator* ) this;
		}
		else if ( interfaceId == IID_IFXMarker )
		{
			*ppInterface = ( IFXMarker* ) this;
		}
		else if ( interfaceId == IID_IFXMarkerX )
		{
			*ppInterface = ( IFXMarkerX* ) this;
		}
		else
		{
			*ppInterface = NULL;

			iResult = IFX_E_UNSUPPORTED;
		}

		if ( IFXSUCCESS( iResult ) )
			( ( IFXUnknown* ) *ppInterface )->AddRef();
	}
	else
		iResult = IFX_E_INVALID_POINTER;

	IFXRETURN(iResult);

}


IFXRESULT IFXAPI_CALLTYPE CIFXGlyph3DGenerator_Factory( IFXREFIID interfaceId,
									   void**   ppInterface )
{
	IFXRESULT iResult;

	if ( ppInterface )
	{
		// It doesn't exist, so try to create it.
		CIFXGlyph3DGenerator  *pComponent = new CIFXGlyph3DGenerator;

		if ( pComponent )
		{
			// Perform a temporary AddRef for our usage of the component.
			pComponent->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			iResult = pComponent->QueryInterface( interfaceId, ppInterface );

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the QI fails, this will cause the
			// component to be destroyed.
			pComponent->Release();
		}
		else
			iResult = IFX_E_OUT_OF_MEMORY;
	}
	else
		iResult = IFX_E_INVALID_POINTER;

	IFXRETURN(iResult);
}


// IFXTextGenerator methods

//---------------------------------------------------------------------------
//  CIFXTextGenerator::Initialize
//
//  This method initializes and cleans up all the data used by the glyph
//  generator
//---------------------------------------------------------------------------
IFXRESULT CIFXGlyph3DGenerator::Initialize(SIFXGlyphGeneratorParams* pParams) {
	IFXRESULT iResult=IFX_OK;

	if(m_pGlyphString!=NULL || m_pContourGenerator!=NULL) {
		// clean up generator instead??
		iResult=IFX_E_ALREADY_INITIALIZED;
	}

	if(IFXSUCCESS(iResult)) {
		m_pGlyphGenParams=new SIFXGlyphGeneratorParams;
		*m_pGlyphGenParams=*pParams; // copy

	}
	IFXRETURN(iResult);
}

//---------------------------------------------------------------------------
IFXRESULT CIFXGlyph3DGenerator::BuildMesh(SIFXGlyphMeshParams* pInitInfo) {
	IFXRESULT iResult=IFX_OK;

	if(m_pGlyphString==NULL)
		iResult=IFX_E_NOT_INITIALIZED;

	if(IFXSUCCESS(iResult)) {
		U32 uFinalMeshGroupIndex=0;
		IFXMeshGroup* pFinalMeshGroup=NULL;
		IFXMeshGroup* pBuildMeshGroup=NULL;

		// clear out the return mesh group pointer
		IFXRELEASE( pInitInfo->pMeshGroup );

		// allocate mesh group to hold all the extruded and tessellated meshes
		U32 uMeshCount=0;
		iResult=GetTotalMeshCount(pInitInfo, &uMeshCount);
		if(IFXSUCCESS(iResult))
		{
			IFXCreateComponent(CID_IFXMeshGroup, IID_IFXMeshGroup, (void**)&pBuildMeshGroup);
			if(pBuildMeshGroup==NULL)
				iResult=IFX_E_OUT_OF_MEMORY;

			if(IFXSUCCESS(iResult) && uMeshCount!=0)
			{
				iResult=pBuildMeshGroup->Allocate(uMeshCount);
			}

		}

		if(IFXSUCCESS(iResult) && uMeshCount!=0)
		{
			// allocate an array to hold a hold a map of mesh index to glyph index.
			// -1 or (pMehsToGlyphIndex == NULL) means map has not been made yet.

			if(m_pMeshToGlyphIndexMap) {
				// incase BuildMesh had been called more than once, rebuild the map.
				delete m_pMeshToGlyphIndexMap;
				m_pMeshToGlyphIndexMap=NULL;
			}

			m_pMeshToGlyphIndexMap=new I32[uMeshCount];
			if(m_pMeshToGlyphIndexMap!=NULL) {
				U32 i;
				for( i=0;i<uMeshCount;i++) {
					m_pMeshToGlyphIndexMap[i]=-1;

				}
			}
			else {
				iResult=IFX_E_OUT_OF_MEMORY;
			}
		}

		U32 uGlyphCount, uGlyphIndex;
		uFinalMeshGroupIndex=0;

		if(IFXSUCCESS(iResult))
			iResult = m_pGlyphString->GetCount( &uGlyphCount );

		if(IFXSUCCESS(iResult))
		{
			for(uGlyphIndex=0;uGlyphIndex<uGlyphCount;uGlyphIndex++)
			{
				iResult = BuildGlyph(uGlyphIndex, pInitInfo, pBuildMeshGroup, &uFinalMeshGroupIndex );
				if(!IFXSUCCESS(iResult))
				{
					IFXRELEASE(pBuildMeshGroup);
					break;
				}
			}
		}

		IFXRELEASE(pInitInfo->pMeshGroup);
		if(IFXSUCCESS(iResult) && pBuildMeshGroup!=NULL)
		{
			iResult = CollapseFinalMeshGroup(pBuildMeshGroup,
				&pFinalMeshGroup,
				uGlyphCount);
			if(IFXSUCCESS(iResult))
			{
				pInitInfo->pMeshGroup=pFinalMeshGroup;
				IFXRELEASE(pBuildMeshGroup);

				// apply texture coordinates to the mesh group
				iResult = ApplyTextureCoordinates(pFinalMeshGroup);
			}
			else
			{
				IFXRELEASE(pBuildMeshGroup);
				IFXRELEASE(pFinalMeshGroup);
			}
		}
	}

	IFXRETURN(iResult);
}

IFXRESULT CIFXGlyph3DGenerator::BuildGlyph
(
 U32 uGlyphIndex,
 SIFXGlyphMeshParams* pInitInfo,
 IFXMeshGroup* pFinalMeshGroup,
 U32* pFinalMeshGroupIndex
 )
{
	IFXRESULT iResult=IFX_OK;

	if(pFinalMeshGroup==NULL || pFinalMeshGroupIndex==NULL)
		iResult = IFX_E_INVALID_POINTER;

	IFXMeshGroup* pMeshGroupFront=NULL;
	IFXMeshGroup* pMeshGroupBack=NULL;
	IFXMeshGroup* pMeshGroupTunnel=NULL;
	IFXMeshGroup* pMeshGroupBox=NULL;

	IFXUnknown* pUnknown=NULL;
	iResult=m_pGlyphString->Get(uGlyphIndex, &pUnknown);
	IFXASSERT(IFXSUCCESS(iResult));


	IFXSimpleList* pGlyph=NULL;
	if(IFXSUCCESS(iResult)) {
		iResult=pUnknown->QueryInterface(IID_IFXSimpleList, (void**)&pGlyph);
		IFXASSERT(IFXSUCCESS(iResult));
		IFXRELEASE(pUnknown);
	}


	if(IFXSUCCESS(iResult))
		if(pGlyph && pInitInfo->bTessellate==IFX_TRUE)
		{

			IFXContourTessellator* pContourTessellator=NULL;
			iResult=IFXCreateComponent(CID_IFXContourTessellator,
				IID_IFXContourTessellator,
				(void**) &pContourTessellator);

			IFXASSERT(IFXSUCCESS(iResult));

			SIFXTessellatorProperties *pTessPropertiesFront=NULL;
			SIFXTessellatorProperties sTessPropertiesFront=pInitInfo->sTessellator;

			SIFXTessellatorProperties *pTessPropertiesBack=NULL;
			SIFXTessellatorProperties sTessPropertiesBack=pInitInfo->sTessellator;

			if(pInitInfo->sCombiner.bFrontCap==IFX_TRUE) {
				sTessPropertiesFront.fDepth=0;
				switch(pInitInfo->sCombiner.eFrontCapFacing) {
	  case IFXGlyphBothFacing:
		  sTessPropertiesFront.eFacing=IFXGlyphBothFacing;
		  break;
	  case IFXGlyphOutFacing:
		  sTessPropertiesFront.eFacing=IFXGlyphOutFacing;
		  break;
	  case IFXGlyphInFacing:
		  sTessPropertiesFront.eFacing=IFXGlyphInFacing;
		  break;
	  default:
		  iResult=IFX_E_UNSUPPORTED;
				}

				pTessPropertiesFront = &sTessPropertiesFront;


			}

			if(pInitInfo->sCombiner.bBackCap==IFX_TRUE)
			{
				// set depth
				sTessPropertiesBack.fDepth=pInitInfo->sExtruder.fExtrusionDepth;

				switch(pInitInfo->sCombiner.eBackCapFacing)
				{
				case IFXGlyphBothFacing:
					sTessPropertiesBack.eFacing=IFXGlyphBothFacing;
					break;
				case IFXGlyphOutFacing:
					sTessPropertiesBack.eFacing=IFXGlyphInFacing;
					break;
				case IFXGlyphInFacing:
					sTessPropertiesBack.eFacing=IFXGlyphOutFacing;
					break;
				default:
					iResult=IFX_E_UNSUPPORTED;
				}

				pTessPropertiesBack = &sTessPropertiesBack;

			}


			if(IFXSUCCESS(iResult))
			{
				iResult=pContourTessellator->Tessellate(pGlyph,pTessPropertiesFront,&pMeshGroupFront,pTessPropertiesBack,&pMeshGroupBack);
			}


			IFXRELEASE(pContourTessellator);
		}

		// build extrusion component

		if(IFXSUCCESS(iResult))
			if(pGlyph && pInitInfo->bExtrude==IFX_TRUE)
			{
				IFXContourExtruder* pContourExtruder=NULL;
				iResult=IFXCreateComponent(CID_IFXContourExtruder,
					IID_IFXContourExtruder,
					(void**) &pContourExtruder);
				IFXASSERT(IFXSUCCESS(iResult));

				// set extrusion properties
				SIFXExtruderProperties sExtruderProperties=pInitInfo->sExtruder;

				if(IFXSUCCESS(iResult))
				{
					iResult=pContourExtruder->Extrude(&sExtruderProperties, pGlyph, &pMeshGroupTunnel);
					IFXASSERT(IFXSUCCESS(iResult));
				}

				IFXRELEASE(pContourExtruder);
			}

			// Copy messes into final mesh group.
			if(IFXSUCCESS(iResult))
				if(pInitInfo->bTessellate==IFX_TRUE  &&
					pInitInfo->sCombiner.bFrontCap==IFX_TRUE)
					iResult = CopyMeshGroupToFinalMesh(pMeshGroupFront, pFinalMeshGroup, pFinalMeshGroupIndex, uGlyphIndex );

			if(IFXSUCCESS(iResult))
				if(pInitInfo->bTessellate==IFX_TRUE &&
					pInitInfo->sCombiner.bBackCap==IFX_TRUE)
					iResult = CopyMeshGroupToFinalMesh(pMeshGroupBack, pFinalMeshGroup, pFinalMeshGroupIndex, uGlyphIndex );

			if(IFXSUCCESS(iResult))
				if(pInitInfo->bExtrude==IFX_TRUE)
					iResult = CopyMeshGroupToFinalMesh(pMeshGroupTunnel, pFinalMeshGroup, pFinalMeshGroupIndex, uGlyphIndex );

			if(!IFXSUCCESS(iResult) && iResult != IFX_E_OUT_OF_MEMORY)
			{
				// build error box.
				iResult = BuildBoundingBox(pInitInfo, pGlyph, &pMeshGroupBox);
				if(IFXSUCCESS(iResult))
				{
					if (*pFinalMeshGroupIndex < pFinalMeshGroup->GetNumMeshes() )
					{
						iResult = CopyMeshGroupToFinalMesh(pMeshGroupBox, pFinalMeshGroup, pFinalMeshGroupIndex, uGlyphIndex );
						IFXASSERT(IFXSUCCESS(iResult));
					}
					else
					{
						iResult = IFX_E_ABORTED;
					}

				}
			}



			// Delete old mesh groups
			IFXRELEASE(pMeshGroupFront);
			IFXRELEASE(pMeshGroupBack);
			IFXRELEASE(pMeshGroupTunnel);
			IFXRELEASE(pMeshGroupBox);

			IFXRELEASE(pGlyph);

			IFXRETURN(iResult);
}


IFXRESULT CIFXGlyph3DGenerator::BuildBoundingBox
(
 SIFXGlyphMeshParams* pInitInfo,
 IFXSimpleList* pGlyphList,
 IFXMeshGroup** ppMeshGroup
 )
{
	IFXRESULT iResult=IFX_OK;

	if(pInitInfo==NULL || pGlyphList==NULL || ppMeshGroup==NULL )
	{
		IFXRETURN(IFX_E_INVALID_POINTER);
	}

	IFXUnknown* pUnknown=NULL;

	SIFXContourPoint vContourPosition;

	SIFXContourPoint vMinPosition;
	vMinPosition.x = DBL_MAX;
	vMinPosition.y = DBL_MAX;

	SIFXContourPoint vMaxPosition;
	vMaxPosition.x = -DBL_MAX;
	vMaxPosition.y = -DBL_MAX;

	U32 uPathIndex;
	U32 uPathCount;
	U32 uVertexIndex;
	U32 uVertexCount;

	IFXContour* pContour=NULL;

	if(IFXSUCCESS(iResult)) {

		pGlyphList->GetCount( &uPathCount );

		for(uPathIndex=0;uPathIndex<uPathCount;uPathIndex++)
		{
			pUnknown=NULL;
			pGlyphList->Get(uPathIndex, &pUnknown);
			iResult=pUnknown->QueryInterface(IID_IFXContour, (void**)&pContour);
			if(IFXSUCCESS(iResult) && pContour)
			{
				pContour->GetCount( &uVertexCount );
				pContour->GetPosition(uVertexCount-1,&vContourPosition);

				if(vMinPosition.x>vContourPosition.x) vMinPosition.x=vContourPosition.x;
				if(vMinPosition.y>vContourPosition.y) vMinPosition.y=vContourPosition.y;
				if(vMaxPosition.x<vContourPosition.x) vMaxPosition.x=vContourPosition.x;
				if(vMaxPosition.y<vContourPosition.y) vMaxPosition.y=vContourPosition.y;

				for(uVertexIndex=0;uVertexIndex<uVertexCount;uVertexIndex+=2)
				{

					pContour->GetPosition(uVertexCount-1-uVertexIndex,&vContourPosition);
					if(vMinPosition.x>vContourPosition.x) vMinPosition.x=vContourPosition.x;
					if(vMinPosition.y>vContourPosition.y) vMinPosition.y=vContourPosition.y;
					if(vMaxPosition.x<vContourPosition.x) vMaxPosition.x=vContourPosition.x;
					if(vMaxPosition.y<vContourPosition.y) vMaxPosition.y=vContourPosition.y;
				}
			}

			IFXRELEASE(pContour);
			IFXRELEASE(pUnknown);

		}


	}

	IFXMesh* pMesh = 0;
	IFXCreateComponent(CID_IFXMesh, IID_IFXMesh, (void**)&pMesh);
	if(pMesh==NULL)
		iResult=IFX_E_OUT_OF_MEMORY;

	IFXVertexAttributes vertexAttributes;

	const U32 constVertexCount = 24;
	const U32 constFaceCount = 12;
	if(IFXSUCCESS(iResult))
		iResult=pMesh->Allocate(vertexAttributes,constVertexCount,constFaceCount);


	F32 fZmin = 0.0;
	F32 fZmax;
	if(pInitInfo->bExtrude)
	{
		fZmax = pInitInfo->sExtruder.fExtrusionDepth;
	}
	else
	{
		fZmax = 0.0;
	}


	IFXVector3 pvCubePosition[constVertexCount];
	IFXVector3 pvCubeNormal[constVertexCount];


	pvCubePosition[0].Set((F32)vMinPosition.x , (F32) vMinPosition.y , fZmin);
	pvCubePosition[15] =
		pvCubePosition[17] =
		pvCubePosition[00];

	pvCubePosition[1].Set((F32)vMinPosition.x , (F32) vMaxPosition.y , fZmin);
	pvCubePosition[14] =
		pvCubePosition[20] =
		pvCubePosition[1];

	pvCubePosition[2].Set((F32)vMaxPosition.x , (F32) vMaxPosition.y , fZmin);
	pvCubePosition[5] =
		pvCubePosition[23] =
		pvCubePosition[2];

	pvCubePosition[3].Set((F32)vMaxPosition.x , (F32) vMinPosition.y , fZmin);
	pvCubePosition[4] =
		pvCubePosition[18] =
		pvCubePosition[3];

	pvCubePosition[8].Set((F32)vMaxPosition.x , (F32) vMinPosition.y , -fZmax);
	pvCubePosition[7] =
		pvCubePosition[19] =
		pvCubePosition[8];

	pvCubePosition[9].Set((F32)vMaxPosition.x , (F32) vMaxPosition.y , -fZmax);
	pvCubePosition[6] =
		pvCubePosition[22] =
		pvCubePosition[9];

	pvCubePosition[10].Set((F32)vMinPosition.x , (F32) vMaxPosition.y , -fZmax);
	pvCubePosition[13] =
		pvCubePosition[21] =
		pvCubePosition[10];

	pvCubePosition[11].Set((F32)vMinPosition.x , (F32) vMinPosition.y , -fZmax);
	pvCubePosition[12] =
		pvCubePosition[16] =
		pvCubePosition[11];


	pvCubeNormal[3].Set(0,0,1);
	pvCubeNormal[0] =
		pvCubeNormal[1] =
		pvCubeNormal[2] =
		pvCubeNormal[3];

	pvCubeNormal[7].Set(1,0,0);
	pvCubeNormal[4] =
		pvCubeNormal[5] =
		pvCubeNormal[6] =
		pvCubeNormal[7];

	pvCubeNormal[11].Set(0,0,-1);
	pvCubeNormal[8] =
		pvCubeNormal[9] =
		pvCubeNormal[10] =
		pvCubeNormal[11];

	pvCubeNormal[15].Set(-1,0,0);
	pvCubeNormal[12] =
		pvCubeNormal[13] =
		pvCubeNormal[14] =
		pvCubeNormal[15];

	pvCubeNormal[19].Set(0,-1,0);
	pvCubeNormal[16] =
		pvCubeNormal[17] =
		pvCubeNormal[18] =
		pvCubeNormal[19];

	pvCubeNormal[23].Set(0,1,0);
	pvCubeNormal[20] =
		pvCubeNormal[21] =
		pvCubeNormal[22] =
		pvCubeNormal[23];


	IFXVector3Iter v3iPosition;
	IFXVector3 *pvPosition;

	IFXVector3Iter v3iNormal;
	IFXVector3 *pvNormal;

	// get the iterators
	pMesh->GetPositionIter(v3iPosition);
	pMesh->GetNormalIter(v3iNormal);

	U32 i=0;
	for(i=0; i< constVertexCount; i++) {
		pvPosition=v3iPosition.Next();
		*pvPosition=pvCubePosition[i];
		pvNormal=v3iNormal.Next();
		*pvNormal=pvCubeNormal[i];
	}


	IFXFaceIter fiFace;
	IFXFace *pfFace;
	pMesh->GetFaceIter(fiFace);


	U32 j = 0;
	for(i = 0; i<constFaceCount; i+=2)
	{
		pfFace=fiFace.Next();
		pfFace->Set(j,j+2,j+1);

		pfFace=fiFace.Next();
		pfFace->Set(j,j+3,j+2);

		j+=4;
	}

	IFXCreateComponent(CID_IFXMeshGroup, IID_IFXMeshGroup, (void**)ppMeshGroup);
	if(*ppMeshGroup==NULL)
	{
		iResult=IFX_E_OUT_OF_MEMORY;
	}

	if(IFXSUCCESS(iResult))
		if(*ppMeshGroup)
		{
			iResult=(*ppMeshGroup)->Allocate(1);
			if(IFXSUCCESS(iResult))
			{
				(*ppMeshGroup)->SetMesh(0, pMesh);
			}
		}

		IFXRELEASE(pMesh);
		IFXRETURN(iResult);
}

IFXRESULT CIFXGlyph3DGenerator::CopyMeshGroupToFinalMesh(IFXMeshGroup* pMeshGroup, IFXMeshGroup* pFinalMeshGroup, U32* pFinalMeshGroupIndex, U32 uGlyphIndex )
{
	IFXRESULT iResult=IFX_OK;

	if(pMeshGroup==NULL || pFinalMeshGroup==NULL || pFinalMeshGroupIndex==NULL)
		iResult = IFX_E_INVALID_POINTER;


	//IFXUnknown* pUnknown=NULL;


	if(IFXSUCCESS(iResult))
	{

		U32 uMeshIndex=0;
		U32 uMeshCount=0;
		IFXMesh* pMesh=NULL;
		uMeshCount=pMeshGroup->GetNumMeshes();

		for(uMeshIndex=0; uMeshIndex<uMeshCount; uMeshIndex++)
		{
			pMeshGroup->GetMesh(uMeshIndex, pMesh);
			if(pMesh!=NULL)
			{
				pFinalMeshGroup->SetMesh(*pFinalMeshGroupIndex, pMesh);

				if(m_pMeshToGlyphIndexMap) {
					m_pMeshToGlyphIndexMap[*pFinalMeshGroupIndex]=uGlyphIndex;
				}
			}
			(*pFinalMeshGroupIndex)=(*pFinalMeshGroupIndex)+1;
			IFXRELEASE(pMesh);
		}
	}

	IFXRETURN(iResult);
}


//---------------------------------------------------------------------------
//  CIFXTextGenerator::StartGlyphString
//
//  This method starts a new glyph string.  It cleans up the memory and inits
//  all the data structures
//---------------------------------------------------------------------------
IFXRESULT CIFXGlyph3DGenerator::StartGlyphString() {
	IFXRESULT iResult=IFX_OK;

	// allocate a new, clean contour generator for this new string of glyphs
	if(IFXSUCCESS(iResult)) {

		// release old string and command list
		IFXRELEASE(m_pGlyphString);
		IFXRELEASE(m_pGlyphCommands);

		// build the list to hold the contours
		if(IFXSUCCESS(iResult)) {
			iResult=IFXCreateComponent(CID_IFXSimpleList, IID_IFXSimpleList,
				(void**) &m_pGlyphString);
			if(IFXSUCCESS(iResult) && m_pGlyphString) {
				m_pGlyphString->Initialize(1);
				m_dWidthAccumulator=0;
				m_dHeightAccumulator=0;
			}
		}

		// build the list to hold the commands for replay
		if(IFXSUCCESS(iResult)) {
			iResult=IFXCreateComponent(CID_IFXSimpleList, IID_IFXSimpleList,
				(void**) &m_pGlyphCommands);
			if( NULL == m_pGlyphCommands)
				iResult = IFX_E_INVALID_POINTER;
			if(IFXSUCCESS(iResult))
				iResult = m_pGlyphCommands->Initialize(1);
		}

		// store the start glyph string tag command
		IFXUnknown* pUnknown=NULL;
		IFXGlyphTagBlock* pTagBlock = NULL;
		U32 uDummy=0;

		if(IFXSUCCESS(iResult))
			iResult=IFXCreateComponent(CID_IFXGlyphTagBlock, IID_IFXGlyphTagBlock,
			(void**) &pTagBlock);
		if(IFXSUCCESS(iResult))
			iResult = pTagBlock->SetType( IFXGlyphTagBlock::IGG_TYPE_STARTGLYPHSTRING );
		if(IFXSUCCESS(iResult))
			iResult = pTagBlock->QueryInterface(IID_IFXUnknown, (void**) &pUnknown);
		if(IFXSUCCESS(iResult))
			iResult = m_pGlyphCommands->Add(pUnknown, &uDummy);

		// release
		IFXRELEASE(pUnknown);
		IFXRELEASE(pTagBlock);
	}

	IFXRETURN(iResult);
}

//---------------------------------------------------------------------------
//  CIFXTextGenerator::StartGlyph
//
//  This method starts a new glyph.  It cleans up the memory and inits
//  all the data structures.  Which means creating a new contour generator.
//---------------------------------------------------------------------------
IFXRESULT CIFXGlyph3DGenerator::StartGlyph() {
	IFXRESULT iResult=IFX_OK;

	if( NULL == m_pGlyphString )
		iResult = IFX_E_NOT_INITIALIZED;

	// Check if we have reached our maximum number of input characters, If so,
	// then we fail gracefully.
	if( IFXSUCCESS(iResult))
	{
		U32 uGlyphCount;

		// Get our current count of chars.
		iResult = m_pGlyphString->GetCount( &uGlyphCount );
		if( IFXSUCCESS(iResult))
		{
			if( uGlyphCount > GLYPH_GENERATOR_MAX_CHARS )
			{
				iResult = IFX_E_INVALID_RANGE;
			}
		}
	}

	if( IFXSUCCESS(iResult)) {
		// someone started a new glyph without closing the old one
		// close it and start a new one
		if(m_pContourGenerator)
			EndGlyph(m_pGlyphGenParams->fDefaultGlyphWidth, m_pGlyphGenParams->fDefaultGlyphHeight );

		iResult=IFXCreateComponent(CID_IFXContourGenerator, IID_IFXContourGenerator,
			(void**) &m_pContourGenerator);
		if(IFXSUCCESS(iResult) && m_pContourGenerator)
			iResult=m_pContourGenerator->Initialize();

		// store the start glyph tag command
		if(IFXSUCCESS(iResult)) {
			IFXUnknown* pUnknown=NULL;
			IFXGlyphTagBlock* pTagBlock = NULL;
			U32 uDummy=0;

			if(IFXSUCCESS(iResult))
				iResult=IFXCreateComponent(CID_IFXGlyphTagBlock, IID_IFXGlyphTagBlock,
				(void**) &pTagBlock);
			if(IFXSUCCESS(iResult))
				iResult = pTagBlock->SetType( IFXGlyphTagBlock::IGG_TYPE_STARTGLYPH );
			if(IFXSUCCESS(iResult))
				iResult = pTagBlock->QueryInterface(IID_IFXUnknown, (void**) &pUnknown);
			if(IFXSUCCESS(iResult))
				iResult = m_pGlyphCommands->Add(pUnknown, &uDummy);

			// release
			IFXRELEASE(pUnknown);
			IFXRELEASE(pTagBlock);
		}

	}
	IFXRETURN(iResult);
}

//---------------------------------------------------------------------------
//  CIFXTextGenerator::EndGlyph
//
//  This method ends a glyph.  It ends the current contour and stores the
//  current contour list (the glyph) generated by the current contour generator
//  and stores that in the GlyphList.
//---------------------------------------------------------------------------
IFXRESULT CIFXGlyph3DGenerator::EndGlyph(F64 fWidth, F64 fHeight) {
	IFXRESULT iResult=IFX_OK;

	if(m_pGlyphString==NULL)
		iResult=IFX_E_NOT_INITIALIZED;

	if(IFXSUCCESS(iResult)) {
		// if path wasn't closed, end it
		iResult=EndPath();
		if(IFXSUCCESS(iResult)) {
			m_dWidthAccumulator+=fWidth;
			m_dHeightAccumulator+=fHeight;

			// get the pathlist and store it
			IFXSimpleList* pCompletedGlyph=NULL;
			iResult=IFXCreateComponent(CID_IFXSimpleList, IID_IFXSimpleList,
				(void**) &pCompletedGlyph);
			if(IFXSUCCESS(iResult)) {
				pCompletedGlyph->Initialize(1);
				IFXCHECKX( m_pContourGenerator == NULL );
				iResult=m_pContourGenerator->GetContourList(pCompletedGlyph);
				if(IFXSUCCESS(iResult)) {
					U32 uDummy=0;
					IFXUnknown* pObject=NULL;
					iResult=pCompletedGlyph->QueryInterface(IID_IFXUnknown, (void**)&pObject);
					if(IFXSUCCESS(iResult))
						iResult=m_pGlyphString->Add(pObject, &uDummy);
					IFXRELEASE(pObject);
				}
			}
			IFXRELEASE(pCompletedGlyph);
		}
	}

	// store the end glyph tag command
	if(IFXSUCCESS(iResult)) {
		IFXUnknown* pUnknown=NULL;
		IFXGlyphLineToBlock* pEndBlock = NULL;
		U32 uDummy=0;

		if(IFXSUCCESS(iResult))
			iResult=IFXCreateComponent(CID_IFXGlyphTagBlock, IID_IFXGlyphTagBlock,
			(void**) &pEndBlock);
		if(IFXSUCCESS(iResult))
			iResult = pEndBlock->SetType( IFXGlyph2DCommands::IGG_TYPE_ENDGLYPH );
		if(IFXSUCCESS(iResult))
			iResult = pEndBlock->SetData( fWidth, fHeight );
		if(IFXSUCCESS(iResult))
			iResult = pEndBlock->QueryInterface(IID_IFXUnknown, (void**) &pUnknown);
		if(IFXSUCCESS(iResult))
			iResult = m_pGlyphCommands->Add(pUnknown, &uDummy);

		// release
		IFXRELEASE(pUnknown);
		IFXRELEASE(pEndBlock);
	}
	F64 min[2];
	F64 max[2];
	m_pContourGenerator->GetBoundingBox( min, max );
	if( xMin > min[0] )
		xMin = min[0];
	if( xMax < max[0] )
		xMax = max[0];
	if( yMin > min[1] )
		yMin = min[1];
	if( yMax < max[1] )
		yMax = max[1];
	IFXRELEASE(m_pContourGenerator);
	IFXRETURN(iResult);

}

//---------------------------------------------------------------------------
//  CIFXTextGenerator::EndGlyphString
//
//  This method ends a glyph string.  It starts by ending the current contour
//  and incrementing a height parameter so you can start a second line of
//  text.
//---------------------------------------------------------------------------
IFXRESULT CIFXGlyph3DGenerator::EndGlyphString() {
	IFXRESULT iResult=IFX_OK;

	if(m_pGlyphString==NULL)
		iResult=IFX_E_NOT_INITIALIZED;

	if(IFXSUCCESS(iResult)) {
		m_dWidthAccumulator=0;
		m_dHeightAccumulator=0;
	}

	// store the end glyph string tag command
	if(IFXSUCCESS(iResult)) {
		IFXUnknown* pUnknown=NULL;
		IFXGlyphTagBlock* pTagBlock = NULL;
		U32 uDummy=0;

		if(IFXSUCCESS(iResult))
			iResult=IFXCreateComponent(CID_IFXGlyphTagBlock, IID_IFXGlyphTagBlock,
			(void**) &pTagBlock);
		if(IFXSUCCESS(iResult))
			iResult = pTagBlock->SetType( IFXGlyphTagBlock::IGG_TYPE_ENDGLYPHSTRING );
		if(IFXSUCCESS(iResult))
			iResult = pTagBlock->QueryInterface(IID_IFXUnknown, (void**) &pUnknown);
		if(IFXSUCCESS(iResult))
			iResult = m_pGlyphCommands->Add(pUnknown, &uDummy);

		// release
		IFXRELEASE(pUnknown);
		IFXRELEASE(pTagBlock);
	}


	IFXRETURN(iResult);
}


// CIFXContourGenerator methods

IFXRESULT CIFXGlyph3DGenerator::StartPath() {
	IFXRESULT iResult=IFX_OK;
	if(m_pContourGenerator==NULL)
		iResult=IFX_E_NOT_INITIALIZED;

	if(IFXSUCCESS(iResult))
		iResult=m_pContourGenerator->StartPath();


	// store the startpath tag command
	if(IFXSUCCESS(iResult)) {
		IFXUnknown* pUnknown=NULL;
		IFXGlyphTagBlock* pTagBlock = NULL;
		U32 uDummy=0;

		if(IFXSUCCESS(iResult))
			iResult=IFXCreateComponent(CID_IFXGlyphTagBlock, IID_IFXGlyphTagBlock,
			(void**) &pTagBlock);
		if(IFXSUCCESS(iResult))
			iResult = pTagBlock->SetType( IFXGlyphTagBlock::IGG_TYPE_STARTPATH );
		if(IFXSUCCESS(iResult))
			iResult = pTagBlock->QueryInterface(IID_IFXUnknown, (void**) &pUnknown);
		if(IFXSUCCESS(iResult))
			iResult = m_pGlyphCommands->Add(pUnknown, &uDummy);

		// release
		IFXRELEASE(pUnknown);
		IFXRELEASE(pTagBlock);
	}

	IFXRETURN(iResult);

}
IFXRESULT CIFXGlyph3DGenerator::LineTo(F64 fX, F64 fY){
	IFXRESULT iResult=IFX_OK;
	if(m_pContourGenerator==NULL)
		iResult=IFX_E_NOT_INITIALIZED;

	if(IFXSUCCESS(iResult))
		iResult=m_pContourGenerator->LineTo(fX+m_dWidthAccumulator, fY+m_dHeightAccumulator);

	// store the lineto command
	if(IFXSUCCESS(iResult)) {
		IFXUnknown* pUnknown=NULL;
		IFXGlyphLineToBlock* pLineToBlock = NULL;
		U32 uDummy=0;

		if(IFXSUCCESS(iResult))
			iResult=IFXCreateComponent(CID_IFXGlyphLineToBlock, IID_IFXGlyphLineToBlock,
			(void**) &pLineToBlock);
		if(IFXSUCCESS(iResult))
			iResult = pLineToBlock->SetType( IFXGlyphTagBlock::IGG_TYPE_LINETO );
		if(IFXSUCCESS(iResult))
			iResult = pLineToBlock->SetData( fX, fY );
		if(IFXSUCCESS(iResult))
			iResult = pLineToBlock->QueryInterface(IID_IFXUnknown, (void**) &pUnknown);
		if(IFXSUCCESS(iResult))
			iResult = m_pGlyphCommands->Add(pUnknown, &uDummy);

		// release
		IFXRELEASE(pUnknown);
		IFXRELEASE(pLineToBlock);
	}


	IFXRETURN(iResult);

}
IFXRESULT CIFXGlyph3DGenerator::MoveTo(F64 fX, F64 fY){
	IFXRESULT iResult=IFX_OK;
	if(m_pContourGenerator==NULL)
		iResult=IFX_E_NOT_INITIALIZED;

	if(IFXSUCCESS(iResult))
		iResult=m_pContourGenerator->MoveTo(fX+m_dWidthAccumulator, fY+m_dHeightAccumulator);


	// store the moveto command
	if(IFXSUCCESS(iResult)) {
		IFXUnknown* pUnknown=NULL;
		IFXGlyphMoveToBlock* pMoveToBlock = NULL;
		U32 uDummy=0;

		if(IFXSUCCESS(iResult))
			iResult=IFXCreateComponent(CID_IFXGlyphMoveToBlock, IID_IFXGlyphMoveToBlock,
			(void**) &pMoveToBlock);
		if(IFXSUCCESS(iResult))
			iResult = pMoveToBlock->SetType( IFXGlyphTagBlock::IGG_TYPE_MOVETO );
		if(IFXSUCCESS(iResult))
			iResult = pMoveToBlock->SetData( fX, fY );
		if(IFXSUCCESS(iResult))
			iResult = pMoveToBlock->QueryInterface(IID_IFXUnknown, (void**) &pUnknown);
		if(IFXSUCCESS(iResult))
			iResult = m_pGlyphCommands->Add(pUnknown, &uDummy);

		// release
		IFXRELEASE(pUnknown);
		IFXRELEASE(pMoveToBlock);
	}


	IFXRETURN(iResult);

}
IFXRESULT CIFXGlyph3DGenerator::CurveTo(F64 fCx1, F64 fCy1, F64 fCx2, F64 fCy2, F64 fAx, F64 fAy, U32 uNumberOfCurveSteps){
	IFXRESULT iResult=IFX_OK;
	if(m_pContourGenerator==NULL)
		iResult=IFX_E_NOT_INITIALIZED;


	if(IFXSUCCESS(iResult))
		iResult=m_pContourGenerator->CurveTo(fCx1+m_dWidthAccumulator, fCy1+m_dHeightAccumulator, fCx2+m_dWidthAccumulator, fCy2+m_dHeightAccumulator, fAx+m_dWidthAccumulator, fAy+m_dHeightAccumulator, uNumberOfCurveSteps);


	// store the curveto command
	if(IFXSUCCESS(iResult)) {
		IFXUnknown* pUnknown=NULL;
		IFXGlyphCurveToBlock* pCurveToBlock = NULL;
		U32 uDummy=0;

		if(IFXSUCCESS(iResult))
			iResult=IFXCreateComponent(CID_IFXGlyphCurveToBlock, IID_IFXGlyphCurveToBlock,
			(void**) &pCurveToBlock);
		if(IFXSUCCESS(iResult))
			iResult = pCurveToBlock->SetType( IFXGlyphTagBlock::IGG_TYPE_CURVETO );
		if(IFXSUCCESS(iResult))
			iResult = pCurveToBlock->SetData(fCx1, fCy1, fCx2, fCy2, fAx, fAy, uNumberOfCurveSteps);
		if(IFXSUCCESS(iResult))
			iResult = pCurveToBlock->QueryInterface(IID_IFXUnknown, (void**) &pUnknown);
		if(IFXSUCCESS(iResult))
			iResult = m_pGlyphCommands->Add(pUnknown, &uDummy);

		// release
		IFXRELEASE(pUnknown);
		IFXRELEASE(pCurveToBlock);
	}



	IFXRETURN(iResult);

}
IFXRESULT CIFXGlyph3DGenerator::EndPath(){
	IFXRESULT iResult=IFX_OK;
	if(m_pContourGenerator==NULL)
		iResult=IFX_E_NOT_INITIALIZED;

	if(IFXSUCCESS(iResult)) {
		iResult=m_pContourGenerator->EndPath();
	}

	// store the endpath tag command
	if(IFXSUCCESS(iResult)) {
		IFXUnknown* pUnknown=NULL;
		IFXGlyphTagBlock* pTagBlock = NULL;
		U32 uDummy=0;

		if(IFXSUCCESS(iResult))
			iResult=IFXCreateComponent(CID_IFXGlyphTagBlock, IID_IFXGlyphTagBlock,
			(void**) &pTagBlock);
		if(IFXSUCCESS(iResult))
			iResult = pTagBlock->SetType( IFXGlyphTagBlock::IGG_TYPE_ENDPATH );
		if(IFXSUCCESS(iResult))
			iResult = pTagBlock->QueryInterface(IID_IFXUnknown, (void**) &pUnknown);
		if(IFXSUCCESS(iResult))
			iResult = m_pGlyphCommands->Add(pUnknown, &uDummy);

		// release
		IFXRELEASE(pUnknown);
		IFXRELEASE(pTagBlock);
	}

	IFXRETURN(iResult);
}
IFXRESULT CIFXGlyph3DGenerator::GetBoundingBox(F64 pMin[2], F64 pMax[2]){
	IFXRESULT iResult=IFX_OK;
	/*  if(m_pContourGenerator==NULL)
	iResult=IFX_E_NOT_INITIALIZED;

	if(IFXSUCCESS(iResult)) {
	iResult=m_pContourGenerator->GetBoundingBox(pMin, pMax);
	}*/

	pMin[0] = xMin;
	pMin[1] = yMin;
	pMax[0] = xMax;
	pMax[1] = yMax;
	IFXRETURN(iResult);
}
IFXRESULT CIFXGlyph3DGenerator::GetCharIndex(U32 uPickedMeshIndex, I32 *pCharIndex)
{
	// BuildMesh() must be called before GetCharIndex()

	IFXRESULT iResult=IFX_OK;
	if(m_pContourGenerator==NULL)
		iResult=IFX_E_NOT_INITIALIZED;


	if(m_pMeshToGlyphIndexMap==NULL) {
		iResult=IFX_E_NOT_INITIALIZED;
		*pCharIndex = -1;
	}
	else {
		*pCharIndex = m_pMeshToGlyphIndexMap[uPickedMeshIndex];
	}


	IFXRETURN(iResult);
}


// CIFXGlyph3DGenerator cloning methods

//---------------------------------------------------------------------------
//  CIFXTextGenerator::GetGlyphCommandList
//
//  This method returns a pointer to the glyph data that's been created and
//  stored internally.  Its an IFXSimpleList of glyph command blocks that can
//  be used to re-create the glyphs.
//---------------------------------------------------------------------------
IFXRESULT CIFXGlyph3DGenerator::GetGlyphCommandList(IFXSimpleList** ppGlyphList) {
	IFXRESULT iResult = IFX_OK;

	if( NULL == ppGlyphList )
		iResult = IFX_E_INVALID_POINTER;

	if( IFXSUCCESS( iResult ) )
	{
		if( NULL == m_pGlyphCommands )
			iResult = IFX_E_NOT_INITIALIZED;
	}

	if( IFXSUCCESS( iResult) )
	{
		*ppGlyphList = m_pGlyphCommands;
		m_pGlyphCommands->AddRef();
	}

	return iResult;
}

//---------------------------------------------------------------------------
//  CIFXTextGenerator::SetGlyphCommandList
//
//  This method gets glyph command blocks stored in IFXSimpleList and
//  applies commands to the glyph string.
//---------------------------------------------------------------------------
IFXRESULT CIFXGlyph3DGenerator::SetGlyphCommandList(IFXSimpleList* pGlyphList) {
	IFXRESULT iResult = IFX_OK;

	if( NULL == pGlyphList )
		iResult = IFX_E_INVALID_POINTER;

	// playback mechanism
	if(IFXSUCCESS(iResult)) {
		U32 comCnt = 0;
		IFXUnknown* pUnknown = NULL;
		IFXGlyph2DCommands* pCom = NULL;
		IFXGlyphMoveToBlock* pMoveToBlock = NULL;
		IFXGlyphLineToBlock* pLineToBlock = NULL;
		IFXGlyphCurveToBlock* pCurveToBlock = NULL;
		IFXGlyphTagBlock* pTagBlock = NULL;
		IFXGlyph2DCommands::EGLYPH_TYPE comType;

		F64 x1 = 0, x2 = 0, x3 = 0;
		F64 y1 = 0, y2 = 0, y3 = 0;
		U32 numSteps = 1;

		U32 pathDone = 1;
		U32 glyphDone = 1;
		U32 stringDone = 1;
		pGlyphList->GetCount( &comCnt );
		U32 idx;
		for(  idx = 0; idx < comCnt; idx++ )
		{
			pMoveToBlock = NULL;
			pLineToBlock = NULL;
			pCurveToBlock = NULL;
			pTagBlock = NULL;
			pGlyphList->Get( idx, &pUnknown );
			pUnknown->QueryInterface( IID_IFXGlyph2DCommands, (void**)&pCom );
			pCom->GetType( &comType );
			IFXRELEASE( pCom );

			switch( comType )
			{
			case IFXGlyph2DCommands::IGG_TYPE_MOVETO:
				pUnknown->QueryInterface( IID_IFXGlyphMoveToBlock, (void**)&pMoveToBlock );
				pMoveToBlock->GetData( &x1, &y1 );
				MoveTo( x1, y1 );
				IFXRELEASE( pMoveToBlock );
				break;
			case IFXGlyph2DCommands::IGG_TYPE_LINETO:
				pUnknown->QueryInterface( IID_IFXGlyphLineToBlock, (void**)&pLineToBlock );
				pLineToBlock->GetData( &x1, &y1 );
				LineTo( x1, y1 );
				IFXRELEASE( pLineToBlock );
				break;
			case IFXGlyph2DCommands::IGG_TYPE_CURVETO:
				pUnknown->QueryInterface( IID_IFXGlyphCurveToBlock, (void**)&pCurveToBlock );
				pCurveToBlock->GetData( &x1, &y1, &x2, &y2, &x3, &y3, &numSteps );
				CurveTo( x1, y1, x2, y2, x3, y3, numSteps );
				IFXRELEASE( pCurveToBlock );
				break;
			case IFXGlyph2DCommands::IGG_TYPE_STARTGLYPHSTRING:
				StartGlyphString();
				stringDone = 0;
				break;
			case IFXGlyph2DCommands::IGG_TYPE_STARTGLYPH:
				StartGlyph();
				glyphDone = 0;
				break;
			case IFXGlyph2DCommands::IGG_TYPE_STARTPATH:
				StartPath();
				pathDone = 0;
				break;
			case IFXGlyph2DCommands::IGG_TYPE_ENDPATH:
				EndPath();
				stringDone = 1;
				break;
			case IFXGlyph2DCommands::IGG_TYPE_ENDGLYPH:
				pUnknown->QueryInterface( IID_IFXGlyphTagBlock, (void**)&pTagBlock );
				pTagBlock->GetData( &x1, &y1 );
				EndGlyph( x1, y1 );
				IFXRELEASE( pTagBlock );
				glyphDone = 1;
				break;
			case IFXGlyph2DCommands::IGG_TYPE_ENDGLYPHSTRING:
				EndGlyphString();
				stringDone = 1;
				break;
			}
			IFXRELEASE( pUnknown );
		}
		if( !pathDone )
			EndPath();
		if( !glyphDone )
			EndGlyph( m_dWidthAccumulator, m_dHeightAccumulator );
		if( !stringDone )
			EndGlyphString();
	}

	return iResult;
}

IFXRESULT CIFXGlyph3DGenerator::ReplayGlyphCommandList(U32 uCurveSteps) {
	IFXRESULT iResult = IFX_OK;

	IFXSimpleList*    pGlyphList    = m_pGlyphCommands ;
	IFXUnknown*     pUnk      = NULL ;
	IFXGlyph2DCommands* pGlyphCommand = NULL ;
	U32 i       = 0 ;
	U32 uCommandType  = 0 ;
	F64 fDoubleArray[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0} ;

	U32     uTemp     = 0 ;

	if ( (IFXSUCCESS(iResult)) && (pGlyphList != NULL) )
	{
		U32 uListCount = 0;
		iResult = pGlyphList->GetCount( &uListCount );

		for (i=0; (i < uListCount) && (IFXSUCCESS(iResult)); i++)
		{
			iResult = pGlyphList->Get( i, &pUnk );

			if ( (IFXSUCCESS(iResult)) && (pUnk != NULL) )
			{
				// find out what kind of object it is:
				iResult = pUnk->QueryInterface( IID_IFXGlyph2DCommands,
					(void**)&pGlyphCommand );

				if (IFXSUCCESS(iResult))
					iResult = pGlyphCommand->GetType( (IFXGlyph2DCommands::EGLYPH_TYPE*)&uCommandType );

				if (IFXSUCCESS(iResult))
				{
					switch( uCommandType )
					{
					case IFXGlyph2DCommands::IGG_TYPE_STARTGLYPHSTRING:
						// release old string and command list
						IFXRELEASE(m_pGlyphString);
						// build the list to hold the contours
						if(IFXSUCCESS(iResult)) {
							iResult=IFXCreateComponent(CID_IFXSimpleList, IID_IFXSimpleList,
								(void**) &m_pGlyphString);
							if(IFXSUCCESS(iResult) && m_pGlyphString) {
								m_pGlyphString->Initialize(1);
								m_dWidthAccumulator=0;
								m_dHeightAccumulator=0;
							}
						}
						break;


					case IFXGlyph2DCommands::IGG_TYPE_STARTGLYPH:

						// Check if we have reached our maximum number of input characters, If so,
						// then we fail gracefully.
						if( IFXSUCCESS(iResult))
						{
							U32 uGlyphCount;

							// Get our current count of chars.
							iResult = m_pGlyphString->GetCount( &uGlyphCount );
							if( IFXSUCCESS(iResult))
							{
								if( uGlyphCount > GLYPH_GENERATOR_MAX_CHARS )
								{
									iResult = IFX_E_INVALID_RANGE;
								}
							}
						}


						if(m_pContourGenerator)
						{
							IFXRELEASE(m_pContourGenerator);
						}


						if( IFXSUCCESS(iResult)) {

							iResult=IFXCreateComponent(CID_IFXContourGenerator, IID_IFXContourGenerator,
								(void**) &m_pContourGenerator);
							if(IFXSUCCESS(iResult) && m_pContourGenerator)
								iResult=m_pContourGenerator->Initialize();

						}
						break;


					case IFXGlyph2DCommands::IGG_TYPE_STARTPATH:
						m_pContourGenerator->StartPath();
						break;

					case IFXGlyph2DCommands::IGG_TYPE_ENDPATH:
						iResult=m_pContourGenerator->EndPath();
						break;

					case IFXGlyph2DCommands::IGG_TYPE_ENDGLYPHSTRING:
						if(IFXSUCCESS(iResult)) {
							m_dWidthAccumulator=0;
							m_dHeightAccumulator=0;
						}
						break ;

					case IFXGlyph2DCommands::IGG_TYPE_MOVETO:
						{
							// get the MoveTo handle:
							IFXGlyphMoveToBlock* pGlyphMoveTo = NULL ;
							iResult = pGlyphCommand->QueryInterface( IID_IFXGlyphMoveToBlock,
								(void**)&pGlyphMoveTo );
							if (IFXSUCCESS(iResult))
								iResult = pGlyphMoveTo->GetData( &fDoubleArray[0],
								&fDoubleArray[1] );

							IFXRELEASE( pGlyphMoveTo );

							m_pContourGenerator->MoveTo
								(
								fDoubleArray[0]+m_dWidthAccumulator,
								fDoubleArray[1]+m_dHeightAccumulator
								);


						} // end case IGG_TYPE_MOVETO
						break ;

					case IFXGlyph2DCommands::IGG_TYPE_LINETO:
						{
							// write the type first:
							// get the LineTo handle:
							IFXGlyphLineToBlock* pGlyphLineTo = NULL ;
							iResult = pGlyphCommand->QueryInterface( IID_IFXGlyphLineToBlock,
								(void**)&pGlyphLineTo );
							if (IFXSUCCESS(iResult))
								iResult = pGlyphLineTo->GetData( &fDoubleArray[0],
								&fDoubleArray[1] );

							IFXRELEASE( pGlyphLineTo );

							if(IFXSUCCESS(iResult))
								iResult=m_pContourGenerator->LineTo(
								fDoubleArray[0]+m_dWidthAccumulator,
								fDoubleArray[1]+m_dHeightAccumulator
								);


						} // end case IGG_TYPE_LINETO
						break ;

					case IFXGlyph2DCommands::IGG_TYPE_CURVETO:
						{
							// get the CurveTo handle:
							IFXGlyphCurveToBlock* pGlyphCurveTo = NULL ;
							iResult = pGlyphCommand->QueryInterface( IID_IFXGlyphCurveToBlock,
								(void**)&pGlyphCurveTo );
							if (IFXSUCCESS(iResult))
								iResult = pGlyphCurveTo->GetData( &fDoubleArray[0],
								&fDoubleArray[1],
								&fDoubleArray[2],
								&fDoubleArray[3],
								&fDoubleArray[4],
								&fDoubleArray[5],
								&uTemp ); // not needed.
							IFXRELEASE( pGlyphCurveTo );

							if(m_pContourGenerator==NULL)
								iResult=IFX_E_NOT_INITIALIZED;


							if(IFXSUCCESS(iResult))
								iResult=m_pContourGenerator->CurveTo(
								fDoubleArray[0]+m_dWidthAccumulator,
								fDoubleArray[1]+m_dHeightAccumulator,
								fDoubleArray[2]+m_dWidthAccumulator,
								fDoubleArray[3]+m_dHeightAccumulator,
								fDoubleArray[4]+m_dWidthAccumulator,
								fDoubleArray[5]+m_dHeightAccumulator,
								uCurveSteps/* curve steps !! */);


						} // end case IGG_TYPE_CURVETO
						break ;

					case IFXGlyph2DCommands::IGG_TYPE_ENDGLYPH:
						{
							// get the EndGlyph handle:
							// EndGlyph tags use a LineTo component, that's
							// why we're QI'ing for IID_IFXGlyphLineToBlock.
							IFXGlyphTagBlock* pGlyphTag = NULL ;
							iResult = pGlyphCommand->QueryInterface( IID_IFXGlyphTagBlock,
								(void**)&pGlyphTag );
							if (IFXSUCCESS(iResult))
								iResult = pGlyphTag->GetData( &fDoubleArray[0],
								&fDoubleArray[1] );

							IFXRELEASE( pGlyphTag );

							if(m_pGlyphString==NULL)
								iResult=IFX_E_NOT_INITIALIZED;

							if(IFXSUCCESS(iResult)) {

								if(IFXSUCCESS(iResult)) {
									m_dWidthAccumulator+=fDoubleArray[0];

									// get the pathlist and store it
									IFXSimpleList* pCompletedGlyph=NULL;
									iResult=IFXCreateComponent(CID_IFXSimpleList, IID_IFXSimpleList,
										(void**) &pCompletedGlyph);
									if(IFXSUCCESS(iResult) && pCompletedGlyph) {
										pCompletedGlyph->Initialize(1);
										iResult=m_pContourGenerator->GetContourList(pCompletedGlyph);
										if(IFXSUCCESS(iResult)) {
											U32 uDummy=0;
											IFXUnknown* pObject=NULL;
											iResult=pCompletedGlyph->QueryInterface(IID_IFXUnknown, (void**)&pObject);
											if(IFXSUCCESS(iResult))
												iResult=m_pGlyphString->Add(pObject, &uDummy);
											IFXRELEASE(pObject);
										}
									}
									IFXRELEASE(pCompletedGlyph);
								}
							}
							IFXRELEASE(m_pContourGenerator);


						} // end case IGG_TYPE_ENDGLYPH
						break ;

					default:
						// complain?
						break ;
					} // end switch( uCommandType )
				} // end if (we got the command type)

				IFXRELEASE( pGlyphCommand );
			} // end if ( we got the next entry in the list)

			IFXRELEASE( pUnk );
		} // end for (each element in the simple list)
	} // end if (we got the glyph command list)

	return iResult;
}



// CIFXGlyph3DGenerator private methods

CIFXGlyph3DGenerator::CIFXGlyph3DGenerator()
{
	m_uRefCount=0;
	m_pGlyphString=NULL;
	m_pGlyphCommands = NULL;
	m_pContourGenerator=NULL;
	m_pGlyphGenParams=NULL;
	m_dWidthAccumulator=0;
	m_dHeightAccumulator=0;
	m_pMeshToGlyphIndexMap=NULL;
	xMin = (F64)((U32)-1);
	xMax = -(F64)((U32)-1);
	yMin = (F64)((U32)-1);
	yMax = -(F64)((U32)-1);
}

CIFXGlyph3DGenerator::~CIFXGlyph3DGenerator() {

	IFXRELEASE(m_pGlyphString);
	IFXRELEASE(m_pGlyphCommands);
	IFXRELEASE(m_pContourGenerator);

	if(m_pGlyphGenParams)
		delete m_pGlyphGenParams;

	if(m_pMeshToGlyphIndexMap)
		delete m_pMeshToGlyphIndexMap;

}


//---------------------------------------------------------------------------
//  CIFXTextGenerator::CallTessellator
//
//  This private method calls the tessellator on each glyph in the string
//---------------------------------------------------------------------------
IFXRESULT CIFXGlyph3DGenerator::CallTessellator(SIFXGlyphMeshParams* pInitInfo, IFXContourTessellator* pContourTessellator,
												IFXMeshGroup* pFinalMeshGroup, U32* pFinalMeshGroupIndex)
{
	IFXRESULT iResult=IFX_OK;

	U32 uGlyphCount, uGlyphIndex;
	IFXUnknown* pUnknown=NULL;
	IFXSimpleList* pGlyph=NULL;
	IFXMeshGroup* pMeshGroup=NULL;

	// set the Tessellator properties
	SIFXTessellatorProperties sTessProperties=pInitInfo->sTessellator;

	// walk the glyph list and tessellate each one
	iResult = m_pGlyphString->GetCount( &uGlyphCount );
	uGlyphIndex=0;

	while(uGlyphIndex<uGlyphCount)
	{
		pUnknown=NULL;

		if( IFXSUCCESS(iResult) )
		{
			iResult=m_pGlyphString->Get(uGlyphIndex, &pUnknown);
		}

		if( IFXSUCCESS(iResult) )
		{
			iResult=pUnknown->QueryInterface(IID_IFXSimpleList, (void**)&pGlyph);
		}

		IFXRELEASE( pUnknown );

		if(IFXSUCCESS(iResult) )
		{
			if(pGlyph)
			{

				iResult=pContourTessellator->Tessellate(&sTessProperties, pGlyph, &pMeshGroup);

				//If Tessellate() fails, do not insert a mesh and move on to next glyph.  If the
				//glyph is extruded, the extruded part will still show up.
				if(iResult == IFX_E_ABORTED) {
					/// @todo:  Put in an error code for Tessellation failed.
					iResult = IFX_OK;
					uGlyphIndex++;
					continue;
				}

				if(IFXSUCCESS(iResult))
				{

					if(pMeshGroup!=NULL)
					{

						U32 uMeshIndex=0;
						U32 uMeshCount=0;
						IFXMesh* pMesh=NULL;
						uMeshCount=pMeshGroup->GetNumMeshes();

						while(uMeshIndex<uMeshCount)
						{
							pMeshGroup->GetMesh(uMeshIndex, pMesh);
							if(pMesh!=NULL)
							{
								pFinalMeshGroup->SetMesh(*pFinalMeshGroupIndex, pMesh);

								if(m_pMeshToGlyphIndexMap) {
									m_pMeshToGlyphIndexMap[*pFinalMeshGroupIndex]=uGlyphIndex;
								}

								(*pFinalMeshGroupIndex)=(*pFinalMeshGroupIndex)+1;
							}
							IFXRELEASE(pMesh);
							uMeshIndex++;
						}
					}
				}
			}

			IFXRELEASE(pGlyph);
		}

		uGlyphIndex++;
	}

	IFXRETURN(iResult);
}


//---------------------------------------------------------------------------
//  CIFXTextGenerator::GetTotalMeshCount
//
//  This private method counts up all the contours and their facing settings
//  so that we can make an accurately sized mesh group.
//---------------------------------------------------------------------------
IFXRESULT CIFXGlyph3DGenerator::GetTotalMeshCount(SIFXGlyphMeshParams* pParams, U32* pCount) {
	IFXRESULT iResult=IFX_OK;

	if(pParams==NULL || pCount==NULL)
		iResult=IFX_E_INVALID_POINTER;

	if(IFXSUCCESS(iResult)) {
		U32 uTotalContourCount=0;
		U32 uTotalMeshCount=0;

		// walk the glyph list and extrude each one
		U32 uGlyphCount=0;
		U32 uGlyphIndex=0;
		U32 uCount=0;
		IFXUnknown* pUnknown=NULL;
		IFXSimpleList* pGlyph=NULL;
		iResult = m_pGlyphString->GetCount( &uGlyphCount );

		while(uGlyphIndex<uGlyphCount) {
			pUnknown=NULL;
			pGlyph=NULL;
			iResult=m_pGlyphString->Get(uGlyphIndex, &pUnknown);
			iResult=pUnknown->QueryInterface(IID_IFXSimpleList, (void**)&pGlyph);
			if(IFXSUCCESS(iResult) && pGlyph) {
				uCount=0;
				pGlyph->GetCount( &uCount );
				uTotalContourCount+=uCount;
			}
			IFXRELEASE(pGlyph);
			uGlyphIndex++;
			IFXRELEASE(pUnknown);
		}

		if(IFXSUCCESS(iResult)) {
			if(pParams->bExtrude==IFX_TRUE) {

				if(pParams->sExtruder.eFacing==IFXGlyphBothFacing)
					uTotalMeshCount+=2*uTotalContourCount*(pParams->sExtruder.uDepthSteps);
				else
					uTotalMeshCount+=uTotalContourCount*(pParams->sExtruder.uDepthSteps);

			}

			if(pParams->bTessellate==IFX_TRUE) {
				/** @todo: pParams->sTessellator.eFacing is not set yet.  If accessed  at this
				 point in the code, it will be uninitialized.  All meshes
				 are either double or single sided.  In other words, the frontCap,
				 backCap and exturder are all single or double sided.
				 They cannot be set individually.
				*/
				if(pParams->sCombiner.bFrontCap==IFX_TRUE) {
					if(pParams->sCombiner.eFrontCapFacing==IFXGlyphBothFacing) {
						uTotalMeshCount+=2*uGlyphCount;
					} else
						uTotalMeshCount+=uGlyphCount;
				}
				if(pParams->sCombiner.bBackCap==IFX_TRUE) {
					if(pParams->sCombiner.eBackCapFacing==IFXGlyphBothFacing) {
						uTotalMeshCount+=2*uGlyphCount;
					} else
						uTotalMeshCount+=uGlyphCount;
				}

			}
		}
		*pCount=uTotalMeshCount;
	}
	IFXRETURN(iResult);
}


// CIFXGlyph3DGenerator methods for texcoord generation

IFXRESULT CIFXGlyph3DGenerator::ApplyTextureCoordinates(IFXMeshGroup* pMeshGroup)
{
	IFXRESULT iResult = IFX_OK;

	U32 i = 0;

	if(NULL == pMeshGroup)
	{
		iResult = IFX_E_INVALID_POINTER;
	}

	if(IFXSUCCESS(iResult))
	{
		IFXVector3 vMin, vMax;
		U32 uNumMeshes = pMeshGroup->GetNumMeshes();
		IFXMesh* pMesh = 0;
		pMeshGroup->GetMesh(0, pMesh);

		// Set Initial Conditions
		if(pMesh)
		{
			IFXVector3Iter posIter;
			pMesh->GetPositionIter(posIter);
			IFXVector3* pvPos = posIter.Index(0);
			vMin = *pvPos;
			vMax = *pvPos;

			IFXRELEASE(pMesh);

			for(i = 0; i < uNumMeshes && IFXSUCCESS(iResult); i++)
			{
				pMeshGroup->GetMesh(i, pMesh);
				iResult = CalcBoundingBox(pMesh, vMin, vMax);
				IFXRELEASE(pMesh);
			}

			if(IFXSUCCESS(iResult))
			{
				for(i = 0; i < uNumMeshes && IFXSUCCESS(iResult); i++)
				{
					pMeshGroup->GetMesh(i, pMesh);
					iResult = ApplyTextureCoordinates(pMesh, vMin, vMax);
					IFXRELEASE(pMesh);
				}
			}
		}
	}

	return iResult;
}

IFXRESULT CIFXGlyph3DGenerator::ApplyTextureCoordinates(IFXMesh* pMesh,
														IFXVector3 vBoxMin,
														IFXVector3 vBoxMax)
{
	IFXRESULT iResult = IFX_OK;

	if(NULL == pMesh)
		iResult = IFX_E_INVALID_POINTER;

	if(IFXSUCCESS(iResult))
	{
		IFXVertexAttributes attribs;
		attribs = pMesh->GetAttributes();

		IFXASSERT(1 == attribs.m_uData.m_uNumTexCoordLayers);

		IFXVector2Iter texCoordIter;
		IFXVector3Iter posIter;
		IFXVector2* pvTexCoord;
		IFXVector3* pvPos;

		pMesh->GetTexCoordIter(texCoordIter);
		pMesh->GetPositionIter(posIter);

		U32 uNumVerts = pMesh->GetMaxNumVertices();

		IFXVector3 boxSize;
		boxSize.Subtract(vBoxMax,vBoxMin);



		// subtract eps of the unit square.  texture coordinates will go
		// from eps to 1 - eps.  this will aovid flickering textures when
		// near filtering is turned off.
		F32 eps = 1.0f/1024.0f;

		if(boxSize.X())
			boxSize.X() = (1.0f - 2*eps) / boxSize.X();
		if(boxSize.Y())
			boxSize.Y() = (1.0f - 2*eps) / boxSize.Y();

		U32 i;
		for( i = 0; i < uNumVerts; i++)
		{
			pvPos = posIter.Next();
			pvTexCoord = texCoordIter.Next();

			F32 u,v;

			u = (pvPos->X() - vBoxMin.X()) * boxSize.X() + eps;
			v = (pvPos->Y() - vBoxMin.Y()) * boxSize.Y() + eps;

			pvTexCoord->U() = u;
			pvTexCoord->V() = v;
		}
	}

	return iResult;
}

IFXRESULT CIFXGlyph3DGenerator::CalcBoundingBox(IFXMesh* pMesh,
												IFXVector3& vBoxMin,
												IFXVector3& vBoxMax )
{
	IFXRESULT iResult = IFX_OK;

	if(NULL == pMesh)
	{
		iResult = IFX_E_INVALID_POINTER;
	}

	if(IFXSUCCESS(iResult))
	{
		IFXVector3* pvPos;
		IFXVector3Iter posIter;
		pMesh->GetPositionIter(posIter);

		U32 uNumVerts = pMesh->GetMaxNumVertices();
		U32 i;
		for( i = 0; i < uNumVerts; i++)
		{
			pvPos = posIter.Next();

			if(pvPos->X() < vBoxMin.X())
				vBoxMin.X() = pvPos->X();
			if(pvPos->Y() < vBoxMin.Y())
				vBoxMin.Y() = pvPos->Y();

			if(pvPos->X() > vBoxMax.X())
				vBoxMax.X() = pvPos->X();
			if(pvPos->Y() > vBoxMax.Y())
				vBoxMax.Y() = pvPos->Y();
		}
	}

	return iResult;
}


IFXRESULT CIFXGlyph3DGenerator::CollapseFinalMeshGroup(IFXMeshGroup* pInMeshGroup,
													   IFXMeshGroup** ppOutMeshGroup,
													   U32 uGlyphCount )
{
	IFXRESULT iResult = IFX_OK;

	if(NULL == pInMeshGroup)
		iResult = IFX_E_INVALID_POINTER;
	if(NULL == ppOutMeshGroup)
		iResult = IFX_E_INVALID_POINTER;

	IFXMeshGroup* pMeshGroup = NULL;
	U32* pMeshCounts = NULL;
	IFXMesh** ppMeshList = NULL;
	U32 uMeshCount = 0;

	U32 i = 0;
	U32 j = 0;

	if(IFXSUCCESS(iResult))
	{
		IFXCreateComponent(CID_IFXMeshGroup, IID_IFXMeshGroup, (void**)ppOutMeshGroup);
		if(NULL == *ppOutMeshGroup)
			iResult = IFX_E_OUT_OF_MEMORY;
		else
			pMeshGroup = *ppOutMeshGroup;
	}

	if(IFXSUCCESS(iResult))
	{
		iResult = pMeshGroup->Allocate(uGlyphCount);
	}

	if(IFXSUCCESS(iResult))
	{
		pMeshCounts = new U32[uGlyphCount];
		if(NULL == pMeshCounts)
			iResult = IFX_E_OUT_OF_MEMORY;
	}

	if(IFXSUCCESS(iResult))
	{
		uMeshCount = pInMeshGroup->GetNumMeshes();

		// Invalidate pMeshCounts array
		for(i = 0; i < uGlyphCount; i++)
		{
			pMeshCounts[i] = 0;
		}

		for(i = 0; i < uMeshCount; i++)
		{
			// test value of m_pMeshToGlyphIndexMap[i] first.  Unused meshes have an index value of -1.
			if(m_pMeshToGlyphIndexMap[i]>=0 && m_pMeshToGlyphIndexMap[i]<(I32)uGlyphCount)
			{
				pMeshCounts[m_pMeshToGlyphIndexMap[i]]++;
			}
		}

		U32 uCurrentIndex = 0;
		for(i =0; i < uGlyphCount && IFXSUCCESS(iResult); i++)
		{
			ppMeshList = new IFXMesh*[pMeshCounts[i]];
			for(j = 0; j < pMeshCounts[i]; j++)
			{
				pInMeshGroup->GetMesh(uCurrentIndex, ppMeshList[j]);
				uCurrentIndex++;
			}
			IFXMesh* pOutMesh = NULL;

			// well don't do this if there are no meshes to collapse.
			if( pMeshCounts[i] )
				iResult = CollapseGlyph(ppMeshList, &pOutMesh, pMeshCounts[i]);

			// it looks like it's desirable to set the mesh to NULL if the mesh group
			// collapse resulted in an empty mesh (or it never collapsed it due to a zero
			// mesh count)
			if(IFXSUCCESS(iResult))
			{
				pMeshGroup->SetMesh(i, pOutMesh);
			}

			// Release the meshes from the mesh array
			for(j = 0; j < pMeshCounts[i]; j++)
			{
				IFXRELEASE(ppMeshList[j]);
			}

			IFXRELEASE(pOutMesh);

			IFXDELETE_ARRAY(ppMeshList);
		}

		IFXDELETE_ARRAY( pMeshCounts );
	}

	return iResult;
}

IFXRESULT CIFXGlyph3DGenerator::CollapseGlyph(IFXMesh** ppMeshList,
											  IFXMesh** ppOutMesh,
											  U32 uMeshCount )
{
	IFXRESULT iResult = IFX_OK;

	if(NULL == ppMeshList)
		iResult = IFX_E_INVALID_POINTER;
	if(NULL == ppOutMesh)
		iResult = IFX_E_INVALID_POINTER;

	U32 i = 0;
	U32 j = 0;

	U32 uNumVertices = 0;
	U32 uNumFaces = 0;

	IFXMesh* pMesh = NULL;
	if(IFXSUCCESS(iResult))
	{
		for(i = 0; i < uMeshCount; i++)
		{
			uNumVertices += ppMeshList[i]->GetMaxNumVertices();
			uNumFaces += ppMeshList[i]->GetMaxNumFaces();
		}
	}

	if(IFXSUCCESS(iResult))
	{
		IFXCreateComponent(CID_IFXMesh, IID_IFXMesh, (void**)&pMesh);
		if(pMesh == NULL)
			iResult = IFX_E_OUT_OF_MEMORY;
		else
			*ppOutMesh = pMesh;
	}

	if(IFXSUCCESS(iResult))
	{

		IFXVertexAttributes vertexAttributes;

		iResult = pMesh->Allocate(vertexAttributes,uNumVertices,uNumFaces);
	}

	if(IFXSUCCESS(iResult))
	{
		IFXVertexIter dstVertIter;
		IFXVertexIter srcVertIter;
		IFXFaceIter dstFaceIter;
		IFXFaceIter srcFaceIter;

		pMesh->GetVertexIter(dstVertIter);
		pMesh->GetFaceIter(dstFaceIter);

		IFXMesh* pSrcMesh;

		U32 uVerts;
		U32 uFaces;

		U32 uVertexCount = 0;

		for(i = 0; i < uMeshCount; i++)
		{
			pSrcMesh = ppMeshList[i];

			pSrcMesh->GetVertexIter(srcVertIter);
			pSrcMesh->GetFaceIter(srcFaceIter);

			uVerts = pSrcMesh->GetMaxNumVertices();
			uFaces = pSrcMesh->GetMaxNumFaces();

			for(j = 0; j < uVerts; j++)
			{
				*(dstVertIter.GetPosition()) = *(srcVertIter.GetPosition());
				*(dstVertIter.GetNormal()) = *(srcVertIter.GetNormal());
				*(dstVertIter.GetTexCoord()) = *(srcVertIter.GetTexCoord());

				dstVertIter.Next();
				srcVertIter.Next();
			}

			IFXFace* pFace = NULL;
			for(j = 0; j < uFaces; j++)
			{
				pFace = dstFaceIter.Next();
				*pFace = *(srcFaceIter.Next());

				pFace->SetA(pFace->VertexA() + uVertexCount);
				pFace->SetB(pFace->VertexB() + uVertexCount);
				pFace->SetC(pFace->VertexC() + uVertexCount);
			}

			uVertexCount += uVerts;
		}

		pMesh->UpdateVersionWord(IFX_MESH_POSITION);
		pMesh->UpdateVersionWord(IFX_MESH_NORMAL);
		pMesh->UpdateVersionWord(IFX_MESH_TC0);
	}

	return iResult;
}
