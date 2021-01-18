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

/**
	@file  CIFXView.cpp
*/

#include <float.h>

#include "IFXModifierChain.h"
#include "CIFXView.h"
#include "IFXLightSet.h"
#include "IFXPickObject.h"
#include "IFXRenderable.h"
#include "IFXModifierDataElementIter.h"
#include "IFXModifierDataPacket.h"
#include "IFXShader.h"
#include "IFXSimpleList.h"
#include "IFXSpatialSetQuery.h"
#include "IFXAutoRelease.h"
#include "IFXBoundSphereDataElement.h"
#include "IFXDids.h"
#include "IFXRenderWindow.h"

#include "IFXExportingCIDs.h"
#include "IFXRenderingCIDs.h"
#include "IFXGlyph2DModifier.h"

IFXMeshPtr CIFXView::ms_spLayerMesh;
IFXRenderMaterial CIFXView::ms_LayerMaterial;
IFXRenderBlend CIFXView::ms_LayerBlend;
IFXRenderTexUnit CIFXView::ms_LayerTexUnit;

IFXRESULT FindModifier( IFXModel* pModel, IFXREFIID iid, IFXModifier** ppMod, U32 number = 0 );

/*
	This class manages the list of pointers to IFXShader interfaces.
	It does not AddRef and Release any interface pointers.
	It is used solely only by CIFXView::RenderView.
	There's a single global instance of the list that's reused by
	all CIFXView instances for temporary storage.
*/
class IFXShaderPointerList
{
public:
	IFXShaderPointerList()
	:	m_ppShaders( NULL ),
		m_shadersUsed( 0 ),
		m_shadersAllocated( 0 )
	{
	};

	~IFXShaderPointerList()
	{
		if ( NULL != m_ppShaders )
			Deallocate();
	};

	IFXRESULT Allocate()
	{
		IFXRESULT result = IFX_OK;

		if ( NULL == m_ppShaders )
		{
			m_ppShaders =
				(IFXShader**)IFXAllocate( INITIAL_SIZE*sizeof(IFXShader*) );

			if( NULL != m_ppShaders )
			{
				m_shadersAllocated = INITIAL_SIZE;
				memset( m_ppShaders, 0, INITIAL_SIZE * sizeof(IFXShader*) );

				m_shadersUsed = 0;
			}
			else
				result = IFX_E_OUT_OF_MEMORY;
		}

		return result;
	};

	void Deallocate()
	{
		IFXDeallocate( m_ppShaders );
		m_ppShaders = NULL;
		m_shadersAllocated = 0;
		m_shadersUsed = 0;
	};

	void Reset()
	{
		memset( m_ppShaders, 0, m_shadersUsed * sizeof(IFXShader*) );
		m_shadersUsed = 0;
	};

	const U32& GetNumShaders() const
	{
		return m_shadersUsed;
	};

	IFXShader* GetShaderNR( const U32 index ) const
	{
		IFXShader* pShader = NULL;

		if( index < m_shadersUsed && NULL != m_ppShaders[index] )
			pShader = m_ppShaders[index];

		return pShader;
	};

	IFXRESULT SetShader( IFXShader* pShader )
	{
		IFXRESULT result = IFX_OK;

		if( NULL == m_ppShaders )
			result = IFX_E_NOT_INITIALIZED;

		if( m_shadersUsed+1 > m_shadersAllocated )
			result = Reallocate();

		if( IFXSUCCESS( result ) )
		{
			m_ppShaders[m_shadersUsed] = pShader;
			m_shadersUsed++;
		}

		return result;
	};

protected:
	IFXRESULT Reallocate()
	{
		IFXRESULT result = IFX_OK;

		m_ppShaders = (IFXShader**)IFXReallocate(
			m_ppShaders,
			sizeof(IFXShader*)*m_shadersAllocated*GROW_FACTOR);

		if( NULL != m_ppShaders )
		{
			m_shadersAllocated *= GROW_FACTOR;
			memset( &m_ppShaders[m_shadersUsed], 0,
				(m_shadersAllocated-m_shadersUsed)*sizeof(IFXShader*) );
		}
		else
			result = IFX_E_OUT_OF_MEMORY;

		return result;
	};

private:
	IFXShader** m_ppShaders;
	U32         m_shadersUsed;
	U32         m_shadersAllocated;

	static const U32 INITIAL_SIZE;
	static const U32 GROW_FACTOR;
};

const U32 IFXShaderPointerList::INITIAL_SIZE = 32;
const U32 IFXShaderPointerList::GROW_FACTOR = 2;

static IFXShaderPointerList s_shaderPointerList;

CIFXView::CIFXView()
{
	m_uRefCount = 0;
	m_uQualityFactor = 0;
	m_viewResourceID = 0;

	U32 i = 0;
	for (i=0; i < IFX_VIEW_NUM_LAYERS; i++)
	{
		m_pLayer[i] = NULL;
	}
	ms_LayerTexUnit.SetTexRepeatU(FALSE);
	ms_LayerTexUnit.SetTexRepeatV(FALSE);

	m_bViewOrScaleChanged = TRUE;
	m_rcViewport.Set(0,0,1,1);
	m_rcRenderViewport.Set(0,0,1,1);
	m_rcScreenSize.Set(0,0,0,0);
	m_fScaleX = 1;
	m_fScaleY = 1;

	m_layer = 0;

	m_LayerView.SetProjectionMode(IFX_ORTHOGRAPHIC);
	m_LayerView.SetNearClip(1);
	m_LayerView.SetFarClip(100);

	SetProjectionMode(IFX_PERSPECTIVE3);
	SetProjection(30.0);

	SetOrthoHeight(200);

	SetNearClip(1);
	SetFarClip(sqrtf(FLT_MAX));

	m_pCullingSubsystem = NULL;

	if (ms_spLayerMesh.IsValid())
	{
		ms_spLayerMesh.IncRef();
	}
	else
	{
		ms_spLayerMesh.Create(CID_IFXMesh, IID_IFXMesh);

		IFXVertexAttributes vaAttribs;
		vaAttribs.m_uData.m_bHasNormals = FALSE;
		vaAttribs.m_uData.m_bHasDiffuseColors = TRUE;
		vaAttribs.m_uData.m_uNumTexCoordLayers = 1;

		ms_spLayerMesh->Allocate(vaAttribs, 4, 2);

		IFXVertexIter vIter;
		ms_spLayerMesh->GetVertexIter(vIter);

		// Vertex 0 - lower left
		vIter.GetPosition()->Set(0, 0, -50);
		vIter.GetTexCoord()->Set(0, 0);
		vIter.Next();

		// Vertex 1 - lower right
		vIter.GetPosition()->Set(1, 0, -50);
		vIter.GetTexCoord()->Set(1, 0);
		vIter.Next();

		// Vertex 2 - upper left
		vIter.GetPosition()->Set(0, 1, -50);
		vIter.GetTexCoord()->Set(0, 1);
		vIter.Next();

		// Vertex 3 - upper right
		vIter.GetPosition()->Set(1, 1, -50);
		vIter.GetTexCoord()->Set(1, 1);

		IFXU16FaceIter fIter;
		ms_spLayerMesh->GetU16FaceIter(fIter);

		// Face 0
		fIter.Next()->Set(0,1,2);

		// Face 1
		fIter.Get()->Set(2,1,3);
	}

	m_attributes = IFX_PERSPECTIVE3 | IFX_SCREENPIXELS;
}


CIFXView::~CIFXView()
{
	// Overlay/backgroud deallocation
	U32 i;
	for( i = 0; i < IFX_VIEW_NUM_LAYERS; i++)
	{
		IFXDELETE(m_pLayer[i]);
	}

	ms_spLayerMesh.DecRef();

	IFXRELEASE( m_pCullingSubsystem );

	s_shaderPointerList.Deallocate();
}


IFXRESULT IFXAPI_CALLTYPE CIFXView_Factory(IFXREFIID riid, void **ppv)
{
	IFXRESULT result;

	if ( ppv )
	{
		// Create the CIFXClassName component.
		CIFXView *pView = new CIFXView;

		if ( pView )
		{
			// Perform a temporary AddRef for our usage of the component.
			pView->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			result = pView->QueryInterface( riid, ppv );

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the QI fails, this will cause the
			// component to be destroyed.
			pView->Release();
		}
		else result = IFX_E_OUT_OF_MEMORY;
	}
	else result = IFX_E_INVALID_POINTER;

	return result;
}


// IFXUnknown
U32 CIFXView::AddRef()
{
	return ++m_uRefCount;
}


U32 CIFXView::Release()
{
	if (m_uRefCount == 1)
	{
		PreDestruct();
		delete this ;
		return 0 ;
	}
	else
		return (--m_uRefCount);
}


IFXRESULT CIFXView::QueryInterface(IFXREFIID interfaceId, void** ppInterface)
{
	IFXRESULT result  = IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXUnknown )
			*ppInterface = ( IFXUnknown* ) this;
		else if ( interfaceId == IID_IFXMarker )
			*ppInterface = ( IFXMarker* ) this;
		else if ( interfaceId == IID_IFXMarkerX )
			*ppInterface = ( IFXMarkerX* ) this;
		else if ( interfaceId == IID_IFXCollection )
			*ppInterface = ( IFXCollection* ) this;
		else if ( interfaceId == IID_IFXNode )
			*ppInterface = ( IFXNode* ) this;
		else if ( interfaceId == IID_IFXView )
			*ppInterface = ( IFXView* ) this;
		else if ( interfaceId == IID_IFXModifier )
			*ppInterface = ( IFXModifier* ) this;
		else if ( interfaceId == IID_IFXSubject )
			*ppInterface = ( IFXSubject* ) this;
		else if ( interfaceId == IID_IFXObserver )
			*ppInterface = ( IFXObserver* ) this;
		else if ( interfaceId == IID_IFXMetaDataX )
			*ppInterface = ( IFXMetaDataX* ) this;
		else
		{
			*ppInterface = NULL;
			result = IFX_E_UNSUPPORTED;
		}

		if ( IFXSUCCESS( result ) )
			AddRef();
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}

// IFXMarker
IFXRESULT CIFXView::SetSceneGraph( IFXSceneGraph* pInSceneGraph )
{
	IFXRESULT result = IFX_OK;

	result = CIFXNode::SetSceneGraph( pInSceneGraph );

	return result;
}


// IFXMarkerX
void CIFXView::GetEncoderX(IFXEncoderX*& rpEncoderX)
{
	CIFXMarker::GetEncoderX(CID_IFXViewNodeEncoder, rpEncoderX);
}


