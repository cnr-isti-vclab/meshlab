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
#include <list>
#include "Pair.h"

typedef std::list <Pair *> PairList;

class PairHeap
{
	private:

		PairList pairList;	
		PairList::iterator extIterator;

	public:

		Pair *remove ();
		Pair *remove (Pair *p);
		void insert (Pair *p);

		// For iteration:
		inline int size()	{ return pairList.size(); };
		inline int empty()	{ return pairList.empty(); };
		inline void reset()	{ extIterator = pairList.begin(); };
		inline Pair *next()	{ Pair *p = *extIterator; extIterator++; return p; };
		inline Pair *removeNext() 
		{	 
			PairList::iterator tmpIterator = extIterator;
			Pair *p = *extIterator; 
			extIterator++;
			pairList.erase (tmpIterator); 		
			return p; 
		};
		inline int end()	{ return (extIterator == pairList.end()); };
};
