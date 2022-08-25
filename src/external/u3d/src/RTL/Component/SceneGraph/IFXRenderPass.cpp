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
	@file  IFXRenderPass.cpp
*/

#include "IFXRenderContext.h"
#include "IFXNode.h"
#include "IFXSubject.h"
#include "IFXRenderPass.h"

IFXRenderPass::IFXRenderPass()
{
	m_nodeIndex = 0;
	m_nodeInstance = 0;
	m_nodeSet = FALSE;

	SetDefaults(0);
}

IFXRenderPass::~IFXRenderPass()
{
}

// IFXRenderPass
void IFXRenderPass::SetDefaults(U32 uRenderPass)
{
	m_nodeIndex = 0;
	m_nodeInstance = 0;
	m_nodeSet = FALSE;

	m_uRenderPass = uRenderPass;

	m_Clear.InitData();
	m_Fog.InitData();
	m_Stencil.InitData();

	m_bDepthTest = TRUE;
	m_eDepthFunc = IFX_LEQUAL;
	m_bDepthWrite = TRUE;

	m_bFogEnabled = FALSE;
	m_bStencilEnabled = FALSE;
		
	if(uRenderPass)
	{
		m_Clear.SetColorCleared(FALSE);
		m_Clear.SetDepthCleared(FALSE);
		m_Clear.SetStencilCleared(FALSE);
	}
}

IFXRESULT IFXRenderPass::SetRootNode(U32 nodeIndex, U32 nodeInstance)
{
	IFXRESULT rc = IFX_OK;

	m_nodeIndex = nodeIndex;
	m_nodeInstance = nodeInstance;
	m_nodeSet = TRUE;

	return rc;
}

void IFXRenderPass::ClearRootNode()
{
	m_nodeSet = FALSE;
}