// Node
void CIFXView::Counter(EIFXNodeCounterType type, U32* pOutCount)
{
	if ( pOutCount )
	{
		U32 childCount = m_pChildren.GetNumberElements();
		while (childCount)
		{
			m_pChildren[--childCount]->Counter(type, pOutCount);
		}
		if ((type == IFX_NODE_COUNTER_NODES)
			||(type == IFX_NODE_COUNTER_VIEWS))
			(*pOutCount)++;
	}
	else
		IFXTRACE_GENERIC(
		L"CIFXView::Counter() called with an invalid pointer! Line:%d\n",
		__LINE__);
}

// IFXModifier
const IFXGUID* CIFXView::m_scpOutputDIDs[] =
{
	&DID_IFXTransform,
};


IFXRESULT CIFXView::GetOutputs( IFXGUID**& rpOutOutputs,
							   U32&       rOutNumberOfOutputs,
							   U32*&      rpOutOutputDepAttrs )
{
	rOutNumberOfOutputs = sizeof(m_scpOutputDIDs)/sizeof(IFXGUID*);
	rpOutOutputs = (IFXGUID**)&m_scpOutputDIDs;
	rpOutOutputDepAttrs = NULL;

	return IFX_OK;
}


IFXRESULT CIFXView::GetDependencies(
									IFXGUID*   pInOutputDID,
									IFXGUID**& rppOutInputDependencies,
									U32&       rOutNumberInputDependencies,
									IFXGUID**& rppOutOutputDependencies,
									U32&       rOutNumberOfOutputDependencies,
									U32*&      rpOutOutputDepAttrs )
{
	IFXRESULT result = IFX_OK;

	if (pInOutputDID == &DID_IFXTransform)
	{
		rppOutInputDependencies = NULL;
		rOutNumberInputDependencies = 0;
		rppOutOutputDependencies = NULL;
		rOutNumberOfOutputDependencies = 0;
		rpOutOutputDepAttrs = NULL;
	}
	else
	{
		IFXASSERTBOX( 1, "CIFXView::GetDependencies() called with unknown output!" );
		result = IFX_E_UNDEFINED;
	}

	return result;
}


IFXRESULT CIFXView::GenerateOutput( U32    inOutputDataElementIndex,
								   void*& rpOutData, BOOL& rNeedRelease )
{
	IFXRESULT result = IFX_OK;

	result = CIFXNode::GenerateOutput( inOutputDataElementIndex, rpOutData, rNeedRelease );

	return result;
}


IFXRESULT CIFXView::SetDataPacket(
								  IFXModifierDataPacket* pInInputDataPacket,
								  IFXModifierDataPacket* pInDataPacket )
{
	IFXRESULT result = CIFXNode::SetDataPacket(pInInputDataPacket, pInDataPacket);

	return result;
}

IFXRESULT CIFXView::Notify( IFXModifierMessage eInMessage,
									void* pMessageContext )
{
	return IFX_OK;
}

const IFXVector3& CIFXView::ClipPlane( U32 uInClipPlaneId ) const
{
	return m_frustum.GetPlane(uInClipPlaneId).direction;
}


IFXRESULT CIFXView::GetViewport(IFXF32Rect& rcView)
{
	rcView = m_rcViewport;
	return IFX_OK;
}

IFXRESULT CIFXView::GetOrthoHeight(F32* pOrthoHeight)
{
	IFXRESULT result = IFX_OK;

	if ( pOrthoHeight )
		*pOrthoHeight = m_View.GetOrthoHeight();
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}


IFXRESULT CIFXView::GetProjection(F32* pProjectionDegrees)
{
	IFXRESULT result = IFX_OK;

	if ( pProjectionDegrees )
		*pProjectionDegrees = m_View.GetFOVy();
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}


IFXRESULT CIFXView::GetProjectionMode(IFXenum* pProjectionMode)
{
	IFXRESULT result = IFX_OK;

	if ( pProjectionMode )
		*pProjectionMode = m_View.GetProjectionMode();
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}

IFXRESULT CIFXView::Pick(F32 x, F32 y, U32 ViewInstance, IFXModel** ppOutPickedModel, U32* pInstance)
{
	IFXRESULT result = IFX_OK;

	IFXVector3  pos;
	IFXVector3  dir;

	result = this->GenerateRay(x, y, ViewInstance, pos, dir);

	IFXViewResource *pVR = GetViewResource();
	if( NULL == pVR )
		result = IFX_E_UNDEFINED;

	if (IFXSUCCESS(result) )
	{
		if( pVR->GetRenderPass()->m_nodeSet )
		{
			IFXSimpleList* pPickObjectList = NULL;
			IFXDECLARELOCAL(IFXPalette, pNodePalette);
			result = m_pSceneGraph->GetPalette(IFXSceneGraph::NODE, &pNodePalette);
			IFXDECLARELOCAL(IFXNode, pRootNode);
			if ( IFXSUCCESS(result) )
				result = pNodePalette->GetResourcePtr(pVR->GetRenderPass()->m_nodeIndex, IID_IFXNode, (void**)&pRootNode);
			if ( IFXSUCCESS(result) )
				result = pRootNode->Pick(pVR->GetRenderPass()->m_nodeInstance, pos, dir, &pPickObjectList);

			if ( IFXSUCCESS(result) && pPickObjectList )
			{
				IFXPickObject* pPickObject = NULL;
				IFXUnknown* pUnk = NULL;
				result = pPickObjectList->Get( 0, &pUnk );
				pUnk->QueryInterface( IID_IFXPickObject, (void**)&pPickObject );
				if ( IFXSUCCESS(result) ) {
					(*ppOutPickedModel) = pPickObject->GetModel();
					*pInstance = pPickObject->GetInstance();
				}
				IFXRELEASE(pPickObject);
				IFXRELEASE(pUnk);
			}

			IFXRELEASE(pPickObjectList);
		}
	}

	IFXRELEASE( pVR );

	return result;
}

IFXRESULT CIFXView::PickMultiple(F32 x, F32 y, U32 ViewInstance, IFXSimpleList** ppPickedObjectList)
{
	IFXRESULT result  = IFX_OK;

	IFXVector3  pos;
	IFXVector3  dir;

	result = GenerateRay(x, y, ViewInstance, pos, dir);

	IFXViewResource *pVR = GetViewResource();
	if( NULL == pVR )
		result = IFX_E_UNDEFINED;

	if (IFXSUCCESS(result) )
	{
		if( pVR->GetRenderPass()->m_nodeSet )
		{
			IFXDECLARELOCAL(IFXPalette, pNodePalette);
			result = m_pSceneGraph->GetPalette(IFXSceneGraph::NODE, &pNodePalette);
			IFXDECLARELOCAL(IFXNode, pRootNode);
			if ( IFXSUCCESS(result) )
				result = pNodePalette->GetResourcePtr(pVR->GetRenderPass()->m_nodeIndex, IID_IFXNode, (void**)&pRootNode);
			if ( IFXSUCCESS(result) )
				result = pRootNode->Pick(pVR->GetRenderPass()->m_nodeInstance, pos, dir, ppPickedObjectList);
		}
	}

	IFXRELEASE( pVR );

	return result;
}


IFXRESULT CIFXView::WindowToFilm(F32 x, F32 y, IFXVector3* pFilmPt)
{
	UpdateViewport();

	// we use the first width and height since we're recalculating the distance
	// to the projection plane using the first height.  we do this so that the
	// film plane point is consistent regardless of the difference between the
	// first size and the current size.
	//
	// we subtract 1 from the width and height so that the screen space x and y
	// values are for the center of the pixel.

	x =  x - (m_rcRenderViewport.m_Width  - 1)*0.5f;
	y = -y + (m_rcRenderViewport.m_Height - 1)*0.5f;

	// We scale the X coordinate by the ratio of aspect ratios. This
	// ratio maps an X coordinate in sprite space to an X coordinate
	// in the actual viewing rect.
	// Recall that m_rcOriginalView.m_Height is the aspect ratio of the sprite
	// rect and m_aspect is the aspect ratio of the visible window.

	switch(m_View.GetProjectionMode())
	{
	case IFX_PERSPECTIVE3:
		pFilmPt->X() = x;
		pFilmPt->Y() = y;
		pFilmPt->Z() = -m_firstDistanceToProjectionPlane;
		break;

	case IFX_ORTHOGRAPHIC:
		{
			// while the distance to the projection plane doesn't mean much
			// for orthographic cameras, it's a nice point to return.
			F32 worldUnitsPerPixel = FLT_MAX;

			if ( m_rcRenderViewport.m_Height != 0 )
			{
				worldUnitsPerPixel =
				m_View.GetOrthoHeight() / m_rcRenderViewport.m_Height;
			}

			pFilmPt->X() = x * worldUnitsPerPixel;
			pFilmPt->Y() = y * worldUnitsPerPixel;
			pFilmPt->Z() = -m_firstDistanceToProjectionPlane;
		}
		break;

	default:
		IFXASSERT( FALSE );
		break;
	}

	return IFX_OK;
}


IFXRESULT CIFXView::FrustumToWindow(const IFXVector3* pVector, I32* const pX, I32* const pY) const
{
	IFXASSERT(pVector != NULL);
	IFXASSERT(pX != NULL);
	IFXASSERT(pY != NULL);

	IFXRESULT result  = IFX_OK;

	// first make sure that we are between the clipping planes.  this holds
	// true for both orthographic and perspective projections.

	const F32 ZC = pVector->ZC();

	if ((ZC <= -m_nearClip) &&
		(ZC >= -m_farClip))
	{
		// we work in floating point representations for accuracy.  we'll
		// eventually convert to integer representation.

		F32 x = -1.0f;
		F32 y = -1.0f;

		// find window x and y depending on the projection mode.
		switch(m_View.GetProjectionMode())
		{
		case IFX_PERSPECTIVE3:
			{
				const F32 perspectiveScale =
					m_firstDistanceToProjectionPlane / -pVector->ZC();

				x = pVector->XC() * perspectiveScale;
				y = pVector->YC() * perspectiveScale;
			}
			break;

		case IFX_ORTHOGRAPHIC:
			{
				const F32 pixelsPerWorldUnits =
					m_rcRenderViewport.m_Height / m_View.GetOrthoHeight();

				x = pVector->XC() * pixelsPerWorldUnits;
				y = pVector->YC() * pixelsPerWorldUnits;
			}
			break;

		default:
			IFXASSERT( FALSE );
			break;
		}

		// We've scaled the local space coordinates so that they
		// are close to being screen coordinates. We translate these
		// coordinates so that the origin is the upper left corner.
		// We translate by half the width/height (instead of one less
		// than the width/height) because we are computing the
		// window coordinates by taking the floor() of the translated
		// coordinates.

		// We need to convert the frustum x coordinate to the
		// aspect ratio of the sprite (this is the inverse of the
		// scaling that is applied in WindowToFilm(...).

		x += m_rcRenderViewport.m_Width * 0.5f;

		y = -y;
		y += m_rcRenderViewport.m_Height * 0.5f;

		// convert to integer representation.  we use the floor since x and y
		// can be negative.
		I32 ix  = I32(floor(x));
		I32 iy  = I32(floor(y));

		if ((0 <= ix) && (ix < m_rcRenderViewport.m_Width) &&
			(0 <= iy) && (iy < m_rcRenderViewport.m_Height))
		{
			*pX = ix;
			*pY = iy;
		}
		else
		{
			// we're off the window!  return an error.
			result = IFX_E_INVALID_POINT;
		}
	}
	else
	{
		// the point is beyond the clipping planes.
		result = IFX_E_INVALID_POINT;
	}

	return result;
}


