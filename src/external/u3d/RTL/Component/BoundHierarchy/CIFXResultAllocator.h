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
#ifndef CIFXRESULTALLOCATOR_DOT_H
#define CIFXRESULTALLOCATOR_DOT_H

#include "IFXUnitAllocator.h"
#include "CIFXCollisionResult.h"


class CIFXResultAllocator : public IFXUnitAllocator
{

public:

	CIFXResultAllocator(U32 uNumInitialUnits, U32 uGrowByNumUnits);
	~CIFXResultAllocator();

    IFXRESULT Allocate(CIFXCollisionResult **ppResult);
    void Deallocate(CIFXCollisionResult *);

private:

	CIFXCollisionResult *pResult;
	U32                  m_uSizeList;
};


inline void CIFXResultAllocator::Deallocate(CIFXCollisionResult *pResult)
{
    U8 *pu8 = (U8 *) pResult;
    IFXUnitAllocator::Deallocate(pu8);
}

#endif
