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
#ifndef _CIFXOBSERVERSTATE_H_
#define _CIFXOBSERVERSTATE_H_

#include "IFXObserver.h"
#include "IFXSubject.h"
#include "CRedBlackTree.h"

class CompareObservers
{
public :
	I32 operator()(const SIFXObserverRequest* arg1, const SIFXObserverRequest* arg2 ) const
	{
		I32 result = arg1->pObserver == arg2->pObserver ? 0:1;
		if(result)
			result = arg1->pObserver < arg2->pObserver ? -1:1;
		return result;
	}
	I32 operator()(const SIFXObserverRequest* arg1,const IFXObserver* arg2 ) const
	{
		I32 result = arg1->pObserver == arg2 ? 0:1;
		if(result)
			result = arg1->pObserver < arg2 ? -1:1;
		return result;
	}
};



typedef CRedBlackTree<SIFXObserverRequest*, IFXObserver*, CompareObservers> OBSERVERSTATETREE; 

#endif