IFXRESULT CIFXView::SetFarClip(F32 distance)
{
	IFXRESULT result = IFX_OK;

	if ( distance>0 )
		m_farClip = distance;
	else
		result = IFX_E_INVALID_RANGE;

	return result;
}


IFXRESULT CIFXView::SetNearClip(F32 distance)
{
	IFXRESULT result = IFX_OK;

	if ( distance>0 )
		m_nearClip = distance;
	else
		result = IFX_E_INVALID_RANGE;

	return result;
}


// IFXObserver
IFXRESULT CIFXView::Update( IFXSubject* pInSubject, U32 uInChangeBits,IFXREFIID rIType)
{
	CIFXNode::Update( pInSubject, uInChangeBits, rIType );

	return IFX_OK;
}


IFXRESULT CIFXView::SetViewport(const IFXF32Rect& rcView)
{
	IFXRESULT result = IFX_OK;

	if (rcView.m_Height > 0 && rcView.m_Width > 0)
	{
		if (!(m_rcViewport == rcView))
		{
			m_rcViewport = rcView;
			m_bViewOrScaleChanged = TRUE;
		}
	}
	else
	{
		result = IFX_E_INVALID_RANGE;
	}

	return result;
}

/*
*  Sets the scale size.
*/

IFXRESULT CIFXView::SetScale(F32 fScaleX, F32 fScaleY)
{
	IFXRESULT rc = IFX_OK;

	if (fScaleX)
	{
		if (m_fScaleX != fScaleX)
		{
			m_fScaleX = fScaleX;
			m_bViewOrScaleChanged = TRUE;
		}
	}
	else
	{
		rc = IFX_E_INVALID_RANGE;
	}

	if (fScaleY)
	{
		if (m_fScaleY != fScaleY)
		{
			m_fScaleY = fScaleY;
			m_bViewOrScaleChanged = TRUE;
		}
	}
	else
	{
		rc = IFX_E_INVALID_RANGE;
	}

	return rc;
}

IFXRESULT CIFXView::GetScale(F32& fScaleX, F32& fScaleY) const
{
	fScaleX = m_fScaleX;
	fScaleY = m_fScaleY;

	return IFX_OK;
}


IFXRESULT CIFXView::SetOrthoHeight(F32 orthoHeight)
{
	return m_View.SetOrthoHeight(orthoHeight);
}


IFXRESULT CIFXView::SetProjection(F32 degrees)
{
	IFXRESULT result = IFX_OK;

	if ((degrees >= 0) && (degrees <= 180))
	{
		m_View.SetFOVy(degrees);
	}
	else
		result = IFX_E_INVALID_RANGE;

	return result;
}

IFXRESULT CIFXView::SetProjectionMode(IFXenum projectionMode)
{
	return m_View.SetProjectionMode(projectionMode);
}

IFXRESULT CIFXView::UpdateViewport()
{
	IFXRESULT rc = IFX_OK;
	IFXRenderWindow rw;

	if ( !pCurrentRenderContext )
		return IFX_E_INVALID_POINTER;

	pCurrentRenderContext->GetWindow(rw);
	IFXRect rcScreenSize = rw.GetWindowSize();

	// if the view has changed, or view uses screen independent units for viewport
	//	update the viewport.
	if( m_bViewOrScaleChanged || ((m_attributes & IFX_PERCENTDIMEN) && !(m_rcScreenSize == rcScreenSize)) )
	{
		if (m_attributes & IFX_PERCENTDIMEN)
		{
			m_rcRenderViewport.m_X = (I32)(rcScreenSize.m_Width * m_rcViewport.m_X + 0.5001f);
			m_rcRenderViewport.m_Width = (I32)(rcScreenSize.m_Width * m_rcViewport.m_Width + 0.5001f);
			m_rcRenderViewport.m_Y = (I32)(rcScreenSize.m_Height * m_rcViewport.m_Y + 0.5001f);
			m_rcRenderViewport.m_Height = (I32)(rcScreenSize.m_Height * m_rcViewport.m_Height + 0.5001f);
			m_rcScreenSize = rcScreenSize;
		}
		else
		{
			m_rcRenderViewport.m_X = (I32)(m_rcViewport.m_X * m_fScaleX + 0.5001f);
			m_rcRenderViewport.m_Width = (I32)(m_rcViewport.m_Width * m_fScaleX + 0.5001f);
			m_rcRenderViewport.m_Y = (I32)(m_rcViewport.m_Y * m_fScaleY + 0.5001f);
			m_rcRenderViewport.m_Height = (I32)(m_rcViewport.m_Height * m_fScaleY + 0.5001f);
		}
		m_bViewOrScaleChanged = FALSE;
		m_View.SetViewport(m_rcRenderViewport);
		m_LayerView.SetViewport(m_rcRenderViewport);
		m_LayerView.SetOrthoHeight(F32(m_rcRenderViewport.m_Height));

		IFXViewResource *pVR = GetViewResource();
		if( NULL == pVR )
			rc = IFX_E_UNDEFINED;
		else
		{
			IFXRect rect;
			rect.m_X		= (I32)(m_rcRenderViewport.m_X);
			rect.m_Width	= (I32)(m_rcRenderViewport.m_Width);
			rect.m_Y		= (I32)(m_rcRenderViewport.m_Y);
			rect.m_Height	= (I32)(m_rcRenderViewport.m_Height);
			RecalcAllLayerMatrices(&rect, m_fScaleX, m_fScaleY);
		}

		IFXRELEASE( pVR );

		m_firstDistanceToProjectionPlane = GetDistanceToProjectionPlane();
	}
	return rc;
}


IFXRESULT CIFXView::RenderViewLayer(U32 uLayer, IFXRenderContext& rRenderContext, IFXRect *pViewport, F32 fScaleX, F32 fScaleY)
{
	IFXRESULT rc = IFX_OK;

	// Disable fog
	rRenderContext.Disable(IFX_FOG);

	// Disable Depth Test (painter's algorithm)
	rRenderContext.Disable(IFX_DEPTH_TEST);

	// Enable TexUnit 0
	rRenderContext.Enable(IFX_TEXUNIT0);

	// Disable all but first texture units
	U32 i;
	for( i = 1; i < IFX_MAX_TEXUNITS; i++)
	{
		rRenderContext.Disable(IFX_TEXUNIT0 + i);
	}

	// Disable lighting
	rRenderContext.Disable(IFX_LIGHTING);

	// Enable use of vertex colors
	ms_LayerMaterial.SetUseVertexColors(TRUE);
	rRenderContext.SetMaterial(ms_LayerMaterial);

	// Enable correct blending
	ms_LayerBlend.SetBlendFunc(IFX_FB_ALPHA_BLEND);
	ms_LayerBlend.SetTestFunc(IFX_NOT_EQUAL);
	ms_LayerBlend.SetReference(0);

	rRenderContext.Enable(IFX_FB_BLEND);
	rRenderContext.Enable(IFX_FB_ALPHA_TEST);

	rRenderContext.SetBlend(ms_LayerBlend);

	// Set View Matrix to Identity
	IFXMatrix4x4 mViewMatrix;
	mViewMatrix.Reset();
	rRenderContext.SetViewMatrix(mViewMatrix);

	rRenderContext.SetCullMode(IFX_CULL_CW);

	// Render the layers in order
	CIFXViewLayer* pLayer = m_pLayer[uLayer];
	U32 uLayerIndex = 0;
	while(pLayer && IFXSUCCESS(rc))
	{
		// Get Texture object pointer
		IFXTextureObject* pTexture = 0;
		if (IFXSUCCESS(rc))
		{
			IFXDECLARELOCAL(IFXPalette,pTexturePalette);
			rc = m_pSceneGraph->GetPalette( IFXSceneGraph::TEXTURE, &pTexturePalette);

			IFXDECLARELOCAL(IFXUnknown,pUnk);
			if ( IFXSUCCESS(rc) && pTexturePalette)
				rc = pTexturePalette->GetResourcePtr (pLayer->m_uTextureId, &pUnk);

			IFXRELEASE (pTexturePalette);

			if (IFXSUCCESS(rc) && pUnk)
			{
				rc = pUnk->QueryInterface(IID_IFXTextureObject, (void**)&pTexture);
			}

			IFXRELEASE(pUnk);
		}

		if(IFXSUCCESS(rc) && pLayer->m_bTextureDirty)
		{
			pLayer->m_bTextureDirty = FALSE;
			U32 uVal = 0;
			IFXRUN(rc, pTexture->GetWidth(&uVal));
			if(IFXSUCCESS(rc)) pLayer->m_fTexSizeScaleX = (F32)uVal;
			IFXRUN(rc, pTexture->GetHeight(&uVal));
			if(IFXSUCCESS(rc)) pLayer->m_fTexSizeScaleY = (F32)uVal;

			RecalcLayerMatrix(uLayer, uLayerIndex, pViewport, fScaleX, fScaleY);
		}

		if (IFXSUCCESS(rc))
		{
			ms_LayerTexUnit.SetTextureId(pTexture->GetId());
			rc = rRenderContext.SetTexture(*pTexture);
		}

		if (IFXSUCCESS(rc))
		{
			rc = rRenderContext.SetTextureUnit(IFX_TEXUNIT0, ms_LayerTexUnit);
		}

		if (IFXSUCCESS(rc))
		{
			rc = UpdateLayerMesh(pLayer);
		}

		U32 uNumPasses = 0;
		if (IFXSUCCESS(rc))
		{

			uNumPasses = rRenderContext.CalculateNumPasses();

			U32 uPass;
			for( uPass = 0; (uPass < uNumPasses) && IFXSUCCESS(rc); uPass++)
			{
				rc = rRenderContext.ConfigureRenderPass(uPass);

				IFXRUN(rc, rRenderContext.SetWorldMatrix(pLayer->m_Transform));

				if (IFXSUCCESS(rc))
				{
					rc = rRenderContext.DrawMesh(*ms_spLayerMesh.GetPointerNR());
				}
			}
		}

		// Even if we non-fatally fail, try the next layer object
		if (rc != IFX_E_SUBSYSTEM_FAILURE)
			rc = IFX_OK;

		IFXRELEASE(pTexture);

		pLayer = pLayer->m_pNext;
		uLayerIndex++;
	}

	return rc;
}



