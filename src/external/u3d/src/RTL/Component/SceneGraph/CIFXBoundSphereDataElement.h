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
	@file	CIFXBoundSphereDataElement.h

			The header file that defines the IFXBoundSphereDataElement.              
*/


#ifndef __CIFXBOUNDINGSPHEREDATAELEMENT_H__
#define __CIFXBOUNDINGSPHEREDATAELEMENT_H__

#include "IFXBoundSphereDataElement.h"
#include "IFXVector4.h"

class CIFXBoundSphereDataElement : virtual public IFXBoundSphereDataElement
{
            CIFXBoundSphereDataElement();
  virtual  ~CIFXBoundSphereDataElement();
  friend
  IFXRESULT IFXAPI_CALLTYPE CIFXBoundSphereDataElement_Factory( IFXREFIID riid,
                                                void**    ppv );

public:
  // IFXUnknown
  U32 IFXAPI        AddRef ();
  U32 IFXAPI        Release ();
  IFXRESULT IFXAPI  QueryInterface ( IFXREFIID riid,
                            void**    ppv );

  // IFXBoundSphereDataElement
  IFXVector4& IFXAPI Bound( void );
  U32&        IFXAPI RenderableIndex( void ) { return m_uRenderableDataElementIndex; };

private:
  U32        m_uRefCount;
  IFXVector4 m_vBoundingSphere;
  U32        m_uRenderableDataElementIndex;
};


IFXINLINE IFXVector4& CIFXBoundSphereDataElement::Bound( void )
{
  return m_vBoundingSphere;
}


#endif
