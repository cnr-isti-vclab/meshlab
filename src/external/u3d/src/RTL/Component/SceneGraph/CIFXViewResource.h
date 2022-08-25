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
	@file	CIFXViewResource.h                                                            
*/

#ifndef __CIFXViewResource_H__
#define __CIFXViewResource_H__

#include "CIFXMarker.h"
#include "IFXViewResource.h"
#include "IFXFrustum.h"

#include "IFXCoreCIDs.h"

class IFXMaterialResource;
class IFXSpatialSetQuery;

// The Fog density calculations use a saturation value of 1/100 (0.01).
// constant = ln(1/saturation value)
#define EXPONENTIAL_FOG_CONSTANT \
                            ((F32)(4.6051701859880913680359829093687))
// constant = sqrt(ln(1/saturation value))
#define EXPONENTIAL_SQUARED_FOG_CONSTANT \
                            ((F32)(2.14596602628934723963618357029))

class CIFXViewResource : protected CIFXMarker, virtual public IFXViewResource
{
public:
	// IFXUnknown
	U32 IFXAPI        AddRef ();
	U32 IFXAPI        Release ();
	IFXRESULT IFXAPI  QueryInterface ( IFXREFIID riid, void** ppv );

	// IFXMarker
	IFXRESULT IFXAPI	 SetSceneGraph( IFXSceneGraph* pInSceneGraph );

	// IFXMarkerX
	void IFXAPI			 GetEncoderX (IFXEncoderX*& rpEncoderX);

	// IFXViewResource
	IFXRESULT  IFXAPI 	 GetFogEnableValue(  BOOL* pbOutEnable );
	IFXRESULT  IFXAPI 	 FogEnable( BOOL bInEnable );
	IFXRenderFog& IFXAPI GetRenderFog();

	IFXRESULT  IFXAPI 	 GetColorBufferEnabled( BOOL& bEnabled );
	IFXRESULT  IFXAPI 	 SetColorBufferEnabled( BOOL  bEnabled );

	IFXRESULT  IFXAPI 	 GetDepthTestEnabled( BOOL& bEnabled );
	IFXRESULT  IFXAPI 	 SetDepthTestEnabled( BOOL  bEnabled );
	IFXRESULT  IFXAPI 	 GetDepthWriteEnabled( BOOL& bEnabled );
	IFXRESULT  IFXAPI 	 SetDepthWriteEnabled( BOOL  bEnabled );
	IFXRESULT  IFXAPI 	 GetDepthFunc(IFXenum& eDepthFunc );
	IFXRESULT  IFXAPI 	 SetDepthFunc(IFXenum  eDepthFunc );

	IFXRESULT  IFXAPI 	 GetStencilEnabled(BOOL& bEnabled);
	IFXRESULT  IFXAPI 	 SetStencilEnabled(BOOL bEnabled);
	IFXRenderStencil& IFXAPI GetRenderStencil();

	IFXRESULT  IFXAPI 	 GetRootNode(U32* pNodeIndex, U32* pNodeInstance);
	IFXRESULT  IFXAPI 	 SetRootNode(U32 nodeIndex, U32 nodeInstance);
	void  IFXAPI 	 ClearRootNode();
	IFXRenderClear& IFXAPI GetRenderClear();

	// Multipass renderin support
	IFXRESULT  IFXAPI 	SetNumRenderPasses(U32 uNumPasses);
	U32		   IFXAPI	GetNumRenderPasses(void);
	IFXRESULT  IFXAPI 	SetCurrentRenderPass(U32 uPassNum);
	U32		   IFXAPI	GetCurrentRenderPass();
	IFXRenderPass* IFXAPI GetRenderPass(void) { return m_ppRenderPass[m_uCurrentPass]; };
	IFXRenderPass* IFXAPI GetRenderPass(U32 uIndex) { return m_ppRenderPass[uIndex]; };

protected:
	IFXRESULT AllocateRenderPasses(U32 uNumRenderPasses = 1);
	IFXRESULT DeallocateRenderPasses();

	// Data for managing multi-pass state
	U32 m_uNumRenderPasses;
	U32 m_uCurrentPass;
	IFXRenderPass** m_ppRenderPass; ///< The array of render passes

	U32 m_uQualityFactor;

	U32 m_layer;

	// IFXUnknown
	U32 m_uRefCount;

private:
			CIFXViewResource();
	virtual  ~CIFXViewResource();
	friend IFXRESULT IFXAPI_CALLTYPE CIFXViewResource_Factory( IFXREFIID riid, void** ppv );
};


#endif