IFXRESULT CIFXView::UpdateLayerMesh(CIFXViewLayer* pLayer)
{
	IFXRESULT rc = IFX_OK;
	U32 i;
	IFXVertexIter colorIter;
	IFXVector4 vColor(1,1,1,pLayer->m_fBlend);

	ms_spLayerMesh->GetVertexIter(colorIter);
	for( i = 0; i < 4; i++)
	{
		*colorIter.GetColor(0) = vColor.GetBGRA();
		colorIter.Next();
	}

	ms_spLayerMesh->UpdateVersionWord(IFX_MESH_DIFFUSE_COLOR);
	return rc;
}

IFXRESULT CIFXView::ComputeFrustumPlanes(IFXMatrix4x4& mUnscaledWorld)
{
	IFXRay plane;
	IFXRESULT result = IFX_OK;

	// Front
	mUnscaledWorld.TransformVector(IFXVector3(0,0,-m_nearClip), plane.position);
	mUnscaledWorld.RotateVector(IFXVector3(0,0,1), plane.direction);
	plane.direction.Normalize();
	m_frustum.SetPlane(IFX_FRUSTUM_FRONT,plane);

	// Back
	mUnscaledWorld.TransformVector(IFXVector3(0,0,-m_farClip), plane.position);
	mUnscaledWorld.RotateVector(IFXVector3(0,0,-1), plane.direction);
	plane.direction.Normalize();
	m_frustum.SetPlane(IFX_FRUSTUM_BACK,plane);

	IFXVector3 v;
	F32 halfWidth = 0;
	F32 halfHeight = 0;
	IFXenum projectionMode = m_View.GetProjectionMode();

	if(projectionMode == IFX_PERSPECTIVE3)
	{
		halfWidth  = m_rcRenderViewport.m_Width * 0.5f;
		halfHeight = m_rcRenderViewport.m_Height * 0.5f;

		v.Set(0,0,0);
		mUnscaledWorld.TransformVector(v, plane.position);

		const F32 distToProjectionPlane = -GetDistanceToProjectionPlane();

		IFXVector3 v1, v2;

		// Bottom
		v1.Set(-halfWidth, -halfHeight, distToProjectionPlane);
		v2.Set(halfWidth, -halfHeight, distToProjectionPlane);

		v.CrossProduct(v1, v2);
		v.Normalize();
		mUnscaledWorld.RotateVector(v, plane.direction);

		m_frustum.SetPlane(IFX_FRUSTUM_BOTTOM,plane);

		// Top
		v1.Y() = halfHeight;
		v2.Y() = halfHeight;

		v.CrossProduct(v2, v1);
		v.Normalize();
		mUnscaledWorld.RotateVector(v, plane.direction);

		m_frustum.SetPlane(IFX_FRUSTUM_TOP,plane);

		// Right
		v1.Set(halfWidth, -halfHeight, distToProjectionPlane);

		v.CrossProduct(v1, v2);
		v.Normalize();
		mUnscaledWorld.RotateVector(v, plane.direction);

		m_frustum.SetPlane(IFX_FRUSTUM_RIGHT,plane);

		// Left
		v1.X() = -halfWidth;
		v2.X() = -halfWidth;

		v.CrossProduct(v2, v1);
		v.Normalize();
		mUnscaledWorld.RotateVector(v, plane.direction);

		m_frustum.SetPlane(IFX_FRUSTUM_LEFT,plane);

		m_frustum.SetFOV( m_View.GetFOVy() );
	}
	else if(projectionMode == IFX_ORTHOGRAPHIC)
	{
		halfHeight = m_View.GetOrthoHeight() * 0.5f;
		halfWidth  = halfHeight * (m_rcRenderViewport.m_Width) / (m_rcRenderViewport.m_Height);

		v.Set(-halfWidth, halfHeight, 0);
		mUnscaledWorld.TransformVector(v, plane.position);

		// Top
		v.Set(0, 1, 0);
		mUnscaledWorld.RotateVector(v, plane.direction);
		plane.direction.Normalize();
		m_frustum.SetPlane(IFX_FRUSTUM_TOP,plane);

		// Left
		v.Set(-1, 0, 0);
		mUnscaledWorld.RotateVector(v, plane.direction);
		plane.direction.Normalize();
		m_frustum.SetPlane(IFX_FRUSTUM_LEFT,plane);

		v.Set(halfWidth, -halfHeight, 0);
		mUnscaledWorld.TransformVector(v, plane.position);

		// Bottom
		v.Set(0, -1, 0);
		mUnscaledWorld.RotateVector(v, plane.direction);
		plane.direction.Normalize();
		m_frustum.SetPlane(IFX_FRUSTUM_BOTTOM,plane);

		// Right
		v.Set(1, 0, 0);
		mUnscaledWorld.RotateVector(v, plane.direction);
		plane.direction.Normalize();
		m_frustum.SetPlane(IFX_FRUSTUM_RIGHT,plane);

		m_frustum.SetFOV( m_View.GetOrthoHeight() );
	}
	else
	{
		result = IFX_E_UNSUPPORTED;
	}

	if( IFXSUCCESS(result) )
	{
		m_frustum.SetWorldTransform( mUnscaledWorld );
		m_frustum.SetAspect( halfWidth / halfHeight );
	}

	/// @todo	Restore support for scaled transforms w.r.t. near/far clips:

	/*
	IFXMatrix4x4 worldMatrix;
	GetWorldMatrix(&worldMatrix);

	worldMatrix.c[3].x = 0;
	worldMatrix.c[3].y = 0;
	worldMatrix.c[3].z = 0;

	IFXVector4 d = worldMatrix * IFXVector4(0,0,-m_farClip);
	m_scaledFarClip = sqrtf( d.dot(d) );

	d = worldMatrix * IFXVector4(0,0,-m_nearClip);
	m_scaledNearClip = sqrtf( d.dot(d) );
	// todo: Eliminate one of the sqrts above using the following technique:
	// F32 ratio = (m_scaledFarClip/m_farClip);
	// F32 scaledNearClip = m_nearClip * ratio;
	// Before doing this the F32 overflow problem must be mitigated by clampping
	// to the appropriate range or using doubles.

	*/

	return result;
}


IFXViewResource *CIFXView::GetViewResource( void )
{
	IFXRESULT result = IFX_E_UNDEFINED;

	// clear it out so that it's NULL if the resource doesn't exist:
	IFXViewResource *pVR = NULL;

	IFXPalette* pViewResourcePalette = NULL ;

	if( m_pSceneGraph )
		result = m_pSceneGraph->GetPalette( IFXSceneGraph::VIEW, &pViewResourcePalette);
	else
		result = IFX_E_NOT_INITIALIZED;

	IFXUnknown* pUnknown = NULL ;

	if ((IFXSUCCESS(result)) && (NULL != pViewResourcePalette))
		result = pViewResourcePalette->GetResourcePtr( m_viewResourceID, &pUnknown);
	else
		IFXASSERT(0);

	if ((IFXSUCCESS(result)) && (NULL != pUnknown))
		result = pUnknown->QueryInterface( IID_IFXViewResource, (void**)&pVR );

	if( IFXFAILURE( result ) )
		pVR = NULL;

	IFXASSERT(pVR);

	IFXRELEASE( pUnknown );
	IFXRELEASE( pViewResourcePalette );

	return pVR;
}


IFXRESULT CIFXView::GetViewResourceID(U32* pValue)
{
	IFXRESULT result = IFX_E_UNDEFINED;

	if ( pValue )
	{
		*pValue = m_viewResourceID;
		result = IFX_OK;
	}
	else result = IFX_E_INVALID_POINTER;

	return result;
}

IFXRESULT CIFXView::SetViewResourceID(U32 value)
{
	m_viewResourceID = value;

	return IFX_OK;
}

