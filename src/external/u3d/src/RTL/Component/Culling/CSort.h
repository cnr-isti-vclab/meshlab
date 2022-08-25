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
	@file	CSort.h

			This file contains a list of some common sort methods
			1. Insertion Sort.
			2. Selecting the best N.
			3. Heap Sort
*/

#ifndef _CSORT_H_
#define _CSORT_H_

#include "IFXDataTypes.h"

template<class K>
class DEFAULT_GREATER
{
public:
  BOOL operator()(const K& arg1, const K& arg2)
    { return arg1 > arg2; };
};

template<class K>
class DEFAULT_LESSER
{
  public:
    BOOL operator()(const K& arg1, const K& arg2)
    { return arg1 < arg2; };
};

template<class K>
class DEFAULT_EQUAL
{
  public:
    BOOL operator()(const K& arg1, const K& arg2)
    { return arg1 == arg2; };
};

// Select Best N

template <class K>
void SWAP(K &x,K &y)
{
  K tmp = x;
  x=y;
  y=tmp;

}


template<class KEY, class DATA, class CMP>
void Partition(KEY* keyArray,DATA* dataArray,CMP compare, int m, int &p)
{

  int i=m;
  KEY v =  keyArray[m];
  DATA data = dataArray[m];
  for(;;)
  {
    do{i++;}while(compare(keyArray[i],v));
    do{p--;}while(compare(v,keyArray[p]));
    if(i<p )
    {
      SWAP(keyArray[i],keyArray[p]);
      SWAP(dataArray[i],dataArray[p]);
    }else
      break;

  }
    keyArray[m] = keyArray[p];
    keyArray[p] = v;
    dataArray[m] = dataArray[p];
    dataArray[p] = data;
}

template<class KEY, class DATA>
void SelectBestN(KEY* keyArray, DATA* dataArray, int n, int k)
{
  DEFAULT_LESSER<KEY> compare;
  SelectBestN(keyArray,dataArray,compare,n,k);
}

template<class KEY, class DATA, class CMP>
float SelectBestN(KEY* keyArray, DATA * dataArray, CMP compare, int n, int k)
{
  int m=0,r=n,j=n;
  if(n <=k)
    return 0;
  while(1)//n+1>=r && r>=k && k>=m && m>=0)
  {
      j=r;
      Partition(keyArray,dataArray,compare,m,j);
      if(k==j)
        break;
      else if(k<j)
        r=j;
      else
        m = j+1;
  }
  return 0;
}
// Heap Sort
template <class KEY,class DATA>
void HeapSort(KEY* key, DATA* data,unsigned long numElements)
{
  KEY keyElement;
  DATA dataElement;

  unsigned long i=0,indexSelectionPhase=0,j=0,indexCreationPhase=0;

  if (numElements < 2)
    return;

  indexCreationPhase  = (numElements >> 1);
  indexSelectionPhase =  numElements-1;

  //The CreationPhase index will be decremented from its initial value down to 0 during the heap
  //creation phase. Once it reaches 0, the SelectionPhase index will be decremented from its initial value
  //down to 0 during the heap selection phase.

  for (;;)
  {
    if(indexCreationPhase > 0)
    {
      keyElement  = key[--indexCreationPhase];
      dataElement = data[indexCreationPhase];
    }
    else
    {
      keyElement=key[indexSelectionPhase];
      key[indexSelectionPhase]=key[0];

      dataElement = data[indexSelectionPhase];
      data[indexSelectionPhase]= data[0];

      if (--indexSelectionPhase == 0)
      {
        key[0]  = keyElement;
        data[0] = dataElement;
        break;
      }
    }

    i=indexCreationPhase;
    // here setup tosift down element keyElement to its proper level.
    j=indexCreationPhase<<1;
    while (j <= indexSelectionPhase)
    {
      if ((j < indexSelectionPhase )&& (key[j] <  key[j+1]))
        j++; //Compare to the better underling
      if (keyElement < key[j])
      {
        key[i]=key[j];  //Demote keyElement.
        data[i] = data[j];
        i=j;
        j <<= 1;
        j++;
      }
      else
        break; //Found keyElement's level. Terminate the sift-down.
    }

    key[i]  = keyElement; // Put keyElement into its slot.
    data[i] = dataElement;
  }
}


// Insertion Sort

template <class KEY,class DATA>
void InsertionSort(KEY* key,DATA* data, int n)
{
  DEFAULT_GREATER<KEY> compare;
    InsertionSort(key,data,compare,n);
}


template <class KEY,class DATA, class CMP >
void InsertionSort(KEY* key,DATA* data, CMP compare, int n)
{
  int i,j;
  //int n = 49;
  float keyElement;
  float dataElement;
  for (j=1;j<n;j++)
  {
    //Pick out each element in turn.
    keyElement  = key[j];
    dataElement = data[j];
    i=j-1;
    while (i >= 0 && compare(key[i], keyElement))
    {
      // Look for the place to insert it.
      key[i+1]=key[i];
      data[i+1] = data[i];
      i--;
    }
    key[i+1]=keyElement; // Insert it.
    data[i+1]=dataElement;
  }
}

#endif
