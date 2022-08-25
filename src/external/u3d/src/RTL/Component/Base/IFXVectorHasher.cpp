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
	@file	IFXVectorHasher.cpp

			This module defines the IFXVectorHasher interface.
*/

//***************************************************************************
//	Includes
//***************************************************************************

#include <float.h>
#include <memory.h>

#include "IFXVectorHasher.h"


/// 0 == less memory, 1 == better performance
#define SPEED_KNOB	.8f

/// How much to compress/expand normalized space
#define SPACE_SCALING	.99999f

/** Allowable difference in vertex position and still be considered coincident

Smallest value in IEEE 4-byte float is
   1.0 * 2**-126 = FLT_MIN

We choose a value of 1.0 * 2**-125 or 2*FLT_MIN */
#define EQUAL_TOLERANCE		2*FLT_MIN   


IFXVectorHasher::IFXVectorHasher()
{
	m_dim = 0;
	m_dimSquared = 0;
	m_dimCubed = 0;
	m_ppBins = NULL;
}

IFXVectorHasher::~IFXVectorHasher()
{
	IFXVectorHasherBinNode** ppCurrBinList;
	IFXVectorHasherBinNode** ppEndBinList;
	IFXVectorHasherBinNode* pBinNode;
	IFXVectorHasherBinNode* pTmpBinNode;
	IFXVectorHasherVectorNode* pCurrNode;
	IFXVectorHasherVectorNode* pTmpNode;

	if (m_ppBins)
	{
		ppCurrBinList = m_ppBins;
		ppEndBinList = m_ppBins + m_dimCubed;
		while (ppCurrBinList < ppEndBinList)
		{
			pBinNode = *ppCurrBinList;
			while (pBinNode)
			{
				pCurrNode = pBinNode->pFront;
				while (pCurrNode)
				{
					pTmpNode = pCurrNode;
					pCurrNode = pCurrNode->pNext;
					delete pTmpNode;
				}
				pTmpBinNode = pBinNode;
				pBinNode = pBinNode->pNext;
				delete pTmpBinNode;
			}
			++ppCurrBinList;
		}
		delete m_ppBins;
	}
}

IFXRESULT IFXVectorHasher::Initialize(U32 numVectors,
									  IFXVector3* pMin,
									  IFXVector3* pMax)
{
	IFXRESULT result = IFX_OK; 

	if (numVectors != 0)
	{
		// Determine how large to make the 3D hash table
		F32 cubeRoot = powf((F32)(numVectors), (F32)(1.0/3.0));
		m_dim = U32(cubeRoot + SPEED_KNOB);
		m_dimSquared = m_dim * m_dim;
		m_dimCubed = m_dimSquared * m_dim;

		IFXVector3 delta;
		delta.Subtract(*pMax, *pMin);

		// Check for divide by zero
		if(delta.X() < 1.0f)
			delta.X() = 1.0f;
		if(delta.Y() < 1.0f)
			delta.Y() = 1.0f;
		if(delta.Z() < 1.0f)
			delta.Z() = 1.0f;

		m_origin = *pMin;
		m_scale.Set(F32(m_dim), F32(m_dim), F32(m_dim));
		m_scale.Divide(delta);
		m_scale.Scale(SPACE_SCALING);

		m_ppBins = new IFXVectorHasherBinNode*[m_dimCubed];
		
		if (m_ppBins)
		{
			memset(m_ppBins, 0, sizeof(IFXVectorHasherBinNode*)*m_dimCubed);			
		}
		else
		{			
			result = IFX_E_OUT_OF_MEMORY;
		}
	}

	return result;
}

IFXRESULT IFXVectorHasher::AddVector(IFXVector3* pVector, U32 meshIndex,
									 U32 vertexIndex)
{
	IFXVectorHasherBinNode** ppFront = GetBinList(pVector);
	IFXVectorHasherBinNode* pDstBin = GetBinNode(ppFront, pVector);
	return AddVectorNode(&(pDstBin->pFront), meshIndex, vertexIndex);
}

IFXVectorHasherBinNode** IFXVectorHasher::GetBinList(IFXVector3* pVector)
{
	IFXVector3 rescaled;
	rescaled.Subtract(*pVector, m_origin).Multiply(m_scale);
	U32 a = U32(rescaled.X()) % m_dim;
	U32 b = U32(rescaled.Y()) % m_dim;
	U32 c = U32(rescaled.Z()) % m_dim;
	U32 index = (m_dimSquared * a) + (m_dim * b) + c;
	return (m_ppBins + index);
}	

IFXVectorHasherBinNode* IFXVectorHasher::GetBinNode(
						IFXVectorHasherBinNode** ppFront, IFXVector3* pVector)
{
	IFXVectorHasherBinNode **ppPreLink = NULL;
	IFXVectorHasherBinNode* pNode = NULL;

	IFXASSERT(ppFront);	

	if (NULL != ppFront)
	{
		ppPreLink = ppFront;
		pNode = *ppPreLink;

		while (pNode != NULL)
		{
			// test if vector belongs in this node
			// NOTE:  Watch out when comparing floating point values:
			//        0x80000000 is a negative zero, and 0x00000000 is zero
			// was:  *(pNode->pVector) == *pVector
			if (pNode->pVector->IsApproximately(*pVector, EQUAL_TOLERANCE))  
				break;

			ppPreLink = &(pNode->pNext);
			pNode = *ppPreLink;
		}

		if (pNode == NULL)
		{
			// Need to add a new node
			pNode = new IFXVectorHasherBinNode;
			*ppPreLink = pNode;		// add link to list
			pNode->pVector = pVector;
			pNode->pFront = NULL;
			pNode->pNext = NULL;
		}
	}

	return pNode;
}

IFXRESULT IFXVectorHasher::AddVectorNode(IFXVectorHasherVectorNode** ppFront, 
										 U32 meshIndex, U32 index)
{
	IFXRESULT result = IFX_OK;
	
	IFXVectorHasherVectorNode **ppPreLink = ppFront;
	IFXVectorHasherVectorNode* pNode = *ppPreLink;

	while (pNode != NULL)
	{
		ppPreLink = &(pNode->pNext);
		pNode = *ppPreLink;
	}

	// Need to add a new node
	pNode = new IFXVectorHasherVectorNode;
	
	if (pNode)
	{
		*ppPreLink = pNode;		// add link to list
		pNode->meshIndex = meshIndex;
		pNode->index = index;
		pNode->pNext = NULL;
	}
	else
	{
		result = IFX_E_OUT_OF_MEMORY;
	}

	return result;
}

BOOL IFXVectorHasher::GetBinIterator(IFXVectorHasherBinIterator* pIter)
{
	IFXVectorHasherBinNode** ppBinList = m_ppBins;
	IFXVectorHasherBinNode** ppEndBinList = m_ppBins + m_dimCubed;

	// skip to first non NULL bin list
	while (ppBinList < ppEndBinList  &&  *ppBinList == NULL)
	{
		++ppBinList;
	}

	BOOL ret = (ppBinList < ppEndBinList) ? IFX_TRUE : IFX_FALSE;

	if (ret)
	{
		pIter->Initialize(ppBinList, ppEndBinList);
	}

	return ret;
}
