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
/*
@file  cIFXResourceClient.h                                                  */

#ifndef __CIFXRESOURCECLIENT_H__
#define __CIFXRESOURCECLIENT_H__

#include "IFXResourceClient.h"
#include "IFXModifier.h"

class CIFXResourceClient : virtual public IFXResourceClient,
                           virtual public IFXModifier
{
public:
  // IFXResourceClient
  U32   IFXAPI       GetResourceIndex() { return m_uResourceIndex; }
  IFXRESULT IFXAPI    SetResourceIndex( U32 uInEntryIndex );

protected:
            CIFXResourceClient();
  virtual  ~CIFXResourceClient();

  U32       m_uResourceIndex;
};


#endif
