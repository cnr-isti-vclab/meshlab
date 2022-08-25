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
#ifndef IFXVERTEXALLOCATOR_DOT_H
#define IFXVERTEXALLOCATOR_DOT_H

#include "IFXSharedUnitAllocator.h"
#include "IFXTQTVertex.h"



class IFXVertexAllocator : public IFXSharedUnitAllocator
{

public:

    IFXVertexAllocator (U32 uNumInitialUnits, U32 uGrowByNumUnits);
    virtual ~IFXVertexAllocator ();

    IFXTQTVertex *  Allocate ();
    void            Deallocate (IFXTQTVertex *);
    U32             IncRefCount (IFXTQTVertex *);
};




IFXINLINE IFXTQTVertex * IFXVertexAllocator::Allocate()
{
    U8 *pu8 = NULL;

    pu8 = IFXSharedUnitAllocator::Allocate();

    return ((IFXTQTVertex *) pu8);
}

IFXINLINE void IFXVertexAllocator::Deallocate(IFXTQTVertex *pVertex)
{
    U8 *pu8 = (U8 *) pVertex;
    IFXSharedUnitAllocator::Deallocate(pu8);
}


IFXINLINE U32 IFXVertexAllocator::IncRefCount (IFXTQTVertex *pVertex)
{
    U8 *pu8 = (U8 *) pVertex;

    U32 refCount = IFXSharedUnitAllocator::IncRefCount(pu8);

    return refCount;
}

#endif
