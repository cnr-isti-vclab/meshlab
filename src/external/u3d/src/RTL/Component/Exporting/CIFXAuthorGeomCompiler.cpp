//***************************************************************************
//
//  Copyright (c) 2001 - 2006 Intel Corporation
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
	@file	CIFXAuthorGeomCompiler.cpp 
	
			Implementation of the CIFXAuthorGeomCompiler class.
*/

#include "CIFXAuthorGeomCompiler.h"
#include "IFXCoreCIDs.h"
#include "IFXCheckX.h"
#include "IFXAutoRelease.h"
#include "IFXExportingCIDs.h"

class CIFXAuthorGeomCompilerProgress : public IFXProgressCallback
{
public:
	CIFXAuthorGeomCompilerProgress()
	{
		m_pGeomCompiler = NULL;
		m_pProgressCallback = NULL;
		m_LastStepProgress = 0.0f;
		m_CurStepRange = 0.0f;
		m_CurMaxStep = 0.0f;
		m_CurStep = 0.0f;
	};

	void Initialize(CIFXAuthorGeomCompiler* pInGeomCompiler)
	{
		m_pGeomCompiler = pInGeomCompiler;
		if( m_pGeomCompiler->m_pParams->pProgressCallback)
		{
			m_pProgressCallback = m_pGeomCompiler->m_pParams->pProgressCallback;
			m_pProgressCallback->InitializeProgress(1.0f);
		}
		m_LastStepProgress = 0.0f;
		m_CurStepRange = 0.0f;
		m_CurMaxStep = 0.0f;
		m_CurStep = 0.0f;
	};

	void NextStep(F32 in_Range)
	{
		IFXASSERT(in_Range <= 1.0f);
		m_LastStepProgress = m_LastStepProgress + m_CurStepRange;
		m_CurStepRange = in_Range;
	};

	void IFXAPI	InitializeProgress( F32 in_Max )
	{
		m_CurMaxStep = in_Max;
	};

	BOOL IFXAPI	UpdateProgress( F32 pInrogress)
	{
		IFXASSERT(pInrogress <= m_CurMaxStep);
		if(m_pProgressCallback)
		{
			return m_pProgressCallback->UpdateProgress(m_LastStepProgress +
				((pInrogress/m_CurMaxStep) * m_CurStepRange));
		}
		return TRUE;
	};

private:
	CIFXAuthorGeomCompiler* m_pGeomCompiler;
	IFXProgressCallback* m_pProgressCallback;

	F32 m_LastStepProgress;
	F32 m_CurStepRange;
	F32 m_CurMaxStep;
	F32 m_CurStep;
};

//---------------------------------------------------------------------------
//  CIFXAuthorGeomCompiler_Factory
//
//  This is the CIFXAuthorGeomCompiler component factory function.  The
//  CIFXAuthorGeomCompiler component can be instaniated multiple times.
//---------------------------------------------------------------------------

