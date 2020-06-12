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

#include "IFXList.h"

/******************************************************************************
void IFXListNode::DecReferences(void)

******************************************************************************/
void IFXListNode::DecReferences(void)
{
	m_references--;
	if(!m_references)
	{
		if(m_heir)
			m_heir->DecReferences();

		//delete this;
#if IFXLIST_USEALLOCATOR==3
		IFXASSERT(m_pCoreList);
		m_pCoreList->Deallocate(this);
#else
		IFXCoreList::Deallocate(this);
#endif
//		printf("delete 0x%x\n",this);
	}
}
