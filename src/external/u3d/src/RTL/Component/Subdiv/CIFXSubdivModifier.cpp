
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

#include "CIFXSubdivModifier.h"
#include "IFXSubdivisionManager.h"
#include "IFXScreenSpaceMetric.h"
#include "IFXModifierDataPacket.h"
#include "IFXDids.h"
#include "IFXFrustum.h"
#include "IFXMesh.h"


#include "IFXExportingCIDs.h"

CIFXSubdivModifier::CIFXSubdivModifier()
{
	m_uRefCount = 0;

	m_pSubdivMgr          = NULL;
	m_pScreenSpaceMetric  = NULL;
	m_puOtherChangeCounts = NULL;
	m_puFaceChangeCounts  = NULL;
	m_bEnabled            = IFX_TRUE;

	m_uNeighborMeshDataElementIndex = (U32)-1;
	m_uMeshGroupDataElementIndex    = (U32)-1;
	m_uTransformDataElementIndex    = (U32)-1;
	m_uFrustumDataElementIndex      = (U32)-1;

	ResetInitData();
}


CIFXSubdivModifier::~CIFXSubdivModifier()
{
	IFXDELETE(m_pSubdivMgr);
	IFXDELETE(m_pScreenSpaceMetric);
	IFXDELETE_ARRAY(m_puOtherChangeCounts);
	IFXDELETE_ARRAY(m_puFaceChangeCounts);
}


void CIFXSubdivModifier::ResetInitData()
{
	// Reset to defaults:
	m_InitData.bActive    = FALSE;
	m_InitData.uDepth   = 1;
	m_InitData.fError   = 0.0f;
	m_InitData.fTension   = 65.0f;
	m_InitData.bAdaptive  = FALSE;
}


