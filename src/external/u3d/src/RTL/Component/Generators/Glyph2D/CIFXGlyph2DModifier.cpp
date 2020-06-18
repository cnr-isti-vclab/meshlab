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
//  CIFXGlyph2DModifier.cpp
//
//  DESCRIPTION
//    Implementation file for the 2D glyph modifier class.
//
//  NOTES
//      None.
//
//***************************************************************************

#include "CIFXGlyph2DModifier.h"
#include "IFXNeighborMesh.h"
#include "IFXCheckX.h"
#include "IFXEncoderX.h"
#include "IFXMarkerX.h"
#include "IFXException.h"
#include "IFXModifierDataPacket.h"
#include "IFXModifierChain.h"
#include "IFXCoreCIDs.h"
#include "IFXDids.h"
#include "IFXArray.h"

#include "IFXExportingCIDs.h"

CIFXGlyph2DModifier::CIFXGlyph2DModifier()
{
	m_uRefCount = 0;

	m_uMeshGroupDataElementIndex    = (U32)-1;
	m_uNeighborMeshDataElementIndex = (U32)-1;
	m_uBoundSphereDataElementIndex  = (U32)-1;


	IFXCreateComponent( CID_IFXGlyph3DGenerator,
		IID_IFXGlyph3DGenerator,
		(void**)&m_pGlyphGenerator);


	// set state for Replaying Glyph Commands on
	// changing smoothness setting.
	m_bBuildingContours=IFX_FALSE;

	// set some nifty default parameters
	m_sParameters.pMeshGroup=NULL;
	m_sParameters.pNeighborMesh=NULL;
	m_attributes = 0;

	m_sParameters.bTessellate=IFX_TRUE;
	m_sParameters.sTessellator.bNormals=IFX_TRUE;
	m_sParameters.sTessellator.eNormalOrientation=IFXGlyphRegularNormals;
	m_sParameters.sTessellator.bWindClockWise=IFX_FALSE;
	m_sParameters.sCombiner.bFrontCap=IFX_TRUE;
	m_sParameters.sCombiner.eFrontCapFacing=IFXGlyphOutFacing;
	m_sParameters.sCombiner.bBackCap=IFX_TRUE;
	m_sParameters.sCombiner.eBackCapFacing=IFXGlyphOutFacing;
	m_sParameters.sCombiner.uNumberOfCurveSteps=8;

	m_sParameters.bExtrude=IFX_FALSE;
	m_sParameters.sExtruder.bNormals=IFX_TRUE;
	m_sParameters.sExtruder.eFacing=IFXGlyphOutFacing;
	m_sParameters.sExtruder.fExtrusionDepth=0.1f;
	m_sParameters.sExtruder.uDepthSteps=1;

	m_sParameters.sExtruder.bBevel=false;
	m_sParameters.sExtruder.fBevelDepth=1.0;
	m_sParameters.sExtruder.eBevelType=IFXGlyphBevelMiter;
	m_sParameters.sExtruder.eBevelFace=IFXGlyphBevelFaceBoth;

	m_sParameters.bCombine=IFX_TRUE;

	m_pBoundSphereDataElement = NULL;


	m_drawOffset.MakeIdentity();

	m_viewTransform.MakeIdentity();

	m_glyphMeshStart = 0;
	m_meshGroupChangeCount = 0;
	m_modelBoundingDistance = 0;
	m_rebuild = TRUE;
}


CIFXGlyph2DModifier::~CIFXGlyph2DModifier()
{
	IFXRELEASE(m_pGlyphGenerator);
	IFXRELEASE(m_sParameters.pMeshGroup);
	IFXRELEASE(m_sParameters.pNeighborMesh);
	IFXRELEASE(m_pBoundSphereDataElement);
}


