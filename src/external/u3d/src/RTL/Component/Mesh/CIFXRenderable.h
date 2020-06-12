//***************************************************************************
//
//  Copyright (c) 2001 - 2006 Intel Corporation
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
//
// CIFXRenderable.h
//
// Description:
//
// NOTES
//
//*****************************************************************************
#ifndef __CIFXRenderable_H__
#define __CIFXRenderable_H__
#include "IFXRenderable.h"
#include "IFXInterleavedData.h"

class CIFXRenderable : virtual public IFXRenderable
{
public:
  CIFXRenderable();
  virtual ~CIFXRenderable();

  // IFXRenderable
  virtual IFXRESULT IFXAPI  TransferData(IFXRenderable&, BOOL, BOOL) = 0;
  virtual const IFXIID& IFXAPI GetElementType() = 0;

  U32     IFXAPI     GetNumElements() { return m_uNumElements; }
  virtual void** GetElementList() = 0;

  BOOL     IFXAPI    GetEnabled() { return m_bEnabled; }
  IFXRESULT IFXAPI   SetEnabled( BOOL bInEnabled )
          { m_bEnabled = bInEnabled; return IFX_OK; }
  IFXRESULT IFXAPI   GetElementShaderList( U32 uInElement, IFXShaderList** out_ppShaderList );
  IFXRESULT IFXAPI   SetElementShaderList( U32 uInElement, IFXShaderList* in_pShaderList);

  IFXRESULT IFXAPI GetShaderDataArray(IFXShaderList*** pShaderArray);

protected:
  IFXRESULT AllocateShaders(IFXShaderList** in_ppData = NULL, BOOL in_bCopy = TRUE);
  void Deallocate();
  virtual void IFXAPI  DeallocateObject() {};

  // IFXRenderable
  BOOL m_bEnabled;

  U32 m_uNumElements;

  IFXShaderList**    m_ppShaders;
private:
  // IFXUnknown
  U32            m_uRefCount;
};

#endif
