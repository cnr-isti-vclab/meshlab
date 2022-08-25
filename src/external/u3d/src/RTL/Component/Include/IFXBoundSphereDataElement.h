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
	@file	IFXBoundSphereDataElement.h

			The header file that defines the IFXBoundSphereDataElement.              
*/

#ifndef __IFXBOUNDINGSPHEREDATAELEMENT_H__
#define __IFXBOUNDINGSPHEREDATAELEMENT_H__

#include "IFXUnknown.h"
#include "IFXVector4.h"

IFXDEFINE_GUID(IID_IFXBoundSphereDataElement,
0x58851c7a, 0xd427, 0x4344, 0x80, 0xa7, 0xd5, 0x3, 0x82, 0x4e, 0xdb, 0x15);

/**
 *  The IFXBoundSphereDataElement interface presents the bounding sphere and
 *  an index of the bounded renderable within the modifier data packet.
 *
 *  The bound is disassociated from the bounded renderables to facilitate
 *  different request frequencies.  The request for a IFXBoundSphereDataElement
 *  is used for intersection testing against a view frustum or a ray at
 *  higher frequencies than the bounded renderables (which are usually
 *  requested as a result of a positive intersection test).
 *
 *  The set of IFXBoundSphereDataElements within a modifier data packet can be
 *  iterated by getting an iterator from the IFXDataPacket interface specifying
 *  the IFX_DID_BOUND iterator type:
 *    result = pDataPacket->GetIterator(IFX_DID_BOUND, &pBoundingSphereIter);
 */
class IFXBoundSphereDataElement : virtual public IFXUnknown
{
public:
  /**
   *  Accesses the bounding sphere.  The first three elements are the
   *  position in model space of the center of the sphere.  The fourth
   *  element is the sphere's radius.
   *
   *  @return A reference to the IFXVector4 bounding sphere value.
   */
  virtual IFXVector4& IFXAPI Bound( void )    = 0;

  /**
   *  Accesses the bounded renderable index.
   *
   *  @return A reference to the U32 index of the renderable within
   *  the modifier data packet that is bounded by bound().
   */
  virtual U32& IFXAPI RenderableIndex( void ) = 0;
};

#endif