IFXRESULT CIFXView::RenderView(IFXRenderContext* pRenderContext, U32 ViewInstance)
{
	F32 optimizedNearClip = FLT_MAX;
	F32 optimizedFarClip = -FLT_MAX;
	U32 passNum = 0;
	IFXRESULT result = IFX_OK;
	IFXPalette* pShaderPalette = NULL;
	IFXDECLARELOCAL( IFXSpatialSetQuery, pCullingSubsystem );

	if ( !m_pSceneGraph  )
		return IFX_E_NOT_INITIALIZED;

	if ( !pRenderContext )
		return IFX_E_INVALID_POINTER;

	IFXViewResource *pVR = GetViewResource();
	if( NULL == pVR )
		result = IFX_E_UNDEFINED;

	if( IFXSUCCESS( result ) )
		result = GetCullingSubsystem( pCullingSubsystem );

	if( IFXSUCCESS( result ) )
		result = s_shaderPointerList.Allocate();

	if( IFXSUCCESS( result ) )
	{
		pCurrentRenderContext = pRenderContext;
		pCurrentRenderContext->AddRef();
		UpdateViewport();

		IFXModifierChain* pModifierChain = NULL;
		IFXMatrix4x4* pWorldTransform = NULL;
		IFXVector3 scale;
		IFXMatrix4x4 mUnscaledWorld;
		IFXMatrix4x4 mUnscaledWorldInverse;

		IFXArray<IFXMatrix4x4> *pAWorldTransform;

		// Cull Phase
		for(passNum = 0; passNum < pVR->GetNumRenderPasses() && IFXSUCCESS(result); passNum++)
		{
			if( (pVR->GetRenderPass(passNum) == NULL) || (!pVR->GetRenderPass(passNum)->m_nodeSet) )
				continue;

			// Attach root node for this pass to the culling system
			IFXREFCID rCollectionCID = pCullingSubsystem->GetCollectionCID();
			IFXCollection* pCollection = NULL;
			IFXDECLARELOCAL(IFXPalette, pNodePalette);
			result = m_pSceneGraph->GetPalette(IFXSceneGraph::NODE, &pNodePalette);
			IFXDECLARELOCAL(IFXNode, pRootNode);
			if ( IFXSUCCESS(result) )
				result = pNodePalette->GetResourcePtr(pVR->GetRenderPass(passNum)->m_nodeIndex, IID_IFXNode, (void**)&pRootNode);
			if ( IFXSUCCESS(result) )
				result = pRootNode->GetCollection( rCollectionCID, &pCollection );
			if ( IFXSUCCESS(result) )
				pCullingSubsystem->SetCollection( pCollection );
			IFXRELEASE(pCollection);

			if (IFXSUCCESS(result))
				result = GetWorldMatrices(&pAWorldTransform);
			if (IFXSUCCESS(result) && pAWorldTransform->GetNumberElements() < 1)
				result = IFX_E_NOT_INITIALIZED;
			if (IFXSUCCESS(result))
				pWorldTransform = &pAWorldTransform->GetElement(ViewInstance);

			if (NULL == pWorldTransform)
				result = IFX_E_NOT_INITIALIZED;

			F32 nearClip = FLT_MAX;
			F32 farClip = -FLT_MAX;
			if ( IFXSUCCESS(result) )
			{
				// Determine scale
				pWorldTransform->CalcAxisScale(scale);

				// Extract scale
				mUnscaledWorld = *pWorldTransform;
				IFXVector3 oneOverScale = scale;
				mUnscaledWorld.Scale3x4(oneOverScale.Reciprocate());

				// Invert
				mUnscaledWorldInverse.Invert3x4(mUnscaledWorld);
				pVR->GetRenderPass(passNum)->m_UnscaledWorldMatrixInverse = mUnscaledWorldInverse;

				ComputeFrustumPlanes(mUnscaledWorld);

				// Model culling, light association, and ordering, near/far clips
				SPATIALINSTANCE_LIST * pOpaque = pVR->GetRenderPass(passNum)->m_pOpaque;
				SPATIALINSTANCE_LIST * pTranslucent = pVR->GetRenderPass(passNum)->m_pTranslucent;
				pOpaque->clear();
				pTranslucent->clear();

				result = pCullingSubsystem->QueryForSpatialSet(
					&m_frustum,
					pOpaque,
					pTranslucent,
					nearClip,
					farClip );
				//IFXTRACE_GENERIC( "Number of visible opaque models:%x\n", uNumberOfOpaqueAssociations );
			}
			//if ( !g_CountDown ) GetTime( g_MidTime1 );

			if ( IFXSUCCESS(result) ) {
				// Near/Far clip distances
				farClip += m_nearClip; // Far clip plane is based relative to the frustum plane 0 which is set to -m_nearClip
				optimizedNearClip = optimizedNearClip < nearClip ? optimizedNearClip:nearClip;
				optimizedFarClip = optimizedFarClip > farClip ? optimizedFarClip:farClip;
			}
		}

		// Optimize zbuffer
		if ( optimizedFarClip < m_nearClip )
		{ // This is usually only hit if there are no models in the scene
			optimizedNearClip = 1;
			optimizedFarClip = 100000;
		}

		optimizedNearClip = optimizedNearClip < m_nearClip ? m_nearClip : optimizedNearClip;
		optimizedFarClip = optimizedFarClip > m_farClip ? m_farClip : optimizedFarClip;

		if ( IFXSUCCESS(result) )
			result = m_View.SetNearClip(optimizedNearClip);
		if ( IFXSUCCESS(result) )
			result = m_View.SetFarClip(optimizedFarClip);

		if ( IFXSUCCESS(result) )
			// Render Pass
			result = m_pSceneGraph->GetPalette( IFXSceneGraph::SHADER, &pShaderPalette );

		for( passNum = 0; passNum < pVR->GetNumRenderPasses() && IFXSUCCESS(result); passNum++)
		{
			SPATIALINSTANCE_LIST *pOpaqueAssociations = pVR->GetRenderPass(passNum)->m_pOpaque;
			U32 uNumberOfOpaqueAssociations = pVR->GetRenderPass(passNum)->m_pOpaque->size();
			/**@todo: never is used. let's examine it			IFXSpatial** pTranslucentAssociations = pVR->GetRenderPass(passNum)->m_pTranslucent->m_data;
			U32 uNumberOfTranslucentAssociations = pVR->GetRenderPass(passNum)->m_pTranslucent->size();*/

			if( (pVR->GetRenderPass(passNum) == NULL) || (!pVR->GetRenderPass(passNum)->m_nodeSet) )
				continue;

			// Backdrops
			if (GetLayer(IFX_VIEW_BACKDROP) && passNum == 0 )
			{
				result = pRenderContext->SetView( m_LayerView );

				// Clear
				if ( IFXSUCCESS(result) )
					result = pRenderContext->Clear(pVR->GetRenderPass(passNum)->m_Clear);

				// Backdrops
				if ( IFXSUCCESS(result) )
				{
					IFXRect rect;
					rect.m_X		= m_rcRenderViewport.m_X;
					rect.m_Width	= m_rcRenderViewport.m_Width;
					rect.m_Y		= m_rcRenderViewport.m_Y;
					rect.m_Height	= m_rcRenderViewport.m_Height;

					result = RenderViewLayer(IFX_VIEW_BACKDROP, *pRenderContext, &rect, m_fScaleX, m_fScaleY);
				}
			}


			if(passNum == 0 && IFXSUCCESS(result))
			{
				result = pRenderContext->SetView(m_View);
			}

			if(IFXSUCCESS(result))
			{
				result = pRenderContext->SetViewMatrix(
					pVR->GetRenderPass(passNum)->m_UnscaledWorldMatrixInverse);
			}

			// Fog
			if ( pVR->GetRenderPass(passNum)->m_bFogEnabled )
			{
				if ( IFXSUCCESS(result) )
					result = pRenderContext->Enable( IFX_FOG );
				if ( IFXSUCCESS(result) )
					result = pRenderContext->SetFog( pVR->GetRenderPass(passNum)->m_Fog );
			}
			else if ( IFXSUCCESS(result) )
				result = pRenderContext->Disable( IFX_FOG );


			// Depth
			if (IFXSUCCESS(result) && pVR->GetRenderPass(passNum)->m_bDepthTest)
			{
				result = pRenderContext->Enable(IFX_DEPTH_TEST);

				if (IFXSUCCESS(result) && pVR->GetRenderPass(passNum)->m_bDepthWrite)
				{
					result = pRenderContext->Enable(IFX_DEPTH_WRITE);
				}
				else
				{
					result = pRenderContext->Disable(IFX_DEPTH_WRITE);
				}

				if ( IFXSUCCESS(result) )
				{
					result = pRenderContext->SetDepthMode(pVR->GetRenderPass(passNum)->m_eDepthFunc);
				}
			}
			else if ( IFXSUCCESS(result) )
			{
				result = pRenderContext->Disable(IFX_DEPTH_TEST);
				if ( IFXSUCCESS(result) )
				{
					result = pRenderContext->Disable(IFX_DEPTH_WRITE);
				}
			}

			// Stencil
			if (pVR->GetRenderPass(passNum)->m_bStencilEnabled)
			{
				if ( IFXSUCCESS(result) )
				{
					result = pRenderContext->Enable(IFX_STENCIL);
				}

				if ( IFXSUCCESS(result) )
				{
					result = pRenderContext->SetStencil(pVR->GetRenderPass(passNum)->m_Stencil);
				}
			}
			else if ( IFXSUCCESS(result) )
			{
				result = pRenderContext->Disable(IFX_STENCIL);
			}

			// Render
			if ( GetLayer(IFX_VIEW_BACKDROP) == 0 || passNum)
				result = pRenderContext->Clear(pVR->GetRenderPass(passNum)->m_Clear);

			IFXDECLARELOCAL(IFXUnknown,pUnk);
			IFXDECLARELOCAL(IFXRenderable,pRenderable);
			IFXDECLARELOCAL(IFXModifierDataPacket,pDataPacket);
			IFXDECLARELOCAL(IFXModel,pModel);
			IFXDECLARELOCAL(IFXShader,pShader);
			IFXMatrix4x4* pTransform=NULL;
			IFXDECLARELOCAL(IFXLightSet,pLightSet);

			IFXASSERT( pShaderPalette );

			if( IFXSUCCESS(result) )
			{
				// For now all models are treated as opaque to this point.
				U32 uModel;
				for (  uModel=0;
					uModel<uNumberOfOpaqueAssociations && IFXSUCCESS(result);
					uModel++ )
				{
					result = (*pOpaqueAssociations)[uModel].m_pSpatial->QueryInterface( IID_IFXModel, (void**)&pModel );
					IFXASSERT( pModel );

					U32 bFrontAndOrBack = 0, bNoDepth = 0;
					if(IFXSUCCESS(result)) {
						bFrontAndOrBack = pModel->GetVisibility();
						bNoDepth = bFrontAndOrBack > 3;
						if(bNoDepth)
							bFrontAndOrBack -= 3;
					}

					if (IFXSUCCESS(result) && bFrontAndOrBack)
					{
						// ViewData
						result = pModel->SetFrustum(m_frustum);
						if (IFXSUCCESS(result))
							result = pModel->SetViewSize(((U32)m_rcRenderViewport.m_Width<<16)|(U32)m_rcRenderViewport.m_Height);
						U32 modCnt = 0;
						IFXRESULT modSearch = IFX_OK;
						IFXMatrix4x4* pWM = NULL;
						while( modSearch == IFX_OK )
						{
							IFXDECLARELOCAL( IFXModifier, pGlyph2DRequest );
							modSearch = FindModifier( pModel, IID_IFXGlyph2DModifier, &pGlyph2DRequest, modCnt );
							if( IFXSUCCESS( modSearch ) ) //glyph modifier is found
							{
								IFXDECLARELOCAL( IFXGlyph2DModifier, pGlyph2DModifier );
								pGlyph2DRequest->QueryInterface( IID_IFXGlyph2DModifier, (void**) &pGlyph2DModifier );
								GetWorldMatrix( ViewInstance, &pWM );
								pGlyph2DModifier->SetViewTransform( *pWM );
								modCnt++;
							}
						}

						// Process Data Elements
						if (IFXSUCCESS(result))
						{
							if ( IFXSUCCESS(pModel->GetModifierChain( &pModifierChain )) )
							{
								IFXDECLARELOCAL(IFXBoundSphereDataElement,pBound);
								IFXDECLARELOCAL(IFXUnknown,pBoundUnk);

								result = pModifierChain->GetDataPacket( pDataPacket );
								if(IFXSUCCESS(result))
									result = pModel->GetWorldMatrix((*pOpaqueAssociations)[uModel].m_Instance, &pTransform);
								IFXDECLARELOCAL(IFXModifierDataElementIterator,pBoundingSphereIter);
								if( IFXSUCCESS(result) && pTransform)
									result = pDataPacket->GetIterator(IFX_DID_BOUND, &pBoundingSphereIter);
								if( IFXSUCCESS(result) )
								{
									pBoundUnk = (IFXUnknown*)pBoundingSphereIter->First();
									if (pBoundUnk)
										result = pBoundUnk->QueryInterface( IID_IFXBoundSphereDataElement, (void**)&pBound );
								}

								U32 uShaderID;
								/// @todo This is waste full as we are re-culling all the models.
								// Visible & partially visible
								// We need to test only partially visible models for an innner cull.
								while( IFXSUCCESS(result) && pBound )
								{
									// Inner cull
									/// @todo Frustum Planes might be incorrect due to the split of the culling & rendering pass.

									U32 visibilityBits = 0x3F; // Fully visible
									U32 intersectionBits = 0;
									F32 fDistanceToFrontOfBound = 0;

									// store previous radius to prevent an overwrite in Set
									F32 radius = pBound->Bound().Radius();
									IFXVector3 src(pBound->Bound().RawConst()), res;
									pTransform->TransformVector(src.RawConst(), res);
									IFXVector4 Bound(res), dist;

									// Compensate for scale
									IFXVector3 scale;
									pTransform->CalcAxisScale(scale);
									Bound.Radius() = radius*IFXMAX( scale.X(), IFXMAX( scale.Y(), scale.Z() ) );

									IFXVector3 clipPlaneNormal, clipPlanePosition;
									const IFXRay* pInConvexPlanes = m_frustum.GetPlanes();
									const U32     uInNumberOfConvexPlanes = m_frustum.GetPlaneCount();
									U32 plane;
									for (  plane = 0 ; plane < uInNumberOfConvexPlanes; plane++)
									{
										clipPlaneNormal = pInConvexPlanes[plane].GetDirection();
										clipPlanePosition = pInConvexPlanes[plane].GetPosition();
										dist.Subtract(Bound,clipPlanePosition);
										F32 r = dist.DotProduct3(clipPlaneNormal);
										if ( plane == IFX_FRUSTUM_FRONT )
											fDistanceToFrontOfBound = Bound.Radius() - r;
										if (r<Bound.Radius())
										{
											U32 inside = (r<(-Bound.Radius()));
											visibilityBits |= (inside<<plane);
											intersectionBits |= (!inside<<plane);
										}
										else
										{
											visibilityBits =0;
											intersectionBits = 0;
											break;
										}
									}

									if ( visibilityBits | intersectionBits )
									{
										pRenderable = NULL;
										result = pDataPacket->GetDataElement( pBound->RenderableIndex(),
											IID_IFXRenderable,
											(void**)&pRenderable );
										if(IFXSUCCESS(result) && pRenderable && pRenderable->GetEnabled())
										{
											const U32 uRenderableCount = pRenderable->GetNumElements();
											U32 uMesh;
											for (  uMesh=0;
												uMesh<uRenderableCount && IFXSUCCESS(result);
												uMesh++ )
											{
												IFXShaderList* pShaderList = NULL;
												pRenderable->GetElementShaderList( uMesh, &pShaderList );
												const U32 nShaders = pShaderList->GetNumShaders();
												U32 uShader;
												for (  uShader=0;
													uShader<nShaders && IFXSUCCESS(result);
													uShader++ )
												{
													result = pShaderList->GetShader(uShader, &uShaderID);
													pShader = NULL;
													if (IFXSUCCESS(result))
														result = pShaderPalette->GetResourcePtr( uShaderID,
														IID_IFXShader,
														(void**)&pShader );
													if (IFXSUCCESS(result) && pShader )
													{
														IFXShader::IFXShaderOpacity opacity = pShader->Opacity();

														if ( opacity == IFXShader::IFX_SHADER_OPACITY_DEPENDENT )
														{ // Resolve dependency w/ DID_IFXVertexColorOpacity
														}

														if ( opacity )
														{ // Opaque
															// Ensure that the shaders bin is large enough.
															if ( 0 == pShader->ShadedElementList().GetNumberElements() )
																result = s_shaderPointerList.SetShader( pShader );

															if (IFXSUCCESS(result) )
															{
																// Opt. use the unit allocator w/ Clear() instead of
																// the following new w/ DeleteAll()
																IFXShader::IFXShadedElement* pSE = new IFXShader::IFXShadedElement;
																pSE->pDataPacket = pDataPacket;
																pSE->u16Flags = bFrontAndOrBack | (intersectionBits<<2) | (visibilityBits<<8);
																pSE->u16Flags |= (bNoDepth | (!pVR->GetRenderPass(passNum)->m_bDepthWrite)) << 15;
																pSE->u16DataElementIndex = pBound->RenderableIndex();
																pSE->uRenderableElementIndex = uMesh;
																pSE->pfTransform = pTransform;
																pSE->pLightSet = &pModel->GetLightSet();
																pSE->pFrustum = &pModel->GetFrustum();

																// Add the renderable to the shader's bin.
																pShader->ShadedElementList().Append(pSE);
															}
														}
														else
														{ // Translucent
															// Opt. use the unit allocator w/ Clear() instead of
															// the following new w/ DeleteAll()
															IFXTranslucentElement** ppTE = new IFXTranslucentElement*;
															*ppTE = new IFXTranslucentElement;
															(*ppTE)->se.pDataPacket = pDataPacket;
															(*ppTE)->se.u16Flags = bFrontAndOrBack | (intersectionBits<<2) | (visibilityBits<<8);
															(*ppTE)->se.u16Flags |= (bNoDepth | (!pVR->GetRenderPass(passNum)->m_bDepthWrite)) << 15;
															(*ppTE)->se.u16DataElementIndex = pBound->RenderableIndex();
															(*ppTE)->se.uRenderableElementIndex = uMesh;
															(*ppTE)->se.pfTransform = pTransform;
															(*ppTE)->se.pLightSet = &pModel->GetLightSet();
															(*ppTE)->se.pFrustum = &pModel->GetFrustum();
															(*ppTE)->pShader = pShader;
															(*ppTE)->fDepth = fDistanceToFrontOfBound;

															// Add the translucent renderables list.
															m_translucents.Append(ppTE);
														}
													}
													else
													{
														IFXTRACE_GENERIC(__WFILE__ L"(" __THISLINE__ L") : ERROR: NULL shader palette entry found in render pipeline!\n");
													}

													IFXRELEASE( pShader );
												}
												IFXRELEASE( pShaderList );
											}
										}
										IFXRELEASE( pRenderable );
									}
									IFXRELEASE(pBoundUnk);
									pBoundUnk = (IFXUnknown*)pBoundingSphereIter->Next();
									IFXRELEASE(pBound);
									if (pBoundUnk)
										result = pBoundUnk->QueryInterface( IID_IFXBoundSphereDataElement, (void**)&pBound );
								}
							}
							else
								IFXTRACE_GENERIC(L"Failed attempt to render a model without a ModifierChain!\n");
						}
						IFXRELEASE( pModifierChain );
						IFXRELEASE( pDataPacket );
					} // if bFrontOrBack
					IFXRELEASE( pModel );
				} // for uModel

				// Render opaques
				U32 s;
				IFXRESULT rc = IFX_OK;

				for( s = 0; s < s_shaderPointerList.GetNumShaders() && IFXSUCCESS(rc); s++ )
				{
					IFXShader* pS = s_shaderPointerList.GetShaderNR( s );

					IFXASSERT( pS != NULL );

					if( pS != NULL )
						rc = pS->Render( *pRenderContext, passNum );
				}

				for( s = 0; s < s_shaderPointerList.GetNumShaders(); s++ )
				{
					IFXShader* pS = s_shaderPointerList.GetShaderNR( s );

					IFXASSERT( pS != NULL );

					if( pS != NULL )
						pS->ShadedElementList().DeleteAll();
				}

				s_shaderPointerList.Reset();

				// Render translucents
				InsertionSort(&m_translucents, compFunc);

				IFXTranslucentElement** ppTE = NULL;
				static IFXListContext context;
				IFXDECLARELOCAL(IFXShader,pShader);
				m_translucents.ToHead(context);
				while ( (ppTE = m_translucents.PostIncrement(context)) && IFXSUCCESS(rc) )
				{
					IFXADDREF((*ppTE)->pShader);
					IFXRELEASE(pShader);
					pShader = (*ppTE)->pShader;
					pShader->ShadedElementList().Append(&((*ppTE)->se));

					while( (ppTE = m_translucents.PostIncrement(context)) && ((*ppTE)->pShader == pShader) )
						pShader->ShadedElementList().Append(&((*ppTE)->se));

					if (ppTE)
						m_translucents.PreDecrement(context);

					rc = pShader->Render( *pRenderContext, passNum );
					pShader->ShadedElementList().RemoveAll();
				}
				m_translucents.ToHead(context);
				while( NULL != (ppTE = m_translucents.PostIncrement(context)) )
					delete *ppTE;
				m_translucents.DeleteAll();

				result = rc;
			}

			// Overlays
			if (GetLayer(IFX_VIEW_OVERLAY) && passNum == (pVR->GetNumRenderPasses() -1) )
			{
				if ( IFXSUCCESS(result) )
					result = pRenderContext->SetView( m_LayerView );

				if ( IFXSUCCESS(result) )
				{
					IFXRect rect;
					rect.m_X		= m_rcRenderViewport.m_X;
					rect.m_Width	= m_rcRenderViewport.m_Width;
					rect.m_Y		= m_rcRenderViewport.m_Y;
					rect.m_Height	= m_rcRenderViewport.m_Height;

					result = RenderViewLayer(IFX_VIEW_OVERLAY, *pRenderContext, &rect, m_fScaleX, m_fScaleY);
				}
			}
		} // for passNum

		IFXRELEASE( pShaderPalette );

		// This result means that the viewport not on screen, we can ignore the error
		if(result == IFX_E_INVALID_VIEWPORT)
			result = IFX_OK;

		IFXRELEASE(pCurrentRenderContext);
	}

	IFXRELEASE( pVR );

	return result;
}

