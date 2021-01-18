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
	@file	CIFXLightResource.h                                                           
*/

#ifndef __CIFXLightResource_H__
#define __CIFXLightResource_H__

#include "IFXLightResource.h"
#include "CIFXMarker.h"


class CIFXLightResource : private CIFXMarker, virtual public IFXLightResource
{
            CIFXLightResource();
  virtual  ~CIFXLightResource();
  friend
  IFXRESULT IFXAPI_CALLTYPE CIFXLightResource_Factory( IFXREFIID riid,
                               void**    ppv );
public:
  // IFXUnknown
  U32 IFXAPI        AddRef ();
  U32 IFXAPI        Release ();
  IFXRESULT IFXAPI  QueryInterface ( IFXREFIID riid,
                            void**    ppv );

  // IFXMarkerX
  void IFXAPI       GetEncoderX ( IFXEncoderX*& rpEncoderX );

  // IFXLightResource
  LightType IFXAPI GetType() { return (LightType)(m_Light.GetType()-IFX_AMBIENT); };
  void   IFXAPI    SetType( LightType type ) { m_Light.SetType((U32)type+IFX_AMBIENT); };

  U8     IFXAPI    GetAttributes() { return m_u8Attributes; };
  void   IFXAPI    SetAttributes( U8 uIn ) { m_u8Attributes = uIn; };

  const IFXVector4& IFXAPI GetColor() { return m_Light.GetDiffuse(); };
  void  IFXAPI     SetColor( IFXVector4 vIn );

  IFXRESULT IFXAPI GetAttenuation( F32* fOut );
  IFXRESULT IFXAPI SetAttenuation( F32* fIn );

  F32 IFXAPI GetIntensity();
  IFXRESULT IFXAPI SetIntensity( F32 fIn );

  F32       IFXAPI GetSpotAngle();
  void      IFXAPI SetSpotAngle( F32 fAngle );

  F32       IFXAPI GetExtent() { return m_Light.GetRange(); };

  IFXRenderLight& GetRenderLight() { return m_Light; }

private:

  // IFXLightResource
  U8 m_u8Attributes;
  static const F32         m_scfMaxExtent;
  static const F32         m_scfAttenuationEpsilon;
  void CalculateRange();
  IFXRenderLight m_Light;

  U32 m_uRefCount;
};


#endif
