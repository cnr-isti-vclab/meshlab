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
	@file IFXBoneContext.h
*/

#ifndef IFXBONECONTEXT_H
#define IFXBONECONTEXT_H

/**
	Contains items that can be shared between similar characters.

	@note Minimally accessible, so manual ref-counting is fine.
*/
class IFXBoneContext
{
public:
	IFXBoneContext(void)
	{
		m_references = 1;
	}

	void IncReferences(void)
	{ 
		++m_references;
	}
	
	void DecReferences(void)
	{
		--m_references;
		if(!m_references)
			delete this;
	}

	IFXSkin& Skin(void)
	{ 
		return m_skin; 
	}

private:
	IFXSkin m_skin;
	U32 m_references;
};

#endif