F32 CIFXView::GetDistanceToProjectionPlane()
{
	F32 d = (F32)tan(m_View.GetFOVy() * (0.5f * (IFXPI/180.0f)));
	if ( d != 0 )
		return (m_rcRenderViewport.m_Height * 0.5f)/d;
	else
		return FLT_MAX;
}

IFXRESULT CIFXView::GetCullingSubsystem( IFXSpatialSetQuery*& rpOutCuller )
{
	IFXRESULT result = IFX_OK;

	if ( NULL == m_pCullingSubsystem )
	{
		// Create the initial culling system
		result = IFXCreateComponent(
							CID_IFXSceneGraphCuller,
							IID_IFXSpatialSetQuery,
							(void**)&m_pCullingSubsystem );
	}

	if( IFXSUCCESS( result ) )
	{
		rpOutCuller = m_pCullingSubsystem;
		m_pCullingSubsystem->AddRef();
	}

	return result;
}

IFXRESULT CIFXView::SetCullingSubsystem( IFXSpatialSetQuery& rInCuller )
{
	IFXRESULT result = IFX_OK;

	IFXDECLARELOCAL(IFXViewResource, pVR);
	pVR = GetViewResource();
	if( NULL == pVR )
		result = IFX_E_UNDEFINED;

	if( IFXSUCCESS( result ) )
	{
		if ( &rInCuller != m_pCullingSubsystem )
		{
			if ( pVR->GetRenderPass()->m_nodeSet )
			{
				IFXREFCID rCollectionCID = rInCuller.GetCollectionCID();
				IFXDECLARELOCAL(IFXCollection, pCollection);
				IFXDECLARELOCAL(IFXPalette, pNodePalette);
				result = m_pSceneGraph->GetPalette(IFXSceneGraph::NODE, &pNodePalette);
				IFXDECLARELOCAL(IFXNode, pRootNode);
				if ( IFXSUCCESS(result) )
					result = pNodePalette->GetResourcePtr(pVR->GetRenderPass()->m_nodeIndex, IID_IFXNode, (void**)&pRootNode);
				if ( IFXSUCCESS(result) )
					result = pRootNode->GetCollection( rCollectionCID, &pCollection );
				if ( IFXSUCCESS(result) )
				{
					// Release old culler only if the new one initalizes correctly
					IFXRELEASE( m_pCullingSubsystem );
					m_pCullingSubsystem = &rInCuller;
					m_pCullingSubsystem->AddRef();
					result = m_pCullingSubsystem->SetCollection( pCollection );
				}
			}
		}
	}

	return IFX_OK;
}