IFXRESULT IFXAPI_CALLTYPE CIFXAuthorGeomCompiler_Factory( IFXREFIID interfaceId,
										 void**   ppInterface )
{
	IFXRESULT result;

	if ( ppInterface )
	{
		// Create the CIFXAuthorMeshGroup component.
		CIFXAuthorGeomCompiler  *pComponent = new CIFXAuthorGeomCompiler;

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


// Construction/Destruction

CIFXAuthorGeomCompiler::CIFXAuthorGeomCompiler()
{
	m_pSceneGraph = NULL;
	m_pParams = NULL;
	m_refCount = 0;
}

CIFXAuthorGeomCompiler::~CIFXAuthorGeomCompiler()
{
	IFXRELEASE(m_pSceneGraph);
}


//---------------------------------------------------------------------------
//  CIFXAuthorGeomCompiler::AddRef
//
//  This method increments the reference count for an interface on a
//  component.  It should be called for every new copy of a pointer to an
//  interface on a given component.  It returns a U32 that contains a value
//  from 1 to 2^32 - 1 that defines the new reference count.  The return
//  value should only be used for debugging purposes.
//---------------------------------------------------------------------------

U32 CIFXAuthorGeomCompiler::AddRef()
{
	return ++m_refCount;
}

//---------------------------------------------------------------------------
//  CIFXAuthorGeomCompiler::Release
//
//  This method decrements the reference count for the calling interface on a
//  component.  It returns a U32 that contains a value from 1 to 2^32 - 1
//  that defines the new reference count.  The return value should only be
//  used for debugging purposes.  If the reference count on a component falls
//  to zero, the component is destroyed.
//---------------------------------------------------------------------------

U32 CIFXAuthorGeomCompiler::Release()
{
	if ( !( --m_refCount ) )
	{

		delete this;
		// This second return point is used so that the deleted object's
		// reference count isn't referenced after the memory is released.
		return 0;
	}

	return m_refCount;
}

//---------------------------------------------------------------------------
//  CIFXAuthorGeomCompiler::QueryInterface
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

IFXRESULT CIFXAuthorGeomCompiler::QueryInterface( IFXREFIID interfaceId,
												 void** ppInterface )
{
	IFXRESULT result = IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXAuthorGeomCompiler )
		{
			*ppInterface = ( IFXAuthorGeomCompiler* ) this;
			this->AddRef();
		}
		else if ( interfaceId == IID_IFXUnknown )
		{
			*ppInterface = ( IFXUnknown* ) this;
			this->AddRef();
		}
		else
		{
			*ppInterface = NULL;
			result = IFX_E_UNSUPPORTED;
		}
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}


//---------------------------------------------------------
// IFXAuthorGeomCompiler Iterface Implementation
//---------------------------------------------------------

IFXRESULT CIFXAuthorGeomCompiler::SetSceneGraph(IFXSceneGraph* pInSceneGraph)
{
	IFXRESULT result = IFX_OK;

	if(IFXSUCCESS(result))
	{
		if (m_pSceneGraph)
			IFXRELEASE(m_pSceneGraph);

		m_pSceneGraph = pInSceneGraph;
		m_pSceneGraph->AddRef();
	}

	return result;

}

static IFXAuthorGeomCompilerParams s_Params;


IFXRESULT CIFXAuthorGeomCompiler::Compile(IFXString& rName,
										  IFXAuthorMesh* pInMesh,
										  IFXAuthorCLODResource** ppOutResource,
										  BOOL forceCompress,
										  IFXAuthorGeomCompilerParams* pInParams)
{
	IFXRESULT			result = IFX_OK;
	IFXAuthorMesh*		pScrubbedMesh = NULL;
	IFXAuthorCLODMesh*	pCLODMesh = NULL;
	IFXAuthorCLODResource*	pResource = NULL;
	CIFXAuthorGeomCompilerProgress Progress;
	IFXAuthorMeshMap*	pMasterAuthorMeshMap = NULL;
	IFXMeshMap*			pMeshMap = NULL;

	m_pParams = pInParams;

	if (!m_pParams)
	{
		m_pParams = &s_Params;
	}

	// Build an IFXMeshMap and initialize it with the data from the
	// Master IFXAuthorMeshMap.
	result = IFXCreateComponent(
						CID_IFXMeshMap, IID_IFXMeshMap, (void**)&pMeshMap);

	if(pMeshMap && IFXSUCCESS(result))
	{
		result = pMeshMap->Allocate(pInMesh);
	}

	Progress.Initialize(this);

	// Create the mesh mapping object
	if(IFXSUCCESS(result))
	{
		result = IFXCreateComponent(
						CID_IFXAuthorMeshMap, IID_IFXAuthorMeshMap, 
						(void**)&pMasterAuthorMeshMap);
	}

	if(IFXSUCCESS(result))
	{
		result = pMasterAuthorMeshMap->Allocate(pInMesh);
	}

	// Scrub the mesh
	if(IFXSUCCESS(result))
	{
		if(m_pParams->bScrub)
		{
			IFXAuthorMeshScrub* pScrubber = NULL;
			IFXAuthorMeshMap* pScrubMap = NULL;

			// Create the mesh scrubber
			if(IFXSUCCESS(result))
			{
				result = IFXCreateComponent(
								CID_IFXAuthorMeshScrub, IID_IFXAuthorMeshScrub, 
								(void**)&pScrubber);
			}

			// Scrub the mesh
			if(IFXSUCCESS(result))
			{
				Progress.NextStep(0.25f);
				result = pScrubber->Scrub(
										pInMesh, &pScrubbedMesh,
										&pScrubMap, &(m_pParams->ScrubParams),
										&Progress);
			}

			// Update the master mesh map with the reordering that
			// happened as a result of scrubbing.
			if (pScrubMap && IFXSUCCESS(result))
			{
				result = pMasterAuthorMeshMap->Concatenate(pScrubMap);
			}

			IFXRELEASE(pScrubber);
			IFXRELEASE(pScrubMap);
		}
		else
		{
			result = pInMesh->Copy(IID_IFXAuthorMesh, (void**)&pScrubbedMesh);
		}
	}

	// CLOD the Mesh
	if(IFXSUCCESS(result))
	{
		if(m_pParams->bCLOD)
		{
			IFXAuthorCLODGen* pCLODGen = NULL;
			IFXAuthorMeshMap* pCLODMap = NULL;

			// Create the CLOD generator
			result = IFXCreateComponent(
							CID_IFXAuthorCLODGen, IID_IFXAuthorCLODGen, 
							(void**)&pCLODGen);

			// Now generate the CLOD records
			if(IFXSUCCESS(result))
			{
				IFXAuthorCLODMesh* pScrubCLODMesh = NULL;

				result = pScrubbedMesh->QueryInterface(
											IID_IFXAuthorCLODMesh,
											(void**)&pScrubCLODMesh);
				Progress.NextStep(0.65f);

				if( IFXSUCCESS( result ) )
				{
					pScrubCLODMesh->SetMaxResolution( 
										pScrubCLODMesh->GetFinalMaxResolution() );

					result = pCLODGen->Generate(
										&m_pParams->CLODParams, &Progress,
										pScrubCLODMesh, &pCLODMesh, &pCLODMap);

					IFXRELEASE(pScrubCLODMesh);
				}
			}

			// Update the master mesh map with the reordering that
			// happened as a result of CLOD.
			if (pCLODMap && IFXSUCCESS(result))
			{
				result = pMasterAuthorMeshMap->Concatenate(pCLODMap);
			}

			IFXRELEASE(pCLODMap);
			IFXRELEASE(pCLODGen);
		}
		else
		{
			// Copy the mesh so that the user does not inadvertantly overwrite
			// the data
			//
			IFXAuthorMesh* pMesh = NULL;
			result = pScrubbedMesh->Copy(IID_IFXAuthorMesh,(void**)&pMesh);
			if(IFXSUCCESS(result))
			{
				IFXRELEASE(pCLODMesh);
				result = pMesh->QueryInterface(	
									IID_IFXAuthorCLODMesh, (void**)&pCLODMesh);
			}
			IFXRELEASE(pMesh);

			if(IFXSUCCESS(result))
			{
				U32 finMaxRes = pCLODMesh->GetFinalMaxResolution();
				pCLODMesh->SetMaxResolution( finMaxRes );
				pCLODMesh->SetMinResolution( finMaxRes );
			}
		}
	}

	// Create the Resource
	if(IFXSUCCESS(result))
	{
		result = IFXCreateComponent(
						CID_IFXAuthorCLODResource, IID_IFXAuthorCLODResource, 
						(void**)&pResource);
	}

	// initialize the resource
	if(IFXSUCCESS(result))
	{
		result = pResource->SetSceneGraph(m_pSceneGraph);
	}

	// Set compression parameters for mesh
	if(IFXSUCCESS(result))
	{
		if(m_pParams->bCompressSettings)
		{
			try
			{
				if(m_pParams->CompressParams.bSetMinimumResolution)
				{
					U32 uTemp = 
						pCLODMesh->SetMinResolution(
										m_pParams->CompressParams.uMinimumResolution);
					// minResolution if 1 or 2 is illegal and gets truncated down to zero
					if (m_pParams->CompressParams.uMinimumResolution < 3)
					{
						if (uTemp != 0)
						{
							IFXCHECKX(IFX_E_UNDEFINED); /// @todo Define error
						}
					}
					else
					{
						/*
						if (uTemp != m_pParams->CompressParams.uMinimumResolution)
						{
						IFXCHECKX(IFX_E_UNDEFINED); /// @todo Define error
						}
						*/
						;
					}
				}
			}
			catch(IFXException e)
			{
				result = e.GetIFXResult();
			}
		}
	}

	// Give the mesh to the resource
	if(IFXSUCCESS(result))
	{
		result = pResource->SetAuthorMesh(pCLODMesh);
	}

	if (IFXSUCCESS(result))
	{
		result = pMeshMap->PopulateMeshMap(pMasterAuthorMeshMap);
	}

	// Put this into the resource.
	if(IFXSUCCESS(result))
	{
		result = pResource->SetAuthorMeshMap(pMeshMap);
	}


	// Set compression parameters for resource
	if(IFXSUCCESS(result))
	{
		if(m_pParams->bCompressSettings)
		{
			try
			{
				if(m_pParams->CompressParams.bSetDefaultQuality)
				{
					pResource->SetQualityFactorX(
									m_pParams->CompressParams.uDefaultQuality, 
									IFXMarkerX::ALL);
				}
				if(m_pParams->CompressParams.bSetPositionQuality)
				{
					pResource->SetQualityFactorX(
									m_pParams->CompressParams.uPositionQuality, 
									IFXMarkerX::POSITION_QUALITY);
				}
				if(m_pParams->CompressParams.bSetTexCoordQuality)
				{
					pResource->SetQualityFactorX(
									m_pParams->CompressParams.uTexCoordQuality, 
									IFXMarkerX::TEXCOORD_QUALITY);
				}
				if(m_pParams->CompressParams.bSetNormalQuality)
				{
					pResource->SetQualityFactorX(
									m_pParams->CompressParams.uNormalQuality, 
									IFXMarkerX::NORMAL_QUALITY );
				}
				if(m_pParams->CompressParams.bSetDiffuseQuality)
				{
					pResource->SetQualityFactorX(
									m_pParams->CompressParams.uDiffuseQuality, 
									IFXMarkerX::DIFFUSE_QUALITY );
				}
				if(m_pParams->CompressParams.bSetSpecularQuality)
				{
					pResource->SetQualityFactorX(
									m_pParams->CompressParams.uSpecularQuality, 
									IFXMarkerX::SPECULAR_QUALITY );
				}

				if(m_pParams->CompressParams.bSetNormalCreaseParameter)
				{
					pResource->SetNormalCreaseParameter(
									m_pParams->CompressParams.fNormalCreaseParameter);
				}
				if(m_pParams->CompressParams.bSetNormalUpdateParameter)
				{
					pResource->SetNormalUpdateParameter(
									m_pParams->CompressParams.fNormalUpdateParameter);
				}
				if(m_pParams->CompressParams.bSetNormalTolerance)
				{
					pResource->SetNormalTolerance(
									m_pParams->CompressParams.fNormalTolerance);
				}

				if(m_pParams->CompressParams.bSetStreamingPriority)
				{
					pResource->SetPriority(
									m_pParams->CompressParams.uStreamingPriority);
				}

				if(m_pParams->CompressParams.bSetExcludeNormals)
				{
					pResource->SetExcludeNormals(
									m_pParams->CompressParams.bExcludeNormals);
				}
			}
			catch(IFXException e)
			{
				result = e.GetIFXResult();
			}
		}
	}


	// FORCECOMPRESS and Compile

	if(IFXSUCCESS(result))
	{
		Progress.NextStep(0.10f);
		//    result = pResource->ForceCompress(pCLODMesh, void*);
	}

	// Force the compression of the author CLOD resource
	if( IFXSUCCESS(result) && forceCompress )
	{
		try
		{
			// Create a local data block queue
			IFXDECLARELOCAL(IFXDataBlockQueueX,pDataBlockQueueX);
			IFXCHECKX(IFXCreateComponent(
							CID_IFXDataBlockQueueX, 
							IID_IFXDataBlockQueueX,
							(void**)&pDataBlockQueueX));

			// Get the encoder from the resource
			IFXDECLARELOCAL(IFXEncoderX,pEncoderX);
			pResource->GetEncoderX(pEncoderX);

			// Encode: This will fill the data block queue with blocks representing
			// the compressed author CLOD resource
			pEncoderX->EncodeX(rName,*pDataBlockQueueX);

			// Give the data block queue back to the resource
			IFXCHECKX(pResource->BuildDataBlockQueue());
			IFXDECLARELOCAL(IFXDataBlockQueueX,pResourceDataBlockQueueX);
			pResource->GetDataBlockQueueX(pResourceDataBlockQueueX);

			BOOL bDone = FALSE;
			while(!bDone)
			{
				IFXDECLARELOCAL(IFXDataBlockX,pDataBlockX);
				pDataBlockQueueX->GetNextBlockX(pDataBlockX,bDone);
				if(pDataBlockX) {
					pResourceDataBlockQueueX->AppendBlockX(*pDataBlockX);
				}
			}

		}
		catch(IFXException e)
		{
			result = e.GetIFXResult();
		}
	}

	// All done - return results
	if(IFXSUCCESS(result))
	{
		*ppOutResource = pResource;
	}
	else
	{
		IFXRELEASE(pResource);
	}

	IFXRELEASE(pMasterAuthorMeshMap);
	IFXRELEASE(pMeshMap);
	IFXRELEASE(pScrubbedMesh);
	IFXRELEASE(pCLODMesh);

	return result;

}

/// @todo	Unless we want to separate the CLOD and compress pass (good idea!!!)
IFXRESULT CIFXAuthorGeomCompiler::Recompile(IFXAuthorCLODResource* pInResource,
											IFXAuthorMeshMap* pInMeshMap,
											IFXAuthorMeshMap** ppOutMeshMap)
{
	IFXRESULT result = IFX_OK;

	if(IFXSUCCESS(result))
	{
	}

	return result;
}
