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
	@file IFXCoreNode.cpp
*/

#include "IFXBoneNodeList.h"

IFXCoreNode::IFXCoreNode(IFXCoreNode *pCloneOf)
{
	m_pChildren = new IFXBoneNodeList();
	m_pEffections = new IFXBoneNodeList();

	Reset();

	if(pCloneOf)
	{
		m_share=pCloneOf->m_share;
		++m_share->m_count;

		m_displacement=pCloneOf->m_displacement;
		m_rotation=pCloneOf->m_rotation;
		m_scale=pCloneOf->m_scale;
	}
	else
	{
		m_share = new IFXCoreNodeShare();
		m_share->m_count = 1;
	}
}

IFXCoreNode::~IFXCoreNode(void)
{
	if(!(--m_share->m_count))
	{
		IFXDELETE(m_share);
	}

	IFXDELETE(m_pEffections);
	IFXDELETE(m_pChildren);
}

void IFXCoreNode::Reset(void)
{
	SetParent(NULL);
	m_displacement.Reset();
	m_rotation.MakeIdentity();
	m_scale.Set(1.0f,1.0f,1.0f);
	m_storedTransform.Reset();
	Effections().Clear();
}

void IFXCoreNode::AppendChild(IFXBoneNode *bonenode)
{
	Children().Append(bonenode);
	bonenode->SetParent(this);
}



void IFXCoreNode::RemoveChild(IFXBoneNode *bonenode)
{
	Children().Remove(bonenode);
	bonenode->SetParent(NULL);
}
