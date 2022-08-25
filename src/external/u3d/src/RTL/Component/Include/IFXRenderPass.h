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
@file  IFXRenderPass.h                                                      */

#ifndef IFX_RENDER_PASS_H
#define IFX_RENDER_PASS_H

#include "IFXEnums.h"
#include "IFXRenderClear.h"
#include "IFXRenderFog.h"
#include "IFXRenderStencil.h"
#include "IFXSmartPtr.h"
#include "IFXSpatialSetQuery.h"
#include "IFXCoreCIDs.h"
#include "IFXAutoRelease.h"

class IFXRenderPass
{
public:
	void IFXAPI SetDefaults(U32 uRenderPass);
	IFXRESULT IFXAPI SetRootNode(U32 nodeIndex, U32 nodeInstance);
	void IFXAPI ClearRootNode();

	U32 m_nodeIndex, m_nodeInstance;
	BOOL m_nodeSet;

	IFXRenderClear    m_Clear;

	BOOL        m_bFogEnabled;
	IFXRenderFog    m_Fog;

	BOOL        m_bColorBuffer; /// @todo: implement color buffer enabling

	BOOL        m_bDepthTest;
	BOOL        m_bDepthWrite;
	IFXenum       m_eDepthFunc;

	BOOL        m_bStencilEnabled;
	IFXRenderStencil  m_Stencil;

	U32         m_uRenderPass;
	IFXMatrix4x4    m_UnscaledWorldMatrixInverse;
	SPATIALINSTANCE_LIST   m_pOpaque[16];
	SPATIALINSTANCE_LIST   m_pTranslucent[8];

	IFXRenderPass();
	~IFXRenderPass();
};

#endif
