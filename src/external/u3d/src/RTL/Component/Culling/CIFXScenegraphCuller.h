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
@file  CIFXSceneGraphCuller.h

The header file of the CIFXSceneGraphCuller component.
*/

#ifndef __CIFXSCENEGRAPHCULLER_H__
#define __CIFXSCENEGRAPHCULLER_H__

#include "IFXCollection.h"
#include "IFXSpatialSetQuery.h"
#include "IFXCoreCIDs.h"

class CIFXSceneGraphCuller : virtual public IFXSpatialSetQuery
{
            CIFXSceneGraphCuller();
  virtual  ~CIFXSceneGraphCuller();
  friend
  IFXRESULT IFXAPI_CALLTYPE CIFXSceneGraphCuller_Factory( IFXREFIID interfaceId,
                                          void**    ppInterface );
public:
  // IFXUnknown
  U32 IFXAPI        AddRef ();
  U32 IFXAPI        Release ();
  IFXRESULT IFXAPI  QueryInterface ( 
					IFXREFIID interfaceId,
                    void**    ppInterface );

  // IFXSpatialSetQuery
  IFXRESULT IFXAPI 	QueryForSpatialSet(
										const IFXFrustum*		pInFrustum,
										SPATIALINSTANCE_LIST*&  rpOpaque,
										SPATIALINSTANCE_LIST*&  rpTranslucent,
										F32&					ruOutNearClipPlane,
										F32&					ruOutFarClipPlane );
  const IFXCID& IFXAPI GetCollectionCID() const {return CID_IFXSimpleCollection;}
  IFXRESULT IFXAPI 	SetCollection( IFXCollection* pInCollection );
  IFXRESULT IFXAPI 	GetCollection( IFXCollection*& rpOutCollection ) const;

private:
  IFXCollection*	m_pCollection;

  static F64*       m_pHugeSortKeys;
  static U32        m_uHugeSortKeys;
  static U32        m_uHugeSortKeysAllocated;

  U32               m_uRefCount;
};

#endif