// ----------------------------------------------------------------------------
// CIFXView::GenerateRay
// ----------------------------------------------------------------------------
//
// generates a world space ray from screen coordinates for picking.
//
IFXRESULT CIFXView::GenerateRay(const F32   x,
								const F32   y,
								U32 ViewInstance,
								IFXVector3& pos,
								IFXVector3& dir)
{
	// Make sure the viewPort information is up to date
	UpdateViewport();

	// get a point on the film plane in camera space.
	IFXVector3 direction;
	IFXRESULT result = WindowToFilm(x, y, &direction);

	if ( IFXSUCCESS(result) )
	{
		// move dir and pos into world space.
		IFXMatrix4x4* pWorldTransform = NULL;
		IFXArray<IFXMatrix4x4> *pAWorldTransform;
		result = GetWorldMatrices(&pAWorldTransform);

		pWorldTransform = &pAWorldTransform->GetElement(ViewInstance);

		IFXMatrix4x4 mUnscaledWorld;
		IFXVector3   translation;
		IFXVector3   scale;

		// Decompose the world matrix
		result = pWorldTransform->Decompose(translation, mUnscaledWorld, scale);

		if (IFXSUCCESS(result))
		{
			mUnscaledWorld.SetTranslation(translation);

			switch(m_View.GetProjectionMode())
			{
			case IFX_PERSPECTIVE3:
				mUnscaledWorld.TransformVector(direction, dir);

				// eye point in world space.
				//memcpy( &pos.X(), mUnscaledWorld.RawConst()+12, 16);
				pos = translation;

				break;

			case IFX_ORTHOGRAPHIC:
				{
					// with orthographic projection, we don't use the position of
					// the camera as the eye point.  rather, everything is parallel
					// in camera space, so pos will just be dir with z == 0.
					IFXVector3 position(direction);
					position.Z() = 0;

					// move both into world space.
					mUnscaledWorld.TransformVector(direction, dir);
					mUnscaledWorld.TransformVector(position, pos);
					break;
				}

			default:
				result = IFX_E_UNSUPPORTED;
				IFXASSERT( FALSE );
				break;
			}

			// at this point, pos and dir are world space points where (dir - pos)
			// is really the direction vector that we want.  make dir = (dir -
			// pos).
			dir.Subtract(dir, pos);
		}
	}

	return result;
}


IFXRESULT CIFXView::GetLayer(IFXenum uLayer, U32 uIndex, IFXViewLayer& Layer)
{
	IFXRESULT result = IFX_OK;

	if ((uLayer >= IFX_VIEW_NUM_LAYERS)) // layers 0...n
		return IFX_E_INVALID_RANGE;

	CIFXViewLayer* pLayer = 0;

	if ( IFXSUCCESS(result) )
	{
		result = FindLayerByIndex(uLayer, uIndex, pLayer);
	}

	if ( IFXSUCCESS(result) )
	{
		Layer = *(IFXViewLayer*)pLayer;
	}

	return result;
}

IFXRESULT CIFXView::SetLayer(IFXenum uLayer, U32 uIndex, const IFXViewLayer& Layer, IFXRect *pViewport, F32 fScaleX, F32 fScaleY)
{
	IFXRESULT rc = IFX_OK;

	if ((uLayer >= IFX_VIEW_NUM_LAYERS)) // layers 0...n
		return IFX_E_INVALID_RANGE;


	CIFXViewLayer* pLayer = 0;

	if (IFXSUCCESS(rc))
	{
		rc = FindLayerByIndex(uLayer, uIndex, pLayer);
	}

	if (IFXSUCCESS(rc))
	{
		BOOL bDirty = FALSE;

		if (pLayer->m_iLocX != Layer.m_iLocX)
			bDirty = TRUE;
		else if (pLayer->m_iLocY != Layer.m_iLocY)
			bDirty = TRUE;
		else if (pLayer->m_iRegX != Layer.m_iRegX)
			bDirty = TRUE;
		else if (pLayer->m_iRegY != Layer.m_iRegY)
			bDirty = TRUE;
		else if (pLayer->m_fRotation != Layer.m_fRotation)
			bDirty = TRUE;
		else if (! (pLayer->m_vScale == Layer.m_vScale) )
			bDirty = TRUE;

		if (pLayer->m_uTextureId != Layer.m_uTextureId)
		{
			bDirty = TRUE;

			pLayer->m_uTextureId = Layer.m_uTextureId;

			IFXTextureObject* pTexture = 0;
			// GetTexture dimensions now.
			if (IFXSUCCESS(rc))
			{
				IFXUnknown* pUnk = 0;
				IFXPalette *pTexturePalette = NULL;
				rc = m_pSceneGraph->GetPalette( IFXSceneGraph::TEXTURE, &pTexturePalette);

				if ( IFXSUCCESS(rc) && pTexturePalette)
					rc = pTexturePalette->GetResourcePtr (pLayer->m_uTextureId, &pUnk);

				IFXRELEASE (pTexturePalette);

				if (IFXSUCCESS(rc) && pUnk)
				{
					rc = pUnk->QueryInterface(IID_IFXTextureObject, (void**)&pTexture);
				}
				else
				{
					rc = IFX_OK;
				}

				IFXRELEASE(pUnk);
			}

			if (IFXSUCCESS(rc) && pTexture)
			{
				U32 uWidth = 0;
				U32 uHeight = 0;
				pTexture->GetWidth(&uWidth);
				pTexture->GetHeight(&uHeight);

				pLayer->m_fTexSizeScaleX = (F32)uWidth;
				pLayer->m_fTexSizeScaleY = (F32)uHeight;
			}
			else
			{
				rc = IFX_OK;
				bDirty = FALSE;
				pLayer->m_bTextureDirty = TRUE;
				pLayer->m_fTexSizeScaleX = 1.0f;
				pLayer->m_fTexSizeScaleY = 1.0f;
			}

			IFXRELEASE(pTexture);

		}

		*pLayer = Layer;

		if (bDirty)
		{
			RecalcLayerMatrix(uLayer, uIndex, pViewport, fScaleX, fScaleY);
		}

	}

	return rc;
}


// Returns the count of the number of Layers for this view
IFXRESULT CIFXView::GetLayerCount(IFXenum uLayer, U32& uLayerCount)
{
	IFXRESULT result = IFX_OK;

	if ((uLayer >= IFX_VIEW_NUM_LAYERS)) // layers 0...n
		return IFX_E_INVALID_RANGE;


	CIFXViewLayer* pLayer = m_pLayer[uLayer];
	uLayerCount = 0;

	while(pLayer)
	{
		uLayerCount++;
		pLayer = pLayer->m_pNext;
	}

	return result;
}


// Inserts the Layer at the given position in the list (0 = first Layer)
IFXRESULT CIFXView::InsertLayer(IFXenum uLayer, U32 uIndex, const IFXViewLayer& Layer, IFXRect *pViewport, F32 fScaleX, F32 fScaleY)
{
	IFXRESULT rc = IFX_OK;

	CIFXViewLayer* pLayer = 0;

	// Add to start of chain
	if (uIndex == 0)
	{
		pLayer = new CIFXViewLayer;

		pLayer->m_pNext = m_pLayer[uLayer];

		if (pLayer->m_pNext)
		{
			pLayer->m_pNext->m_pPrev = pLayer;
		}

		m_pLayer[uLayer] = pLayer;
	}
	else
	{
		rc = FindLayerByIndex(uLayer, uIndex, pLayer);

		// Add to middle of chain
		if (IFXSUCCESS(rc))
		{
			CIFXViewLayer* pNewLayer = new CIFXViewLayer;

			pNewLayer->m_pPrev = pLayer->m_pPrev;
			pNewLayer->m_pNext = pLayer;
			pLayer->m_pPrev = pNewLayer;
			if (pNewLayer->m_pPrev)
				pNewLayer->m_pPrev->m_pNext = pNewLayer;
		}
		// Add to end of chain
		else if (IFXSUCCESS(rc = FindLayerByIndex(uLayer,uIndex-1, pLayer)))
		{
			CIFXViewLayer* pNewLayer = new CIFXViewLayer;

			pLayer->m_pNext = pNewLayer;
			pNewLayer->m_pPrev = pLayer;
		}
	}

	if (IFXSUCCESS(rc))
	{
		rc = SetLayer(uLayer, uIndex, Layer, pViewport, fScaleX, fScaleY);
	}

	return rc;
}

// Adds the Layer at the end of the list
IFXRESULT CIFXView::AddLayer(IFXenum uLayer, const IFXViewLayer& Layer, IFXRect *pViewport, F32 fScaleX, F32 fScaleY)
{
	IFXRESULT result = IFX_OK;

	U32 uIndex = 0;

	CIFXViewLayer* pLayer = m_pLayer[uLayer];

	if (!pLayer)
	{
		m_pLayer[uLayer] = new CIFXViewLayer;
	}
	else
	{
		uIndex++;
		while(pLayer->m_pNext)
		{
			uIndex++;
			pLayer = pLayer->m_pNext;
		}

		pLayer->m_pNext = new CIFXViewLayer;
		pLayer->m_pNext->m_pPrev = pLayer;
	}

	result = SetLayer(uLayer, uIndex, Layer, pViewport, fScaleX, fScaleY);

	return result;
}

// Removes the Layer given by index
IFXRESULT CIFXView::RemoveLayer(U32 uLayer, U32 uIndex)
{
	IFXRESULT rc = IFX_OK;

	CIFXViewLayer* pLayer = 0;
	rc = FindLayerByIndex(uLayer, uIndex, pLayer);

	if (IFXSUCCESS(rc))
	{
		if (pLayer->m_pPrev)
		{
			pLayer->m_pPrev->m_pNext = pLayer->m_pNext;
			pLayer->m_pPrev = 0;
		}

		if (pLayer->m_pNext)
		{
			pLayer->m_pNext->m_pPrev = pLayer->m_pPrev;
		}

		if (m_pLayer[uLayer] == pLayer)
		{
			m_pLayer[uLayer] = pLayer->m_pNext;
		}

		pLayer->m_pNext = 0;

		IFXDELETE(pLayer);
	}

	return rc;

}

