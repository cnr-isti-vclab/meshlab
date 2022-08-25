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
/*
@file  cIFXResourceClient.cpp                                                
*/

#include "CIFXResourceClient.h"
#include "IFXPalette.h"
#include "IFXModifierChain.h"
#include "IFXSceneGraph.h"
#include "IFXModel.h"


CIFXResourceClient::CIFXResourceClient()
{
	m_uResourceIndex = 0;
}


CIFXResourceClient::~CIFXResourceClient()
{
}


IFXRESULT CIFXResourceClient::SetResourceIndex( U32 uInEntryIndex )
{
	IFXSceneGraph* pSceneGraph = NULL;
	IFXPalette*    pPalette    = NULL;
	
	IFXRESULT result = GetSceneGraph(&pSceneGraph);

	if( IFXSUCCESS(result) )
	{
		result = pSceneGraph->GetPalette( 
					(IFXSceneGraph::EIFXPalette)GetResourcePalette(), &pPalette );
	}

	// Set the observer
	if( IFXSUCCESS(result) )
	{
		IFXModel* pModel = NULL;
		result = this->QueryInterface(IID_IFXModel, (void**)&pModel);

		if( IFXSUCCESS(result) )
			result = pPalette->SetResourceObserver(m_uResourceIndex, uInEntryIndex, pModel);

		IFXRELEASE(pModel);
	}

	if( IFXSUCCESS(result) )
		m_uResourceIndex = uInEntryIndex;
	
	IFXRELEASE(pSceneGraph);
	IFXRELEASE(pPalette);

	return result;
}
