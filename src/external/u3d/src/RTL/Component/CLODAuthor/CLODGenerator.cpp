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
//***************************************************************************
#include "CLODGenerator.h"
#include "VertexPairContractor.h"
#include "IFXCoreCIDs.h"


CLODGenerator::CLODGenerator ( U32 baseVert, U32* pBaseVert )
{
	m_pVPC = NULL;
	m_GenerateRunning = FALSE;
	m_GenerateCompletedOK = FALSE;

	// default values for parameters
	m_Params.mergeThresh = -1.0f; // don't pair any unconnected vertices.
	m_Params.mergeWithin  = FALSE;  // no vertex merging within object
	m_Params.progressCallback = NULL;  // no callback
	m_Params.progressFrequency = 1000; // do progress callback every 1000 edge removals.
	m_Params.numBaseVertices = baseVert;
	m_Params.baseVertices = pBaseVert;
	m_Params.normalsMode = TrackSurfaceChanges;
    // In TrackSurfaceChanges mode form a hard edge when the angle between face normals is >= to this
	m_Params.normalsCreaseAngle = 75.0f;
	m_Params.meshDamage = NULL;

	m_Params.pMesh = NULL;
	m_Params.pMeshMap = NULL;
	m_Params.pProgressCallBackUserData = NULL;
	m_Params.maxNormalChange = 89.9f;

	m_pProgress	= NULL;
}

CLODGenerator::~CLODGenerator ()
{
	IFXDELETE(m_pVPC);
}

IFXRESULT CLODGenerator::SetNormalModeTrackSurfaceChanges(F32 creaseAngle = 75.0f)
{
	m_Params.normalsMode = TrackSurfaceChanges;
	m_Params.normalsCreaseAngle = creaseAngle;
	return IFX_OK;
}

IFXRESULT CLODGenerator::SetNormalModeNoUpdates()
{
	m_Params.normalsMode = NoUpdates;
	return IFX_OK;
}

IFXRESULT CLODGenerator::SetNormalModeNone()
{
	m_Params.normalsMode = None;
	return IFX_OK;
}

IFXRESULT CLODGenerator::SetMaxNormalChange(float degrees)
{
	m_Params.maxNormalChange = degrees;
	return IFX_OK;
}
IFXRESULT CLODGenerator::SetInputMesh(IFXAuthorMesh *m)
{
	IFXRESULT result = IFX_OK;

	if(m_GenerateRunning)   // don't give new input while CLODGen is in progress.
		return result = IFX_E_CANNOT_CHANGE;

	if (m == NULL)
		return result = IFX_E_INVALID_POINTER;

	m_Params.pMesh = (IFXAuthorCLODMesh*) m;
	if(m->GetMaxMeshDesc()->NumNormals == 0)
		m_Params.normalsMode = None;
	m_GenerateCompletedOK = FALSE;
	return result;
}

IFXAuthorCLODMesh* CLODGenerator::GetOutputMesh()
{
	if(m_GenerateCompletedOK)
	{
		m_Params.pMesh->AddRef();
		return m_Params.pMesh;
	}
	else
		return NULL;
}

IFXAuthorMeshMap* CLODGenerator::GetMeshMap()
{
	if(m_GenerateCompletedOK)
	{
		m_Params.pMeshMap->AddRef();
		return m_Params.pMeshMap;
	}
	else
		return NULL;
}

IFXRESULT CLODGenerator::Generate()
{
	IFXRESULT result = IFX_OK;
	BOOL generateOK;

	if(m_GenerateRunning)   // don't start another while CLODGen is in progress.
		return result = IFX_E_UNSUPPORTED;

	if(m_Params.pMesh == NULL)
		return result = IFX_OK;  // nothing to do

	m_GenerateRunning = TRUE;
	m_GenerateCompletedOK = FALSE;


	m_Params.pMesh->Lock();

	result = IFXCreateComponent(CID_IFXAuthorMeshMap,
		IID_IFXAuthorMeshMap, (void**)&m_Params.pMeshMap);

	if( IFXSUCCESS( result ) )
	{
		result = m_Params.pMeshMap->Allocate(m_Params.pMesh);
	}

	if( IFXSUCCESS( result ) )
	{
		m_pVPC = new VertexPairContractor();
		if( !m_pVPC )
			result = IFX_E_OUT_OF_MEMORY;
	}

	if( IFXSUCCESS( result ) )
	{
		BOOL res;
		result = m_pVPC->init(&m_Params, res);
	}

	if( IFXSUCCESS( result ) )
	{
		generateOK = m_pVPC->contractAll( m_pProgress );

		if(generateOK && m_pVPC)
		{
			delete (VertexPairContractor *) m_pVPC;
			m_pVPC = NULL;
			m_GenerateRunning = FALSE;
			m_GenerateCompletedOK = TRUE;
		}
		else
		{
			m_GenerateRunning = FALSE;
			m_GenerateCompletedOK = FALSE;
		}
	}

	return result;
}
