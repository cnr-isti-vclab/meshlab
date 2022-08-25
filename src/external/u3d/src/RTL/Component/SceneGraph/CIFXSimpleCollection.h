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
	@file	CIFXSimpleCollection.h

			The header file for the base CCIFXSimpleCollection class.
*/

#ifndef __CIFXSIMPLECOLLECTION_H__
#define __CIFXSIMPLECOLLECTION_H__

#include "IFXSpatial.h"
#include "IFXCollection.h"
#include "IFXCoreCIDs.h"

class CIFXSimpleCollection : virtual public IFXCollection
{
            CIFXSimpleCollection();
  virtual  ~CIFXSimpleCollection();
  friend
  IFXRESULT IFXAPI_CALLTYPE CIFXSimpleCollection_Factory(
                                  IFXREFIID         interfaceId,
                                  void**            ppInterface );
public:
  // IFXUnknown
  U32 IFXAPI        AddRef ();
  U32 IFXAPI        Release ();
  IFXRESULT IFXAPI  QueryInterface (       IFXREFIID         riid,
                                  void**            ppv );
  // IFXCollection
    const
  IFXGUID&  GetCID() { return CID_IFXSimpleCollection; };

  IFXRESULT  IFXAPI 	 InitializeCollection( IFXSpatial**      pInSpatials,
                                  U32               uInNumberOfSpatials,
                                  IFXSpatial::eType eInType );
  IFXRESULT  IFXAPI 	 InitializeCollection( IFXCollection* pInCollection);

  IFXRESULT  IFXAPI 	 AddSpatials(          IFXSpatial**      pInSpatials,
                                  U32               uInNumberOfSpatials,
                                  IFXSpatial::eType eInType );
  IFXRESULT  IFXAPI 	 RemoveSpatials(       IFXSpatial**      pInSpatials,
                                  U32               uInNumberOfSpatials,
                                  IFXSpatial::eType eInType );
  IFXRESULT  IFXAPI 	 GetSpatials(          IFXSpatial**&     rpOutSpatials,
                                  U32&              ruOutNumberOfSpatials,
                                  IFXSpatial::eType eInType               );
private:
  IFXRESULT _AddSpatials(         IFXSpatial**      pInSpatials,
                                  U32               uInNumberOfSpatials,
                                  IFXSpatial::eType eInType );
  void      Destruct();

  IFXSpatial** m_pSpatials[IFXSpatial::TYPE_COUNT];
  U32          m_uSpatials[IFXSpatial::TYPE_COUNT];
  U32          m_uSpatialsAllocated[IFXSpatial::TYPE_COUNT];

  static
  const U32    m_allocationPad;

  U32          m_uRefCount;
};

#endif
