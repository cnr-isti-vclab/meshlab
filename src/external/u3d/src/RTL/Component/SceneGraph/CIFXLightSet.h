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
	@file	CIFXLightSet.h

			The header file that defines the base implementation class of the
			CIFXLightSet.
*/

#ifndef __CIFXLIGHTSET_H__
#define __CIFXLIGHTSET_H__

#include "IFXLightSet.h"
#include "IFXCoreCIDs.h"
#include "IFXSpatial.h"

class CIFXLightSet : virtual public IFXLightSet
{

        CIFXLightSet();
  virtual  ~CIFXLightSet();
  friend
  IFXRESULT IFXAPI_CALLTYPE CIFXLightSet_Factory( IFXREFIID iid, void** ppv );

public:
  // IFXUnknown
  U32 IFXAPI        AddRef ();
  U32 IFXAPI        Release ();
  IFXRESULT IFXAPI  QueryInterface (   IFXREFIID  riid,
                              void**     ppv );
  // IFXLightSet
  IFXRESULT IFXAPI Set( U32 uInCount, SPATIALINSTANCE_LIST** ppInLights );
  U32       IFXAPI GetNumLights() const {return m_uCount;}
  IFXRESULT IFXAPI GetLight( U32 uLightIndex, IFXLight*& rpOutLight, U32& lightInstance ) const;
  IFXRESULT IFXAPI Append(U32 uInCount, SPATIALINSTANCE_LIST** ppInLights);

private:
  // IFXUnknown
  U32                     m_uRefCount;

  // IFXLightSet
  IFXSpatialInstance*    m_pLights;
  U32                     m_uCount;
  U32                     m_uAllocated;
};

#endif