// Finds the Layer mirror at a given index
IFXRESULT CIFXView::FindLayerByIndex (IFXenum uLayer, U32 uIndex, CIFXViewLayer*& pLayer)
{
	IFXRESULT result = IFX_OK;

	// 14nov00 jjc:  it seems to me that this check should cause an
	// IFX_E_INVALID_RANGE error.  i will investigate.
	if (uLayer >= IFX_VIEW_NUM_LAYERS || !(m_pLayer[uLayer]))
		return IFX_E_INVALID_RANGE;

	if (IFXFAILURE(result))
		return result;

	// iterate through the Layers and stop when finding the correct one
	pLayer = m_pLayer[uLayer];

	while(uIndex && pLayer)
	{
		uIndex--;
		pLayer = pLayer->m_pNext;
	}

	if (pLayer == 0 || uIndex)
	{
		pLayer = 0;
		result = IFX_E_INVALID_RANGE;
	}

	return result;
}

// Recalculates the layer matrix
IFXRESULT CIFXView::RecalcLayerMatrix (U32 uLayer, U32 uIndex, IFXRect *pViewport, F32 fScaleX, F32 fScaleY)
{
	IFXRESULT rc = IFX_OK;

	CIFXViewLayer* pLayer = 0;
	rc = FindLayerByIndex(uLayer, uIndex, pLayer);

	if (IFXSUCCESS(rc))
	{
		IFXMatrix4x4 mPreRotation;
		IFXMatrix4x4 mRotation;
		IFXMatrix4x4 mPostRotation;
		IFXMatrix4x4 mTranslation;
		IFXMatrix4x4 mScale;
		IFXMatrix4x4 mRenderScale;

		IFXMatrix4x4 mTmp;
		mTmp.Reset();

		IFXVector3 vScale(pLayer->m_fTexSizeScaleX, pLayer->m_fTexSizeScaleY, 1);
		vScale.Multiply(IFXVector3(pLayer->m_vScale.X(), pLayer->m_vScale.Y(), 1));

		mScale.Reset();
		mScale.Scale3x4(vScale);

		mRenderScale.Reset();
		mRenderScale.Scale3x4(IFXVector3(fScaleX, fScaleY, 1));

		mRotation.Reset();
		mRotation.Rotate3x4(-pLayer->m_fRotation*IFXTO_RAD, IFX_Z_AXIS);

		IFXVector3 vOffset(-0.5f, -0.5f, 0);
		vOffset.Multiply(vScale);

		mPreRotation.Reset();
		mPreRotation.SetTranslation(vOffset);

		mPostRotation.Reset();
		vOffset.Negate();
		mPostRotation.SetTranslation(vOffset);

		IFXVector3 vCenter(0,0,0);

		I32 iLocX = (I32)((m_attributes&IFX_PERCENTDIMEN) ? pLayer->m_iLocX * pViewport->m_Width : pLayer->m_iLocX);
		I32 iLocY = (I32)((m_attributes&IFX_PERCENTDIMEN) ? pLayer->m_iLocY * pViewport->m_Height : pLayer->m_iLocY);
		vCenter.X() = (iLocX - pLayer->m_iRegX) - pViewport->m_Width*0.5f;
		vCenter.Y() = pViewport->m_Height*0.5f - (iLocY - pLayer->m_iRegY + (I32)(pLayer->m_fTexSizeScaleY * pLayer->m_vScale.Y()));
		mTranslation.Reset();
		mTranslation.Translate3x4(vCenter);

		pLayer->m_Transform.Reset();

		pLayer->m_Transform = mTmp.Multiply3x4(mScale, pLayer->m_Transform);
		pLayer->m_Transform = mTmp.Multiply3x4(mPreRotation, pLayer->m_Transform);
		pLayer->m_Transform = mTmp.Multiply3x4(mRotation, pLayer->m_Transform);
		pLayer->m_Transform = mTmp.Multiply3x4(mPostRotation, pLayer->m_Transform);
		pLayer->m_Transform = mTmp.Multiply3x4(mTranslation, pLayer->m_Transform);
		pLayer->m_Transform = mTmp.Multiply3x4(mRenderScale, pLayer->m_Transform);

		// Offset for aligning texels and pixels
		mTranslation.Reset();
		mTranslation.Translate3x4(IFXVector3(0.375f, 0.375f,0));
		pLayer->m_Transform = mTmp.Multiply3x4(mTranslation, pLayer->m_Transform);

	}

	return rc;
}

// ----------------------------------------------------------------------------
// CIFXView::RecalcAllLayerMatrices
// ----------------------------------------------------------------------------
//
// recalculates every layer matrix of this view.
//
IFXRESULT CIFXView::RecalcAllLayerMatrices(IFXRect *pViewport, F32 fScaleX, F32 fScaleY)
{
	IFXRESULT result  = IFX_OK;

	U32 i;
	for( i = 0;
		(i < IFX_VIEW_NUM_LAYERS) && ( IFXSUCCESS(result) );
		++i)
	{
		// iterate over all layer types.

		// for this type, get the number of layers.
		U32 layerCount;
		result = this->GetLayerCount(i, layerCount);

		U32 j;
		for( j = 0; (j < layerCount) && IFXSUCCESS(result); ++j)
		{
			// recalculate the layer matrix for all layers.
			result = this->RecalcLayerMatrix(i, j, pViewport, fScaleX, fScaleY);
		}
	}

	return result;
}

void CIFXView::SetAttributes( U32 att )
{
	m_attributes = att;
	const U32 attributesMasked = att & ~IFX_PERCENTDIMEN; // mask screen position unit

	if( IFX_PERSPECTIVE3 == attributesMasked )
	{
		this->SetProjectionMode( IFX_PERSPECTIVE3 );
	}
	else if( IFX_ORTHOGRAPHIC == attributesMasked ) // ortho
	{
		this->SetProjectionMode( IFX_ORTHOGRAPHIC );
	}
	else
	{
		IFXASSERT( FALSE );
	}

	m_bViewOrScaleChanged = TRUE;
}

IFXRESULT CIFXView::SetProjection(IFXVector3 projVector)
{
	return IFX_E_UNSUPPORTED;
}

IFXRESULT CIFXView::GetProjection(IFXVector3* pProjVector)
{
	if( ( m_attributes & IFX_PERSPECTIVE2 ) &&
		( m_attributes & IFX_PERSPECTIVE1 ) )
	{
		(*pProjVector).X() = 0.0f;
		(*pProjVector).Y() = 0.0f;
		(*pProjVector).Z() = 0.0f;
	}

	return IFX_E_UNSUPPORTED;
}


//===============================
// CIFXViewLayer
//===============================
CIFXViewLayer::CIFXViewLayer()
{
	m_fTexSizeScaleX = 1;
	m_fTexSizeScaleY = 1;

	m_Transform.Reset();

	m_bTextureDirty = TRUE;

	m_pNext = 0;
	m_pPrev = 0;
}

CIFXViewLayer::~CIFXViewLayer()
{
	m_pPrev = 0;
	IFXDELETE(m_pNext);
}

const CIFXViewLayer& CIFXViewLayer::operator=(const IFXViewLayer& Layer)
{
	*(IFXViewLayer*)this = Layer;

	while(m_fRotation > 180)
		m_fRotation -= 360;
	while(m_fRotation < -180)
		m_fRotation += 360;

	return *this;
}

/**
	This function looks for a modifier with the requested IID through
	both the node and resource modifier chain of pModel
*/
IFXRESULT FindModifier( IFXModel* pModel, IFXREFIID iid, IFXModifier** pMod, U32 number )
{
	IFXRESULT result = IFX_OK;

	if( !pModel )
		result = IFX_E_INVALID_POINTER;

	IFXDECLARELOCAL( IFXModifierChain, pModifierChain );
	U32 i, modCnt;
	U32 cnt = 0;

	if( IFXSUCCESS( result ) )
		result = pModel->GetModifierChain( &pModifierChain );

	if( IFXSUCCESS( result ) )
	{
		pModifierChain->GetModifierCount( modCnt );

		for( i = 0; i < modCnt; i++ )
		{
			IFXDECLARELOCAL( IFXModifier, pModifier );
			IFXDECLARELOCAL( IFXModifier, pRequest );

			result = pModifierChain->GetModifier( i, pModifier );

			if( IFXSUCCESS( result ) )
				result = pModifier->QueryInterface( iid, (void**) &pRequest );

			if( IFXSUCCESS( result ) )
			{
				if( number == cnt )
				{
					*pMod = pRequest;
					(*pMod)->AddRef();
					break;
				}
				else
					result = IFX_E_NOT_FOUND;

				cnt++;
			}
		}
		if( !IFXSUCCESS( result ) )
			//no glyph in node modifier chain, looking in resource mod chain
		{
			IFXDECLARELOCAL( IFXPalette, pPal );
			IFXDECLARELOCAL( IFXModifier, pModifier );
			IFXDECLARELOCAL( IFXModifierChain, pResModChain );
			IFXDECLARELOCAL( IFXSceneGraph, pSceneGraph );
			U32 resIndex = pModel->GetResourceIndex();
			U32 resPalette = pModel->GetResourcePalette();

			result = pModel->GetSceneGraph( &pSceneGraph );

			if( IFXSUCCESS( result ) )
				result = pSceneGraph->GetPalette( ( IFXSceneGraph::EIFXPalette )resPalette, &pPal );

			if( IFXSUCCESS( result ) )
				result = pPal->GetResourcePtr( resIndex, IID_IFXModifier, (void**) &pModifier );

			if( IFXSUCCESS( result ) )
				result = pModifier->GetModifierChain( &pResModChain );

			U32 i, modCnt = 0;

			if( IFXSUCCESS( result ) )
				result = pResModChain->GetModifierCount( modCnt );

			for( i = 0; i < modCnt && ( IFXSUCCESS( result ) || result == IFX_E_NOT_FOUND ); i++ )
			{
				IFXDECLARELOCAL( IFXModifier, pModifier );
				IFXDECLARELOCAL( IFXModifier, pRequest );

				result = pResModChain->GetModifier( i, pModifier );

				if( IFXSUCCESS( result ) )
					result = pModifier->QueryInterface( iid, (void**) &pRequest );

				if( IFXSUCCESS( result ) )
				{
					if( number == cnt )
					{
						*pMod = pRequest;
						(*pMod)->AddRef();
						break;
					}
					else
						result = IFX_E_NOT_FOUND;

					cnt++;
				}
			}
		}
	}

	return result;
}