IFXRESULT IFXAPI_CALLTYPE CIFXGlyph2DModifier_Factory( IFXREFIID iid, void** ppv )
{
	IFXRESULT result;

	if (ppv)
	{
		// Create the CIFXClassName component.
		CIFXGlyph2DModifier *pComponent = new CIFXGlyph2DModifier;

		if (pComponent)
		{
			// Perform a temporary AddRef for our usage of the component.
			pComponent->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			result = pComponent->QueryInterface( iid, ppv );

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



// IFXUnknown interface...

//---------------------------------------------------------------------------
//  CIFXGlyph2DModifier::AddRef
//
//  This method increments the reference count for an interface on a
//  component.  It should be called for every new copy of a pointer to an
//  interface on a given component.  It returns a U32 that contains a value
//  from 1 to 2^32 - 1 that defines the new reference count.  The return
//  value should only be used for debugging purposes.
//---------------------------------------------------------------------------
U32 CIFXGlyph2DModifier::AddRef()
{
	return ++m_uRefCount;
}


//---------------------------------------------------------------------------
//  CIFXGlyph2DModifier::Release
//
//  This method decrements the reference count for the calling interface on a
//  component.  It returns a U32 that contains a value from 1 to 2^32 - 1
//  that defines the new reference count.  The return value should only be
//  used for debugging purposes.  If the reference count on a component falls
//  to zero, the component is destroyed.
//---------------------------------------------------------------------------
U32 CIFXGlyph2DModifier::Release() {
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
//  CIFXGlyph2DModifier::QueryInterface
//
//  This method provides access to the various interfaces supported by a
//  component.  Upon success, it increments the component's reference count,
//  hands back a pointer to the specified interface and returns IFX_OK.
//  Otherwise, it returns either IFX_E_INVALID_POINTER or IFX_E_UNSUPPORTED.
//
//  A number of rules must be adhered to by all implementations of
//  QueryInterface.  For a list of such rules, refer to the Microsoft COM
//  description of the IUnknown::QueryInterface method.
//---------------------------------------------------------------------------
IFXRESULT CIFXGlyph2DModifier::QueryInterface( IFXREFIID riid, void **ppv )
{
	IFXRESULT result = IFX_OK;

	if ( ppv )
	{
		if ( riid == IID_IFXUnknown )
			*ppv = (IFXUnknown*)this;
		else if ( riid == IID_IFXGlyph2DModifier )
			*ppv = (IFXGlyph2DModifier*)this;
		else if ( riid == IID_IFXMarker )
			*ppv = (IFXMarker*)this;
		else if ( riid == IID_IFXMarkerX )
			*ppv = (IFXMarkerX*)this;
		else if ( riid == IID_IFXModifier )
			*ppv = (IFXModifier*)this;
		else if ( riid == IID_IFXSubject )
			*ppv = (IFXSubject*)this;
		else if ( riid == IID_IFXGenerator )
			*ppv = (IFXGenerator*)this;
		else if (riid == IID_IFXMetaDataX)
			*ppv = (IFXMetaDataX*) this;
		else
		{
			*ppv = NULL;
			result = IFX_E_UNSUPPORTED;
		}

		if( IFXSUCCESS(result) )
			AddRef();
	}
	else result = IFX_E_INVALID_POINTER;

	return result;
}


// IFXModifier
const IFXGUID* CIFXGlyph2DModifier::m_scpOutputDIDs[] =
{
	&DID_IFXRenderableGroup,
	&DID_IFXNeighborMesh,
	&DID_IFXRenderableGroupBounds
};

const U32 CIFXGlyph2DModifier::m_scpMeshGroupUnchangedAttrs[] =
{	IFX_MESHGROUP_CB_ALL ^
	(IFX_MESHGROUP_CB_NUMMESHES | IFX_MESHGROUP_CB_TRANSFORM),
	0
};

const IFXGUID* CIFXGlyph2DModifier::m_scpNeighborMeshDeps[] =
{
	&DID_IFXRenderableGroup
};


IFXRESULT CIFXGlyph2DModifier::GetOutputs( IFXGUID**& rpOutOutputs,
										  U32&       rOutNumberOfOutputs,
										  U32*&      rpOutOutputDepAttrs )
{
	rOutNumberOfOutputs = sizeof(m_scpOutputDIDs) / sizeof(IFXGUID*);
	rpOutOutputs = (IFXGUID**)&m_scpOutputDIDs;
	rpOutOutputDepAttrs = (U32*)m_scpMeshGroupUnchangedAttrs;
	return IFX_OK;
}


IFXRESULT CIFXGlyph2DModifier::GetDependencies(
	IFXGUID*   pInOutputDID,
	IFXGUID**& rppOutInputDependencies,
	U32&       rOutNumberInputDependencies,
	IFXGUID**& rppOutOutputDependencies,
	U32&       rOutNumberOfOutputDependencies,
	U32*&    rpOutOutputDepAttrs )
{
	IFXRESULT result = IFX_OK;

	if (pInOutputDID == &DID_IFXNeighborMesh)
	{
		rppOutInputDependencies = NULL;
		rOutNumberInputDependencies = 0;
		rppOutOutputDependencies = (IFXGUID**)m_scpNeighborMeshDeps;
		rOutNumberOfOutputDependencies = sizeof(m_scpNeighborMeshDeps) / sizeof(IFXGUID*);
		rpOutOutputDepAttrs = NULL;
	}
	else if ( (pInOutputDID == &DID_IFXRenderableGroup)
		|| (pInOutputDID == &DID_IFXRenderableGroupBounds) )
	{
		rppOutInputDependencies = NULL;
		rOutNumberInputDependencies = 0;
		rppOutOutputDependencies = NULL;
		rOutNumberOfOutputDependencies = 0;
		rpOutOutputDepAttrs = NULL;
	}
	else
	{
		IFXASSERTBOX( 1, "CIFXGlyph2DModifier::GetDependencies() called with unknown output!" );
		result = IFX_E_UNDEFINED;
	}

	return result;
}


IFXRESULT CIFXGlyph2DModifier::GenerateOutput( U32    inOutputDataElementIndex,
											  void*& rpOutData, BOOL& rNeedRelease )
{
	IFXRESULT result = IFX_OK;

	if ( inOutputDataElementIndex == m_uMeshGroupDataElementIndex )
	{
		U32 CurMeshGroupChangeCount = (U32)-1;
		m_pInputDataPacket->GetDataElementChangeCount(m_uMeshGroupDataElementIndex, CurMeshGroupChangeCount);
		if( ( m_meshGroupChangeCount != CurMeshGroupChangeCount && m_rebuild ) || !m_sParameters.pMeshGroup )
		{
			result = m_pGlyphGenerator->BuildMesh(&m_sParameters);
			m_rebuild = TRUE;
			m_meshGroupChangeCount = CurMeshGroupChangeCount;

			if( m_attributes & SINGLE_SHADER )
			{
				U32 numGlyphMeshes = m_sParameters.pMeshGroup->GetNumMeshes();
				IFXMesh* pCollapsedMesh = NULL;
				IFXMesh** ppMeshList = new IFXMesh*[ numGlyphMeshes ];
				if( !ppMeshList )
					result = IFX_E_OUT_OF_MEMORY;
				U32 i = 0;
				if( IFXSUCCESS( result ) )
				{
					for( ; i < numGlyphMeshes; i++ )
						m_sParameters.pMeshGroup->GetMesh( i, ppMeshList[i] );
					m_pGlyphGenerator->CollapseGlyph( ppMeshList, &pCollapsedMesh, numGlyphMeshes );
					for( i = 0; i < numGlyphMeshes; i++ )
						IFXRELEASE( ppMeshList[i] );
					result = m_sParameters.pMeshGroup->Allocate( 1 );
					if( IFXSUCCESS( result ) )
						m_sParameters.pMeshGroup->SetMesh( 0, pCollapsedMesh );
					IFXRELEASE( pCollapsedMesh );
					delete [] ppMeshList;
				}
			}
		}
		if (IFXSUCCESS(result))
		{
			IFXShaderList* pShaderList = NULL;
			IFXMeshGroup* pInputMeshGroup = NULL;
			IFXMeshGroup* pOutputMeshGroup = NULL;
			IFXCreateComponent( CID_IFXMeshGroup, IID_IFXMeshGroup, (void**)&pOutputMeshGroup );
			IFXMatrix4x4 billboardOffset = CalcBillboardOffset();
			//try to get input meshgroup. If there is no one, glyph is a generator.
			result = m_pInputDataPacket->GetDataElement( m_uMeshGroupDataElementIndex,
				IID_IFXMeshGroup,
				(void**)&pInputMeshGroup );
			IFXMesh* pMesh = NULL;
			U32 numGlyphMeshes = m_sParameters.pMeshGroup->GetNumMeshes();
			if (IFXSUCCESS(result))
				//Glyph is a modifier
			{
				m_modelBoundingDistance = pInputMeshGroup->CalcBoundingSphere()[3];
				m_glyphMeshStart = pInputMeshGroup->GetNumMeshes();
				U32 numMeshes = m_glyphMeshStart+numGlyphMeshes;

				pOutputMeshGroup->Allocate( numMeshes );
				U32 i;
				for(  i = 0; i < m_glyphMeshStart; i++ )
				{
					pInputMeshGroup->GetMesh( i, pMesh );
					if( pMesh )
					{
						pInputMeshGroup->GetElementShaderList( i, &pShaderList );
						pOutputMeshGroup->SetMesh( i, pMesh );
						pOutputMeshGroup->SetElementShaderList( i, pShaderList );
						IFXRELEASE(pMesh);
					}
				}
				for(  i = 0; i < numGlyphMeshes; i++ )
				{
					m_sParameters.pMeshGroup->GetMesh( i, pMesh );
					pMesh->SetOffsetTransform( &billboardOffset );
					pMesh->SetRenderableType( IFXRENDERABLE_ELEMENT_TYPE_GLYPH );
					pOutputMeshGroup->SetMesh( m_glyphMeshStart+i, pMesh );
					IFXRELEASE(pMesh);
				}
				IFXRELEASE( pInputMeshGroup );
			}
			else
				//Glyph is a generator
			{
				result = IFX_OK;
				m_glyphMeshStart = 0;
				U32 numMeshes = numGlyphMeshes;
				pOutputMeshGroup->Allocate( numMeshes );
				U32 i;
				for(  i = 0; i < numGlyphMeshes; i++ )
				{
					m_sParameters.pMeshGroup->GetMesh( i, pMesh );
					if( pMesh )
					{
						pMesh->SetOffsetTransform( &billboardOffset );
						pOutputMeshGroup->SetMesh( i, pMesh );
						IFXRELEASE(pMesh);
					}
				}
			}
			pOutputMeshGroup->QueryInterface( IID_IFXUnknown, &rpOutData );
			rNeedRelease = TRUE;
			IFXRELEASE(pOutputMeshGroup);
			IFXRELEASE(pShaderList);
		}
	}
	else if ( inOutputDataElementIndex == m_uBoundSphereDataElementIndex )
	{
		// The following should be simplified using the appropriate implicit function.
		IFXUnknown* pUnk = NULL;
		result = m_pModifierDataPacket->GetDataElement(m_uMeshGroupDataElementIndex, (void**)&pUnk);
		if (pUnk)
		{
			IFXMeshGroup* pMG = NULL;
			pUnk->QueryInterface( IID_IFXMeshGroup, (void**)&pMG );
			m_pBoundSphereDataElement->Bound() =
				pMG->CalcBoundingSphere();
			IFXRELEASE(pUnk);
			IFXRELEASE(pMG);
		}
		m_pBoundSphereDataElement->QueryInterface( IID_IFXUnknown, &rpOutData );
		rNeedRelease = TRUE;
	}
	else if ( inOutputDataElementIndex == m_uNeighborMeshDataElementIndex )
	{
		if ( m_sParameters.pNeighborMesh == NULL )
			IFXCreateComponent(CID_IFXNeighborMesh,
			IID_IFXNeighborMesh,
			(void**)&(m_sParameters.pNeighborMesh));
		if (!m_sParameters.pNeighborMesh)
			result = IFX_E_OUT_OF_MEMORY;
		if (IFXSUCCESS(result))
			result = m_sParameters.pNeighborMesh->Allocate(*(m_sParameters.pMeshGroup));
		if (IFXSUCCESS(result))
			result = m_sParameters.pNeighborMesh->Build(*(m_sParameters.pMeshGroup), NULL);
		if (IFXSUCCESS(result))
			result = m_sParameters.pNeighborMesh->QueryInterface( IID_IFXUnknown, &rpOutData );
		if (IFXSUCCESS(result))
			rNeedRelease = TRUE;
	}
	else
	{
		IFXASSERTBOX(0, "Calling generate output on invalid DID!");
		result = IFX_E_INVALID_RANGE;
	}

	return result;
}

IFXRESULT CIFXGlyph2DModifier::SetDataPacket(
	IFXModifierDataPacket* pInInputDataPacket,
	IFXModifierDataPacket* pInDataPacket )
{
	IFXRESULT result = IFX_OK;

	IFXRELEASE(m_pModifierDataPacket);
	IFXRELEASE(m_pInputDataPacket);

	if (pInDataPacket && pInInputDataPacket)
	{
		pInDataPacket->AddRef();
		m_pModifierDataPacket = pInDataPacket;
		pInInputDataPacket->AddRef();
		m_pInputDataPacket = pInInputDataPacket;
	}
	else
	{
		result = IFX_E_INVALID_POINTER;
	}

	if( IFXSUCCESS(result) )
		result = pInDataPacket->GetDataElementIndex(
		DID_IFXRenderableGroup,
		m_uMeshGroupDataElementIndex );

	if( IFXSUCCESS(result) )
		m_pBoundSphereDataElement->RenderableIndex() =
		m_uMeshGroupDataElementIndex;

	if( IFXSUCCESS(result) )
		result = pInDataPacket->GetDataElementIndex(
		DID_IFXRenderableGroupBounds,
		m_uBoundSphereDataElementIndex );

	if( IFXSUCCESS(result) )
		result = pInDataPacket->GetDataElementIndex(
		DID_IFXNeighborMesh,
		m_uNeighborMeshDataElementIndex );

	return result;
}


IFXRESULT CIFXGlyph2DModifier::Notify( IFXModifierMessage eInMessage,
									  void*               pMessageContext )
{
	return IFX_OK;
}


// IFXMarker
IFXRESULT CIFXGlyph2DModifier::SetSceneGraph( IFXSceneGraph* pInSceneGraph )
{
	IFXRESULT result = IFX_OK;

	if (!m_pBoundSphereDataElement)
		result = IFXCreateComponent(CID_IFXBoundSphereDataElement, IID_IFXBoundSphereDataElement, (void**) &m_pBoundSphereDataElement);
	if ( IFXSUCCESS(result) )
		result = CIFXMarker::SetSceneGraph(pInSceneGraph);

	return result;
}


// IFXMarkerX
void CIFXGlyph2DModifier::GetEncoderX(IFXEncoderX*& rpEncoderX)
{
	CIFXMarker::GetEncoderX(CID_IFXGlyphGeneratorEncoder, rpEncoderX);
}


// IFXGlyph2DModifier
IFXRESULT CIFXGlyph2DModifier::Initialize( F64 width, F64 spacing, F64 height )
{
	SIFXGlyphGeneratorParams par;
	par.fDefaultGlyphHeight = height;
	par.fDefaultGlyphSpacing = spacing;
	par.fDefaultGlyphWidth = width;
	m_pGlyphGenerator->Initialize( &par );
	return IFX_OK;
}

IFXRESULT CIFXGlyph2DModifier::SetAttributes(U32 glyphAttributes)
{
	m_attributes = glyphAttributes;
	if( m_attributes & BILLBOARD || m_attributes & SINGLE_SHADER )
		InvalidateMeshGroup();
	return IFX_OK;
}

IFXRESULT CIFXGlyph2DModifier::GetAttributes(U32* pGlyphAttributes)
{
	IFXRESULT result = IFX_OK;
	if(pGlyphAttributes == NULL)
		result = IFX_E_INVALID_POINTER;

	if(IFXSUCCESS(result))
		*pGlyphAttributes = m_attributes;
	return result;
}

// exposure of contour generator fuctions
IFXRESULT CIFXGlyph2DModifier::StartPath() {
	IFXRESULT iResult=IFX_OK;
	if(m_pGlyphGenerator==NULL)
		iResult=IFX_E_NOT_INITIALIZED;
	else {
		iResult=m_pGlyphGenerator->StartPath();
		InvalidateMeshGroup();
	}
	return iResult;
}
IFXRESULT CIFXGlyph2DModifier::LineTo(F64 fX, F64 fY) {
	IFXRESULT iResult=IFX_OK;
	if(m_pGlyphGenerator==NULL)
		iResult=IFX_E_NOT_INITIALIZED;
	else {
		iResult=m_pGlyphGenerator->LineTo(fX, fY);

		InvalidateMeshGroup();
	}
	return iResult;
}
IFXRESULT CIFXGlyph2DModifier::MoveTo(F64 fX, F64 fY) {
	IFXRESULT iResult=IFX_OK;
	if(m_pGlyphGenerator==NULL)
		iResult=IFX_E_NOT_INITIALIZED;
	else {
		iResult=m_pGlyphGenerator->MoveTo(fX, fY);
		InvalidateMeshGroup();
	}
	return iResult;
}
IFXRESULT CIFXGlyph2DModifier::CurveTo(F64 fCx1, F64 fCy1, F64 fCx2, F64 fCy2, F64 fAx, F64 fAy) {
	IFXRESULT iResult=IFX_OK;
	if(m_pGlyphGenerator==NULL)
		iResult=IFX_E_NOT_INITIALIZED;
	else {
		iResult=m_pGlyphGenerator->CurveTo(fCx1, fCy1, fCx2, fCy2, fAx, fAy, m_sParameters.sCombiner.uNumberOfCurveSteps);
		InvalidateMeshGroup();
	}
	return iResult;
}
IFXRESULT CIFXGlyph2DModifier::EndPath() {
	IFXRESULT iResult=IFX_OK;
	if(m_pGlyphGenerator==NULL)
		iResult=IFX_E_NOT_INITIALIZED;
	else {
		iResult=m_pGlyphGenerator->EndPath();
		InvalidateMeshGroup();
	}
	return iResult;
}
IFXRESULT CIFXGlyph2DModifier::GetBoundingBox(F64 pMin[3], F64 pMax[3])
{
	IFXRESULT iResult=IFX_OK;
	if(m_pGlyphGenerator==NULL)
		iResult=IFX_E_NOT_INITIALIZED;
	else {
		iResult=m_pGlyphGenerator->GetBoundingBox(pMin, pMax);

		pMin[2]=0.0;
		pMax[2]=1.0;

	}
	return iResult;
}
IFXRESULT CIFXGlyph2DModifier::GetCharIndex(U32 uPickedMeshIndex, I32 *pCharIndex)
{
	IFXRESULT iResult=IFX_OK;
	if(m_pGlyphGenerator==NULL)
		iResult=IFX_E_NOT_INITIALIZED;
	else {
		iResult=m_pGlyphGenerator->GetCharIndex(uPickedMeshIndex, pCharIndex);
	}

	return iResult;
}


// additional glyph string generator functions
IFXRESULT CIFXGlyph2DModifier::StartGlyphString() {
	IFXRESULT iResult=IFX_OK;
	if(m_pGlyphGenerator==NULL)
		iResult=IFX_E_NOT_INITIALIZED;
	else {
		iResult=m_pGlyphGenerator->StartGlyphString();
		InvalidateMeshGroup();
		m_bBuildingContours=IFX_TRUE;
	}
	return iResult;
}
IFXRESULT CIFXGlyph2DModifier::StartGlyph() {
	IFXRESULT iResult=IFX_OK;
	if(m_pGlyphGenerator==NULL)
		iResult=IFX_E_NOT_INITIALIZED;
	else {
		iResult=m_pGlyphGenerator->StartGlyph();
		InvalidateMeshGroup();
	}
	return iResult;
}
IFXRESULT CIFXGlyph2DModifier::EndGlyph(F64 fWidth, F64 fHeight) {
	IFXRESULT iResult=IFX_OK;
	if(m_pGlyphGenerator==NULL)
		iResult=IFX_E_NOT_INITIALIZED;
	else {
		iResult=m_pGlyphGenerator->EndGlyph(fWidth, fHeight);
		InvalidateMeshGroup();
	} return iResult;
}
IFXRESULT CIFXGlyph2DModifier::EndGlyphString() {
	IFXRESULT iResult=IFX_OK;
	if(m_pGlyphGenerator==NULL)
		iResult=IFX_E_NOT_INITIALIZED;
	else {
		iResult=m_pGlyphGenerator->EndGlyphString();
		InvalidateMeshGroup();
		m_bBuildingContours=IFX_FALSE;
	}
	return iResult;
}

IFXRESULT CIFXGlyph2DModifier::GetGlyphCommandList(IFXSimpleList** ppGlyphList) {
	IFXRESULT iResult=IFX_OK;
	if( NULL == ppGlyphList || NULL == m_pGlyphGenerator)
		iResult = IFX_E_INVALID_POINTER;

	if(IFXSUCCESS(iResult)) {
		iResult=m_pGlyphGenerator->GetGlyphCommandList(ppGlyphList);
	}
	return iResult;
}

IFXRESULT CIFXGlyph2DModifier::SetGlyphCommandList(IFXSimpleList* pGlyphList) {
	IFXRESULT iResult=IFX_OK;
	if( NULL == pGlyphList || NULL == m_pGlyphGenerator)
		iResult = IFX_E_INVALID_POINTER;

	if(IFXSUCCESS(iResult)) {
		iResult=m_pGlyphGenerator->SetGlyphCommandList(pGlyphList);
		InvalidateMeshGroup();
	}
	return iResult;
}



IFXRESULT CIFXGlyph2DModifier::InvalidateMeshGroup()
{
	// Invalidate the modifier dataPackets meshGroup
	if( m_pModifierDataPacket )
		return ( m_pModifierDataPacket->
		InvalidateDataElement( m_uMeshGroupDataElementIndex ) );
	else
		return IFX_E_NOT_INITIALIZED;
}


IFXRESULT CIFXGlyph2DModifier::SetBoundingSphere(const IFXVector4& vInBoundingSphere)
{
	m_pBoundSphereDataElement->Bound() = vInBoundingSphere;
	return m_pModifierDataPacket->InvalidateDataElement(m_uBoundSphereDataElementIndex);
}


IFXRESULT CIFXGlyph2DModifier::SetTransform(const IFXMatrix4x4& offset)
{
	m_drawOffset = offset;
	return IFX_OK;
}


IFXRESULT CIFXGlyph2DModifier::InvalidateTransform()
{
	if(m_pModifierDataPacket)
	{
		U32 transformDIDIndex = 0;
		m_pModifierDataPacket->GetDataElementIndex( DID_IFXTransform, transformDIDIndex );
		if( transformDIDIndex )
			m_pModifierDataPacket->InvalidateDataElement( transformDIDIndex );
	}
	return IFX_OK;
}

IFXMatrix4x4 CIFXGlyph2DModifier::CalcBillboardOffset()
{
	IFXMatrix4x4 billboardOffset;
	if( m_attributes & BILLBOARD )
	{
		billboardOffset = m_viewTransform;
		billboardOffset[12] = 0;
		billboardOffset[13] = 0;
		billboardOffset[14] = 0;
	}
	else
		billboardOffset.MakeIdentity();

	IFXMatrix4x4 bbOffsetPostDraw, bbOffsetPostBoundCorrection, bbOffsetFinal;
	IFXMatrix4x4 MGTransformInverted, boundCorrection;
	bbOffsetPostDraw.Multiply( billboardOffset, m_drawOffset );
	F64 min[3];
	F64 max[3];
	GetBoundingBox( min, max );
	boundCorrection.MakeIdentity();
	boundCorrection[12] = (F32)(-min[0]-max[0])/2;
	boundCorrection[13] = (F32)(-min[1]-max[1])/2;
	boundCorrection[14] = m_modelBoundingDistance;
	bbOffsetPostBoundCorrection.Multiply( bbOffsetPostDraw, boundCorrection );
	MGTransformInverted.MakeIdentity();

	U32 transformDataElementIndex = 0;
	m_pInputDataPacket->GetDataElementIndex(
		DID_IFXTransform,
		transformDataElementIndex );

	if( transformDataElementIndex != U32(-1) )
	{
		IFXArray<IFXMatrix4x4>* pMeshGroupTransforms = NULL;
		m_pInputDataPacket->GetDataElement( transformDataElementIndex,
			(void**)&pMeshGroupTransforms);
		IFXMatrix4x4 meshGroupTransform = pMeshGroupTransforms->GetElement( 0 );
		MGTransformInverted.Invert( meshGroupTransform );
		MGTransformInverted[12] = 0;
		MGTransformInverted[13] = 0;
		MGTransformInverted[14] = 0;
	}
	bbOffsetFinal.Multiply( MGTransformInverted, bbOffsetPostBoundCorrection );
	return bbOffsetFinal;
}

IFXRESULT CIFXGlyph2DModifier::SetViewTransform( IFXMatrix4x4 transform )
{
  m_viewTransform = transform;
  m_rebuild = FALSE;
  return InvalidateMeshGroup();
}
