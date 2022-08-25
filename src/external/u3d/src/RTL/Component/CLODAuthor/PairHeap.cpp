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
#include "PairHeap.h"

void PairHeap::insert (Pair *pair)
{
	// We have to maintain a sorted list:
	Pair *p;	
	PairList::iterator listPtr;
	listPtr = pairList.begin();
	while ((listPtr != pairList.end()) 
		&& (p = *listPtr) 
		&& (p->getCost() < pair->getCost()))
	{
		listPtr++;
	}

	pairList.insert (listPtr, pair);
}


Pair *PairHeap::remove ()
{
	if (pairList.empty())
		return (NULL);

	Pair *p = *(pairList.begin());
	pairList.erase (pairList.begin());

	return p;
}


Pair *PairHeap::remove (Pair *pair)
{
	if (pairList.empty())
		return NULL;

	PairList::iterator listPtr;
	listPtr = pairList.begin();
	Pair *p = NULL;
	
	while ((listPtr != pairList.end()) && (p = *listPtr) && (p != pair))
	{
		listPtr++;
	}

	if (listPtr == pairList.end())
		return NULL;

	pairList.erase (listPtr);

	return p;
}
