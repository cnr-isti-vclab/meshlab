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

#ifndef __InsertionSort_h__
#define __InsertionSort_h__

#include "IFXList.h"

template <class T> class CDefaultLessThan;

template <class T> IFXRESULT InsertionSort( IFXList<T> *pInputList )
{
  CDefaultLessThan<T> LessThan;
  return InsertionSort(pInputList, LessThan);
}

template <class T, class U> IFXRESULT InsertionSort(IFXList<T> *pInputList, U &LessThan)
{
  IFXRESULT result = IFX_OK;

  if( pInputList )
  {
    IFXListContext context_a, context_b, context_c;

    T* pNode_a = NULL;
    T* pNode_b = NULL;
    T* pNode_c = NULL;

    // Impelementation of Insertion Sort.  List nodes are just reorderd.
    // No new memory is created.

    // context_a is the original head of the list.
    // context_c is always at the end of the list.
    // context_b is the current node used for comparison.

    // elements from the beginning of the list to context_a are
    // always sorted.
    pInputList->ToHead(context_a);
    if( (pNode_a=pInputList->PreIncrement(context_a)) != NULL )
    { //list has at least two elements

      pNode_c=pInputList->ToTail(context_c);
      if ( (pNode_a=pInputList->PostIncrement(context_a)) != NULL)
      { // more than two elements to sort

        // remove a node from the end of the list an place it
        // in order at the beginning of the list.  Do not search
        // past context_a.
        while( pNode_a != (pNode_c = pInputList->ToTail(context_c)) )
        {
          pNode_b=pInputList->ToHead(context_b);
          while(pNode_b != pNode_a && LessThan(*pNode_b, *pNode_c) )
          {
            pNode_b=pInputList->PreIncrement(context_b);
          }

          pInputList->MoveNodeBefore(context_c,context_b);
        }

        // place last element in the list
        pNode_b=pInputList->ToHead(context_b);
        while(pNode_b != pNode_a && LessThan(*pNode_b, *pNode_c))
        {
          pNode_b=pInputList->PreIncrement(context_b);
        }
        if(pNode_c != pNode_b)
        {
          pInputList->MoveNodeBefore(context_c,context_b);
        }
      }
      else
      { // exactly two elements to sort
        pNode_b = pInputList->ToHead(context_b);
        if(!LessThan(*pNode_b, *pNode_c))
        {
          pInputList->MoveNodeBefore(context_c,context_b);
        }
      }
    }
  }
  else
    result = IFX_E_INVALID_POINTER;

  return result;
}

template <class T> class CDefaultLessThan
{
public:

  CDefaultLessThan(){}
  BOOL operator()(const T &a, const T &b)
  {
    return a < b;
  }
};

#endif
