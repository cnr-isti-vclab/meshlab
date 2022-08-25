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
@file  CIFXAuthorCLODGen.cpp

The implementation file of the CIFXAuthorCLODGen component. */

#include "CIFXAuthorCLODGen.h"


CIFXAuthorCLODGen::CIFXAuthorCLODGen()
{
	m_uRefCount   = 0;
	m_TotalPairs  = 0;
}


CIFXAuthorCLODGen::~CIFXAuthorCLODGen()
{
}



IFXRESULT IFXAPI_CALLTYPE CIFXAuthorCLODGen_Factory(IFXREFIID riid, void **ppv)
{
	IFXRESULT result;

	if ( ppv )
	{
		// Create the CIFXClassName component.
		CIFXAuthorCLODGen *pModifierChain = new CIFXAuthorCLODGen;

		if ( pModifierChain )
		{
			// Perform a temporary AddRef for our usage of the component.
			pModifierChain->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			result = pModifierChain->QueryInterface( riid, ppv );

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the QI fails, this will cause the
			// component to be destroyed.
			pModifierChain->Release();
		}
		else
			result = IFX_E_OUT_OF_MEMORY;
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}


// IFXUnknown interface support...
U32 CIFXAuthorCLODGen::AddRef(void)
{
	return ++m_uRefCount;
}


U32 CIFXAuthorCLODGen::Release(void)
{
	if ( !( --m_uRefCount ) )
	{
		delete this;
		return 0;
	}

	return m_uRefCount;
}


IFXRESULT CIFXAuthorCLODGen::QueryInterface( IFXREFIID interfaceId,
											void**    ppInterface )
{
	IFXRESULT result = IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXUnknown )
			*ppInterface = ( IFXUnknown* ) this;
		else if ( interfaceId == IID_IFXAuthorCLODGen )
			*ppInterface = ( IFXAuthorCLODGen* ) this;
		else
		{
			*ppInterface = NULL;
			result = IFX_E_UNSUPPORTED;
		}

		if ( IFXSUCCESS( result ) )
			AddRef();
	}
	else
	{
		result = IFX_E_INVALID_POINTER;
	}

	return result;
}


IFXRESULT CIFXAuthorCLODGen::Generate(const IFXAuthorCLODGenParam* in_pParams,
									  IFXProgressCallback* in_pProgress,
									  IFXAuthorCLODMesh* in_pMesh,
									  IFXAuthorCLODMesh** out_ppMesh,
									  IFXAuthorMeshMap** out_ppMapping)
{
	IFXRESULT result = IFX_OK;

	U32 *pBaseVert;
	if( in_pMesh->GetMaxMeshDesc()->NumBaseVertices )
	{
		in_pMesh->Lock();
		result = in_pMesh->GetBaseVertices( &pBaseVert );
		in_pMesh->Unlock();
	}
	else
	{
		pBaseVert = NULL;
	}

	if(IFXSUCCESS(result))
	{
		CLODGenerator CLODGen( in_pMesh->GetMaxMeshDesc()->NumBaseVertices, pBaseVert );
		CLODGen.SetInputMesh(in_pMesh);
		switch(in_pParams->NormalsMode)
		{
		case NoNormals:
			CLODGen.SetNormalModeNone();
			break;
		case NoUpdates:
			CLODGen.SetNormalModeNoUpdates();
			break;
		case TrackSurfaceChanges:
			CLODGen.SetNormalModeTrackSurfaceChanges(in_pParams->NormalsCreaseAngle);
			break;
		default:
			result = IFX_E_BAD_PARAM;

		}

		if(in_pParams->MergeWithin)
		{
			/// @todo:      CLODGen.EnableUnconnectedVertexMerge(in_pParams->MergeThresh);
		}

		if(IFXSUCCESS(result))
		{
			// Set parameters into CLODGen
			if(in_pProgress)
            {
                CLODGen.SetProgressCallBack(in_pProgress);
			}
		}

		if(IFXSUCCESS(result))
		{
			result = CLODGen.Generate();
		}

		if(IFXSUCCESS(result))
		{
			*out_ppMesh = CLODGen.GetOutputMesh();
			*out_ppMapping = CLODGen.GetMeshMap();
			(*out_ppMapping)->Release();
		}
	}

	m_TotalPairs  = 0;
	return result;
}