IFXRESULT IFXAPI_CALLTYPE CIFXSubdivModifier_Factory( IFXREFIID iid, void** ppv )
{
	IFXRESULT result;

	if (ppv)
	{
		// Create the CIFXClassName component.
		CIFXSubdivModifier *pComponent = new CIFXSubdivModifier;

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
			result = IFX_E_OUT_OF_MEMORY;
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}


// IFXUnknown
U32 CIFXSubdivModifier::AddRef()
{
	return ++m_uRefCount;
}


U32 CIFXSubdivModifier::Release()
{
	if ( 1 == m_uRefCount )
	{
		PreDestruct();
		delete this;
		return 0;
	}

	return --m_uRefCount;
}


IFXRESULT CIFXSubdivModifier::QueryInterface( IFXREFIID riid, void **ppv )
{
	IFXRESULT result = IFX_OK;

	if ( ppv )
	{
		if ( riid == IID_IFXUnknown )
			*ppv = (IFXUnknown*)this;
		else if ( riid == IID_IFXSubdivModifier   )
			*ppv = (IFXSubdivModifier*)this;
		else if ( riid == IID_IFXMarker )
			*ppv = (IFXMarker*)this;
		else if ( riid == IID_IFXMarkerX )
			*ppv = (IFXMarkerX*)this;
		else if ( riid == IID_IFXModifier )
			*ppv = (IFXModifier*)this;
		else if ( riid == IID_IFXSubject )
			*ppv = (IFXSubject*)this;
		else if ( riid == IID_IFXMetaDataX )
			*ppv = ( IFXMetaDataX* ) this;
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

void CIFXSubdivModifier::GetEncoderX(IFXEncoderX*& rpEncoderX)
{
	CIFXMarker::GetEncoderX(CID_IFXSubdivisionModifierEncoder, rpEncoderX);
}

// IFXModifier
const IFXGUID* CIFXSubdivModifier::m_scpOutputDIDs[] =
{
	&DID_IFXRenderableGroup
};


const IFXGUID* CIFXSubdivModifier::m_scpInputDIDs[] =
{
	&DID_IFXRenderableGroup,
		&DID_IFXNeighborMesh,
		&DID_IFXTransform,
		&DID_IFXFrustum,
};


IFXRESULT CIFXSubdivModifier::GetOutputs( IFXGUID**& rpOutOutputs,
										 U32&       rOutNumberOfOutputs,
										 U32*&    rpOutOutputDepAttrs )
{
	rOutNumberOfOutputs = 1;
	rpOutOutputDepAttrs = NULL;
	rpOutOutputs = (IFXGUID**)&m_scpOutputDIDs;
	return IFX_OK;
}


IFXRESULT CIFXSubdivModifier::GetDependencies(
	IFXGUID*   pInOutputDID,
	IFXGUID**& rppOutInputDependencies,
	U32&       rOutNumberInputDependencies,
	IFXGUID**& rppOutOutputDependencies,
	U32&       rOutNumberOfOutputDependencies,
	U32*&    rpOutOutputDepAttrs )
{
	IFXRESULT result = IFX_OK;

	if ( pInOutputDID == &DID_IFXRenderableGroup )
	{
		rOutNumberInputDependencies = sizeof(m_scpInputDIDs) / sizeof(IFXGUID*);
		rppOutInputDependencies = (IFXDID**)m_scpInputDIDs;
		rOutNumberOfOutputDependencies = 0;
		rppOutOutputDependencies = NULL;
		rpOutOutputDepAttrs = NULL;
	}
	else
	{
		IFXASSERTBOX( 1, "CIFXSubdivModifier::GetDependencies() called with unknown output!" );
		result = IFX_E_UNDEFINED;
	}

	return result;
}


IFXRESULT CIFXSubdivModifier::GenerateOutput( U32    inOutputDataElementIndex,
											 void*& rpOutData, BOOL& rNeedRelease )
{
	IFXRESULT result = IFX_OK;

	if ( inOutputDataElementIndex == m_uMeshGroupDataElementIndex )
	{
		IFXMeshGroup* pInputMeshGroup  = NULL;
		IFXMeshGroup* pOutputMeshGroup = NULL;
		IFXFrustum*   pFrustum         = NULL;

		result = m_pInputDataPacket->GetDataElement( m_uMeshGroupDataElementIndex,
			IID_IFXMeshGroup,
			(void**)&pInputMeshGroup );

		if( IFXSUCCESS(result) )
			result = m_pInputDataPacket->GetDataElement( m_uFrustumDataElementIndex,
			(void**)&pFrustum);

		// Check to see if change counts have changed
		if( IFXSUCCESS(result) )
			result = DataChanged(pInputMeshGroup);

		if( IFXSUCCESS(result) )
		{
			if (!m_bEnabled)
			{
				result = m_pInputDataPacket->
					GetDataElement( m_uMeshGroupDataElementIndex, &rpOutData );
				rNeedRelease = TRUE;
			}
			else
			{
				BOOL bJustDeleted = FALSE;
				BOOL bAdaptive    = FALSE;
				F32  fTension     = 0.0f;
				U32  uDepth       = 0;

				/// @todo Still needs some updating, but a temp fix for now.

				if ( m_pSubdivMgr && m_bFaceDataChanged )
				{
					// Grab subdiv mgr state:
					m_pSubdivMgr->GetInteger(IFXSubdivisionManagerInterface::MaxComputeDepth, &uDepth);
					m_pSubdivMgr->GetFloat(IFXSubdivisionManagerInterface::SurfaceTension, &fTension);
					m_pSubdivMgr->GetBool(IFXSubdivisionManagerInterface::Adaptive, &bAdaptive);

					IFXDELETE(m_pSubdivMgr);
					bJustDeleted = TRUE;
				}

				if( m_pSubdivMgr == NULL )
				{
					IFXNeighborMesh* pInputNeighborMesh = NULL;

					// Get the neighborMesh from the dataPacket
					result = m_pInputDataPacket->GetDataElement( m_uNeighborMeshDataElementIndex,
						IID_IFXNeighborMesh,
						(void**)&pInputNeighborMesh);
					if( IFXSUCCESS(result) )
						result = InitializeSubdiv(pInputMeshGroup, pInputNeighborMesh);

					if( IFXFAILURE(result) )
					{
						// Check to see if the model had too many vertices to subdivide
						// If it does then simply return the input meshGroup
						if( IFX_E_SUBDIVMGR_LARGE_MESHES_UNSUPPORTED == result )
						{
							pInputMeshGroup->QueryInterface( IID_IFXUnknown, &rpOutData );
							rNeedRelease = TRUE;

							IFXRELEASE(pInputMeshGroup);

							IFXDELETE(m_pScreenSpaceMetric);
							IFXDELETE(m_pSubdivMgr);

							return IFX_OK;
						}
					}

					IFXRELEASE(pInputNeighborMesh);

					if (IFXSUCCESS(result) && bJustDeleted)
					{
						// restore subdiv mgr state:
						m_pSubdivMgr->SetFloat(IFXSubdivisionManagerInterface::SurfaceTension, fTension);
						m_pSubdivMgr->SetBool(IFXSubdivisionManagerInterface::Adaptive, bAdaptive);
						SetDepth (uDepth);

						m_pSubdivMgr->ResetAll();
					}
				}

				// If initialized failed, we'll disable subdivision:
				if( IFXFAILURE(result) )
				{
					m_bEnabled = FALSE;
					IFXDELETE(m_pSubdivMgr);
				}

				if( IFXSUCCESS(result) && m_pSubdivMgr )
				{
					BOOL bAdaptive = IFX_FALSE;

					result = m_pSubdivMgr->GetBool(IFXSubdivisionManagerInterface::Adaptive, &bAdaptive);

					if(IFXSUCCESS(result) && bAdaptive)
					{
						IFXMatrix4x4* pTransform      = NULL;
						U32 NumberElements = 0;
						IFXArray<IFXMatrix4x4> *pAInputMatrix;
						IFXMatrix4x4  mModelXFormInv;
						IFXFrustum*   pFrustum        = NULL;
						U32 i;
						IFXMatrix4x4 mCameraNewXForm, mCameraTempXForm;

						if( IFXSUCCESS(result) )
							result = m_pInputDataPacket->GetDataElement(
							m_uTransformDataElementIndex, (void**)&pAInputMatrix);
						if (IFXSUCCESS(result))
							NumberElements = pAInputMatrix->GetNumberElements();

						if( IFXSUCCESS(result) )
							result = m_pInputDataPacket->GetDataElement( m_uFrustumDataElementIndex,
							(void**)&pFrustum);

						F32 fAspect = pFrustum->GetAspect();
						F32 fFOV    = pFrustum->GetFOV();
						// Pass on the field of view:
						m_pScreenSpaceMetric->SetFieldofView(fFOV*2);

						mCameraNewXForm.MakeIdentity();
						for (i = 0; i < NumberElements; i++)
						{
							IFXMatrix4x4 tempMatrix;
							pTransform = &pAInputMatrix->GetElement(i);
							mModelXFormInv.Invert3x4(*pTransform);
							// Multiply the two together.  This provides the camera rotation into model space:
							mCameraTempXForm.Multiply(mModelXFormInv, pFrustum->GetWorldTransform());
							// in case of multiparent all instances of Node are taken into account
							tempMatrix = mCameraNewXForm;
							mCameraNewXForm.Multiply(tempMatrix, mCameraTempXForm);
						}

						IFXVector3  cameraX, cameraY, cameraZ, cameraPosition;

						cameraX[0] = mCameraNewXForm[0];
						cameraX[1] = mCameraNewXForm[1];
						cameraX[2] = mCameraNewXForm[2];

						cameraY[0] = mCameraNewXForm[4];
						cameraY[1] = mCameraNewXForm[5];
						cameraY[2] = mCameraNewXForm[6];

						cameraZ[0] = mCameraNewXForm[8];
						cameraZ[1] = mCameraNewXForm[9];
						cameraZ[2] = mCameraNewXForm[10];

						cameraPosition[0] = mCameraNewXForm[12];
						cameraPosition[1] = mCameraNewXForm[13];
						cameraPosition[2] = mCameraNewXForm[14];

						m_pScreenSpaceMetric->UpdateViewpoint(cameraX, cameraY, cameraZ,
							cameraPosition, fAspect, fFOV);
					}
				}

				// 4) If the underlying mesh has changed, do an appropriate reset.
				// Note that if FACES was changed, then this copy is unnecessary
				if (IFXSUCCESS(result) && m_bOtherDataChanged )
				{
					m_pSubdivMgr->CopyVertexData(pInputMeshGroup);
					m_pSubdivMgr->ResetAll();
				}

				// 5) Update the current mesh:
				if( IFXSUCCESS(result) && m_pSubdivMgr )
				{
					IFXRenderable* pRenderableOld = NULL;
					IFXRenderable* pRenderableNew = NULL;
					IFXMesh*       pMeshOld       = NULL;
					IFXMesh*       pMeshNew       = NULL;

					BOOL           bUpdated       = FALSE;
					IFXShaderList* pShaderList    = NULL;

					result = m_pSubdivMgr->UpdateMesh(&pOutputMeshGroup, &bUpdated);

					// 6) Now copy out the output mesh:
					if( bUpdated && IFXSUCCESS(result) )
					{
						// Tell the mesh that it has been updated
						U32 i;
						for( i = 0; i < pOutputMeshGroup->GetNumMeshes(); i++)
						{
							pInputMeshGroup->GetMesh(i, pMeshOld);
							pOutputMeshGroup->GetMesh(i, pMeshNew);

							if( pMeshNew && pMeshOld )
							{
								pMeshNew->UpdateVersionWord(IFX_MESH_POSITION);
								pMeshNew->UpdateVersionWord(IFX_MESH_NORMAL);
								pMeshNew->UpdateVersionWord(IFX_MESH_TC0);
								pMeshNew->UpdateVersionWord(IFX_MESH_FACE);

								// Copy over shader and MeshID information

								// Get the Old Renderable
								result = pInputMeshGroup->QueryInterface( IID_IFXRenderable,
									(void**)&pRenderableOld );

								// Get the shaderID
								if( IFXSUCCESS(result) )
									result = pRenderableOld->GetElementShaderList( i, &pShaderList);

								// Get the New Renderable
								if( IFXSUCCESS(result) )
									result = pOutputMeshGroup->QueryInterface( IID_IFXRenderable,
									(void**)&pRenderableNew );
								// Set the shaderID
								if( IFXSUCCESS(result) )
									result = pRenderableNew->SetElementShaderList( i, pShaderList);

								IFXRELEASE(pShaderList);
								// Get and set the meshID
								if( IFXSUCCESS(result) )
									pMeshNew->SetId(pMeshOld->GetId());

								IFXRELEASE( pRenderableOld );
								IFXRELEASE( pRenderableNew );
							}

							IFXRELEASE( pMeshOld );
							IFXRELEASE( pMeshNew );
						}
					}
				}

				if( IFXSUCCESS(result) )
				{
					pOutputMeshGroup->QueryInterface( IID_IFXUnknown, &rpOutData );
					//pOutputMeshGroup->Release();
					rNeedRelease = TRUE;
				}
			}
		}

		IFXRELEASE(pInputMeshGroup);
	}

	return result;
}


IFXRESULT CIFXSubdivModifier::SetDataPacket(
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
		result = pInInputDataPacket->GetDataElementIndex( DID_IFXRenderableGroup,
		m_uMeshGroupDataElementIndex );

	if( IFXSUCCESS(result) )
		result = pInInputDataPacket->GetDataElementIndex( DID_IFXNeighborMesh,
		m_uNeighborMeshDataElementIndex );

	if( IFXSUCCESS(result) )
		result = pInInputDataPacket->GetDataElementIndex( DID_IFXTransform,
		m_uTransformDataElementIndex );

	if( IFXSUCCESS(result) )
		result = pInInputDataPacket->GetDataElementIndex( DID_IFXFrustum,
		m_uFrustumDataElementIndex );

	return result;
}


IFXRESULT CIFXSubdivModifier::Notify( IFXModifierMessage eInMessage,
									 void*               pMessageContext )

{
	return IFX_OK;
}


// IFXSubdivModifier
IFXRESULT CIFXSubdivModifier::GetEnable(BOOL *pbEnable)
{
	IFXRESULT rc = IFX_OK;

	if (!pbEnable)
		rc = IFX_E_NOT_INITIALIZED;

	if(IFXSUCCESS(rc))
	{
		*pbEnable = m_bEnabled;
	}

	return rc;
}


IFXRESULT CIFXSubdivModifier::SetEnable(BOOL bEnable)
{
	IFXRESULT rc = IFX_OK;

	m_bEnabled = bEnable;

	return rc;
}


IFXRESULT CIFXSubdivModifier::GetTension(F32* pTension)
{
	IFXRESULT rc = IFX_OK;

	IFXASSERT (pTension);

	if (!pTension)
		rc = IFX_E_NOT_INITIALIZED;

	if(IFXSUCCESS(rc))
		*pTension = 0.0f;

	// Is is enabled but the subdiv mgr not initialized?
	if (!m_pSubdivMgr)
	{
		// If not, then grab parameter from the temporary param cache:
		*pTension = m_InitData.fTension;
	}
	else if(IFXSUCCESS(rc))
		rc = m_pSubdivMgr->GetFloat
		(IFXSubdivisionManagerInterface::SurfaceTension, pTension);

	// Unscale it:
	*pTension *= 100.0f;

	return rc;
}


IFXRESULT CIFXSubdivModifier::SetTension(F32 tension)
{
	IFXRESULT rc = IFX_OK;

	// Is is enabled but the subdiv mgr not initialized?
	if (!m_pSubdivMgr)
	{
		// Cache the intended tension:
		m_InitData.bActive    = true;
		m_InitData.fTension   = tension;
	}
	else
	{
		// scale it:
		tension *= 0.01f;

		F32 fSurfaceTensionOld = 0.0;

		if(IFXSUCCESS(rc))
			rc = m_pSubdivMgr->GetFloat (IFXSubdivisionManagerInterface::SurfaceTension, &fSurfaceTensionOld);

		if (fSurfaceTensionOld != tension)
		{
			if(IFXSUCCESS(rc))
				rc = m_pSubdivMgr->SetFloat (IFXSubdivisionManagerInterface::SurfaceTension, tension);

			if(IFXSUCCESS(rc))
				rc = m_pSubdivMgr->ResetAll();

		}
	}

	return rc;
}


IFXRESULT CIFXSubdivModifier::GetDepth  (U32*   pDepth)
{
	IFXRESULT rc = IFX_OK;

	if (!pDepth)
		rc = IFX_E_NOT_INITIALIZED;

	if(IFXSUCCESS(rc))
		*pDepth = 0;

	// Is is the subdiv mgr not initialized?
	if (!m_pSubdivMgr)
	{
		// If not, then grab parameter from the temporary param cache:
		*pDepth = m_InitData.uDepth;
	}
	else if(IFXSUCCESS(rc))
	{
		rc = m_pSubdivMgr->GetInteger
			(IFXSubdivisionManagerInterface::MaxComputeDepth, pDepth);
	}

	return rc;
}


U32 CIFXSubdivModifier::CalculateSafeDepth()
{
	IFXRESULT result = IFX_OK;

	U32 safeDepth = IFX_SDS_MAX_ALLOWED_SUBDIVISION_DEPTH;

	if( m_pInputDataPacket )
	{
		IFXMeshGroup* pMeshGroup    = NULL;
		//U32           uMeshGroupIDX = 0;

		// Grab the meshGroup and changeCount from the ModifierDataPacket
		// Will be used for output purposes later
		result = m_pInputDataPacket->GetDataElement(m_uMeshGroupDataElementIndex, IID_IFXMeshGroup, (void**)&pMeshGroup);

		if( IFXSUCCESS(result) )
		{
			IFXMesh *pMesh = NULL;
			U32 numMeshes = pMeshGroup->GetNumMeshes();
			U32 numTriangles = 0;
			U32 numVertices = 0;
			U32 i = 0;

			for (;i < numMeshes; i++)
			{
				pMeshGroup->GetMesh(i, pMesh);
				if( pMesh )
				{
					numTriangles += pMesh->GetNumFaces();
					numVertices  += pMesh->GetNumVertices();
				}
				IFXRELEASE(pMesh);
			} // end loop over all meshes to count faces

			// now calculate a safe depth
			safeDepth = 0;
			U32 uMaxAllowedTriangles  = 0;
			U32 uMaxAllowedVertices   = 0;

			m_pSubdivMgr->GetInteger(
				IFXSubdivisionManagerInterface::MaxTriangleAllocation,
				&uMaxAllowedTriangles);
			m_pSubdivMgr->GetInteger(
				IFXSubdivisionManagerInterface::MaxVertexAllocation,
				&uMaxAllowedVertices);

			// For uniform subdivision:
			//   numVertices(depth n) ~=
			//       numVertices (depth n-1) + numVertices(depth n-1) * (n-1)

			// # vertices at depth = 1:
			numVertices *= 2;

			while ( (safeDepth < IFX_SDS_MAX_ALLOWED_SUBDIVISION_DEPTH) &&
				(numVertices < uMaxAllowedVertices) &&
				((numTriangles *= 4) < uMaxAllowedTriangles) )
			{
				safeDepth++;

				// # vertices at other depths:
				numVertices = numVertices + numVertices * (safeDepth);
			}

		} // if got a meshgroup

		IFXRELEASE(pMeshGroup);
	}

	return safeDepth;
}


IFXRESULT CIFXSubdivModifier::SetDepth  (U32 depth)
{
	IFXRESULT rc = IFX_OK;

	// Is is the subdiv mgr not initialized?
	if (!m_pSubdivMgr)
	{
		// Cache the intended depth:
		m_InitData.bActive  = true;
		m_InitData.uDepth = depth;
	}
	else
	{
		U32 safeDepth = 0;

		if(IFXSUCCESS(rc))
		{
			safeDepth = CalculateSafeDepth();
		}

		// Range check depth parameter:
		if(IFXSUCCESS(rc))
		{
			if (depth > safeDepth)
				depth = safeDepth;
		}

		if(IFXSUCCESS(rc))
		{
			// Decreasing subdivision depth is costly, requires a consolidate for each level:
			U32 uOldSubdivLevel;

			rc = m_pSubdivMgr->GetInteger (IFXSubdivisionManagerInterface::MaxComputeDepth, &uOldSubdivLevel);

			// First consolidate any old triangle in levels that are no longer active:
			int uL;
			for (uL = (int)uOldSubdivLevel; uL > (int)depth; uL--)
			{
				if(IFXSUCCESS(rc))
					rc = m_pSubdivMgr->ConsolidateLevel(uL);
			}
		}


		if(IFXSUCCESS(rc))
		{
			rc = m_pSubdivMgr->SetInteger
				(IFXSubdivisionManagerInterface::MaxComputeDepth, depth);
		}

		if(IFXSUCCESS(rc))
		{
			rc = m_pSubdivMgr->SetInteger
				(IFXSubdivisionManagerInterface::MaxRenderDepth, depth);
		}
	}


	return rc;
}

#define IFXSCALEFACTOR 0.20f

static void scaleError(F32* pfError)
{
	F32 error = *pfError;

	// Clamp it over [0..100]
	if (error > 100.0f)
		error = 100.0f;

	if (error < 0.0f)
		error = 0.0f;

	// Flip it:
	error = 100.0f - error;

	// Now scale it:
	error *= IFXSCALEFACTOR;

	// Now square it:
	error *= error;

	*pfError = error;
}


static void unscaleError(F32 *pfError)
{
	F32 error = *pfError;

	if (error == 1.0)
		error = 0.0f;

	if (error != 0.0f)
		error = (F32)sqrt (error);

	// unscale it:
	error /= IFXSCALEFACTOR;

	// flip it:
	error = 100.0f - error;

	if (error < 0.0f)
		error = 0.0f;

	if (error > 100.0f)
		error = 100.0f;

	*pfError = error;
}


IFXRESULT CIFXSubdivModifier::GetError (F32* pError)
{
	IFXRESULT rc = IFX_OK;

	if (!pError)
		rc = IFX_E_NOT_INITIALIZED;

	if(IFXSUCCESS(rc))
		*pError = 0.0f;

	// Is is the subdiv mgr not initialized?
	if (!m_pSubdivMgr || !m_pScreenSpaceMetric)
	{
		// If not, then grab parameter from the temporary param cache:
		F32 fError = m_InitData.fError;
		*pError = fError;
	}
	else if(IFXSUCCESS(rc))
	{
		F32 fError = 0;
		fError = m_pScreenSpaceMetric->GetPixelTolerance ();
		unscaleError (&fError);

		*pError = fError;
	}

	return rc;
}


IFXRESULT CIFXSubdivModifier::SetError (F32 error)
{
	IFXRESULT rc = IFX_OK;

	// Is is the subdiv mgr not initialized?
	if (!m_pSubdivMgr || !m_pScreenSpaceMetric)
	{
		// Cache the intended error:
		m_InitData.bActive  = true;
		m_InitData.fError = error;
	}
	else if(IFXSUCCESS(rc))
	{
		scaleError (&error);
		m_pScreenSpaceMetric->SetPixelTolerance (error);
	}

	return rc;
}


IFXRESULT CIFXSubdivModifier::GetAdaptive (BOOL* pbAdaptive)
{
	IFXRESULT rc = IFX_OK;

	// Is is the subdiv mgr not initialized?
	if (!m_pSubdivMgr)
	{
		// If not, then grab parameter from the temporary param cache:
		*pbAdaptive = m_InitData.bAdaptive;
	}
	else if(IFXSUCCESS(rc))
	{
		rc = m_pSubdivMgr->GetBool
			(IFXSubdivisionManagerInterface::Adaptive, pbAdaptive);
	}

	return rc;
}


IFXRESULT CIFXSubdivModifier::SetAdaptive (BOOL bAdaptive)
{
	IFXRESULT rc = IFX_OK;

	// Is is the subdiv mgr not initialized?
	if (!m_pSubdivMgr)
	{
		// Cache the intended adaptive:
		m_InitData.bActive    = true;
		m_InitData.bAdaptive  = bAdaptive;
	}
	else if(IFXSUCCESS(rc))
	{
		rc = m_pSubdivMgr->SetBool
			(IFXSubdivisionManagerInterface::Adaptive, bAdaptive);
	}

	return rc;
}



IFXRESULT CIFXSubdivModifier::InitializeSubdiv(IFXMeshGroup*    pMeshGroup,
											   IFXNeighborMesh* pNeighborMesh)
{
	IFXRESULT result = IFX_OK;

	if( pMeshGroup && pNeighborMesh )
	{
		if (m_pSubdivMgr)
		{
			// Delete the existing one and start over:
			IFXDELETE(m_pSubdivMgr);
		}

		m_pSubdivMgr = new IFXSubdivisionManager;

		if (m_pSubdivMgr)
		{
			// Pick some triangulation limits:
			result = m_pSubdivMgr->SetInteger (IFXSubdivisionManagerInterface::MaxTriangleAllocation, 400000);

			if( IFXSUCCESS(result) )
				result = m_pSubdivMgr->SetInteger (IFXSubdivisionManagerInterface::MaxVertexAllocation, 300000);

			// Set initial subdiv resolution to 1:
			if( IFXSUCCESS(result) )
				result = m_pSubdivMgr->SetInteger (IFXSubdivisionManagerInterface::MaxComputeDepth, 1);

			if( IFXSUCCESS(result) )
				result = m_pSubdivMgr->SetInteger (IFXSubdivisionManagerInterface::MaxRenderDepth,  1);

			// Create an adaptive subdivision metric and hand it to the subdivsion manager:
			if( IFXSUCCESS(result) )
			{
				if( m_pScreenSpaceMetric == NULL )
				{
					m_pScreenSpaceMetric = new IFXScreenSpaceMetric;

					if( m_pScreenSpaceMetric == NULL )
						result = IFX_E_OUT_OF_MEMORY;
				}
			}

			if( IFXSUCCESS(result) )
				result = m_pSubdivMgr->SetAdaptiveMetric (m_pScreenSpaceMetric);

			if (IFXSUCCESS(result) )
				result = m_pSubdivMgr->InitMesh(pMeshGroup, pNeighborMesh);

			if (IFXSUCCESS(result))
			{
				// check to see if we cached any init data. If so, then set the params
				if (m_InitData.bActive)
				{
					SetDepth(m_InitData.uDepth);
					SetTension(m_InitData.fTension);
					SetError(m_InitData.fError);
					SetAdaptive(m_InitData.bAdaptive);

					ResetInitData();
				}
			}
		}
		else
			result = IFX_E_OUT_OF_MEMORY;
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}


IFXRESULT CIFXSubdivModifier::DataChanged(IFXMeshGroup* pMeshGroup)
{
	IFXRESULT result = IFX_OK;

	if( pMeshGroup )
	{
		IFXMesh* pMesh      = NULL;
		U32      uNumMeshes = pMeshGroup->GetNumMeshes();
		U32      uVertexCount;
		U32      uFaceCount;
		U32      i;

		if( !m_puFaceChangeCounts && !m_puOtherChangeCounts )
		{
			m_puFaceChangeCounts  = new U32 [uNumMeshes];
			m_puOtherChangeCounts = new U32 [uNumMeshes];

			if( !m_puFaceChangeCounts || !m_puOtherChangeCounts )
				result = IFX_E_OUT_OF_MEMORY;
			else
			{
				for(i=0; i<uNumMeshes; i++)
				{
					m_puOtherChangeCounts[i] = (U32)-1;
					m_puFaceChangeCounts[i]  = (U32)-1;
				}
			}
		}

		// Reset the changed flag
		m_bOtherDataChanged = FALSE;
		m_bFaceDataChanged   = FALSE;

		for(i=0; i<uNumMeshes && IFXSUCCESS(result); i++)
		{
			pMeshGroup->GetMesh(i, pMesh);

			if( pMesh )
			{
				result = pMesh->GetVersionWord(IFX_MESH_FACE, uFaceCount);

				if( IFXSUCCESS(result) )
					result = pMesh->GetVersionWord(IFX_MESH_POSITION, uVertexCount);

				if( IFXSUCCESS(result) && uFaceCount != m_puFaceChangeCounts[i] )
				{
					m_puFaceChangeCounts[i] = uFaceCount;
					m_bFaceDataChanged      = TRUE;
				}

				if( IFXSUCCESS(result) && uVertexCount != m_puOtherChangeCounts[i] )
				{
					m_puOtherChangeCounts[i] = uVertexCount;
					m_bOtherDataChanged      = TRUE;
				}
			}

			IFXRELEASE(pMesh);
		}
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}
